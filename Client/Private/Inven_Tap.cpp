#include "Inven_Tap.h"
#include "ClientInstance.h"
#include "GameInstance.h"

CInven_Tap::CInven_Tap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Tap{ pDevice, pContext }
{
}

CInven_Tap::CInven_Tap(const CInven_Tap& Prototype)
	: CUI_Tap(Prototype)
{
}

void CInven_Tap::Tap_Enable()
{
	m_iState = ENUM_CLASS(UISTATE::ENABLE);
}

void CInven_Tap::Tap_Disable()
{
	m_iState = ENUM_CLASS(UISTATE::DISABLE);
}

HRESULT CInven_Tap::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	return S_OK;
}

HRESULT CInven_Tap::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CInven_Tap::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CInven_Tap::Update(_float fTimeDelta)
{

	if (m_iState == ENUM_CLASS(UISTATE::DISABLE))
	{
		if (ButtonClick(g_hWnd, false, true))
		{
			CUIObject::BUBBLEEVENT Desc = {};
			Desc.szName = m_szName;

			Bubble_EventCall(&Desc);
		}

		if (ButtonOver(g_hWnd))
		{
			m_vColor = { 1.f, 1.f, 1.f, 1.f };
		}
		else
		{
			m_vColor = { 1.f, 1.f, 1.f, 0.5f };
		}
	}
	else if (m_iState == ENUM_CLASS(UISTATE::ENABLE))
	{
		m_vColor = { 0.941f, 0.769f, 0.329f, 1.f };
	}
	__super::Update(fTimeDelta);
}

void CInven_Tap::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
	__super::Late_Update(fTimeDelta);
}

HRESULT CInven_Tap::Render()
{
	return S_OK;
}

HRESULT CInven_Tap::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);
	m_iShaderPass = 2;
	return S_OK;
}

CInven_Tap* CInven_Tap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CInven_Tap* pInstance = new CInven_Tap(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CInven_Tap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInven_Tap::Clone(void* pArg)
{
	CInven_Tap* pInstance = new CInven_Tap(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CInven_Tap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInven_Tap::Free()
{
	__super::Free();
}
