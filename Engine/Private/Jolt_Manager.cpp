#include "EnginePch.h"
#include "Jolt_Manager.h"

CJolt_Manager::CJolt_Manager()
{
}

HRESULT CJolt_Manager::Initialize(_uint iMaxBodies, _uint iNumBodyMutexes, _uint iMaxBodyPairs, _uint iMaxContactConstraints, _uint iJobThreadCount)
{

    // Jolt 글로벌 초기화(필수)
    RegisterDefaultAllocator();
    Factory::sInstance = new Factory();
    RegisterTypes();

    m_iMaxBodies = iMaxBodies;
    m_iNumBodyMutexes = (iNumBodyMutexes == 0) ? max(8u, thread::hardware_concurrency()) : iNumBodyMutexes;
    m_iMaxBodyPairs = iMaxBodyPairs;
    m_iMaxContactConstraints = iMaxContactConstraints;

    // Job threads
    m_iJobThreadCount = (iJobThreadCount == 0) ? max(1u, thread::hardware_concurrency() - 1u) : iJobThreadCount;

    // 임시 할당자 (예: 10 MB)
    m_pTempAlloc = new TempAllocatorImpl(10 * 1024 * 1024);

    // 스레드풀
    m_pJobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, m_iJobThreadCount);

    // PhysicsSystem 초기화
    m_Physics.Init(
        m_iMaxBodies,
        m_iNumBodyMutexes,
        m_iMaxBodyPairs,
        m_iMaxContactConstraints,
        m_BPLayerIF,
        m_ObjectVsBPLayerFilter,
        m_ObjectPairFilter
    );

    // 리스너 연결(원한다면)
    m_Physics.SetContactListener(&m_ContactListener);

    // 기본 중력
    m_Physics.SetGravity(Vec3(0.0f, -9.81f, 0.0f));

    return S_OK;
}

void CJolt_Manager::Update(_float fDeltaTime)
{
    // 시뮬레이션
    m_Physics.Update(fDeltaTime, 1, m_pTempAlloc, m_pJobSystem);
}

void CJolt_Manager::Clear()
{
    // 모든 바디 제거
    auto& pBodyInterface = m_Physics.GetBodyInterface();
    // 간단한 방식: 전체 iterate → remove/destroy
    // (대규모 게임이면 BodyManager 이용한 효율 루프 또는 BodyID 캐시 필요)
    // 여기서는 최소 구현: Reset()로 정리
    m_Physics.OptimizeBroadPhase();
}

CJolt_Manager* CJolt_Manager::Create()
{
    CJolt_Manager* pInstance = new CJolt_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CJolt_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CJolt_Manager::Free()
{
    __super::Free();
    // 물리 리소스 정리
    // BodyInterface로 제거할 객체가 있으면 미리 모두 DestroyBody 필요
    // 여기서는 PhysicsSystem가 소멸되며 내부 정리를 수행.

    Safe_Delete(m_pJobSystem);
    Safe_Delete(m_pTempAlloc);

    UnregisterTypes();
    Safe_Delete(Factory::sInstance);
   
}
