#include "ThreadPool.h"

HRESULT CThreadPool::Initialize(_uint thread_count /*=0*/)
{
    if (m_Stop.load()) return E_FAIL;

    if (thread_count == 0) {
        _uint hc = static_cast<_uint>(thread::hardware_concurrency());
        thread_count = (hc == 0) ? 4 : hc;
    }

    try {
        m_Workers.reserve(thread_count);
        for (_uint i = 0; i < thread_count; ++i) {
            m_Workers.emplace_back([this] { WorkerLoop(); });
        }
    }
    catch (...) {
        StopNow();
        return E_FAIL;
    }
    return S_OK;
}

void CThreadPool::WorkerLoop()
{
    for (;;) {
        std::function<void()> task;
        {
            unique_lock<mutex> lock(m_Mtx);
            m_CV.wait(lock, [this] {
                return m_Stop.load(memory_order_acquire) || !m_Tasks.empty();
                });
            if (m_Stop.load(memory_order_acquire) && m_Tasks.empty())
                return;

            task = move(m_Tasks.front());
            m_Tasks.pop();
        }
        task();
    }
}

void CThreadPool::PushJob(function<void()> job)
{
    {
        lock_guard<mutex> lock(m_Mtx);
        if (m_Stop.load(memory_order_acquire))
        {
            throw runtime_error("Enqueue on stopped CThreadPool");
        }
        m_Tasks.emplace(move(job));
    }
    m_CV.notify_one();
}

// 1) 완료만 기다리는 형태
future<void> CThreadPool::Enqueue(function<void()> job)
{
    auto task = make_shared<packaged_task<void()>>(move(job));
    future<void> fut = task->get_future();
    PushJob([task] { (*task)(); });
    return fut;
}

// 2) any 결과 받기
future<any> CThreadPool::EnqueueAny(function<any()> job)
{
    auto task = make_shared<packaged_task<any()>>(move(job));
    future<any> fut = task->get_future();
    PushJob([task] { (*task)(); });
    return fut;
}

// 3) fire-and-forget
void CThreadPool::Submit(function<void()> job)
{
    PushJob(move(job));
}

void CThreadPool::DrainAndStop()
{
    bool expected = false;
    if (m_Stop.compare_exchange_strong(expected, true, memory_order_acq_rel)) {
        m_CV.notify_all();
    }
    else {
        m_CV.notify_all();
    }

    for (auto& t : m_Workers) if (t.joinable()) t.join();
    m_Workers.clear();

    // 남아있을 수 있는 잔여 폐기(안전조치)
    {
        lock_guard<mutex> lock(m_Mtx);
        queue<std::function<void()>> empty;
        swap(m_Tasks, empty);
    }
}

void CThreadPool::StopNow()
{
    m_Stop.store(true, memory_order_release);
    {
        lock_guard<mutex> lock(m_Mtx);
        queue<std::function<void()>> empty;
        swap(m_Tasks, empty);
    }
    m_CV.notify_all();

    for (auto& t : m_Workers) if (t.joinable()) t.join();
    m_Workers.clear();
}

CThreadPool* CThreadPool::Create(_uint thread_count)
{
    CThreadPool* pInstance = new CThreadPool();

    if (FAILED(pInstance->Initialize(thread_count)))
    {
        MSG_BOX(TEXT("Failed to Created : CThreadPool"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CThreadPool::Free()
{
    DrainAndStop();
    __super::Free();
}

