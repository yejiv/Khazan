#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_BladeNexus_Map final : public CUI_Panel
{
public:
    enum class UIANIMSTATE { ON, OFF, END };
    enum class ONTYPE { HEINMACH, EMBARS, END };

    typedef struct Map_Bubble_Tag : public CUIObject::BUBBLEEVENT
    {
        _int iIndex;
        _bool isClick;
    }BUBBLE_MAP_DESC;


private:
    CUI_BladeNexus_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_BladeNexus_Map(const CUI_BladeNexus_Map& Prototype);
    virtual ~CUI_BladeNexus_Map() = default;

public:
    void								On_Panel(ONTYPE eType);
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
    class CUI_BackGround*                   m_pBackGround = { nullptr };
    vector<class CUI_BladeNexus_Map_List*>  m_pMapList;

    class CBladeNexus_Map_BG*               m_pMapBg = { nullptr };
    class CUI_Default_Tex*                  m_pMapTex = {nullptr};

    UIANIMSTATE							    m_eAnimState = { UIANIMSTATE::END };
    ONTYPE                                  m_eOnType = { ONTYPE::END };

    _float								    m_fAccTime = {};
    _int								    m_iSeleteIndex = {};
    _int                                    m_iMaxSelete = {};

private:
    virtual	HRESULT						Ready_Prototype();
    HRESULT								Ready_Object();

    void								UI_Animation(_float fTimeDelta);
    void								Next_Event();
public:
    static CUI_BladeNexus_Map*          Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void						Free() override;
};

NS_END