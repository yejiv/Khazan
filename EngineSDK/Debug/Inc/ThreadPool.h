#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CThreadPool final : public CBase
{
private:
    CThreadPool() = default;
    virtual ~CThreadPool() = default;

public:
    HRESULT Initialize(_uint thread_count = 0);
    future<void> Enqueue(std::function<void()> job);
    future<any> EnqueueAny(std::function<any()> job);
    void Submit(std::function<void()> job);

    _uint Size() const { return static_cast<_uint>(m_Workers.size()); }
    void DrainAndStop();
    void StopNow();


private:
    vector<thread>          m_Workers;
    queue<std::function<void()>> m_Tasks;

    mutable mutex                m_Mtx;
    condition_variable           m_CV;
    atomic<bool>                 m_Stop{ false };

public:
    static CThreadPool* Create(_uint thread_count = 0);
    virtual void Free() override;

private:
    void WorkerLoop();

    // ≥ª∫Œ «Ô∆€
    void PushJob(function<void()> job);

};

NS_END