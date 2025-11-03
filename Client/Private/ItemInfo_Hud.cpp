#include "ItemInfo_Hud.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"
CItemInfo_Hud::CItemInfo_Hud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CItemInfo_Hud::CItemInfo_Hud(const CItemInfo_Hud& Prototype)
	: CUI_Panel(Prototype)
{
}

void CItemInfo_Hud::Add_Item(_int iItemIndex)
{
	m_fAccTime = 0.f;
	m_eAnimState = UIAnim::ON;
	m_isDead = false;

	const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(iItemIndex);

	m_pItemText->Set_Text(pData->strName);
	
	m_pItemIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV(WStringToAnsi(pData->strIconName), pData->iTexPass), pData->iTexPass);

	m_iTexPass = pData->iGrade;
}

void CItemInfo_Hud::Update_Pos(_int iIndex, _int iMaxIndex, _float fOffsetY)
{
	m_vWorldPos.y = m_vLocalPos.y + ((iMaxIndex - iIndex) * fOffsetY);

	Update_Transform(nullptr, m_vWorldPos);
}

HRESULT CItemInfo_Hud::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemInfo_Hud::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	CHECK_FAILED(Ready_Componet(), E_FAIL);
	return S_OK;
}

void CItemInfo_Hud::Priority_Update(_float fTimeDelta)
{
}

void CItemInfo_Hud::Update(_float fTimeDelta)
{
}

void CItemInfo_Hud::Late_Update(_float fTimeDelta)
{
	
	if(m_eAnimState == UIAnim::ON)
	{
		m_fAccTime += fTimeDelta;

		if (m_fAccTime >= 1.f)
		{
			m_fAccTime = 1.f;
			m_eAnimState = UIAnim::END;
		}

		m_vWorldPos.x = m_vLocalPos.x + (200 - 200.f * Clamp(m_fAccTime * 2.f));
		Update_Transform(nullptr, m_vWorldPos);

		m_fAlpha = m_fAccTime;
	}
	else if (m_eAnimState == UIAnim::END)
	{
		m_fAccTime += fTimeDelta;
		if (m_fAccTime >= 2.f)
		{
			m_fAccTime = 1.f;
			m_eAnimState = UIAnim::OFF;
		}
	}
	else if (m_eAnimState == UIAnim::OFF)
	{
		m_fAccTime -= fTimeDelta;
		if (m_fAccTime <= 0.f)
		{
			m_fAccTime = 0.f;
			m_isDead = true;;
		}
		m_fAlpha = m_fAccTime;
	}

	Update_Alpha(m_fAlpha);

	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Late_Update(fTimeDelta);

}

HRESULT CItemInfo_Hud::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", 6)))
		return E_FAIL;

	
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(10);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CItemInfo_Hud::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	for (auto Child : m_Children)
	{
		string strName = Child->Get_Name();
		if (strName == "Hud_ItemInfo_Icon")
		{
			m_pItemIcon = static_cast<CUI_Atlas_Icon*>(Child);
			Safe_AddRef(m_pItemIcon);
		}

		if (strName == "Hud_ItemInfo_Name")
		{
			m_pItemText = static_cast<CUI_TextBox*>(Child);
			Safe_AddRef(m_pItemText);
		}

		if (strName == "Hud_ItemInfo_Line_Top" || strName == "Hud_ItemInfo_Line_Bottom")
		{
			Child->Set_Color({ 0.f, 0.f, 0.f, 0.8f });
			Child->Set_ShaderPass(2);
		}
	}

	return S_OK;
}

HRESULT CItemInfo_Hud::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_UI_ItemInfo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Hud/ItemInfo/T_BG_ItemInfo_B_%d.png"), 7)), E_FAIL);

	return S_OK;
}

HRESULT CItemInfo_Hud::Ready_Componet()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_ItemInfo"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CItemInfo_Hud* CItemInfo_Hud::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CItemInfo_Hud* pInstance = new CItemInfo_Hud(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CItemInfo_Hud"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CItemInfo_Hud::Clone(void* pArg)
{
	CItemInfo_Hud* pInstance = new CItemInfo_Hud(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CItemInfo_Hud"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItemInfo_Hud::Free()
{
	__super::Free();

	Safe_Release(m_pItemText);
	Safe_Release(m_pItemIcon);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
