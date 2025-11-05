#include "Tutorial_Tex.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CTutorial_Tex::CTutorial_Tex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CTutorial_Tex::CTutorial_Tex(const CTutorial_Tex& Prototype)
	: CUI_Texture(Prototype)
{
}

HRESULT CTutorial_Tex::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTutorial_Tex::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Component(), E_FAIL);
	m_vColor = { 1.f, 1.f, 1.f, 1.f };
	return S_OK;
}

void CTutorial_Tex::Priority_Update(_float fTimeDelta)
{
}

void CTutorial_Tex::Update(_float fTimeDelta)
{
	//++m_iTexPass;

	//if (m_iTexPass >= 251)
	//	m_iTexPass = 0;

}

void CTutorial_Tex::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CTutorial_Tex::Render()
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

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTutorial_Tex::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_GuidePage"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

	return S_OK;
}

CTutorial_Tex* CTutorial_Tex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTutorial_Tex* pInstance = new CTutorial_Tex(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CTutorial_Tex"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTutorial_Tex::Clone(void* pArg)
{
	CTutorial_Tex* pInstance = new CTutorial_Tex(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CTutorial_Tex"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTutorial_Tex::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
