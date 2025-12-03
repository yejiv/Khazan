#pragma once
#include "Client_Defines.h"
#include "Sequence_Interface.h"

NS_BEGIN(Client)

class CSequence_Dummy : public ISeqInstance
{
private:
    CSequence_Dummy();
    virtual ~CSequence_Dummy() = default;

public:
    HRESULT Initialize(const SEQ_REQ_PLAY_DESC& tDesc) override;
    void    Update(_float fTimeDelta) override;

public:
    void    Pause() override;
    void    Resume() override;
    void    StopImmediate() override;
    void    Jump(_float fTime) override;
    _bool   IsEnd() const override;
    SEQ_ID  GetId() const override { return m_Id; }
    
private:
    enum class STATE { Idle, Playing, Paused, End };
    STATE  m_State{ STATE::Idle };
    SEQ_ID m_Id{};
    _float  m_Time{ 0.f };

public:
    static CSequence_Dummy* Create();
    virtual void Free() override;
};

NS_END