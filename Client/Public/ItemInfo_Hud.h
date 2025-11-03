#pragma once
#include "UI_Panel.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CItemInfo_Hud final : public CUI_Panel
{
public:
	enum class UIAnim{ON, OFF, END};
private:
	CItemInfo_Hud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemInfo_Hud(const CItemInfo_Hud& Prototype);
	virtual ~CItemInfo_Hud() = default;

public:
	void						Add_Item(_int iItemIndex);
	void						Update_Pos(_int iIndex, _int iMaxIndex, _float fOffsetY);

public:
	virtual HRESULT				Initialize_Prototype(_uint iLevel);
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };

	class CUI_Atlas_Icon*		m_pItemIcon = { nullptr };
	class CUI_TextBox*			m_pItemText = { nullptr };

	_float						m_fAccTime = {};
	UIAnim						m_eAnimState = { UIAnim::END};
private:
	HRESULT						Ready_Prototype();
	HRESULT						Ready_Componet();

public:
	static CItemInfo_Hud*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END	
