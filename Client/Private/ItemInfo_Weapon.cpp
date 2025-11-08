#include "ItemInfo_Weapon.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"
#include "UI_Default_Tex.h"

CItemInfo_Weapon::CItemInfo_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CItemInfo_Weapon::CItemInfo_Weapon(const CItemInfo_Weapon& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CItemInfo_Weapon::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemInfo_Weapon::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	CHECK_FAILED(Ready_Componet(), E_FAIL);
	return S_OK;
}

void CItemInfo_Weapon::Priority_Update(_float fTimeDelta)
{
	m_IsUpdate = false;
}

void CItemInfo_Weapon::Update(_float fTimeDelta)
{
}

void CItemInfo_Weapon::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Late_Update(fTimeDelta);

}

HRESULT CItemInfo_Weapon::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CItemInfo_Weapon::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	m_pEffectIcon.resize(3);
	m_pEffectText.resize(3);
	m_pEffectValue.resize(3);

	for (auto Child : m_Children)
	{
		string strName = Child->Get_Name();

		if (strName == "ItemInfo_BG_Line_Top" || strName == "ItemInfo_BG_Line_Bottom" || strName == "ItemInfo_BG_Line_Left" || strName == "ItemInfo_BG_Line_Right")
		{
			Child->Set_Color({ 1.f, 1.f, 1.f, 0.2f });
			Child->Set_ShaderPass(2);
		}
		else if (strName == "ItemInfo_BG_Top")
		{
			m_pTopBg = static_cast<CUI_Default_Tex*>(Child);
			Safe_AddRef(m_pTopBg);
		}
		else if (strName == "ItemInfo_BG_Bottom")
		{
			m_pBottomBg = static_cast<CUI_Default_Tex*>(Child);
			Safe_AddRef(m_pBottomBg);
		}
		else if (strName == "ItemInfo_Equip_Deco")
		{
			m_pEquip_Deco = static_cast<CUI_Default_Tex*>(Child);
			Safe_AddRef(m_pEquip_Deco);
		}
		else if (strName == "ItemInfo_ItemICon")
		{
			m_pItemIcon = static_cast<CUI_Atlas_Icon*>(Child);
			Safe_AddRef(m_pItemIcon);
		}
		else if (strName == "ItemInfo_Equip_Text")
		{
			m_pEquip_Text = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pEquip_Text);
		}
		else if (strName == "ItemInfo_Text_Name")
		{
			m_pItemName = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pItemName);
		}
		else if (strName == "ItemInfo_Atk_Value")
		{
			m_pValue = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pValue);
		}
		else if (strName == "ItemInfo_Atk_Text")
		{
			m_pValueText = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pValueText);
		}
		else if (strName == "ItemInfo_Item_Grade")
		{
			m_pGradeText = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pGradeText);
		}
		else if (strName == "ItemInfo_Item_Type_Text")
		{
			m_pTypeText = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pTypeText);
		}
		else if (strName == "ItemInfo_Level")
		{
			m_pLevelText = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pLevelText);
		}
		else if (strName == "Item_Weight")
		{
			m_pWeightText = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pWeightText);
		}
		else if (strName == "Item_SubType")
		{
			m_pSubTypeText = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pSubTypeText);
		}
		else if (strName == "ItemInfo_Effect_Icon_1")
		{
			m_pEffectIcon[0] = static_cast<CUI_Atlas_Icon*>(Child);
			Safe_AddRef(Child);
		}
		else if (strName == "ItemInfo_Effect_Icon_2")
		{
			m_pEffectIcon[1] = static_cast<CUI_Atlas_Icon*>(Child);
			Safe_AddRef(Child);
		}
		else if (strName == "ItemInfo_Effect_Icon_3")
		{
			m_pEffectIcon[2] = static_cast<CUI_Atlas_Icon*>(Child);
			Safe_AddRef(Child);
		}
		else if (strName == "ItemInfo_Effect_Name_1")
		{
			m_pEffectText[0] = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(Child);
		}
		else if (strName == "ItemInfo_Effect_Name_2")
		{
			m_pEffectText[1] = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(Child);
		}
		else if (strName == "ItemInfo_Effect_Name_3")
		{
			m_pEffectText[2] = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(Child);
		}
		else if (strName == "ItemInfo_Effect_Value_1")
		{
			m_pEffectValue[0] = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(Child);
			}
		else if (strName == "ItemInfo_Effect_Value_2")
		{
			m_pEffectValue[1] = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(Child);
			}
		else if (strName == "ItemInfo_Effect_Value_3")
		{
			m_pEffectValue[2] = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(Child);
		}
		else if (strName == "ItemInfo_Item_TypeLine")
		{
			m_pItemLine = static_cast<CUI_Atlas_Icon*>(Child);
			Safe_AddRef(Child);
		}
	}


	return S_OK;
}

HRESULT CItemInfo_Weapon::Update_Switch(void* pArg)
{
	m_IsUpdate = true;
	WEAPONINFO_DESC* pDesc = static_cast<WEAPONINFO_DESC*>(pArg);

	m_vLocalPos = pDesc->iOffsetPos;
	Update_Transform(nullptr, m_vLocalPos);

	pDesc->isEquip ? m_pEquip_Deco->Update_Visible(true) : m_pEquip_Deco->Update_Visible(false);
	pDesc->isEquip ? m_pEquip_Text->Update_Visible(true) : m_pEquip_Text->Update_Visible(false);

	if (m_iItemIndex == pDesc->iItemIndex)
		return S_OK;

	const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(pDesc->iItemIndex);
	const EQUIPITEM_DATA* pEffectData = CClientInstance::GetInstance()->Get_Data<EQUIPITEM_DATA>(pData->iEffect_ID);
	
	m_pItemIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV(WStringToAnsi(pData->strIconName), pData->iTexPass), pData->iTexPass);

	m_pTopBg->Set_TexPass(pData->iGrade);
	m_pBottomBg->Set_TexPass(pData->iGrade);
	m_pItemName->Set_Text(pData->strName);
	
	m_pValue->Set_Text(to_wstring(pEffectData->iValue_1));
	if(pEffectData->iValue_1 > 999)
		m_pValueText->Setting_Pivot(-105, -255);
	else if (pEffectData->iValue_1 > 99)
		m_pValueText->Setting_Pivot(-120, -255);
	else if (pEffectData->iValue_1 > 9)
		m_pValueText->Setting_Pivot(-135, -255);

	_wstring wstrLevel = TEXT("Lv. ") + to_wstring(pData->iLevel);
	m_pLevelText->Set_Text(wstrLevel);

	Item_TypeCheck(pEffectData);
	Item_GradeCheck(pData);
	Item_SubTypeCheck(pEffectData);
	Item_AmorCheck(pData, pEffectData);
	Item_EffectCheck(pEffectData);
	
	return S_OK;
}

HRESULT CItemInfo_Weapon::Ready_Prototype()
{
	return S_OK;
}

HRESULT CItemInfo_Weapon::Ready_Componet()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_ItemInfo_BG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

void CItemInfo_Weapon::Item_TypeCheck(const EQUIPITEM_DATA* pEffectData)
{
	if (pEffectData->iType <= 2)
		m_pValueText->Set_Text(TEXT("최종 공격력"));
	else if (pEffectData->iType == 8)
		m_pValueText->Set_Text(TEXT("최대 기력"));
	else if (pEffectData->iType == 9)
		m_pValueText->Set_Text(TEXT("최대 생명력"));
	else
		m_pValueText->Set_Text(TEXT("최종 방어력"));
}

void CItemInfo_Weapon::Item_GradeCheck(const ITEM_DATA* pData)
{
	_wstring wstrGrade = {};
	_int iTexNum = 0;

	switch (pData->iGrade)
	{
	case 0: wstrGrade = TEXT("커먼"); iTexNum = 2; break;
	case 1: wstrGrade = TEXT("언커먼"); iTexNum = 3; break;
	case 2: wstrGrade = TEXT("레어"); iTexNum = 2; break;
	case 3: wstrGrade = TEXT("유니크"); iTexNum = 3; break;
	case 4: wstrGrade = TEXT("레전더리"); iTexNum = 4; break;
	case 5: wstrGrade = TEXT("에픽"); iTexNum = 2; break;
	}
	m_pGradeText->Set_Text(wstrGrade);

	if(iTexNum == 2)
		m_pItemLine->Set_LocalPos({ -122.f, 357.f }, this);
	else if (iTexNum == 3)
		m_pItemLine->Set_LocalPos({ -115.f, 357.f }, this);
	else if (iTexNum == 2)
		m_pItemLine->Set_LocalPos({ -105.f, 357.f }, this);
}

void CItemInfo_Weapon::Item_SubTypeCheck(const EQUIPITEM_DATA* pEffectData)
{
	_wstring wstrType = {};
	switch (pEffectData->iType)
	{
	case 1: wstrType = TEXT("창"); break;
	case 2: wstrType = TEXT("대검"); break;
	case 3: wstrType = TEXT("투구"); break;
	case 4: wstrType = TEXT("상의"); break;
	case 5: wstrType = TEXT("장갑"); break;
	case 6: wstrType = TEXT("하의"); break;
	case 7: wstrType = TEXT("신발"); break;
	case 8: wstrType = TEXT("목걸이"); break;
	case 9: wstrType = TEXT("반지"); break;
	}
	m_pTypeText->Set_Text(wstrType);
}

void CItemInfo_Weapon::Item_AmorCheck(const ITEM_DATA* pData, const EQUIPITEM_DATA* pEffectData)
{
	if (pData->iType == 5)
	{
		m_pWeightText->Update_Visible(true);
		m_pSubTypeText->Update_Visible(true);

		_wstring wstrWeight = TEXT("무게 ") + to_wstring(pEffectData->fWeight).substr(0, 3);
		m_pWeightText->Set_Text(wstrWeight);

		if (pEffectData->iClother_Type == 1)
			m_pSubTypeText->Set_Text(TEXT("경갑"));
		else if (pEffectData->iClother_Type == 2)
			m_pSubTypeText->Set_Text(TEXT("중갑"));
		else
			m_pSubTypeText->Update_Visible(false);
	}
	else
	{
		m_pWeightText->Update_Visible(false);
		m_pSubTypeText->Update_Visible(false);
	}
}

void CItemInfo_Weapon::Item_EffectCheck(const EQUIPITEM_DATA* pEffectData)
{
	if (pEffectData->iValue_Type_1 != 0)
	{
		_float4 vUV = {};
		_wstring wstrText = {};
		Mapping_ValueType(pEffectData->iValue_Type_1, vUV, wstrText);
		m_pEffectIcon[0]->Set_Texture(vUV, 2);
		m_pEffectText[0]->Set_Text(wstrText);
		m_pEffectValue[0]->Set_Text(to_wstring(pEffectData->iValue_1));
		m_pEffectIcon[0]->Update_Visible(true);
		m_pEffectText[0]->Update_Visible(true);
		m_pEffectValue[0]->Update_Visible(true);

		if (pEffectData->iValue_Type_2 != 0)
		{
			Mapping_ValueType(pEffectData->iValue_Type_2, vUV, wstrText);
			m_pEffectIcon[1]->Set_Texture(vUV, 2);
			m_pEffectText[1]->Set_Text(wstrText);
			m_pEffectValue[1]->Set_Text(to_wstring(pEffectData->iValue_1));
			m_pEffectIcon[1]->Update_Visible(true);
			m_pEffectText[1]->Update_Visible(true);
			m_pEffectValue[1]->Update_Visible(true);

			if (pEffectData->iValue_Type_3 != 0)
			{
				Mapping_ValueType(pEffectData->iValue_Type_2, vUV, wstrText);
				m_pEffectIcon[2]->Set_Texture(vUV, 2);
				m_pEffectText[2]->Set_Text(wstrText);
				m_pEffectValue[2]->Set_Text(to_wstring(pEffectData->iValue_1));
				m_pEffectIcon[2]->Update_Visible(true);
				m_pEffectText[2]->Update_Visible(true);
				m_pEffectValue[2]->Update_Visible(true);
			}
			else
			{
				m_pEffectIcon[3]->Update_Visible(false);
				m_pEffectText[3]->Update_Visible(false);
				m_pEffectValue[3]->Update_Visible(false);
			}
		}
		else
		{
			for (_int i = 1; i < 3; ++i)
			{
				m_pEffectIcon[i]->Update_Visible(false);
				m_pEffectText[i]->Update_Visible(false);
				m_pEffectValue[i]->Update_Visible(false);
			}
		}
	}
	else
	{
		for (_int i = 0; i < 3; ++i)
		{
			m_pEffectIcon[i]->Update_Visible(false);
			m_pEffectText[i]->Update_Visible(false);
			m_pEffectValue[i]->Update_Visible(false);
		}
	}
}

void CItemInfo_Weapon::Mapping_ValueType(_int iType, _float4& pOutUV, _wstring& pOutText)
{
	switch (iType)
	{
	case 0:	return;
	case 1:
		pOutUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_01_WeaponPower.png", 2);
		pOutText = TEXT("공격력");
		return;
	case 2:
		pOutUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_02_ArmorDefence.png", 2);
		pOutText = TEXT("방어력");
		return;
	case 3:
		pOutUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_09_HP.png", 2);
		pOutText = TEXT("체력");
		return;
	case 4:
		pOutUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_08_Stamina.png", 2);
		pOutText = TEXT("기력");
		return;
	case 5:
		pOutUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_03_Body.png", 2);
		pOutText = TEXT("활력");
		return;
	case 6:
		pOutUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_04_Heart.png", 2);
		pOutText = TEXT("지구력");
		return;
	case 7:
		pOutUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_07_Strength.png", 2);
		pOutText = TEXT("힘");
		return;
	case 8:
		pOutUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_06_Force.png", 2);
		pOutText = TEXT("의지");
		return;
	case 9:
		pOutUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_05_Skill.png", 2);
		pOutText = TEXT("역량");
		return;
	case 10:
		pOutUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_10_Level.png", 2);
		pOutText = TEXT("레벨");
		return;
	}
}

CItemInfo_Weapon* CItemInfo_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CItemInfo_Weapon* pInstance = new CItemInfo_Weapon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CItemInfo_Weapon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CItemInfo_Weapon::Clone(void* pArg)
{
	CItemInfo_Weapon* pInstance = new CItemInfo_Weapon(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CItemInfo_Weapon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItemInfo_Weapon::Free()
{
	__super::Free();

	for (auto Icon : m_pEffectIcon)
		Safe_Release(Icon);
	m_pEffectIcon.clear();

	for (auto Text : m_pEffectText)
		Safe_Release(Text);
	m_pEffectText.clear();

	for (auto Text : m_pEffectValue)
		Safe_Release(Text);
	m_pEffectValue.clear();

	Safe_Release(m_pItemIcon);
	Safe_Release(m_pTopBg);
	Safe_Release(m_pBottomBg);
	Safe_Release(m_pItemName);
	Safe_Release(m_pValue);
	Safe_Release(m_pValueText);

	Safe_Release(m_pGradeText);
	Safe_Release(m_pTypeText);
	Safe_Release(m_pLevelText);
	Safe_Release(m_pWeightText);
	Safe_Release(m_pSubTypeText);
	Safe_Release(m_pEquip_Deco);
	Safe_Release(m_pEquip_Text);
	Safe_Release(m_pItemLine);
	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
