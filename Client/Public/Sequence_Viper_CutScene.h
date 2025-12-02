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

class CSequence_Viper_CutScene : public ISeqInstance
{
private:
    CSequence_Viper_CutScene(class CViper* pViper);
    virtual ~CSequence_Viper_CutScene() = default;

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

    _bool   m_isFadeOut = { false };
    _bool   m_isCameraSet = { false };
    _bool   m_isFadeIn = { false };
    _bool   m_isFirstScene = { false };
    _bool   m_isFirstSceneEnd = { false };
    _bool   m_isSecondCameraSet = { false };
    _bool   m_isSecondScene = { false };
    _bool   m_isSecondSceneEnd = { false };
    _bool   m_isSecondFadeOut = { false };
    _bool   m_isSecondFadeIn = { false };
    _bool   m_isThirdScene = { false };
    _bool   m_isThirdSceneEnd = { false };

    
    _bool   m_isEnd = { false };

    class CGameInstance* m_pGameInstance = { nullptr };
    class CClientInstance* m_pClientInstance = { nullptr };
    class CViper* m_pViper = { nullptr };
    class CCamera_Compre* m_pCamera = { nullptr };
    class CSkipButton* m_pSkipButton = { nullptr };

public:
    static CSequence_Viper_CutScene* Create(class CViper* pViper);
    virtual void Free() override;
};

NS_END