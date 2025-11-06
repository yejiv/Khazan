#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Tutorial final : public CUI_Panel
{
private:
	enum class UIANIMSTATE { ON, OFF, END};

private:
	CUI_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Tutorial(const CUI_Tutorial& Prototype);
	virtual ~CUI_Tutorial() = default;

public:
	void								NextPage();
	void								ReturnPage(); 

	void								On_Panel(GUIDE_TYPE eType);
	void								Off_Panel();
public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;
	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	virtual void						Bubble_EventCall(BUBBLEEVENT* pArg) override;
	virtual	HRESULT						Update_Switch(void* pArg);
private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	CVIBuffer_Rect*						m_pVIBufferCom = { nullptr };

	class CUI_BackGround*				m_pBackGround = { nullptr };
	class CUI_Default_Button*			m_pButtonUp = { nullptr };
	class CUI_Default_Button*			m_pButtonDown = { nullptr };

	class CUI_TextBox*					m_pTitleName = { nullptr };
	class CTutorial_Tex*				m_pTutorialTex = { nullptr };
	class CTutorial_Panel*				m_pTutorialPanel = {nullptr };

	class CUI_Guide_Icon*				m_pIconPageMove = { nullptr };
	class CUI_Guide_Icon*				m_pIconPageExit = { nullptr };

	_int								m_iMaxPage = {1};
	_int								m_iSeletePage = {};

	vector<class CUI_Default_Tex*>		m_pPageIcon;

	_float								m_fAccTime = {};
	UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };
	GUIDE_TYPE							m_eGuideType = { GUIDE_TYPE::END };

	INPUT_TYPE							m_ePreInputType = {};
private:
	HRESULT								Ready_Prototype();
	HRESULT								Ready_Component();
	HRESULT								Ready_Object();

	void								UI_Animation(_float fTimeDelta);

	void								Setting_GuidePagae();
public:
	static CUI_Tutorial*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END