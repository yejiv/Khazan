#include "EnginePch.h"
#include "PhysicsBody.h"
#include "GameInstance.h"
#include "Transform.h"


// ====== 유틸 변환 ======
static inline RVec3 ToRVec3(const _float3& v) { return RVec3(v.x, v.y, v.z); }
static inline _float3 FromRVec3(const RVec3& v) { return { _float(v.GetX()), _float(v.GetY()), _float(v.GetZ()) }; }

// _float4 = (x, y, z, w)  ←→  JPH::Quat(x, y, z, w)
static inline JPH::Quat ToQuat(const _float4& q) {
    return JPH::Quat(q.x, q.y, q.z, q.w);
}

static inline _float4 FromQuat(const JPH::Quat& q) {
    return { _float(q.GetX()), _float(q.GetY()), _float(q.GetZ()), _float(q.GetW()) };
}
CPhysicsBody::CPhysicsBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

CPhysicsBody::CPhysicsBody(const CPhysicsBody& Prototype)
    : CComponent{ Prototype }
    , m_pGameInstance { Prototype.m_pGameInstance }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPhysicsBody::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CPhysicsBody::Initialize_Clone(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    m_pPhysics = &m_pGameInstance->Get_PhysicsSystem();
    m_eMotion = pDesc->eMotion;
    m_tMat = pDesc->tMat;
    m_iLayer = pDesc->iObjectLayer;
    XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

    // Transform에서 초기 TR 분해
    _float3 vPos{};
    _float4 qRot{};
    XMStoreFloat3(&vPos, pDesc->pTransform->Get_State(STATE::POSITION));
    XMStoreFloat4(&qRot, pDesc->pTransform->Get_Rotation_Quat());

    // Shape 생성
    RefConst<Shape> shp = Build_Shape(pDesc->tShape);
    if (!shp) return E_FAIL;

    BodyCreationSettings bs(
        shp,
        ToRVec3(vPos),
        ToQuat(qRot),
        m_eMotion,
        m_iLayer
    );

    // Motion Quality
    bs.mMotionQuality = pDesc->eQuality;

    // Trigger(센서) 처리
    bs.mIsSensor = pDesc->bIsTrigger;

    // Dynamic 질량/관성(필요 시 주석 해제하여 특정 질량/관성 지정)
    if (m_eMotion == EMotionType::Dynamic)
    {
        // 예시) 질량만 오버라이드
         bs.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
         bs.mMassPropertiesOverride.mMass = pDesc->fMass;
    }

    BodyInterface& pBodyInterface = m_pPhysics->GetBodyInterface();
    m_BodyID = pBodyInterface.CreateAndAddBody(bs, pDesc->bStartActive ? EActivation::Activate : EActivation::DontActivate);
    if (m_BodyID.IsInvalid()) return E_FAIL;

    // 머티리얼 반영
    {
        BodyLockWrite lock(m_pPhysics->GetBodyLockInterface(), m_BodyID);
        if (lock.Succeeded())
        {
            Body& b = lock.GetBody();
            Apply_Material(b, m_tMat);
        }
    }

    return S_OK;
}

void CPhysicsBody::Update(_float fTimeDelta, CTransform* pTransform)
{
    if (m_eMotion == EMotionType::Dynamic)
        Fetch_To_Transform(pTransform);
}

void CPhysicsBody::Sync_Kinematic(const _float4x4& matWorld)
{
    if (m_eMotion != EMotionType::Kinematic || m_BodyID.IsInvalid()) return;

    _float3 vPos; _float4 qRot;
    // 행렬 → TR 분해(네 함수 사용)
    DecomposeMatrixTR(matWorld, &vPos, &qRot);

    BodyInterface& bi = m_pPhysics->GetBodyInterface();
    bi.SetPositionAndRotation(m_BodyID, ToRVec3(vPos), ToQuat(qRot), EActivation::Activate);

    m_matWorld = matWorld;
}

void CPhysicsBody::Set_Position(const _float3& vPos)
{
    if (m_BodyID.IsInvalid()) return;
    m_pPhysics->GetBodyInterface().SetPosition(m_BodyID, ToRVec3(vPos), EActivation::Activate);
}

void CPhysicsBody::Set_Rotation(const _float4& qRot)
{
    if (m_BodyID.IsInvalid()) return;
    m_pPhysics->GetBodyInterface().SetRotation(m_BodyID, ToQuat(qRot), EActivation::Activate);
}

void CPhysicsBody::Set_LinearVelocity(const _float3& vVelocity)
{
    if (m_BodyID.IsInvalid()) return;
    m_pPhysics->GetBodyInterface().SetLinearVelocity(m_BodyID, Vec3(vVelocity.x, vVelocity.y, vVelocity.z));
}

void CPhysicsBody::Add_Force(const _float3& fForce)
{
    if (m_BodyID.IsInvalid()) return;
    m_pPhysics->GetBodyInterface().AddForce(m_BodyID, Vec3(fForce.x, fForce.y, fForce.z));
}

void CPhysicsBody::Add_Torque(const _float3& vTorque)
{
    if (m_BodyID.IsInvalid()) return;
    m_pPhysics->GetBodyInterface().AddTorque(m_BodyID, Vec3(vTorque.x, vTorque.y, vTorque.z));
}

RefConst<Shape> CPhysicsBody::Build_Shape(const BODY_SHAPE_DESC& BodyDesc)
{
    switch (BodyDesc.eType)
    {
    case SHAPE::BOX:
        return new BoxShape(Vec3(BodyDesc.vHalfExtents.x, BodyDesc.vHalfExtents.y, BodyDesc.vHalfExtents.z));
    case SHAPE::SPHERE:
        return new SphereShape(BodyDesc.fRadius);
    case SHAPE::CAPSULE:
        // Jolt: CapsuleShape(halfHeight, radius)
        return new CapsuleShape(BodyDesc.fHalfHeight, BodyDesc.fRadius);
    default:
        return nullptr;
    }
}

void CPhysicsBody::Apply_Material(Body& Body, const BODY_MATERIAL_DESC& MaterialDesc)
{
    // Jolt Body는 기본적으로 마찰/반발 세터를 제공
    Body.SetFriction(MaterialDesc.fFriction);
    Body.SetRestitution(MaterialDesc.fRestitution);
}

void CPhysicsBody::Fetch_To_Transform(CTransform* pTransform)
{
    if (m_BodyID.IsInvalid() || !pTransform) return;

    BodyLockRead lock(m_pPhysics->GetBodyLockInterface(), m_BodyID);
    if (!lock.Succeeded()) return;

    const Body& Body = lock.GetBody();
    const RVec3 rvPos = Body.GetPosition();
    const Quat  qRot = Body.GetRotation();

    // Transform 반영
    _float3 vPos = FromRVec3(rvPos);
    pTransform->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
    pTransform->Rotation(qRot.GetX(), qRot.GetY(), qRot.GetZ());
}

CPhysicsBody* CPhysicsBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPhysicsBody* pInstance = new CPhysicsBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPhysicsBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CPhysicsBody::Clone(void* pArg)
{
    CPhysicsBody* pInstance = new CPhysicsBody(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CPhysicsBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CPhysicsBody::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);

    if (m_pPhysics && m_BodyID.IsInvalid() == false)
    {
        BodyInterface& bi = m_pPhysics->GetBodyInterface();
        bi.RemoveBody(m_BodyID);
        bi.DestroyBody(m_BodyID);
        m_BodyID = BodyID();
    }
    m_pPhysics = nullptr;
}
