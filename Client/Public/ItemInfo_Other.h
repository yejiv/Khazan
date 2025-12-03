#pragma once
#include "UI_Panel.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CItemInfo_Other final : public CUI_Panel
{
public:
	typedef struct Other_Info {
		_int iItemIndex;
		_float2 iOffsetPos;
		_bool isEquip;
		_int iMaxItem;
		_int iCurItem;
	}OTHERINFO_DESC;
private:
	CItemInfo_Other(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemInfo_Other(const CItemInfo_Other& Prototype);
	virtual ~CItemInfo_Other() = default;

public:
	virtual HRESULT				Initialize_Prototype(_uint iLevel);
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	virtual	HRESULT				Update_Switch(void* pArg);
private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };

	_int						m_iItemIndex = { -1 };

	class CUI_Atlas_Icon*		m_pItemIcon = { nullptr };
	class CUI_Atlas_Icon*		m_pInfoLine = { nullptr };
	class CUI_Atlas_Icon*		m_pItemLine = { nullptr };

	class CUI_TextBox*			m_pItemName = { nullptr };
	class CUI_TextBox*			m_pItemCountValue = { nullptr };
	class CUI_TextBox*			m_pInfoText = { nullptr };
	class CUI_TextBox*			m_pSubInfoText = { nullptr };
	class CUI_TextBox*			m_pGradeText = { nullptr };
	class CUI_TextBox*			m_pTypeText = { nullptr };


	class CUI_Default_Tex*		m_pTopBg = { nullptr };
	class CUI_Default_Tex*		m_pBottomBg = { nullptr };

	class CUI_Default_Tex*		m_pEquip_Deco = { nullptr };
	class CUI_TextBox*			m_pEquip_Text = { nullptr };


private:
	HRESULT						Ready_Prototype();
	HRESULT						Ready_Componet();

	void						Item_GradeCheck(const ITEM_DATA* pData);
public:
	static CItemInfo_Other*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END
