#include "UI_Fade.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Fade::CUI_Fade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CUI_Fade::CUI_Fade(const CUI_Fade& Prototype)
	: CUI_Texture(Prototype)
{
}

void CUI_Fade::Fade_In(function<void()> FadeEvent)
{
	m_eFadeType = FADE_STATE::FADE_IN;
	m_fAlpha = 1.f;
	m_FadeEvent = FadeEvent;
}

void CUI_Fade::Fade_Out(function<void()> FadeEvent)
{
	m_eFadeType = FADE_STATE::FADE_OUT;
	m_fAlpha = 0.f;
	m_FadeEvent = FadeEvent;
}

HRESULT CUI_Fade::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Fade::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Component(), E_FAIL);
	m_vColor = { 0.f, 0.f, 0.f, 1.f };

	return S_OK;
}

void CUI_Fade::Priority_Update(_float fTimeDelta)
{
}

void CUI_Fade::Update(_float fTimeDelta)
{
}

void CUI_Fade::Late_Update(_float fTimeDelta)
{
	if (m_eFadeType == FADE_STATE::FADE_IN)
		m_fAlpha -= fTimeDelta;
	else if (m_eFadeType == FADE_STATE::FADE_OUT)
		m_fAlpha += fTimeDelta;
	if (m_eFadeType != FADE_STATE::END)
	{
		if (m_fAlpha >= 1.f || m_fAlpha <= 0.f)
		{
			m_eFadeType = FADE_STATE::END;
			if (m_FadeEvent != nullptr)
			{
				m_FadeEvent();
				m_FadeEvent = nullptr;
			}
		}
	}
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_Fade::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(4);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Fade::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Mask"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

	return S_OK;
}

CUI_Fade* CUI_Fade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Fade* pInstance = new CUI_Fade(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Fade"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Fade::Clone(void* pArg)
{
	CUI_Fade* pInstance = new CUI_Fade(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Fade"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Fade::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
