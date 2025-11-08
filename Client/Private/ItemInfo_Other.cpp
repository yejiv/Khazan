#include "ItemInfo_Other.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"
#include "UI_Default_Tex.h"

CItemInfo_Other::CItemInfo_Other(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CItemInfo_Other::CItemInfo_Other(const CItemInfo_Other& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CItemInfo_Other::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemInfo_Other::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	CHECK_FAILED(Ready_Componet(), E_FAIL);
	return S_OK;
}

void CItemInfo_Other::Priority_Update(_float fTimeDelta)
{
	m_IsUpdate = false;
}

void CItemInfo_Other::Update(_float fTimeDelta)
{
}

void CItemInfo_Other::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Late_Update(fTimeDelta);

}

HRESULT CItemInfo_Other::Render()
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

HRESULT CItemInfo_Other::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

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
		else if (strName == "ItemInfo_Count_Value")
		{
			m_pItemCountValue = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pItemCountValue);
		}
		else if (strName == "ItemInfo_Info_Text")
		{
			m_pInfoText = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pInfoText);
		}
		else if (strName == "ItemInfo_Info_SubText")
		{
			m_pSubInfoText = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pSubInfoText);
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
		else if (strName == "ItemInfo_Info_Line")
		{
			Child->Set_Color({ 1.f, 1.f, 1.f, 0.2f });
			Child->Set_ShaderPass(2);

			m_pInfoLine = static_cast<CUI_Atlas_Icon*>(Child);
			Safe_AddRef(m_pInfoLine);
		}
		else if (strName == "ItemInfo_Item_TypeLine")
		{
			m_pItemLine = static_cast<CUI_Atlas_Icon*>(Child);
			Safe_AddRef(Child);
		}
	}
	return S_OK;
}

HRESULT CItemInfo_Other::Update_Switch(void* pArg)
{
	m_IsUpdate = true;
	OTHERINFO_DESC* pDesc = static_cast<OTHERINFO_DESC*>(pArg);

	m_vLocalPos = pDesc->iOffsetPos;
	Update_Transform(nullptr, m_vLocalPos);

	pDesc->isEquip ? m_pEquip_Deco->Update_Visible(true) : m_pEquip_Deco->Update_Visible(false);
	pDesc->isEquip ? m_pEquip_Text->Update_Visible(true) : m_pEquip_Text->Update_Visible(false);
	wstring wstrCount = to_wstring(pDesc->iCurItem) + TEXT(" / ") + to_wstring(pDesc->iMaxItem);
	m_pItemCountValue->Set_Text(wstrCount);

	if (m_iItemIndex == pDesc->iItemIndex)
		return S_OK;
	m_iItemIndex = pDesc->iItemIndex;
	const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(pDesc->iItemIndex);
	
	Item_GradeCheck(pData);

	m_pItemIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV(WStringToAnsi(pData->strIconName), pData->iTexPass), pData->iTexPass);
	if (pData->iEffect_ID > 0)
	{
		const OTHERITEM_DATA* pEffectData = CClientInstance::GetInstance()->Get_Data<OTHERITEM_DATA>(pData->iEffect_ID);
		m_pSubInfoText->Set_Text(pEffectData->strText);
		m_pSubInfoText->Update_Visible(true);
		m_pInfoLine->Update_Visible(true);
	}
	else
	{
		m_pSubInfoText->Update_Visible(false);
		m_pInfoLine->Update_Visible(false);
	}
	m_pTopBg->Set_TexPass(pData->iGrade);
	m_pBottomBg->Set_TexPass(pData->iGrade);

	m_pItemName->Set_Text(pData->strName);
	
	m_pInfoText->Set_Text(pData->strText);

	Item_GradeCheck(pData);


	_wstring wstrType = {};
	switch (pData->iType)
	{
	case 1:
		if(pData->iEffect_ID < 10)
			wstrType = TEXT("소모품");
		else
			wstrType = TEXT("등불");
		break;
	case 2: wstrType = TEXT("수집품"); break;
	case 3: wstrType = TEXT("재료"); break;
	}
	m_pTypeText->Set_Text(wstrType);
	return S_OK;
}

HRESULT CItemInfo_Other::Ready_Prototype()
{
	return S_OK;
}

HRESULT CItemInfo_Other::Ready_Componet()
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

void CItemInfo_Other::Item_GradeCheck(const ITEM_DATA* pData)
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
	if (iTexNum == 2)
		m_pItemLine->Set_LocalPos({ -122.f, 272.f }, this);
	else if (iTexNum == 3)
		m_pItemLine->Set_LocalPos({ -115.f, 272.f }, this);
	else if (iTexNum == 2)
		m_pItemLine->Set_LocalPos({ -105.f, 272.f }, this);
}

CItemInfo_Other* CItemInfo_Other::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CItemInfo_Other* pInstance = new CItemInfo_Other(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CItemInfo_Other"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CItemInfo_Other::Clone(void* pArg)
{
	CItemInfo_Other* pInstance = new CItemInfo_Other(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CItemInfo_Other"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItemInfo_Other::Free()
{
	__super::Free();

	Safe_Release(m_pInfoLine);
	Safe_Release(m_pItemIcon);
	Safe_Release(m_pItemLine);

	Safe_Release(m_pItemName);
	Safe_Release(m_pItemCountValue);
	Safe_Release(m_pInfoText);
	Safe_Release(m_pSubInfoText);
	Safe_Release(m_pGradeText);
	Safe_Release(m_pTypeText);

	Safe_Release(m_pTopBg);
	Safe_Release(m_pBottomBg);

	Safe_Release(m_pEquip_Deco);
	Safe_Release(m_pEquip_Text);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
