#pragma once
#include "UI_Tap.h"
#include "Client_Defines.h"
#include "UI_BladeNexus.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CBladeNexus_List final : public CUI_Tap
{
public:
	typedef struct tagMainMenu_ListDesc : public CUIObject::UIOBJECT_DESC
	{
		CUI_BladeNexus::MENULIST eMenu = {};

	}MAINMENULIST_DESC;
private:
	CBladeNexus_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBladeNexus_List(const CBladeNexus_List& Prototype);
	virtual ~CBladeNexus_List() = default;

public:
	void								Update_Pos(_int iIndex, _float2 vPos, _float fOffSetY);
	void								OnAnime(_float fAccTime, _float fOffsetX, CUIObject* pParent);
	void								Set_Selete(_bool isSelete);
public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
private:
	CUI_BladeNexus::MENULIST				m_eMenuType = {};
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	class CUI_TextBox* m_pTextBox = { nullptr };
	class CMainMune_Deco* m_pDeco = { nullptr };

	_bool								m_bIsSelete = { false };
	_float								m_fAccTime = { 1.f };
private:
	HRESULT								Ready_Componet();
public:
	static CBladeNexus_List* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END