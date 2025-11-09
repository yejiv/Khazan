#include "ThreadPool.h"
#include "GameInstance.h"

extern thread_local uint32_t t_worker_idx = 0;

CThreadPool::CThreadPool()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CThreadPool::Initialize(_uint thread_count)
{
    if (thread_count == 0) {
        unsigned int hc = std::thread::hardware_concurrency();
        thread_count = (hc == 0) ? 4u : hc;
    }
    m_Workers.reserve(thread_count);
    for (unsigned int i = 0; i < thread_count; ++i) {
        m_Workers.emplace_back([this, i] { this->Worker_Thread(i); });
    }
    m_iNumWokers = thread_count;
    m_pGameInstance->CreateDeferredContexts(m_iNumWokers);

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

void CThreadPool::Add_FireTask(std::function<HRESULT()> task)
{
	lock_guard<mutex> lock(m_Mutex);
    m_Tasks.push([t = move(task)]() mutable {
        try {
            HRESULT hr = t();
        }
        catch (...) {
            throw runtime_error("ThreadPool 문제있음");
        }
        });

	m_CV.notify_one();
}


void CThreadPool::PushJob(function<void()> job)
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    if (m_isStopAll)
        throw std::runtime_error("ThreadPool 사용 중지됨");

    m_Tasks.push(std::move(job));
    lock.unlock();
    m_CV.notify_one();

}

void CThreadPool::Worker_Thread(uint32_t worker_idx)
{
    t_worker_idx = worker_idx;
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    auto coGuard = std::unique_ptr<void, void(*)(void*)>{ (void*)1, [](void*) { CoUninitialize(); } };

    for (;;) {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_CV.wait(lock, [this] { return m_isStopAll || !m_Tasks.empty(); });

        if (m_isStopAll && m_Tasks.empty())
            break; // return 말고 break

        auto task = std::move(m_Tasks.front());
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

    __super::Free();

    Safe_Release(m_pGameInstance);
}

