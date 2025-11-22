#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_Store final : public CUI_Panel
{
public:
    enum class UIANIMSTATE { ON, OFF, END };
    typedef struct tagStoreOpenTag
    {
        string szName;
        _bool isOpen;
        vector<_int> ItemIndex;
    }STOER_DESC;

private:
    CUI_Store(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Store(const CUI_Store& Prototype);
    virtual ~CUI_Store() = default;

public:
    void								On_Panel();
    void								Off_Panel();

public:
    virtual HRESULT						Initialize_Prototype(_uint iLevel);
    virtual HRESULT						Initialize_Clone(void* pArg) override;
    virtual void						Priority_Update(_float fTimeDelta) override;
    virtual void						Update(_float fTimeDelta) override;
    virtual void						Late_Update(_float fTimeDelta) override;

    virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
    virtual void						Bubble_EventCall(BUBBLEEVENT* pArg) override;
    virtual	HRESULT						Update_Switch(void* pArg);

private:
    class CUI_BackGround*               m_pBackGround = { nullptr };

    UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };
    _float								m_fAccTime = {};
    string								m_strReturnName = {};

    vector<class CStore_List*>          m_StoreList;

    _int                                m_iSeleteIndex = {};
    _int                                m_iMaxSeleteIndex = {};
private:
    virtual	HRESULT						Ready_Prototype();
    HRESULT								Ready_Object();

    void								UI_Animation(_float fTimeDelta);
    void                                Update_Selete();
public:
    static CUI_Store*                   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void						Free() override;
};

NS_END