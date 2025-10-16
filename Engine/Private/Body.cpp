#include "Body.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"

CBody::CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CRigidBody{ pDevice, pContext }
{
}

CBody::CBody(const CBody& Prototype)
    : CRigidBody{ Prototype }
{
}

HRESULT CBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody::Initialize_Clone(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    RefConst<Shape> BodyShape;

    Build_Shape(pDesc, BodyShape);

    m_eMotion = pDesc->eMotion;
    m_iObjectLayer = pDesc->iObjectLayer;
    BodyCreationSettings BCS(
        BodyShape,
        Vec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z),
        Quat(pDesc->vQuat.x, pDesc->vQuat.y, pDesc->vQuat.z, pDesc->vQuat.w),
        m_eMotion,
        ObjectLayer(m_iObjectLayer)
    );
    // Motion Quality
    BCS.mMotionQuality = pDesc->eQuality;

    // Trigger(센서) 처리
    BCS.mIsSensor = pDesc->bIsTrigger;

    // Dynamic 질량/관성(필요 시 주석 해제하여 특정 질량/관성 지정)
    if (m_eMotion == EMotionType::Dynamic)
    {
        // 예시) 질량만 오버라이드
        BCS.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
        BCS.mMassPropertiesOverride.mMass = pDesc->fMass;
    }

    if (pDesc->eShapeType != SHAPE::MESH)
    {
        m_pBody = m_pGameInstance->CreateAndAdd_Body(BCS, &m_pBodyInterface);
        m_BodyID = m_pBody->GetID();
    }


    //// 머티리얼 반영
    //{
    //    BodyLockWrite lock(m_pPhysics->GetBodyLockInterface(), m_BodyID);
    //    if (lock.Succeeded())
    //    {
    //        Body& b = lock.GetBody();
    //        Apply_Material(b, m_tMat);
    //    }
    //}

    return S_OK;
}

void CBody::Update(_float fTimeDelta, class CTransform* pTransform)
{
    _vector vScale{}, vRotation{}, vTranslation{};

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, pTransform->Get_WorldMatrix());

    m_pBodyInterface->MoveKinematic(m_BodyID, LoadVec3(vTranslation), LoadQuat(vRotation), fTimeDelta);

    Vec3 vPos;
    Quat qRotation;
    m_pBodyInterface->GetPositionAndRotation(m_BodyID, vPos, qRotation);

    _vector vQuaternion = XMVectorSet(qRotation.GetX(), qRotation.GetY(), qRotation.GetZ(), qRotation.GetW());
    pTransform->Set_Quaternion(vQuaternion);
    pTransform->Set_State(STATE::POSITION, XMVectorSet(vPos.GetX(), vPos.GetY(), vPos.GetZ(), 1.f));
}

void CBody::Sync_Update(CTransform* pTransform)
{
    Set_PosRot(pTransform->Get_State(STATE::POSITION), pTransform->Get_Rotation_Quat());
}


void CBody::Set_PosRot(_vector vPos, _vector vRot)
{
    m_pBodyInterface->SetPositionAndRotation(m_BodyID, LoadVec3(vPos), LoadQuat(vRot), EActivation::Activate);
}

void CBody::Add_Force(const _float3& fForce)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddForce(m_BodyID, Vec3(fForce.x, fForce.y, fForce.z));
}

void CBody::Add_Torque(const _float3& vTorque)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddTorque(m_BodyID, Vec3(vTorque.x, vTorque.y, vTorque.z));
}

void CBody::Set_MeshShape(void* pArg)
{

}

void CBody::Build_Shape(BODY_DESC* pDesc, RefConst<Shape>& pShape)
{
    switch (pDesc->eShapeType)
    {
    case SHAPE::BOX:
    {
        BODY_BOXSHAPE_DESC* pBoxDesc = static_cast<BODY_BOXSHAPE_DESC*>(pDesc);
        pShape = new BoxShape(Vec3(pBoxDesc->vExtent.x, pBoxDesc->vExtent.y, pBoxDesc->vExtent.z));
        break;
    }
    case SHAPE::SPHERE:
    {
        BODY_SPHERESHAPE_DESC* pSphereDesc = static_cast<BODY_SPHERESHAPE_DESC*>(pDesc);
        pShape = new SphereShape(pSphereDesc->fRadius);
        break;
    }
    case SHAPE::CAPSULE:
    {
        BODY_CAPSULESHAPE_DESC* pCapsuleDesc = static_cast<BODY_CAPSULESHAPE_DESC*>(pDesc);
        pShape = new CapsuleShape(pCapsuleDesc->fHeight * 0.5f, pCapsuleDesc->fRadius);
        break;
    }

    case SHAPE::MESH:
    {
        BODY_MESHSHAPE_DESC* pMeshDesc = static_cast<BODY_MESHSHAPE_DESC*>(pDesc);
        Make_MeshShape(pMeshDesc);
        break;
    }
    case SHAPE::CONVEX:
    {
        BODY_CONVEXSHAPE_DESC* pConvexHullDesc = static_cast<BODY_CONVEXSHAPE_DESC*>(pDesc);
        if (pConvexHullDesc->pModel == nullptr)
            return;
        Ref<ConvexHullShapeSettings> ConvexHullSetting = new ConvexHullShapeSettings(ConvertToArrayVec3(pConvexHullDesc->pModel));
        pShape = ConvexHullSetting->Create().Get();
        break;
    }
    default:
        break;
    }
}

const JPH::Array<Vec3> CBody::ConvertToArrayVec3(CModel* pModel)
{
    JPH::Array<Vec3> Vertices;

    vector<_float3> ModelVertices = pModel->Get_VerticesPos(0);

    for (size_t i = 0; i < ModelVertices.size(); ++i)
        Vertices.push_back(LoadVec3(ModelVertices[i]));

    return Vertices;
}

const JPH::Array<Float3> CBody::ConvertToArrayFloat3(CModel* pModel, _uint iIndex)
{
    JPH::Array<Float3> Vertices;

    vector<_float3> ModelVertices = pModel->Get_VerticesPos(iIndex);

    for (size_t i = 0; i < ModelVertices.size(); ++i)
        Vertices.push_back(Float3(ModelVertices[i].x, ModelVertices[i].y, ModelVertices[i].z));

    return Vertices;
}

const JPH::Array<IndexedTriangle> CBody::ConvertToArrayTri(CModel* pModel, _uint iIndex)
{
    JPH::Array<IndexedTriangle> Indices;

    vector<_uint> ModelIndices = pModel->Get_Indices(iIndex);

    for (size_t i = 0; i < ModelIndices.size(); i += 3)
        Indices.push_back(IndexedTriangle(ModelIndices[i], ModelIndices[i + 1], ModelIndices[i + 2]));

    return Indices;
}

void CBody::Make_MeshShape(BODY_MESHSHAPE_DESC* pDesc)
{
    _uint iNumMesh = pDesc->pModel->Get_NumMeshes();

    for (_uint i = 0; i < iNumMesh; ++i)
    {
        RefConst<Shape> BodyShape;

        MeshShapeSettings MeshSetting{};
        MeshSetting = MeshShapeSettings(ConvertToArrayFloat3(pDesc->pModel, i), ConvertToArrayTri(pDesc->pModel, i));
        BodyShape = MeshSetting.Create().Get();

        BodyCreationSettings bodySetting(
            BodyShape,
            Vec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z),
            Quat(pDesc->vQuat.x, pDesc->vQuat.y, pDesc->vQuat.z, pDesc->vQuat.w),
            pDesc->eMotion,
            ObjectLayer(pDesc->iObjectLayer)
        );

        bodySetting.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
        MassProperties mp;
        mp.mMass = 50.0f; // 예: 50kg, 상황에 맞게 조정

        bodySetting.mMassPropertiesOverride = mp;


        m_pGameInstance->CreateAndAdd_Body(bodySetting, &m_pBodyInterface);
    }
}

CBody* CBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody* pInstance = new CBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CBody::Clone(void* pArg)
{
    CBody* pInstance = new CBody(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CBody::Free()
{
    __super::Free();

}
