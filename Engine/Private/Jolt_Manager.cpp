#include "Jolt_Manager.h"
#include "GameInstance.h"

CJolt_Manager::CJolt_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}


HRESULT CJolt_Manager::Initialize(_uint iNumObjectLayer)
{
    // Jolt 글로벌 초기화(필수)
    RegisterDefaultAllocator();
    Factory::sInstance = new Factory();
    RegisterTypes();

    m_iNumObjectLayer = iNumObjectLayer;

    // Job threads
    m_iJobThreadCount = thread::hardware_concurrency();

    m_pTempAlloc = new TempAllocatorImpl(16 * 1024 * 1024);
    if (m_pTempAlloc == nullptr)
        return E_FAIL;

    // 스레드풀
    m_pJobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, m_iJobThreadCount);
    if (m_pJobSystem == nullptr)
        return E_FAIL;


    //레이어
    m_pBPLayerIF = new CJolt_BPLayerIF(m_iNumObjectLayer);
    if (m_pBPLayerIF == nullptr)
        return E_FAIL;
    m_pObjectLayerPairFilter = new CJolt_ObjectLayerPairFilter(m_iNumObjectLayer);
    if (m_pObjectLayerPairFilter == nullptr)
        return E_FAIL;
    m_pObjectVsBPLayerFilter = new CJolt_ObjectVsBPLayerFilter(m_iNumObjectLayer);
    if (m_pObjectVsBPLayerFilter == nullptr)
        return E_FAIL;

    m_pObjectLayerFilter = new CJolt_ObjectLayerFilter(m_iNumObjectLayer);
    if (m_pObjectLayerFilter == nullptr)
        return E_FAIL;

    //m_pGroupFilterTable = new GroupFilterTable(64, 64);

#ifdef _DEBUG

    BodyDrawFilter BodyFilter{};
    m_DrawFilter = new DrawOnlyLayers();
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

    m_BodyDescs.emplace(body->GetID(), body->GetUserData());

    return body;
}

CharacterVirtual* CJolt_Manager::CreateCharacterVirtual(const CharacterVirtualSettings* inSettings, RVec3Arg inPosition, QuatArg inRotation, uint64 inUserData, BodyInterface** pBodyInterface)
{
    CharacterVirtual* pCharVir = new CharacterVirtual(inSettings, inPosition, inRotation, inUserData, m_pPhysics);

    *pBodyInterface = &m_pPhysics->GetBodyInterface();

    pCharVir->SetListener(m_pCharContactListener);

    m_pCharVsCharCollision->Add(pCharVir);
    pCharVir->SetCharacterVsCharacterCollision(m_pCharVsCharCollision);

    m_CharacterVirtuals.emplace(pCharVir->GetID(), pCharVir);

    return pCharVir;
}

HRESULT CJolt_Manager::Set_PhysicsSystem()
{
    // PhysicsSystem 초기화
    m_pPhysics = new PhysicsSystem();
    m_pPhysics->Init(
        m_iMaxBodies,
        m_iNumBodyMutexes,
        m_iMaxBodyPairs,
        m_iMaxContactConstraints,
        *m_pBPLayerIF,
        *m_pObjectVsBPLayerFilter,
        *m_pObjectLayerPairFilter
    );

    m_pPhysics->SetPhysicsSettings(m_PhysicsSetting);

    //리스너
    m_pContactListener = new CJolt_ContactListener(&m_pPhysics->GetBodyInterface());
    if (m_pContactListener == nullptr)
        return E_FAIL;

    m_pCharVsCharCollision = new CharacterVsCharacterCollisionSimple();
    if (m_pCharVsCharCollision == nullptr)
        return E_FAIL;

    m_pCharContactListener = new CJolt_CharacterContactListener(&m_pPhysics->GetBodyInterface());
    if (m_pCharContactListener == nullptr)
        return E_FAIL;

    m_pPhysics->SetContactListener(m_pContactListener);
    // 기본 중력
    m_pPhysics->SetGravity(Vec3(0.0f, g_fGravity, 0.0f));

    return S_OK;
}

void CJolt_Manager::Set_Gravity(_vector vGravity)
{
    m_pPhysics->SetGravity(vGravity);
}

void CJolt_Manager::Reset_Gravity()
{
    m_pPhysics->SetGravity(Vec3(0.0f, g_fGravity, 0.0f));
}

void CJolt_Manager::CharVir_Update(_float fTimeDelta, CharacterVirtual* pCharVir, Vec3 vGravity, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter)
{
    pCharVir->Update(
        fTimeDelta,
        vGravity,
        m_pPhysics->GetDefaultBroadPhaseLayerFilter(iObjectLayer),
        m_pPhysics->GetDefaultLayerFilter(iObjectLayer),
        *pBodyFilter,
        *pShapeFilter,
        *m_pTempAlloc
    );
}

void CJolt_Manager::CharVir_ExtendedUpdate(_float fTimeDelta, CharacterVirtual* pCharVir, Vec3 vGravity, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter, CharacterVirtual::ExtendedUpdateSettings tSetting)
{
    pCharVir->ExtendedUpdate(
        fTimeDelta,
        vGravity,
        tSetting,
        m_pPhysics->GetDefaultBroadPhaseLayerFilter(iObjectLayer),
        m_pPhysics->GetDefaultLayerFilter(iObjectLayer),
        *pBodyFilter,
        *pShapeFilter,
        *m_pTempAlloc
    );
}

CharacterVirtual* CJolt_Manager::Find_CharacterVirtual(CharacterID id)
{
    auto iter = m_CharacterVirtuals.find(id);
    if (iter != m_CharacterVirtuals.end())
        return iter->second;
    return nullptr;
}

void CJolt_Manager::Remove_CharacterVirtual(CharacterID id)
{
    auto iter = m_CharacterVirtuals.find(id);
    if (iter != m_CharacterVirtuals.end())
    {
        CharacterVirtual* pChar = iter->second;

        if (m_pCharVsCharCollision)
            m_pCharVsCharCollision->Remove(pChar);

        // BodyDesc도 관리 중이면 같이 제거
        BodyID innerId = pChar->GetInnerBodyID();
        Remove_BodyDesc(innerId);

        pChar->Release();
        m_CharacterVirtuals.erase(iter);
    }
}

void CJolt_Manager::Push_BodyDesc(BodyID id, uint64 BodyDesc)
{
    m_BodyDescs.emplace(id, BodyDesc);
}

uint64 CJolt_Manager::Find_BodyDesc(BodyID id)
{
    auto iter = m_BodyDescs.find(id);
    if (iter != m_BodyDescs.end())
        return iter->second;
    return 0;
}

void CJolt_Manager::Remove_BodyDesc(BodyID id)
{
    auto iter = m_BodyDescs.find(id);
    if (iter != m_BodyDescs.end())
    {
        m_BodyDescs.erase(iter);
    }
}

void CJolt_Manager::Destroy_Body(BodyID& id)
{
    if (!m_pPhysics || id.IsInvalid())
        return;

    BodyInterface& bi = m_pPhysics->GetBodyInterface();
    auto& bli = m_pPhysics->GetBodyLockInterfaceNoLock();

    Body* pBody = bli.TryGetBody(id);
    if (pBody == nullptr)
    {
        id = BodyID();
        return;
    }
    if (bi.IsAdded(id))
        bi.RemoveBody(id);

    bi.DestroyBody(id);

    id = BodyID();

    Remove_BodyDesc(id);
}

_bool CJolt_Manager::RayCast(_float3 vStart, _float3 vEnd, _float& outFraction, _float4& outPosition, _float3* outNormal)
{
    Vec3 origin = LoadVec3(vStart);
    Vec3 dir = LoadVec3(vEnd) - origin;
    if (dir.LengthSq() <= 1e-12f) return false;

    RRayCast ray(origin, dir);

    RayCastSettings rc;
    rc.mBackFaceModeTriangles = EBackFaceMode::CollideWithBackFaces;
    rc.mTreatConvexAsSolid = true;

    ClosestHitCollisionCollector<CastRayCollector> collector;

    m_pPhysics->GetNarrowPhaseQuery().CastRay(
        ray, rc, collector,
        BroadPhaseLayerFilter(),
        *m_pObjectLayerFilter,
        BodyFilter(),
        ShapeFilter()
    );

    if (!collector.HadHit())
        return false;

    const auto& hit = collector.mHit;
    const float fraction = clamp(collector.mHit.mFraction, 0.0f, 1.0f);
    outFraction = fraction;

    _vector vDir = XMVectorSet(dir.GetX(), dir.GetY(), dir.GetZ(), 0.f);
    vDir = XMVector3Normalize(vDir);

    const Vec3 hitPos = ray.GetPointOnRay(fraction);

    outPosition = _float4(hitPos.GetX(), hitPos.GetY(), hitPos.GetZ(), 1.f);

    if (outNormal)
    {
        // Body 읽기 락 후 바디 포인터 획득
        BodyLockRead lock(m_pPhysics->GetBodyLockInterfaceNoLock(), hit.mBodyID);
        if (lock.Succeeded())
        {
            const Body& pBody = lock.GetBody();
            const Vec3 n = pBody.GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, hitPos);
            _float3 N = { n.GetX(), n.GetY(), n.GetZ() };

            // 레이 방향과 같은 방향을 보고 있으면 뒤집어 데칼이 항상 표면 위로 향하게
            Vec3 vDir = dir.Normalized();
            if (vDir.Dot(n) > 0.0f) {
                N.x *= -1.f; N.y *= -1.f; N.z *= -1.f;
            }

            *outNormal = N;
        }
    }

#ifdef _DEBUG
    RayCastDesc RayCastDesc1{};
    RayCastDesc1.vStart = vStart;
    RayCastDesc1.vEnd = vEnd;
    RayCastDesc1.vColor = Color::sGreen;
    m_RayCasts.push_back(RayCastDesc1);
   
    _vector EndPlus;
    EndPlus = XMLoadFloat3(&RayCastDesc1.vEnd) + vDir;
    XMVectorSetW(EndPlus, 1.f);

    RayCastDesc RayCastDesc2{};
    RayCastDesc2.vStart = RayCastDesc1.vEnd;
    XMStoreFloat3(&RayCastDesc2.vEnd, EndPlus);
    RayCastDesc2.vColor = Color::sRed;
    m_RayCasts.push_back(RayCastDesc2);
#endif


    return true;
}

//void CJolt_Manager::Clear()
//{
//    m_CharacterVirtuals.clear();
//    m_BodyDescs.clear();
//    m_RayCasts.clear();
//}

#ifdef  _DEBUG



void CJolt_Manager::Test()
{
    BoxShapeSettings floor_shape_settings(Vec3(10.0f, 1.0f, 10.0f));
    floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

    ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
    ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

    BodyCreationSettings floor_settings(floor_shape, RVec3(Real(0.0), Real(-5.0), Real(0.0)), Quat::sIdentity(), EMotionType::Static, ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    Body* floor = m_pPhysics->GetBodyInterface().CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

    BodyCreationSettings floor_settings2(floor_shape, RVec3(Real(0.0), Real(-5.0), Real(0.0)), Quat::sRotation(Vec3::sAxisX(), DegreesToRadians(45.0f)), EMotionType::Static, ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    Body* floor2 = m_pPhysics->GetBodyInterface().CreateBody(floor_settings2); // Note that if we run out of bodies this can return nullptr
    m_pPhysics->GetBodyInterface().AddBody(floor->GetID(), EActivation::DontActivate);
    m_pPhysics->GetBodyInterface().AddBody(floor2->GetID(), EActivation::DontActivate);

    m_pPhysics->OptimizeBroadPhase();
}
void CJolt_Manager::Debug_Render()
{
    if (!m_pPhysics)
        return;
    // 디버그 렌더 패스 시작
    m_pDebugRenderer->BeginFrame();
    // Jolt가 내부적으로 수백/수천번 DrawLine()을 호출
    m_pPhysics->DrawBodies(m_DrawSetting, m_pDebugRenderer, m_DrawFilter);
    for (RayCastDesc RC : m_RayCasts)
        m_pDebugRenderer->DrawLine(LoadVec3(RC.vStart), LoadVec3(RC.vEnd), RC.vColor);
    // 디버그 렌더 패스 종료
    m_pDebugRenderer->EndFrame();
}
void CJolt_Manager::RayCast_Render_Clear()
{
    m_RayCasts.clear();
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
    for (auto& pair : m_CharacterVirtuals)
    {
        CharacterVirtual* pChar = pair.second;
        if (m_pCharVsCharCollision)
            m_pCharVsCharCollision->Remove(pChar);

        Safe_Delete(pChar);
    }
    m_CharacterVirtuals.clear();
    BodyInterface& bi = m_pPhysics->GetBodyInterface();

    // 등록된 BodyDesc 기준으로 모두 제거
    /*for (auto& pair : m_BodyDescs)
    {
        BodyID id = pair.first;
        if (!id.IsInvalid())
        {
            if (bi.IsAdded(id))
                bi.RemoveBody(id);

            bi.DestroyBody(id);
        }
    }*/
    m_BodyDescs.clear();

#ifdef _DEBUG
    Safe_Delete(m_pDebugRenderer);
    Safe_Delete(m_DrawFilter);
#endif
    Safe_Delete(m_pGroupFilterTable);
    Safe_Delete(m_pBPLayerIF);

    Safe_Delete(m_pObjectLayerPairFilter);
    Safe_Delete(m_pObjectVsBPLayerFilter);
    Safe_Delete(m_pObjectLayerFilter);
    
    Safe_Delete(m_pPhysics);
    Safe_Delete(m_pContactListener);
    Safe_Delete(m_pCharContactListener);
    Safe_Delete(m_pCharVsCharCollision);
    Safe_Delete(m_pJobSystem);
    Safe_Delete(m_pTempAlloc);
    Safe_Delete(Factory::sInstance);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
   
}
