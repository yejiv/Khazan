#include "RootBody.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "Bone.h"
#include "ChildBody.h"

static inline bool IsFiniteVec3(const JPH::Vec3& v) {
    return std::isfinite(v.GetX()) && std::isfinite(v.GetY()) && std::isfinite(v.GetZ());
}

CRootBody::CRootBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CRootBody::Initialize(ROOT_BODY_DESC* pDesc)
{
    m_pModel = pDesc->pModel;
    m_iBoneIndex = pDesc->iBoneIndex;
    m_pBone = m_pModel->Find_Bone(pDesc->iBoneIndex);    
    m_strBoneName = m_pBone->Get_Name();
    m_pOwnerTransform = pDesc->pOwnerTransform;
    m_iObjectLayer = pDesc->iObjectLayer;

    if (FAILED(Ready_Body(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Child(pDesc)))
        return E_FAIL;

    
    XMStoreFloat4x4(&m_CombindMatrix, XMMatrixIdentity());

    return S_OK;
}

void CRootBody::Priority_Update(_float fTimeDelta)
{
    SyncFromAnimation(fTimeDelta);

    for (auto Child : m_ChildBodys)
    {
        Child->Priority_Update(fTimeDelta);
    }
}

void CRootBody::Update(_float fTimeDelta)
{
    if (m_fFreezeTimer > 0.f)
    {
        m_fFreezeTimer -= fTimeDelta;
        if (m_fFreezeTimer < 0.f) 
            m_fFreezeTimer = 0.f;
    }

    for (auto Child : m_ChildBodys)
    {
        Child->Update(fTimeDelta, m_fFreezeTimer > 0.f);
    }
}

void CRootBody::Late_Update(_float fTimeDelta)
{    
    for (auto Child : m_ChildBodys)
    {
        Child->Late_Update(0.5f, m_fFreezeTimer > 0.f);
    }
}

void CRootBody::SyncFromAnimation(_float fTimeDelta)
{
    if (!m_pBody || !m_pOwnerTransform)
        return;

    _matrix NonScaleBoneMatrix = m_pBone->Get_CombinedTransformationMatrix();
    for (_uint i = 0; i < 3; ++i)
        NonScaleBoneMatrix.r[i] = XMVector3Normalize(NonScaleBoneMatrix.r[i]);

    _matrix Combined = NonScaleBoneMatrix * m_pOwnerTransform->Get_WorldMatrix();

    _vector vScale, vRotQ, vTrans;
    XMMatrixDecompose(&vScale, &vRotQ, &vTrans, Combined);

    RVec3 pos((float)vTrans.m128_f32[0], (float)vTrans.m128_f32[1], (float)vTrans.m128_f32[2]);
    Quat  rot(vRotQ.m128_f32[0], vRotQ.m128_f32[1], vRotQ.m128_f32[2], vRotQ.m128_f32[3]);

    if (!IsFiniteVec3(Vec3(pos)) || !rot.IsNormalized())
        return;

    // === 1) 첫 프레임 처리 ===
    if (m_isFirstSync)
    {
        m_isFirstSync = false;
        m_vPrevAnimPos = pos;
        m_qPrevAnimRot = rot;

        HardSnapToAnimation(pos, rot); // 맨 처음에는 그냥 붙여두기
        return;
    }

    // === 2) 이동량 / 회전량 체크 ===
    Vec3 delta = Vec3(pos - m_vPrevAnimPos);
    float dist = delta.Length();   // 한 프레임 이동 거리(월드)

    // 회전 변화량 (쿼터니언 내적 → 각도)
    float dotRot = m_qPrevAnimRot.Dot(rot);
    dotRot = clamp(dotRot, -1.0f, 1.0f);
    float angle = 2.0f * acosf(fabsf(dotRot)); // 라디안

    const float teleportDist = 0.6f;        // 한 프레임에 60cm 이상 이동하면 텔레포트 취급
    const float teleportAngle = XMConvertToRadians(70.0f); // 70도 이상이면 텔레포트 취급

    bool bTeleport = (dist > teleportDist) || (angle > teleportAngle);

    if (bTeleport)
    {
        // 넉백, 순간이동, 강한 피격 등
        HardSnapToAnimation(pos, rot);
        // 몇 프레임 동안 cloth 물리 끔 (애니메이션에만 붙이기)
        m_fFreezeTimer = 0.2f; // 0.2초 정도
    }
    else
    {
        // 평상시에는 정상적으로 키네마틱 이동
        m_pBodyInterface->MoveKinematic(m_BodyID, pos, rot, fTimeDelta);
    }

    m_vPrevAnimPos = pos;
    m_qPrevAnimRot = rot;
}

void CRootBody::ApplyToBones(_float fAlpha)
{
    CBone* ParentBone = m_pModel->Find_Bone(m_pBone->Get_ParentBoneIndex());

    _matrix bodyWorld = MakeMatrixFromBody(m_pBody);
    
    _matrix boneModel = bodyWorld * m_pOwnerTransform->Get_WorldMatrix_Inverse();
    
    _matrix parentCombined;    
    parentCombined = m_pModel->Find_Bone(m_pBone->Get_ParentBoneIndex())->Get_CombinedTransformationMatrix();

    _matrix invParent = XMMatrixInverse(nullptr, parentCombined);
    
    _matrix physLocalM = boneModel * invParent;

    _float4x4 physLocal;
    XMStoreFloat4x4(&physLocal, physLocalM);

    _matrix animLocal = m_pBone->Get_TransformationMatrix();
    _float4x4 blended = BlendLocal(animLocal, physLocal, fAlpha);
  
    m_pBone->Set_TransformationMatrix(XMLoadFloat4x4(&blended));

    m_pBone->Update_CombinedTransformationMatrix(ParentBone);
}

HRESULT CRootBody::Ready_Child(ROOT_BODY_DESC* pDesc)
{
    vector<_int> ChildBoneIdx = m_pBone->Get_ChildBones();
    for (_int i = 0; i < ChildBoneIdx.size(); i++) 
    {
        CChildBody::CHILD_BODY_DESC desc;
        desc.pModel = m_pModel;
        desc.iBoneIndex = ChildBoneIdx[i];
        desc.pOwnerTransform = m_pOwnerTransform;
        desc.iObjectLayer = m_iObjectLayer;
        desc.pParentBody = m_pBody;
        desc.iDepth = 0;

        desc.fGravity = pDesc->fGravity;
        desc.fAngularDamping = pDesc->fAngularDamping;
        desc.fLinearDamping = pDesc->fLinearDamping;
        desc.fMass = pDesc->fMass;

        desc.fMinDistance = pDesc->fMinDistance;
        desc.fMaxDistance = pDesc->fMaxDistance;
        desc.fSpringFrequency = pDesc->fSpringFrequency;
        desc.fSpringDamping = pDesc->fSpringDamping;

        desc.eType = pDesc->eType;
        desc.pRootBody = m_pBody;

        m_ChildBodys.push_back(CChildBody::Create(m_pDevice, m_pContext, &desc));
    }
    return S_OK;
}

HRESULT CRootBody::Ready_Body(ROOT_BODY_DESC* pDesc)
{        
    RefConst<Shape> pShape;
    pShape = new SphereShape(0.05f);
    pShape = new RotatedTranslatedShape(Vec3(0.f, 0.f, 0.f), Quat::sIdentity(), pShape);

    _matrix NonScaleBoneMatrix = m_pBone->Get_CombinedTransformationMatrix();
    for (_uint i = 0; i < 3; i++)
        NonScaleBoneMatrix.r[i] =
        XMVector3Normalize(NonScaleBoneMatrix.r[i]);

    _matrix CombindedMatirx = NonScaleBoneMatrix * m_pOwnerTransform->Get_WorldMatrix();

    _vector vScale, vTrans, vQuat;

    XMMatrixDecompose(&vScale, &vQuat, &vTrans, CombindedMatirx);

    BodyCreationSettings BCS(
        pShape,
        Vec3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]),
        Quat(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]),
        m_eMotion,
        ObjectLayer(m_iObjectLayer)
    );

    BCS.mMotionQuality = EMotionQuality::LinearCast;
    BCS.mIsSensor = false;
    BCS.mGravityFactor = 0.f;
    BCS.mAllowSleeping = false;
    BCS.mUserData = static_cast<uint64>(reinterpret_cast<uintptr_t>(pDesc->pCollisionDesc));

    m_pBody = m_pGameInstance->CreateAndAdd_Body(BCS, &m_pBodyInterface);
    m_BodyID = m_pBody->GetID();

    return S_OK;
}

void CRootBody::HardSnapToAnimation(const RVec3& pos, const Quat& rot)
{
    // 루트 바디를 애니메이션 포즈로 강제 세팅
    m_pBodyInterface->SetPositionAndRotation(m_BodyID, pos, rot, EActivation::DontActivate);
    m_pBodyInterface->SetLinearVelocity(m_BodyID, Vec3::sZero());
    m_pBodyInterface->SetAngularVelocity(m_BodyID, Vec3::sZero());

    // 이 루트 아래 모든 ChildBody 도 애니메이션 기준으로 리셋
    for (auto Child : m_ChildBodys)
        Child->HardSnapToAnimationRecursive();
}

CRootBody* CRootBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ROOT_BODY_DESC* tDesc)
{
    CRootBody* pInstance = new CRootBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize(tDesc)))
    {
        MSG_BOX(TEXT("Failed to Created : CRootBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRootBody::Free()
{
    if (!m_BodyID.IsInvalid() && m_pBodyInterface)
    {
        if (!m_BodyID.IsInvalid())
        {
            m_pGameInstance->Destroy_Body(m_BodyID);
            m_BodyID = BodyID();
            m_pBody = nullptr;
        }

    }
    
    for (auto pChild : m_ChildBodys)
        Safe_Release(pChild);
    m_ChildBodys.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    __super::Free();

}
