#include "Edit_ProgressBar.h"
#include "GameInstance.h"


CEdit_ProgressBar::CEdit_ProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI_ProgressBar{ pDevice,pContext }
{
}

CEdit_ProgressBar::CEdit_ProgressBar(const CEdit_ProgressBar& Prototype)
	:CUI_ProgressBar{ Prototype }
{
}

HRESULT CEdit_ProgressBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEdit_ProgressBar::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEdit_ProgressBar::Priority_Update(_float fTimeDelta)
{

}

void CEdit_ProgressBar::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CEdit_ProgressBar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CEdit_ProgressBar::Render()
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

HRESULT CEdit_ProgressBar::Ready_Components()
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

CEdit_ProgressBar* CEdit_ProgressBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEdit_ProgressBar* pInstance = new CEdit_ProgressBar(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CEdit_ProgressBar"));
	}
	return pInstance;
}

CGameObject* CEdit_ProgressBar::Clone(void* pArg)
{
	CEdit_ProgressBar* pInstance = new CEdit_ProgressBar(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Clone : CEdit_ProgressBar"));
	}
	return pInstance;
}

void CEdit_ProgressBar::Free()
{
	__super::Free();
}
