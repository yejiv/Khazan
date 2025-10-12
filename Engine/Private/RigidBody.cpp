#include "RigidBody.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CRigidBody::CRigidBody(const CRigidBody& Prototype)
    : CComponent{ Prototype }
{
}

HRESULT CRigidBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRigidBody::Initialize_Clone(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    RIGIDBODY_DESC* pDesc = static_cast<RIGIDBODY_DESC*>(pArg);

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

    m_pBody = m_pGameInstance->CreateAndAdd_Body(BCS, &m_pBodyInterface);
    m_BodyID = m_pBody->GetID();

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

void CRigidBody::Update(_float fTimeDelta, const _matrix& matWorld)
{
    _vector vScale{}, vRotation{}, vTranslation{};

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, matWorld);

    m_pBodyInterface->MoveKinematic(m_BodyID, LoadVec3(vTranslation), LoadQuat(vRotation), fTimeDelta);
}

void CRigidBody::Sync_Update(CTransform* pTransform)
{
    Vec3 vPos;
    Quat vRotation;
    m_pBodyInterface->GetPositionAndRotation(m_BodyID, vPos, vRotation);

    _vector vQuaternion = XMVectorSet(vRotation.GetX(), vRotation.GetY(), vRotation.GetZ(), vRotation.GetW());
    pTransform->Set_Quaternion(vQuaternion);
    pTransform->Set_State(STATE::POSITION, XMVectorSet(vPos.GetX(), vPos.GetY(), vPos.GetZ(), 1.f));
}


void CRigidBody::Add_Force(const _float3& fForce)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddForce(m_BodyID, Vec3(fForce.x, fForce.y, fForce.z));
}

void CRigidBody::Add_Torque(const _float3& vTorque)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddTorque(m_BodyID, Vec3(vTorque.x, vTorque.y, vTorque.z));
}

void CRigidBody::Set_MeshShape(void* pArg)
{

}

void CRigidBody::Build_Shape(RIGIDBODY_DESC* pDesc, RefConst<Shape>& pShape)
{
    switch (pDesc->eShapeType)
    {
    case SHAPE::BOX:
    {
        RIGID_BOXSHAPE_DESC* pBoxDesc = static_cast<RIGID_BOXSHAPE_DESC*>(pDesc);
        pShape = new BoxShape(Vec3(pBoxDesc->vExtent.x, pBoxDesc->vExtent.y, pBoxDesc->vExtent.z));
        break;
    }
    case SHAPE::SPHERE:
    {
        RIGID_SPHERESHAPE_DESC* pSphereDesc = static_cast<RIGID_SPHERESHAPE_DESC*>(pDesc);
        pShape = new SphereShape(pSphereDesc->fRadius);
        break;
    }
    case SHAPE::CAPSULE:
    {
        RIGID_CAPSULESHAPE_DESC* pCapsuleDesc = static_cast<RIGID_CAPSULESHAPE_DESC*>(pDesc);
        pShape = new CapsuleShape(pCapsuleDesc->fHeight * 0.5f, pCapsuleDesc->fRadius);
        break;
    }

    case SHAPE::MESH:
    {
        //Make_MeshShape(pArg);
        //return S_OK;
        break;
    }
    case SHAPE::CONVEX:
    {
       /* CONVEXSHAPE_DESC* pConvexHullDesc = static_cast<CONVEXSHAPE_DESC*>(pDesc);
        if (pConvexHullDesc->pModel == nullptr)
            return E_FAIL;*/
        /*Ref<ConvexHullShapeSettings> ConvexHullSetting = new ConvexHullShapeSettings(ConvertToArrayVec3(pConvexHullDesc->pModel));
        BodyShape = ConvexHullSetting->Create().Get();*/
        break;
    }
    default:
        break;
        //return E_FAIL;
    }
}

//const JPH::Array<Vec3> CRigidBody::ModelVec3(CModel* pModel)
//{
//    //JPH::Array<Vec3> Vertices;
//
//    //vector<_float3> ModelVertices = pModel->Get_VerticesPos(0);
//
//    //for (size_t i = 0; i < ModelVertices.size(); ++i)
//    //    Vertices.push_back(LoadVec3(ModelVertices[i]));
//
//    //return Vertices;
//}
//
//const JPH::Array<Float3> CRigidBody::ModelFloat3(CModel* pModel, _uint iIndex)
//{
//    //JPH::Array<IndexedTriangle> Indices;
//
//    //vector<_uint> ModelIndices = pModel->Get_Indices(iIndex);
//
//    //for (size_t i = 0; i < ModelIndices.size(); i += 3)
//    //    Indices.push_back(IndexedTriangle(ModelIndices[i], ModelIndices[i + 1], ModelIndices[i + 2]));
//
//    //return Indices;
//}
//
//const JPH::Array<IndexedTriangle> CRigidBody::ModelTri(CModel* pModel, _uint iIndex)
//{
//    //MESHBODY_DESC* pDesc = static_cast<MESHBODY_DESC*>(pArg);
//
//    //_uint iNumMesh = pDesc->pModel->Get_NumMesh();
//
//    //for (_uint i = 0; i < iNumMesh; ++i)
//    //{
//    //    RefConst<Shape> BodyShape;
//
//    //    Ref<MeshShapeSettings> MeshSetting;
//    //    MeshSetting = new MeshShapeSettings(ConvertToArrayFloat3(pDesc->pModel, i), ConvertToArrayTri(pDesc->pModel, i));
//    //    BodyShape = MeshSetting->Create().Get();
//
//    //    BodyCreationSettings bodySetting(
//    //        BodyShape,																					// Shape
//    //        Vec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z),								// Position
//    //        Quat(pDesc->vQuat.x, pDesc->vQuat.y, pDesc->vQuat.z, pDesc->vQuat.w),	// Quat
//    //        pDesc->eType,																				// Motion Type
//    //        ObjectLayer(pDesc->iLayer)																// Collision Layer
//    //    );
//
//    //    ASSERT_CRASH(m_pGameInstance->Register_Body(bodySetting, &m_pBodyInterface));
//    //}
//    return JPH::Array<IndexedTriangle>;
//}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRigidBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CRigidBody::Clone(void* pArg)
{
    CRigidBody* pInstance = new CRigidBody(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRigidBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CRigidBody::Free()
{
    __super::Free();

}
