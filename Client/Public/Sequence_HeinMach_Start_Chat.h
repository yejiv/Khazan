#pragma once
#include "Client_Defines.h"
#include "Sequence_Interface.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CSequence_HeinMach_Start_Chat : public ISeqInstance
{
private:
    CSequence_HeinMach_Start_Chat();
    virtual ~CSequence_HeinMach_Start_Chat() = default;

public:
    HRESULT Initialize(const SEQ_REQ_PLAY_DESC& tDesc) override;
    void    Update(_float fTimeDelta) override;

public:
    void    Pause() override;
    void    Resume() override;
    void    StopImmediate() override;
    void    Jump(_float fTime) override;
    _bool   IsEnd() const override;
    void    Event_Announce_Talk(_int iIndex);
    SEQ_ID  GetId() const override { return m_Id; }

private:
    SEQ_ID m_Id{};
    _float  m_fTime{ 0.f };

    _bool m_isTalk3 = { false };
    _bool m_isTalk4 = { false };
    _bool m_isTalk5 = { false };
    _bool m_isTalk6 = { false };
    _bool m_isTalk7 = { false };
    _bool m_isTalk8 = { false };
    _bool m_isTalk9 = { false };
    _bool m_isEnd = { false };

    class CGameInstance* m_pGameInstance = { nullptr };

public:
    static CSequence_HeinMach_Start_Chat* Create();
    virtual void Free() override;
};

NS_END