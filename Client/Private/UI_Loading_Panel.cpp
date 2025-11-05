#include "UI_Loading_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"

CUI_Loading_Panel::CUI_Loading_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_Loading_Panel::CUI_Loading_Panel(const CUI_Loading_Panel& Prototype)
	: CUI_Panel(Prototype)
{
}

void CUI_Loading_Panel::Start_Load()
{
	m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
	m_fAlpha = 1.f;
	m_eAnimState = UIANIM::LOAD;
	m_vUV[0] = CClientInstance::GetInstance()->Get_AtlasUV("Loading_2.png", 4);
	m_iTexPass = 4;
}

void CUI_Loading_Panel::Finsh_Load()
{
	m_eAnimState = UIANIM::FINSH_START;
	m_fAlpha = 1.f;
}

HRESULT CUI_Loading_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Loading_Panel::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;
	m_eAnimState = UIANIM::LOAD;
	return S_OK;
}

void CUI_Loading_Panel::Priority_Update(_float fTimeDelta)
{
}

void CUI_Loading_Panel::Update(_float fTimeDelta)
{
}

void CUI_Loading_Panel::Late_Update(_float fTimeDelta)
{
	if(m_eAnimState == UIANIM::LOAD)
		Update_DeltaAlpha(fTimeDelta);
	else if (m_eAnimState == UIANIM::FINSH_START)
	{
		m_fAlpha -= fTimeDelta * 1.3f;
		if (m_fAlpha <= 0.f)
		{
			m_fAlpha = 0.f;
			m_eAnimState = UIANIM::FINSH;
		}
		Update_Alpha(m_fAlpha);
		m_pDecoL->Set_LocalPos({ -60 + (-60 * (1.f - m_fAlpha)), 0.f }, this);
		m_pDecoR->Set_LocalPos({ 60 + (60 * (1.f - m_fAlpha)), 0.f }, this);
	}
	else if (m_eAnimState == UIANIM::FINSH)
	{
		m_fAlpha += fTimeDelta * 10.f;
		if (m_fAlpha >= 1.f)
		{
			m_pTransformCom->Scale(_float3{ m_vLocalSize.x * 0.5f, m_vLocalSize.y * 0.5f, 1.f });
			m_fAlpha = 1.f;
			m_eAnimState = UIANIM::END;
			m_vUV[0] = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_F.png", 3);
			m_iTexPass = 3;
		}
		Update_Alpha(m_fAlpha);
		m_pDecoL->Set_LocalPos({ -60 + (-60 * (1.f - m_fAlpha)), 0.f }, this);
		m_pDecoR->Set_LocalPos({ 60 + (60 * (1.f - m_fAlpha)), 0.f }, this);
	}
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Loading_Panel::Render()
{
	return S_OK;
}

HRESULT CUI_Loading_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);
	for (auto child : m_Children)
	{
		string strName = child->Get_Name();

		if (strName == "Loading_Icon_L")
		{
			m_pDecoL = static_cast<CUI_Atlas_Icon*>(child);
			Safe_AddRef(child);
		}
		else if (strName == "Loading_Icon_R")
		{
			m_pDecoR = static_cast<CUI_Atlas_Icon*>(child);
			Safe_AddRef(child);
		}

	}
	return S_OK;
}

void CUI_Loading_Panel::Update_DeltaAlpha(_float fTimeDelta)
{
	if (m_isUpDelta)
		m_fAlpha += fTimeDelta * 0.5;
	else
		m_fAlpha -= fTimeDelta * 0.5;
	if (m_fAlpha < 0.3f)
		m_isUpDelta = true;
	else if (m_fAlpha > 0.9f)
		m_isUpDelta = false;

	Update_Alpha(m_fAlpha);
}

CUI_Loading_Panel* CUI_Loading_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Loading_Panel* pInstance = new CUI_Loading_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Loading_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Loading_Panel::Clone(void* pArg)
{
	CUI_Loading_Panel* pInstance = new CUI_Loading_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Loading_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Loading_Panel::Free()
{
	__super::Free();

	Safe_Release(m_pDecoL);
	Safe_Release(m_pDecoR);
}
