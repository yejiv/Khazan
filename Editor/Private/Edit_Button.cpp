#include "Edit_Button.h"
#include "GameInstance.h"


CEdit_Button::CEdit_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Button{pDevice,pContext}
{
}

CEdit_Button::CEdit_Button(const CEdit_Button& Prototype)
	:CUI_Button{ Prototype }
{
}

HRESULT CEdit_Button::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEdit_Button::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEdit_Button::Priority_Update(_float fTimeDelta)
{

}

void CEdit_Button::Update(_float fTimeDelta)
{

}

void CEdit_Button::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CEdit_Button::Render()
{
	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;


	/*if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;*/

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();


	return S_OK;
}

HRESULT CEdit_Button::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/*if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_StartButton"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;*/

	return S_OK;

}

CEdit_Button* CEdit_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEdit_Button* pInstance = new CEdit_Button(pDevice,pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CEdit_Button"));
	}
	return pInstance;
}

CGameObject* CEdit_Button::Clone(void* pArg)
{
	CEdit_Button* pInstance = new CEdit_Button(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Clone : CEdit_Button"));
	}
	return pInstance;
}

void CEdit_Button::Free()
{
	__super::Free();
}
