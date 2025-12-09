#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CMiniGame_Gacha final : public CGameObject
{
    enum GACHA_STATE { SUCCES_NOTICE, SHUFFLE, SHUFFLE_SET, END};
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
    GACHA_STATE                 m_eState = { GACHA_STATE::END };
    
    _int                        m_iSuffleIndex1 = {};
    _int                        m_iSuffleIndex2 = {};

    _float                      m_fAcctime = {};
    vector<_vector>             m_vSuffleVector1;
    vector<_vector>             m_vSuffleVector2;

    _int                        m_iCulSuffleIndex = {};

    _int                        m_iSuffleCount = {};

    _int                        m_iSpeedCount = {};
    _float                      m_fSpeed = {};
    _int                        m_iSuccesNum = {};
    _int                        m_iSeleteNum = {};

    _float                      m_fGuidePosY = {};
    _int                        m_fGuideCount = {};
private:
    HRESULT                     Ready_Prototype();
    HRESULT                     Setting_Object();

    void                        Setting_Suffle();
    void                        Update_Suffle(_float fTimeDelta);
    void                        Update_Notice(_float fTimeDelta);
    void                        Input_Key();
public:
    static CMiniGame_Gacha*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void                Free() override;
};
NS_END