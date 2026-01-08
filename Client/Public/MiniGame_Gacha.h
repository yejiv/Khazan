#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CMiniGame_Gacha final : public CGameObject
{
public:
    enum class MINIGAME_LEVEL {EASY, NORMAL, HARD, END};
    enum GACHA_STATE { SUCCES_NOTICE, SHUFFLE, SHUFFLE_SET, SELETE_NUM, SELECT_END0, SELECT_END1, SELECT_END2, END};
    enum class ANIM_STATE {START, DANCE, END};
public:
    void                        Start_MiniGame(MINIGAME_LEVEL eLevel);
    GACHA_STATE                 Get_GachaState() { return m_eState; }
    _bool                       isSucces() { return m_isSucces; }
public:
    CMiniGame_Gacha(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMiniGame_Gacha(const CMiniGame_Gacha& Prototype);
    virtual ~CMiniGame_Gacha() = default;

public:
    virtual HRESULT             Initialize_Prototype(_int iLevel);
    virtual HRESULT             Initialize_Clone(void* pArg);
    virtual void                Priority_Update(_float fTimeDelta);
    virtual void                Update(_float fTimeDelta);
    virtual void                Late_Update(_float fTimeDelta);
    
private:
    vector<class CRandomBox*>   m_pBox;
    class CUI_Gacha_Selete*     m_pSeleteUI = {nullptr};
    class CMinigame_Mirrorball* m_pMirrorball = { nullptr };
    GACHA_STATE                 m_eState = { GACHA_STATE::END };
    MINIGAME_LEVEL              m_eMiniGameLevel = { MINIGAME_LEVEL::END };
    _int                        m_iSuffleIndex1 = {};
    _int                        m_iSuffleIndex2 = {};

    _float                      m_fAcctime = {};
    vector<_vector>             m_vSuffleVector1;
    vector<_vector>             m_vSuffleVector2;

    _int                        m_iCulSuffleIndex = {};

    _int                        m_iSuffleCount = {};

    _int                        m_iSpeedCount = {};
    _float                      m_fSpeed = {};
    _float                      m_fAddSpeed = {};
    _int                        m_fSpeedCount = {};
    _int                        m_iSuccesNum = {};
    _int                        m_iSeleteNum = {};

    _float                      m_fGuidePosY = {};
    _int                        m_fGuideCount = {};

    _bool                       m_isSucces = {};

    _int                        m_isHardCount = {};
    _float                      m_fLightTime = {};
    _bool                       m_isHardLight = {};

    _float                      m_fEffectTime = {};

    _bool                       m_isItemEffect = {};
    //애니메이션 관련
    ANIM_STATE                  m_eEndAnime = { ANIM_STATE ::END};
    _bool                       m_isGet_Pet = { false };
private:
    HRESULT                     Ready_Prototype();
    HRESULT                     Setting_Object();

    void                        Update_Notice(_float fTimeDelta);
    void                        Setting_Suffle();
    void                        Update_Suffle(_float fTimeDelta);
    void                        Update_Selete(_float fTimeDelta);
    void                        Update_Selete_End0(_float fTimeDelta);
    void                        Update_Selete_End1(_float fTimeDelta);
    void                        Update_Selete_End2(_float fTimeDelta);
public:
    static CMiniGame_Gacha*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void                Free() override;
};
NS_END