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

class CSequence_Viper_SecondPhase : public ISeqInstance
{
private:
    CSequence_Viper_SecondPhase(class CViper* pViper, class CKhazan_GSword* pKhazan);
    virtual ~CSequence_Viper_SecondPhase() = default;

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
    void Push_Obelisk(class CObelisk* pObelisk);

public:
    void Set_Skip(_bool isSkip) { m_isSkip = true; }

public:
    void Skip_KeyInput(_float fTimeDelta);

private:
    SEQ_ID m_Id{};
    _float  m_fTime{ 0.f };
    _float  m_fSkipTime = { 0.f };

    _bool m_isFadeOut1 = { false };
    _bool m_isFadeIn1 = { false };
    _bool m_isFadeOut2 = { false };
    _bool m_isFadeIn2 = { false };
    _bool m_isFadeOut3 = { false };
    _bool m_isFadeIn3 = { false };
    _bool m_isFadeOut4 = { false };
    _bool m_isFadeIn4 = { false };
    _bool m_isFadeOut5 = { false };
    _bool m_isFadeIn5 = { false };
    _bool m_isFadeOut6 = { false };
    _bool m_isFadeIn6 = { false };
    _bool m_isFadeOut7 = { false };
    _bool m_isFadeIn7 = { false };
    _bool m_isFadeOut8 = { false };
    _bool m_isFadeIn8 = { false };

    _bool m_isScene1 = { false };
    _bool m_isCameraSet1 = { false };
    _bool m_isScene2 = { false };
    _bool m_isCameraSet2 = { false };
    _bool m_isScene3 = { false };
    _bool m_isCameraSet3 = { false };
    _bool m_isScene4 = { false };
    _bool m_isCameraSet4 = { false };
    _bool m_isScene5 = { false };
    _bool m_isCameraSet5 = { false };
    _bool m_isScene6 = { false };
    _bool m_isCameraSet6 = { false };
    _bool m_isScene7 = { false };
    _bool m_isCameraSet7 = { false };
    _bool m_isScene8 = { false };
    _bool m_isCameraSet8 = { false };
    _bool m_isScene9 = { false };
    _bool m_isCameraSet9 = { false };
    _bool m_isScene10 = { false };
    _bool m_isCameraSet10 = { false };
    _bool m_isScene11 = { false };
    _bool m_isCameraSet11 = { false };
    _bool m_isScene12 = { false };
    _bool m_isCameraSet12 = { false };











    queue<class CObelisk*> m_Obelisks;

    _bool   m_isSkip = { false };
    _bool   m_isSkipFadeOut = { false };
    _bool   m_isSkipCameraSet = { false };
    _bool   m_isSkipFadeIn = { false };
    _bool   m_isSkipKey = { false };

    _bool   m_isEnd = { false };

    class CGameInstance* m_pGameInstance = { nullptr };
    class CClientInstance* m_pClientInstance = { nullptr };
    class CViper* m_pViper = { nullptr };
    class CCamera_Compre* m_pCamera = { nullptr };
    class CSkipButton* m_pSkipButton = { nullptr };
    class CKhazan_GSword* m_pKhazan = { nullptr };

public:
    static CSequence_Viper_SecondPhase* Create(class CViper* pViper, class CKhazan_GSword* pKhazan);
    virtual void Free() override;
};

NS_END