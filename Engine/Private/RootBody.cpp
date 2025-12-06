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

    if (FAILED(Ready_Body()))
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
    for (auto Child : m_ChildBodys)
    {
        Child->Update(fTimeDelta);
    }
}

void CRootBody::Late_Update(_float fTimeDelta)
{    
    for (auto Child : m_ChildBodys)
    {
        Child->Late_Update(0.5f);
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

    m_pBodyInterface->MoveKinematic(m_BodyID, pos, rot, fTimeDelta);
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

HRESULT CRootBody::Ready_Body()
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

    m_pBody = m_pGameInstance->CreateAndAdd_Body(BCS, &m_pBodyInterface);
    m_BodyID = m_pBody->GetID();

    return S_OK;
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

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    __super::Free();

}
