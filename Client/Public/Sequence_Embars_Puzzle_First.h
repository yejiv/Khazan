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

class CSequence_Embars_Puzzle_First : public ISeqInstance
{
private:
    CSequence_Embars_Puzzle_First();
    virtual ~CSequence_Embars_Puzzle_First() = default;

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
    void PlayerMove(_float fTimeDelta);

private:
    SEQ_ID m_Id{};
    _float  m_fTime{ 0.f };
    _bool   m_isElevatorFadeOut = { false };
    _bool   m_isElevatorCamera = { false };
    _bool   m_isVerticalGateFadeOut = { false };
    _bool   m_isVerticalGateCamera = { false };
    _bool   m_isMovePlayerFadeOut = { false };
    _bool   m_isMovePlayer = { false };




    _bool   m_isEnd = { false };

    class CGameInstance* m_pGameInstance = { nullptr };
    class CClientInstance* m_pClientInstance = { nullptr };
public:
    static CSequence_Embars_Puzzle_First* Create();
    virtual void Free() override;
};

NS_END