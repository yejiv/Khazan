#include "ThreadPool.h"

CThreadPool::CThreadPool()
{

}

HRESULT CThreadPool::Initialize(_uint thread_count)
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (thread_count == 0) {
        unsigned int hc = std::thread::hardware_concurrency();
        thread_count = (hc == 0) ? 4u : hc;
    }
    m_Workers.reserve(thread_count);
    for (unsigned int i = 0; i < thread_count; ++i) {
        m_Workers.emplace_back([this] { this->Worker_Thread(); });
    }

    return S_OK;
}
future<HRESULT> CThreadPool::Add_Task(std::function<HRESULT()> task)
{
    auto newTask = std::make_shared<std::packaged_task<HRESULT()>>(std::move(task));
    future<HRESULT> future = newTask->get_future();

    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Tasks.push([newTask]() {
            (*newTask)();
            });
    }

    m_CV.notify_one();
    return future;
}


void CThreadPool::PushJob(function<void()> job)
{
    if (m_isStopAll)
    {
        throw runtime_error("ThreadPool »ç¿ë ÁßÁöµÊ");
    }

    {
    lock_guard<mutex> lock(m_Mutex);
    m_Tasks.push(move(job));
    }

    m_CV.notify_one();

}

void CThreadPool::Worker_Thread()
{
    while (true)
    {
        unique_lock<mutex> lock(m_Mutex);
        m_CV.wait(lock, [this] { return m_isStopAll || !m_Tasks.empty(); });
        if (m_isStopAll && m_Tasks.empty())
            return;

        function<void()> task = move(m_Tasks.front());
        m_Tasks.pop();
        lock.unlock();

        task();
    }
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
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_isStopAll = true;
    }

    m_CV.notify_all();

    for (auto& th : m_Workers) {
        if (th.joinable()) th.join();
    }
    m_Workers.clear();

    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        std::queue<std::function<void()>> empty;
        m_Tasks.swap(empty);
    }

    CoUninitialize();

    __super::Free();
}

