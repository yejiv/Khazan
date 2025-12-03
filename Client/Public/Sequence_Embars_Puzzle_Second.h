#pragma once
#include "Client_Defines.h"
#include "Sequence_Interface.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CCharacterVirtual;
class ISeqInstance;
NS_END

NS_BEGIN(Client)

class CSequence_Embars_Puzzle_Second : public ISeqInstance
{
private:
    CSequence_Embars_Puzzle_Second();
    virtual ~CSequence_Embars_Puzzle_Second() = default;

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

public:
    void Set_Skip(_bool isSkip) { m_isSkip = true; }

public:
    void Skip_KeyInput(_float fTimeDelta);

private:
    SEQ_ID m_Id{};
    _float  m_fTime{ 0.f };
    _float  m_fSkipTime = { 0.f };

    _bool   m_isElevatorFadeOut = { false };
    _bool   m_isElevatorCamera = { false };
    _bool   m_isVerticalGateFadeOut = { false };
    _bool   m_isVerticalGateCamera = { false };
    _bool   m_isMovePlayerFadeOut = { false };
    _bool   m_isMovePlayer = { false };

    _bool   m_isSkip = { false };
    _bool   m_isSkipFadeOut = { false };
    _bool   m_isSkipCameraSet = { false };
    _bool   m_isSkipFadeIn = { false };
    _bool   m_isSkipKey = { false };

    _bool   m_isEnd = { false };

    class CGameInstance* m_pGameInstance = { nullptr };
    class CClientInstance* m_pClientInstance = { nullptr };
public:
    static CSequence_Embars_Puzzle_Second* Create();
    virtual void Free() override;
};

NS_END