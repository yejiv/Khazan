#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Talk_Dangin final : public CUI_Panel
{
private:
    enum class UIANIMSTATE { ON, OFF, END };
    enum class TALK_TYPE { START, TALK_SELETE, TALK, END };
private:
    CUI_Talk_Dangin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_Talk_Dangin(const CUI_Talk_Dangin& Prototype);
    virtual ~CUI_Talk_Dangin() = default;

public:
    void                            On_Panel();
    void                            Off_Panel();
public:
    void                            Update_UITransform(_vector vPos);

public:
    virtual HRESULT			        Initialize_Prototype() override;
    virtual HRESULT			        Initialize_Clone(void* pArg) override;
    virtual void			        Priority_Update(_float fTimeDelta) override;
    virtual void			        Update(_float fTimeDelta) override;
    virtual void			        Late_Update(_float fTimeDelta) override;
    virtual HRESULT			        Render() override;
    virtual	HRESULT				    Update_Switch(void* pArg);
private:
    CShader*                        m_pShaderCom = { nullptr };
    CTexture*                       m_pTextureCom = { nullptr };
    CVIBuffer_Rect*                 m_pVIBufferCom = { nullptr };

    class CUI_WorldTextBox*         m_pName = { nullptr };
    class CUI_WorldTextBox*         m_pText1 = { nullptr };
    class CUI_WorldTextBox*         m_pText2 = { nullptr };
    class CUI_WorldTextBox*         m_pText3 = { nullptr };

    vector<class CUI_WorldTex*>     m_BG_Line;
    vector<class CUI_WorldTex*>     m_Key_Guide;
    vector<class CUI_WorldList*>    m_pList;

    class CUI_WorldTex*             m_pNameBG = { nullptr };

    _int                            m_iSelete = {};
    _int                            m_iTalkIndex = {};
    _int                            m_iMaxTalk = { 3 };
    TALK_TYPE                       m_eType = { TALK_TYPE::END };

    _bool                           m_isUp = {};

    _float							m_fAccTime = {};
    UIANIMSTATE						m_eAnimState = { UIANIMSTATE::END };
    _bool                           m_isUIOpen = {};

    vector<_int>                    m_ItemIndex;

    _float                          m_fSpeedWeight = {};
private:
    HRESULT					        Ready_Component();
    HRESULT                         Ready_Children();
    void                            Ready_Item();

    void                            Update_Selete();
    void                            Setting_Talk();

    void                            List_Selete();
    void						    UI_Animation(_float fTimeDelta);
public:
    static CUI_Talk_Dangin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void			        Free() override;
};

NS_END