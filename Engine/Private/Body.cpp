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
    pDesc->pCollisionDesc->iObjectLayer = m_iObjectLayer;
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
    BCS.mUserData = static_cast<uint64>(reinterpret_cast<uintptr_t>(pDesc->pCollisionDesc));
    // Dynamic 질량/관성(필요 시 주석 해제하여 특정 질량/관성 지정)
    if (m_eMotion == EMotionType::Dynamic)
    {
        // 예시) 질량만 오버라이드
        BCS.mAngularDamping = pDesc->fAngularDamping;
        BCS.mOverrideMassProperties = EOverrideMassProperties::CalculateMassAndInertia;
        BCS.mMassPropertiesOverride.mMass = pDesc->fMass;
    }

    BCS.mGravityFactor = 0;
    if (pDesc->eShapeType != SHAPE::MESH)
    {
        m_pBody = m_pGameInstance->CreateAndAdd_Body(BCS, &m_pBodyInterface);
        m_pBody->SetCollideKinematicVsNonDynamic(pDesc->isCollideKinematicVsNonDynamic);
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
    if(!m_pBodyInterface->IsActive(m_BodyID))
        m_pBodyInterface->ActivateBody(m_BodyID);

    if (m_pBody->GetMotionType() == EMotionType::Kinematic)
    {
        _vector vScale{}, vRotation{}, vTranslation{};

        XMMatrixDecompose(&vScale, &vRotation, &vTranslation, pTransform->Get_WorldMatrix());

        m_pBodyInterface->MoveKinematic(m_BodyID, LoadVec3(vTranslation), LoadQuat(vRotation), fTimeDelta);

    }

    Vec3 vPos;
    Quat qRotation;
    m_pBodyInterface->GetPositionAndRotation(m_BodyID, vPos, qRotation);

    _vector vQuaternion = XMVectorSet(qRotation.GetX(), qRotation.GetY(), qRotation.GetZ(), qRotation.GetW());
    pTransform->Set_Quaternion(vQuaternion);
    pTransform->Set_State(STATE::POSITION, XMVectorSet(vPos.GetX(), vPos.GetY(), vPos.GetZ(), 1.f));
    
}

void CBody::Sync_Update(CTransform* pTransform)
{
    if (m_pBody->GetMotionType() == EMotionType::Kinematic)
        Set_PosRot(pTransform->Get_State(STATE::POSITION), pTransform->Get_Rotation_Quat());
}

void CBody::Update(_float fTimeDelta, _matrix WorldMatirx, _vector& outQuatRotation, _vector& outPosition)
{
    if (!m_pBodyInterface->IsActive(m_BodyID))
        m_pBodyInterface->ActivateBody(m_BodyID);

    if (m_pBody->GetMotionType() == EMotionType::Kinematic)
    {
        _vector vScale{}, vRotation{}, vTranslation{};

        XMMatrixDecompose(&vScale, &vRotation, &vTranslation, WorldMatirx);

        m_pBodyInterface->MoveKinematic(m_BodyID, LoadVec3(vTranslation), LoadQuat(vRotation), fTimeDelta);

    }

    Vec3 vPos;
    Quat qRotation;
    m_pBodyInterface->GetPositionAndRotation(m_BodyID, vPos, qRotation);

    _vector vQuaternion = XMVectorSet(qRotation.GetX(), qRotation.GetY(), qRotation.GetZ(), qRotation.GetW());

    outQuatRotation = vQuaternion;
    outPosition = XMVectorSet(vPos.GetX(), vPos.GetY(), vPos.GetZ(), 1.f);
}

void CBody::Sync_Update(_matrix WorldMatirx)
{
    _vector vScale, vRotation, vTranslation;

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, WorldMatirx);

    if (m_pBody->GetMotionType() == EMotionType::Kinematic)
        Set_PosRot(vTranslation, vRotation);
}

void CBody::MeshUpdate()
{

}


void CBody::Set_PosRot(_vector vPos, _vector vRot)
{
    m_pBodyInterface->SetPositionAndRotation(m_BodyID, LoadVec3(vPos), LoadQuat(vRot), EActivation::Activate);
}

void CBody::Add_Force(_float fMass)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddForce(m_BodyID, m_pBody->GetLinearVelocity() * fMass);
}

void CBody::Add_Torque(_float fMass)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddTorque(m_BodyID, m_pBody->GetLinearVelocity() * fMass);
}

void CBody::Add_Impulse(_float fMass)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddImpulse(m_BodyID, m_pBody->GetLinearVelocity() * fMass);
}

void CBody::Set_Velocity(const _float3& vVelocity)
{
    m_pBody->SetLinearVelocity(LoadVec3(vVelocity));
}

void CBody::Collision_Active(_bool isActive)
{
    if (isActive)
    {
        if (!m_pBodyInterface->IsAdded(m_BodyID))
        {
            m_pBodyInterface->AddBody(m_BodyID, EActivation::Activate);
        }
    }
    else
    {
        if (m_pBodyInterface->IsAdded(m_BodyID))
        {
            m_pBodyInterface->RemoveBody(m_BodyID);
        }
    }
}

void CBody::Build_Shape(BODY_DESC* pDesc, RefConst<Shape>& pShape)
{
    switch (pDesc->eShapeType)
    {
    case SHAPE::BOX:
    {
        BODY_BOXSHAPE_DESC* pBoxDesc = static_cast<BODY_BOXSHAPE_DESC*>(pDesc);
        pShape = new BoxShape(Vec3(pBoxDesc->vExtent.x, pBoxDesc->vExtent.y, pBoxDesc->vExtent.z));
        pShape = new RotatedTranslatedShape(LoadVec3(pBoxDesc->vShapeOffset), LoadQuat(pBoxDesc->vShapeRotation), pShape);
        break;
    }
    case SHAPE::SPHERE:
    {
        BODY_SPHERESHAPE_DESC* pSphereDesc = static_cast<BODY_SPHERESHAPE_DESC*>(pDesc);
        pShape = new SphereShape(pSphereDesc->fRadius);
        pShape = new RotatedTranslatedShape(LoadVec3(pSphereDesc->vShapeOffset), LoadQuat(pSphereDesc->vShapeRotation), pShape);
        break;
    }
    case SHAPE::CAPSULE:
    {
        BODY_CAPSULESHAPE_DESC* pCapsuleDesc = static_cast<BODY_CAPSULESHAPE_DESC*>(pDesc);
        pShape = new CapsuleShape(pCapsuleDesc->fHeight * 0.5f, pCapsuleDesc->fRadius);
        pShape = new RotatedTranslatedShape(LoadVec3(pCapsuleDesc->vShapeOffset), LoadQuat(pCapsuleDesc->vShapeRotation), pShape);
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

    //for (_uint i = 0; i < iNumMesh; ++i)
    //{
    //    RefConst<Shape> BodyShape;

    //    MeshShapeSettings MeshSetting{};
    //    MeshSetting = MeshShapeSettings(ConvertToArrayFloat3(pDesc->pModel, i), ConvertToArrayTri(pDesc->pModel, i));
    //    BodyShape = MeshSetting.Create().Get();

    //    BodyCreationSettings bodySetting(
    //        BodyShape,
    //        Vec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z),
    //        Quat(pDesc->vQuat.x, pDesc->vQuat.y, pDesc->vQuat.z, pDesc->vQuat.w),
    //        pDesc->eMotion,
    //        ObjectLayer(pDesc->iObjectLayer)
    //    );

    //    bodySetting.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
    //    MassProperties mp;
    //    mp.mMass = 50.0f; // 예: 50kg, 상황에 맞게 조정

    //    bodySetting.mMassPropertiesOverride = mp;

    //    m_pBody = m_pGameInstance->CreateAndAdd_Body(bodySetting, &m_pBodyInterface);
    //    m_BodyID = m_pBody->GetID();
    //}

    iNumMesh = pDesc->pModel->Get_NumMeshes();

    Vec3 vScale = Vec3(pDesc->pTransform->Get_Scaled().x, pDesc->pTransform->Get_Scaled().y, pDesc->pTransform->Get_Scaled().z); // Transform의 스케일

    for (_uint i = 0; i < iNumMesh; ++i)
    {
        // 1) 원본 메쉬로 Shape 생성
        MeshShapeSettings meshSettings{
            ConvertToArrayFloat3(pDesc->pModel, i),
            ConvertToArrayTri(pDesc->pModel, i)
        };
        RefConst<Shape> baseShape = meshSettings.Create().Get();

        // 2) 스케일 적용 (ScaledShape)
        RefConst<Shape> scaledShape = new ScaledShape(baseShape, vScale);

        // (선택) 서브메쉬에 로컬 오프셋/회전이 있다면 RotatedTranslatedShape로 한 번 더 감쌈
        // RMat44 local = ...; // 모델 내부에서 i번째 메쉬의 로컬 변환이 있다면
        // scaledShape = new RotatedTranslatedShape(local.GetTranslation(), Quat::sFromMat44(local.GetRotation()), scaledShape);

        BodyCreationSettings bodySetting(
            scaledShape,
            Vec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z),
            Quat(pDesc->vQuat.x, pDesc->vQuat.y, pDesc->vQuat.z, pDesc->vQuat.w),
            pDesc->eMotion,
            ObjectLayer(pDesc->iObjectLayer)
        );

        // Dynamic으로 삼각형 메쉬를 쓰는 건 비추입니다. (가능하더라도 안정성이 떨어짐)
        // 꼭 써야 한다면 질량/관성 재정의
        bodySetting.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
        MassProperties mp;
        mp.mMass = 50.0f;
        // 관성 텐서는 상황에 맞게 설정해야 합니다. (삼각형 메쉬는 실제 부피가 없어 비현실적일 수 있음)
        bodySetting.mMassPropertiesOverride = mp;
        bodySetting.mUserData = static_cast<uint64>(reinterpret_cast<uintptr_t>(pDesc->pCollisionDesc));

        Body* body = m_pGameInstance->CreateAndAdd_Body(bodySetting, &m_pBodyInterface);
        // 여러 메쉬라면 BodyID를 vector에 보관하세요. 지금처럼 m_pBody에 덮어쓰면 마지막 것만 남습니다.
        //m_BodyIDs.push_back(body->GetID());
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
