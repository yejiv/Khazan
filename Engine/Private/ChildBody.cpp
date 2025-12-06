#include "ChildBody.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "Bone.h"

static inline bool IsFiniteVec3(const JPH::Vec3& v) {
    return std::isfinite(v.GetX()) && std::isfinite(v.GetY()) && std::isfinite(v.GetZ());
}

CChildBody::CChildBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
{

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CChildBody::Initialize(CHILD_BODY_DESC* pDesc)
{
    m_pModel = pDesc->pModel;
    m_iBoneIndex = pDesc->iBoneIndex;
    m_pBone = m_pModel->Find_Bone(pDesc->iBoneIndex);
    m_strBoneName = m_pBone->Get_Name();
    m_pOwnerTransform = pDesc->pOwnerTransform;
    m_iObjectLayer = pDesc->iObjectLayer;
    m_pParentBody = pDesc->pParentBody;
    m_iDepth = pDesc->iDepth;

    m_fGravity = pDesc->fGravity;
    m_fAngularDamping = pDesc->fAngularDamping;
    m_fLinearDamping = pDesc->fLinearDamping;
    m_fMass = pDesc->fMass;

    m_fMinDistance = pDesc->fMinDistance;
    m_fMaxDistance = pDesc->fMaxDistance;
    m_fSpringFrequency = pDesc->fSpringFrequency;
    m_fSpringDamping = pDesc->fSpringDamping;

    m_pRootBody = pDesc->pRootBody;

    m_eClothType = pDesc->eType;

    if (FAILED(Ready_Body()))
        return E_FAIL;

    if (FAILED(Ready_Child()))
        return E_FAIL;

    _matrix restLocal = m_pBone->Get_TransformationMatrix();
    _vector vScale, vQuat, vTrans;
    XMMatrixDecompose(&vScale, &vQuat, &vTrans, restLocal);

    XMStoreFloat3(&m_vRestLocalPos, vTrans);
    XMStoreFloat4(&m_vRestLocalRot, vQuat);
    

    return S_OK;
}

void CChildBody::Priority_Update(_float fTimeDelta)
{
    _matrix NonScaleBoneMatrix = m_pBone->Get_CombinedTransformationMatrix();
    for (_uint i = 0; i < 3; i++)
        NonScaleBoneMatrix.r[i] =
        XMVector3Normalize(NonScaleBoneMatrix.r[i]); // 0~2번 행만 노멀라이즈, 3번은 그대로

    XMStoreFloat4x4(&m_CombindMatrix,
        NonScaleBoneMatrix * m_pOwnerTransform->Get_WorldMatrix());

    for (auto Child : m_ChildBodys)
    {
        Child->Priority_Update(fTimeDelta);
    }
}

void CChildBody::Update(_float fTimeDelta)
{
    Type_Update(fTimeDelta);
}

void CChildBody::Late_Update(_float fAlpha)
{
    float baseAlpha = 1.f;

    if (m_eClothType == CLOTHTYPE::SKIRT)
        baseAlpha = 0.5f;
    else if (m_eClothType == CLOTHTYPE::CAPE)
        baseAlpha = 1.0f;

    ApplyToBones(fAlpha * baseAlpha);

    for (auto Child : m_ChildBodys)
    {
        Child->Late_Update(fAlpha);
    }
}

void CChildBody::ApplyToBones(_float fAlpha)
{
    if (!m_pBody)
        return;

    const _int parentIndex = m_pBone->Get_ParentBoneIndex();
    if (parentIndex < 0)
        return;

    CBone* pParentBone = m_pModel->Find_Bone(parentIndex);
    if (!pParentBone)
        return;

    _matrix parentCombined = pParentBone->Get_CombinedTransformationMatrix();
    _matrix parentWorld = parentCombined * m_pOwnerTransform->Get_WorldMatrix();
    _matrix invParentWorld = XMMatrixInverse(nullptr, parentWorld);

    RVec3 bodyPosR = m_pBodyInterface->GetCenterOfMassPosition(m_BodyID);
    _vector bodyWorldPos = XMVectorSet(
        (float)bodyPosR.GetX(),
        (float)bodyPosR.GetY(),
        (float)bodyPosR.GetZ(),
        1.f
    );

    _vector physLocalPos = XMVector3TransformCoord(bodyWorldPos, invParentWorld);

    // 기준 로컬 포즈
    _vector tRest = XMLoadFloat3(&m_vRestLocalPos);
    _vector rRest = XMLoadFloat4(&m_vRestLocalRot);

    _vector delta = physLocalPos - tRest;

    
    // ========= 3-1. 위/아래 분리 =========
    XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    XMVECTOR vertical = XMVector3Dot(delta, up) * up;   // Y 방향
    XMVECTOR horizontal = delta - vertical;               // XZ 방향

    float vLen = XMVectorGetX(XMVector3Dot(vertical, up)); // +면 위, -면 아래

    float maxHoriz;
    float verticalUpScale;
    float baseSagPerDepth;

    if (m_eClothType == CLOTHTYPE::SKIRT)
    {
        maxHoriz = 0.35f;  // 치마는 많이 퍼지지 않게
        verticalUpScale = 0.3f;   // 위로는 30%만
        baseSagPerDepth = 0.03f;  // 3cm씩 아래로
    }
    else // CAPE
    {
        maxHoriz = 0.6f;   // 망토는 더 퍼져도 됨
        verticalUpScale = 0.7f;   // 위로도 꽤 움직이게
        baseSagPerDepth = 0.02f;  // 너무 축 늘어지지 않게
    }

    XMVECTOR verticalClamped;
    if (vLen > 0.f)
        verticalClamped = vertical * verticalUpScale;
    else
        verticalClamped = vertical;

    float horizLen = XMVectorGetX(XMVector3Length(horizontal));
    if (horizLen > maxHoriz && horizLen > 1e-6f)
        horizontal = horizontal * (maxHoriz / horizLen);

    delta = horizontal + verticalClamped;

    float depthFactor = 0.4f + 0.3f * (float)m_iDepth;
    if (depthFactor > 1.f) depthFactor = 1.f;

    float finalAlpha = fAlpha * depthFactor;

    float sag = baseSagPerDepth * (float)m_iDepth;
    XMVECTOR sagOffset = XMVectorSet(0.f, -sag, 0.f, 0.f);

    XMVECTOR finalPos = tRest + delta * finalAlpha + sagOffset;

    XMVECTOR sRest = XMVectorSet(1.f, 1.f, 1.f, 0.f);
    XMMATRIX finalLocalM = XMMatrixAffineTransformation(
        sRest,
        XMVectorZero(),
        rRest,
        finalPos
    );

    m_pBone->Set_TransformationMatrix(finalLocalM);
    m_pBone->Update_CombinedTransformationMatrix(pParentBone);
}

void CChildBody::Type_Update(_float fTimeDelta)
{
    if (m_eClothType == CLOTHTYPE::CAPE)
    {
        Cape_Update(fTimeDelta);
    }
}

void CChildBody::Cape_Update(_float fTimeDelta)
{
    if (!m_pBody || !m_pParentBody || !m_pRootBody)
        return;

    Vec3 charVel = m_pRootBody->GetLinearVelocity();
    Vec3 horizVel(charVel.GetX(), 0.f, charVel.GetZ());
    float speed = horizVel.Length();

    Vec3 charForward = LoadVec3(m_pOwnerTransform->Get_State(STATE::LOOK));
    charForward.SetY(0.f);
    charForward = charForward.Normalized();

    // --- 1) 전진 → 뒤로 바람 ---
    if (speed > 0.1f)
    {
        horizVel /= speed;
        float dirDot = horizVel.Dot(charForward);
        if (dirDot > 0.1f)
        {
            Vec3 windDir = -charForward;
            float windStrength = 20.0f;
            Vec3 force = windDir * (windStrength * dirDot);
            m_pBodyInterface->AddForce(m_BodyID, force);
        }
    }

    // --- 2) 회전 속도 → 옆으로 스윙 (원하면) ---
    float yawVel = m_pRootBody->GetAngularVelocity().GetY(); // 이런 함수 하나 더 만들어도 좋고
    if (fabsf(yawVel) > 0.1f)
    {
        Vec3 right = LoadVec3(m_pOwnerTransform->Get_State(STATE::RIGHT));
        right.SetY(0.f);
        right = right.Normalized();

        float swingStrength = 10.0f;
        Vec3 sideForce = right * (swingStrength * yawVel);
        m_pBodyInterface->AddForce(m_BodyID, sideForce);
    }
}

HRESULT CChildBody::Ready_Child()
{
    vector<_int> ChildBoneIdx = m_pBone->Get_ChildBones();
    for (_int i = 0; i < ChildBoneIdx.size(); i++)
    {
        CHILD_BODY_DESC desc;
        desc.pModel = m_pModel;
        desc.iBoneIndex = ChildBoneIdx[i];
        desc.pOwnerTransform = m_pOwnerTransform;
        desc.iObjectLayer = m_iObjectLayer;
        desc.pParentBody = m_pBody;
        desc.iDepth = m_iDepth + 1;

        desc.fGravity = m_fGravity;
        desc.fAngularDamping = m_fAngularDamping;
        desc.fLinearDamping = m_fLinearDamping;
        desc.fMass = m_fMass;

        desc.fMinDistance = m_fMinDistance;
        desc.fMaxDistance = m_fMaxDistance;
        desc.fSpringFrequency = m_fSpringFrequency;
        desc.fSpringDamping = m_fSpringDamping;

        desc.eType = m_eClothType;

        desc.pRootBody = m_pRootBody;
        m_ChildBodys.push_back(CChildBody::Create(m_pDevice, m_pContext, &desc));
    }
    return S_OK;
}

HRESULT CChildBody::Ready_Body()
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
    BCS.mGravityFactor = m_fGravity;      // 완전 0 말고 살짝
    BCS.mAngularDamping = m_fAngularDamping;     // 회전 감쇠 너무 크지 않게
    BCS.mLinearDamping = m_fLinearDamping;     // 직선 감쇠도
    BCS.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
    BCS.mMassPropertiesOverride.mMass = m_fMass; // 너무 무겁지 않게

    m_pBody = m_pGameInstance->CreateAndAdd_Body(BCS, &m_pBodyInterface);
    m_BodyID = m_pBody->GetID();


    // 제약 설정
    if (!m_pParentBody->GetID().IsInvalid())
    {
        // 부모/자식 COM 월드 위치     
        RVec3 parentPos = m_pParentBody->GetCenterOfMassPosition();
        RVec3 childPos = m_pBody->GetCenterOfMassPosition();

        DistanceConstraintSettings settings;
        settings.mSpace = EConstraintSpace::WorldSpace;
        settings.mPoint1 = parentPos;
        settings.mPoint2 = childPos;

        // rest 거리
        _float restLen = (_float)(childPos - parentPos).Length();

        // 살짝 늘어났다 줄었다 할 수 있게 여유 줌
        settings.mMinDistance = restLen * m_fMinDistance;
        settings.mMaxDistance = restLen * m_fMaxDistance;

        // 부드러운 스프링
        settings.mLimitsSpringSettings.mFrequency = m_fSpringFrequency; // 2Hz 정도 (너무 크면 탱탱볼처럼 튐)
        settings.mLimitsSpringSettings.mDamping = m_fSpringDamping; // 약간 감쇠

        m_pConstraint = static_cast<DistanceConstraint*>(settings.Create(*m_pParentBody, *m_pBody));

        // PhysicsSystem에 등록 (엔진 래퍼에 맞게)
        m_pGameInstance->Add_Constraint(m_pConstraint);
    }

    return S_OK;
}

CChildBody* CChildBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CHILD_BODY_DESC* pDesc)
{
    CChildBody* pInstance = new CChildBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX(TEXT("Failed to Created : CChildBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChildBody::Free()
{

    if (m_pConstraint != nullptr)
    {
        m_pGameInstance->Remove_Constraint(m_pConstraint);
        m_pConstraint = nullptr;
    }

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
