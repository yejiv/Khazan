#include "Jolt_Manager.h"


CJolt_Manager::CJolt_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }, m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}


HRESULT CJolt_Manager::Initialize(_uint iNumObjectLayer)
{
    // Jolt 글로벌 초기화(필수)
    RegisterDefaultAllocator();
    Factory::sInstance = new Factory();
    RegisterTypes();

    // Job threads
    m_iJobThreadCount = thread::hardware_concurrency();

    m_pTempAlloc = new TempAllocatorImpl(64 * 1024 * 1024);
    if (m_pTempAlloc == nullptr)
        return E_FAIL;

    // 스레드풀
    m_pJobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, m_iJobThreadCount);
    if (m_pJobSystem == nullptr)
        return E_FAIL;


    //레이어
      m_pBPLayer = new CJolt_BPLayer(iNumObjectLayer);
    if (m_pBPLayer == nullptr)
        return E_FAIL;
    m_pObjectLayerPairFilter = new CJolt_ObjectLayerPairFilter(iNumObjectLayer);
    if (m_pObjectLayerPairFilter == nullptr)
        return E_FAIL;
    m_pObjectVsBPLayerFilter = new CJolt_ObjectVsBPLayerFilter(iNumObjectLayer);
    if (m_pObjectVsBPLayerFilter == nullptr)
        return E_FAIL;

    //리스너
    m_pContactListener = new CJolt_ContactListener();
    if (m_pContactListener == nullptr)
        return E_FAIL;

#ifdef _DEBUG
    m_pDebugRenderer = new CJolt_DebugRenderer(m_pDevice, m_pContext);

#endif

    return S_OK;
}

void CJolt_Manager::Update(_float fDeltaTime)
{
    if (!m_pPhysics)
        return;

    m_pPhysics->Update(fDeltaTime, 1, m_pTempAlloc, m_pJobSystem);
}

Body* CJolt_Manager::CreateAndAdd_Body(const BodyCreationSettings& BodySetting, BodyInterface** pBodyInterface)
{
    Body* body = m_pPhysics->GetBodyInterface().CreateBody(BodySetting);
    if (body == nullptr)
        return nullptr;

    m_pPhysics->GetBodyInterface().AddBody(body->GetID(), EActivation::Activate);

    *pBodyInterface = &m_pPhysics->GetBodyInterface();

    return body;
}

void CJolt_Manager::Set_PhysicsSystem()
{
    // PhysicsSystem 초기화
    m_pPhysics = new PhysicsSystem();
    m_pPhysics->Init(
        m_iMaxBodies,
        m_iNumBodyMutexes,
        m_iMaxBodyPairs,
        m_iMaxContactConstraints,
        *m_pBPLayer,
        *m_pObjectVsBPLayerFilter,
        *m_pObjectLayerPairFilter
    );

    m_pPhysics->SetPhysicsSettings(m_PhysicsSetting);
    m_pPhysics->SetContactListener(m_pContactListener);

    // 기본 중력
    m_pPhysics->SetGravity(Vec3(0.0f, -9.81f, 0.0f));
}

#ifdef  _DEBUG
void CJolt_Manager::Test()
{
    BoxShapeSettings floor_shape_settings(Vec3(10.0f, 1.0f, 10.0f));
    floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

    ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
    ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

    BodyCreationSettings floor_settings(floor_shape, RVec3(Real(0.0), Real(-1.0), Real(0.0)), Quat::sIdentity(), EMotionType::Static, ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    Body* floor = m_pPhysics->GetBodyInterface().CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

    m_pPhysics->GetBodyInterface().AddBody(floor->GetID(), EActivation::DontActivate);

    for (_uint i = 0; i < 5; i++)
    {
        BodyCreationSettings sphere_settings(new SphereShape(0.5f), RVec3(Real(0.1), Real(i * 5), Real(0.0)), Quat::sIdentity(), EMotionType::Dynamic, ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
        BodyID sphere_id = m_pPhysics->GetBodyInterface().CreateAndAddBody(sphere_settings, EActivation::Activate);

        m_pPhysics->GetBodyInterface().SetLinearVelocity(sphere_id, Vec3(0.0f, 0.0f, 0.0f));
    }

    m_pPhysics->OptimizeBroadPhase();
}
void CJolt_Manager::Debug_Render()
{
    if (!m_pPhysics)
        return;

    m_pPhysics->DrawBodies(m_DrawSetting, m_pDebugRenderer);
}
#endif 
CJolt_Manager* CJolt_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumObjectLayer)
{
    CJolt_Manager* pInstance = new CJolt_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iNumObjectLayer)))
    {
        MSG_BOX(TEXT("Failed to Created : CJolt_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CJolt_Manager::Free()
{
    __super::Free();
#ifdef _DEBUG
    Safe_Delete(m_pDebugRenderer);
#endif

    Safe_Delete(m_pBPLayer);
    Safe_Delete(m_pObjectLayerPairFilter);
    Safe_Delete(m_pObjectVsBPLayerFilter);

    Safe_Delete(m_pPhysics);
    Safe_Delete(m_pContactListener);
    Safe_Delete(m_pJobSystem);
    Safe_Delete(m_pTempAlloc);
    Safe_Delete(Factory::sInstance);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
   
}
