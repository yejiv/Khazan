#pragma once
#include "UI_Panel.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CItemInfo_Weapon final : public CUI_Panel
{
public:
	typedef struct Weapon_Info {
		_int iItemIndex;
		_float2 iOffsetPos;
		_bool isEquip;

        _int iEffect_Type;
        _int iEffect_Value;
	}WEAPONINFO_DESC;

private:
	CItemInfo_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemInfo_Weapon(const CItemInfo_Weapon& Prototype);
	virtual ~CItemInfo_Weapon() = default;

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

	vector<class CUI_Atlas_Icon*> m_pEffectIcon;
	vector<class CUI_TextBox*>	m_pEffectText;
	vector<class CUI_TextBox*>	m_pEffectValue;

	class CUI_Atlas_Icon*		m_pItemIcon = { nullptr };
	class CUI_Atlas_Icon*		m_pItemLine = { nullptr };

	class CUI_Default_Tex*		m_pTopBg = { nullptr };
	class CUI_Default_Tex*		m_pBottomBg = { nullptr };

	class CUI_TextBox*			m_pItemName = { nullptr };
	class CUI_TextBox*			m_pValue = { nullptr };
	class CUI_TextBox*			m_pValueText = { nullptr };
	class CUI_TextBox*			m_pGradeText = { nullptr };
	class CUI_TextBox*			m_pTypeText = { nullptr };
	class CUI_TextBox*			m_pLevelText = { nullptr };
	class CUI_TextBox*			m_pWeightText = { nullptr };
	class CUI_TextBox*			m_pSubTypeText = { nullptr };


	class CUI_Default_Tex*		m_pEquip_Deco = { nullptr };
	class CUI_TextBox*			m_pEquip_Text = { nullptr };

    class CItemInfo_RandomEffect* m_pEffect_Info = { nullptr };
private:
	HRESULT						Ready_Prototype();
	HRESULT						Ready_Componet();

	void						Item_TypeCheck(const EQUIPITEM_DATA* pEffectData);
	void						Item_GradeCheck(const ITEM_DATA* pData);
	void						Item_SubTypeCheck(const EQUIPITEM_DATA* pEffectData);
	void						Item_AmorCheck(const ITEM_DATA* pData, const EQUIPITEM_DATA* pEffectData);
	void						Item_EffectCheck(const EQUIPITEM_DATA* pEffectData);

	void						Mapping_ValueType(_int iType, _float4& pOutUV, _wstring& pOutText);
public:
	static CItemInfo_Weapon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END
