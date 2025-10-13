#include "Edit_Panel.h"
#include "GameInstance.h"


CEdit_Panel::CEdit_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_Panel{ pDevice,pContext }
{
}

CEdit_Panel::CEdit_Panel(const CEdit_Panel& Prototype)
	:CUI_Panel{ Prototype }
{
}

HRESULT CEdit_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEdit_Panel::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEdit_Panel::Priority_Update(_float fTimeDelta)
{

}

void CEdit_Panel::Update(_float fTimeDelta)
{

}

void CEdit_Panel::Late_Update(_float fTimeDelta)
{
	Update_LayOut();
	Update_Transform();

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI,this)))
		return;

}

HRESULT CEdit_Panel::Render()
{
	/*if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();*/


	return S_OK;
}

HRESULT CEdit_Panel::Ready_Components()
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
}

CEdit_Panel* CEdit_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEdit_Panel* pInstance = new CEdit_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CEdit_Panel"));
	}
	return pInstance;
}

CGameObject* CEdit_Panel::Clone(void* pArg)
{
	CEdit_Panel* pInstance = new CEdit_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Clone : CEdit_Panel"));
	}
	return pInstance;
}

void CEdit_Panel::Free()
{
	__super::Free();
}
