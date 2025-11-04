#include "BrutalAttack_Point.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CBrutalAttack_Point::CBrutalAttack_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CBrutalAttack_Point::CBrutalAttack_Point(const CBrutalAttack_Point& Prototype)
	: CUI_Texture(Prototype)
{
}

HRESULT CBrutalAttack_Point::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBrutalAttack_Point::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Component(), E_FAIL);

	return S_OK;
}

void CBrutalAttack_Point::Priority_Update(_float fTimeDelta)
{
}

void CBrutalAttack_Point::Update(_float fTimeDelta)
{
}

void CBrutalAttack_Point::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
}

HRESULT CBrutalAttack_Point::Render()
{
	//if (m_iShaderPass == 3)
	//	m_iShaderPass = 5;
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

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBrutalAttack_Point::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_BrutalAttack"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

	return S_OK;
}

CBrutalAttack_Point* CBrutalAttack_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBrutalAttack_Point* pInstance = new CBrutalAttack_Point(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CBrutalAttack_Point"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBrutalAttack_Point::Clone(void* pArg)
{
	CBrutalAttack_Point* pInstance = new CBrutalAttack_Point(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CBrutalAttack_Point"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBrutalAttack_Point::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
