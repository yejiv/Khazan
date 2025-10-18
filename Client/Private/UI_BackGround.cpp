#include "UI_BackGround.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_BackGround::CUI_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Frame{ pDevice, pContext }
{
}

CUI_BackGround::CUI_BackGround(const CUI_BackGround& Prototype)
	: CUI_Frame(Prototype)
{
}

HRESULT CUI_BackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BackGround::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;
}
void CUI_BackGround::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_BackGround::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_BackGround::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_BackGround::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_BackGround::Bubble_EventCall()
{
	MSG_BOX(TEXT("TEST"));
}

HRESULT CUI_BackGround::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Texture_BackGround"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CUI_BackGround* CUI_BackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BackGround* pInstance = new CUI_BackGround(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_BackGround"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_BackGround::Clone(void* pArg)
{
	CUI_BackGround* pInstance = new CUI_BackGround(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_BackGround"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_BackGround::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

}
