#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CThreadPool final : public CBase
{


    //auto f = pool->Enqueue([] {
    //    // heavy work
    //    });
    //f.get();

    //pool->Submit([] {
    //    // fire-and-forget
    //    });

    //pool->Free();
private:
    CThreadPool() = default;
    virtual ~CThreadPool() = default;

public:
    // thread_count==0 → hardware_concurrency() 사용
    HRESULT Initialize(_uint thread_count = 0);

    // 1) 완료 대기만 필요할 때
    future<void> Enqueue(std::function<void()> job);

    // 2) 결과가 필요할 때 (any로 통일)
    future<any> EnqueueAny(std::function<any()> job);

    // 3) fire-and-forget
    void Submit(std::function<void()> job);

    // 제어
    _uint Size() const { return static_cast<_uint>(m_Workers.size()); }
    void DrainAndStop();  // 남은 작업 처리 후 정지
    void StopNow();       // 큐 비우고 즉시 정지

public:
    static CThreadPool* Create(_uint thread_count = 0);
    virtual void Free() override;

private:
    void WorkerLoop();

    // 내부 헬퍼
    void PushJob(function<void()> job);

private:
    vector<thread>          m_Workers;
    queue<std::function<void()>> m_Tasks;

    mutable mutex                m_Mtx;
    condition_variable           m_CV;
    atomic<bool>                 m_Stop{ false };
};

NS_END