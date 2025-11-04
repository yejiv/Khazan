#include "BrutalAttack_Progress.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CBrutalAttack_Progress::CBrutalAttack_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_ProgressBar{ pDevice, pContext }
{
}

CBrutalAttack_Progress::CBrutalAttack_Progress(const CBrutalAttack_Progress& Prototype)
	: CUI_ProgressBar(Prototype)
{
}

HRESULT CBrutalAttack_Progress::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBrutalAttack_Progress::Initialize_Clone(void* pArg)
{
	GUIDEGAUGE_DESC* pDesc = static_cast<GUIDEGAUGE_DESC*>(pArg);

	m_pMaxValue = pDesc->pMaxValue;
	m_pCulValue = pDesc->pCulValue;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CBrutalAttack_Progress::Priority_Update(_float fTimeDelta)
{
}

void CBrutalAttack_Progress::Update(_float fTimeDelta)
{
	if (m_pCulValue == nullptr || m_pMaxValue == nullptr)
		return;
	m_fMaxValue = *m_pMaxValue;
	m_fCurrentValue = *m_pCulValue;

	if (m_fCurrentValue < 0)
		m_fCurrentValue = 0;
	if (m_fCurrentValue > m_fMaxValue)
		m_fCurrentValue = m_fMaxValue;

	Progress_Update();
}

void CBrutalAttack_Progress::Late_Update(_float fTimeDelta)
{
	if (m_pCulValue == nullptr || m_pMaxValue == nullptr)
		return;

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);

}

HRESULT CBrutalAttack_Progress::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fProgressValue", &m_fProgress_Value, sizeof(_float)), E_FAIL);

	m_pShaderCom->Begin(2);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBrutalAttack_Progress::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_BrutalAttack"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CBrutalAttack_Progress* CBrutalAttack_Progress::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBrutalAttack_Progress* pInstance = new CBrutalAttack_Progress(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CBrutalAttack_Progress"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBrutalAttack_Progress::Clone(void* pArg)
{
	CBrutalAttack_Progress* pInstance = new CBrutalAttack_Progress(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CBrutalAttack_Progress"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBrutalAttack_Progress::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
