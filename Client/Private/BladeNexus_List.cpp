#include "BladeNexus_List.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "MainMenu_Deco.h"

CBladeNexus_List::CBladeNexus_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Tap{ pDevice, pContext }
{
}

CBladeNexus_List::CBladeNexus_List(const CBladeNexus_List& Prototype)
	: CUI_Tap(Prototype)
{
}

void CBladeNexus_List::Update_Pos(_int iIndex, _float2 vPos, _float fOffSetY)
{
	m_vLocalPos.x = vPos.x;
	m_vLocalPos.y = vPos.y + iIndex * fOffSetY;

	__super::Update_Transform(nullptr, m_vWorldPos);
}

void CBladeNexus_List::OnAnime(_float fAccTime, _float fOffsetX, CUIObject* pParent)
{
	if (fAccTime >= 1.f)
	{
		m_vWorldPos.x = pParent->Get_WolrdPos().x + m_vLocalPos.x;
		m_vWorldPos.y = pParent->Get_WolrdPos().y + m_vLocalPos.y;
	}
	else
	{
		m_vWorldPos.x = pParent->Get_WolrdPos().x + m_vLocalPos.x - fOffsetX * (1.f - fAccTime);
		m_vWorldPos.y = m_vWorldPos.y;
	}

	__super::Update_Transform(nullptr, m_vWorldPos);
}

void CBladeNexus_List::Set_Selete(_bool isSelete)
{
	if (m_bIsSelete == isSelete)
		return;

	m_bIsSelete = isSelete;

	if (m_bIsSelete)
		m_fAccTime = 0.35f;
	m_pDeco->Update_AccTime(0.f);
}

HRESULT CBladeNexus_List::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	return S_OK;
}

HRESULT CBladeNexus_List::Initialize_Clone(void* pArg)
{
	MAINMENULIST_DESC* pDesc = static_cast<MAINMENULIST_DESC*>(pArg);
	m_eMenuType = pDesc->eMenu;

	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Componet(), E_FAIL);

	return S_OK;
}

void CBladeNexus_List::Priority_Update(_float fTimeDelta)
{
}

void CBladeNexus_List::Update(_float fTimeDelta)
{
	if (m_fAccTime < 1.f)
	{
		m_fAccTime += fTimeDelta;
		m_pDeco->Update_AccTime(m_fAccTime);
	}
	if (ButtonOver(g_hWnd))
	{
		CUI_BladeNexus::MAINMENUBUBBLE_DESC Desc{};
		Desc.eListType = m_eMenuType;
		Desc.isClick = false;
		Bubble_EventCall(&Desc);
	}
	if (ButtonClick(g_hWnd, false, true, INPUT_TYPE::UI) || m_bIsSelete && m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::UI))
	{
		CUI_BladeNexus::MAINMENUBUBBLE_DESC Desc{};
		Desc.eListType = m_eMenuType;
		Desc.isClick = true;
		Bubble_EventCall(&Desc);
	}
	if (m_bIsSelete)
	{
		m_isVisible = true;
		m_pTextBox->Set_Color({ 0.145f, 0.141f, 0.149f, 1.0f });
	}
	else
	{
		m_isVisible = false;
		m_pTextBox->Set_Color({ 0.749f, 0.749f, 0.749f, 1.0f });
	}


}

void CBladeNexus_List::Late_Update(_float fTimeDelta)
{
	if (m_isVisible)
	{
		CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
		m_pDeco->Late_Update(fTimeDelta);
	}
	m_pTextBox->Late_Update(fTimeDelta);
}

HRESULT CBladeNexus_List::Render()
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
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fValue", &m_fAccTime, sizeof(_float)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBladeNexus_List::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);
	m_iShaderPass = 6;
	m_iState = ENUM_CLASS(UISTATE::ENABLE);

	for (auto pChild : m_Children)
	{
		string strName = pChild->Get_Name();
		if (strName == "Menu_Name")
		{
			m_pTextBox = static_cast<CUI_TextBox*>(pChild);
			Safe_AddRef(m_pTextBox);
		}
		else if (strName == "Menu_List_Deco")
		{
			m_pDeco = static_cast<CMainMune_Deco*>(pChild);
			Safe_AddRef(m_pDeco);
		}
	}

	_wstring szText = {};
	switch (m_eMenuType)
	{
	case CUI_BladeNexus::MENULIST::STATE:
		szText = TEXT("능력 강화");
		break;
	case CUI_BladeNexus::MENULIST::WARP:
		szText = TEXT("귀검 이동");
		break;
	case CUI_BladeNexus::MENULIST::CREVICE:
		szText = TEXT("엠바스 유적지");
		break;
	}

	m_pTextBox->Set_Text(szText);
	return S_OK;
}

HRESULT CBladeNexus_List::Ready_Componet()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_MenuList"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CBladeNexus_List* CBladeNexus_List::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CBladeNexus_List* pInstance = new CBladeNexus_List(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CBladeNexus_List"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBladeNexus_List::Clone(void* pArg)
{
	CBladeNexus_List* pInstance = new CBladeNexus_List(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CBladeNexus_List"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBladeNexus_List::Free()
{
	__super::Free();

	Safe_Release(m_pTextBox);
	Safe_Release(m_pDeco);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
