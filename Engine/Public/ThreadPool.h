#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CThreadPool final : public CBase
{
private:
    CThreadPool();
    ~CThreadPool() = default;

public:
    using Job = std::function<void()>;

public:
    HRESULT Initialize(_uint thread_count = 0);
    future<HRESULT> Add_Task(std::function<HRESULT()> task);
    void Add_FireTask(std::function<HRESULT()> task);
    void PushJob(function<void()> job);
    _uint Get_ThreadCount() const { 
        return m_iNumWokers;
    }

private:
    void Worker_Thread(uint32_t worker_idx);

private:
    _uint                   m_iNumWokers;
    vector<thread>          m_Workers;
    queue<std::function<void()>> m_Tasks;

    mutex                       m_Mutex;
    condition_variable          m_CV;
    _bool                       m_isStopAll = {};

    class CGameInstance* m_pGameInstance = { nullptr };

public:
    static CThreadPool* Create(_uint thread_count = 0);
    virtual void Free() override;


};

NS_END