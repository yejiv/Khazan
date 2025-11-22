#pragma once
#include "Client_Defines.h"
#include "Sequence_Interface.h"

NS_BEGIN(Engine)
class CGameInstance;
class ISeqInstance;
NS_END

NS_BEGIN(Client)

class CSequence_HeinMach_Start : public ISeqInstance
{
private:
    CSequence_HeinMach_Start();
    virtual ~CSequence_HeinMach_Start() = default;

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
    SEQ_ID m_Id{};
    _float  m_fTime{ 0.f };

    _bool m_isCamera = { false };
    _bool m_isFadeIn = { false };

    _bool m_isEnd = { false };

    class CClientInstance* m_pClientInstance = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

public:
    static CSequence_HeinMach_Start* Create();
    virtual void Free() override;
};

NS_END