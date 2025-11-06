#pragma once
#include "Client_Defines.h"
#include "Sequence_Interface.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CCharacterVirtual;
NS_END

NS_BEGIN(Client)

class CSequence_HeinMach_Yetuga : public ISeqInstance
{
private:
    CSequence_HeinMach_Yetuga(class CCamera_Compre* pCamera, class CCreature* pPlayer);
    virtual ~CSequence_HeinMach_Yetuga() = default;

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

    _bool   m_isFadeIn = { false };
    _bool   m_isPlayerMove = { false };
    _bool   m_isFadeOut = { false };


    _bool   m_isEnd = { false };

    _bool   m_isCameraStart = { false };

    class CCamera_Compre* m_pCamera_Compre = { nullptr };
    class CCreature* m_pPlayer = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

public:
    static CSequence_HeinMach_Yetuga* Create(class CCamera_Compre* pCamera, class CCreature* pPlayer);
    virtual void Free() override;
};

NS_END