#pragma once
#include "Client_Defines.h"
#include "Sequence_Interface.h""

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CCharacterVirtual;
class ISeqInstance;
NS_END

NS_BEGIN(Client)

class CSequence_Yetuga_CutScene : public ISeqInstance
{
private:
    CSequence_Yetuga_CutScene(class CYetuga* pYetuga);
    virtual ~CSequence_Yetuga_CutScene() = default;

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
    void PlayerMove(_float fTimeDelta);

public:
    void Skip_KeyInput(_float fTimeDelta);

private:
    SEQ_ID m_Id{};
    _float  m_fTime{ 0.f };
    _float  m_fSnowTime{ 0.f };
    _float  m_fSkipTime = { 0.f };

    _bool   m_isEnd = { false };

    _bool   m_isCameraAnimation = { false };
    _bool   m_isFadeIn = { false };

    _bool   m_isSnowSmoke = { true };
    _bool   m_isYetugaJump = { false };
    _bool   m_isYetugaLand = { false };
    _bool   m_isYetugaRoar1 = { false };
    _bool   m_isYetugaRoar2 = { false };


    _bool   m_isRoar1Effect = { false };
    _bool   m_isRoar2Effect = { false };
    _bool   m_isDistortion = { false };
    _bool   m_isVignette = { false };
    _bool   m_isRadial = { false };

    _bool   m_isSkip = { false };
    _bool   m_isSkipFadeOut = { false };
    _bool   m_isSkipCameraSet = { false };
    _bool   m_isSkipFadeIn = { false };
    _bool   m_isSkipKey = { false };

    class CYetuga* m_pYetuga = { nullptr };
    class CClientInstance* m_pClientInstance = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };
    class CCamera_Compre* m_pCamera = { nullptr };
    class CSkipButton* m_pSkipButton = { nullptr };

public:
    static CSequence_Yetuga_CutScene* Create(class CYetuga* pYetuga);
    virtual void Free() override;
};

NS_END