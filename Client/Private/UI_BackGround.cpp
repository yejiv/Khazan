#include "UI_BackGround.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_BackGround::CUI_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CUI_BackGround::CUI_BackGround(const CUI_BackGround& Prototype)
	: CUI_Texture(Prototype)
{
}

void CUI_BackGround::Setting_BG(UIBGTYPE eType)
{
	m_eBGType = eType;
}

HRESULT CUI_BackGround::Initialize_Prototype()
{
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_BackGround::Initialize_Clone(void* pArg)
{
	m_vColor = { 1.f,1.f, 1.f, 0.9f };
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}
void CUI_BackGround::Priority_Update(_float fTimeDelta)
{
}

void CUI_BackGround::Update(_float fTimeDelta)
{
}

void CUI_BackGround::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
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
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);
	
	Bind_Mask();
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();
	
	return S_OK;
}

void CUI_BackGround::Bubble_EventCall()
{
}

HRESULT CUI_BackGround::Ready_Prototype()
{
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UI_BackGround_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Mask/BG_Mask_0%d.png"), 2))))
		return E_FAIL;
	return S_OK;
}

HRESULT CUI_BackGround::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Mask"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UI_BackGround"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UI_BackGround_Mask"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BackGround::Bind_Mask()
{
	if (m_eBGType == UIBGTYPE::MAIN)
	{
		m_pShaderCom->Begin(1);
	}
	else if (m_eBGType == UIBGTYPE::ITEM)
	{
		m_pShaderCom->Begin(1);
	}
	else if (m_eBGType == UIBGTYPE::BLADENEXUS)
	{
		m_pShaderCom->Begin(2);
	}
	else if (m_eBGType == UIBGTYPE::STATE)
	{
		m_pShaderCom->Begin(3);
	}
	else if (m_eBGType == UIBGTYPE::LOADING)
	{
		m_pShaderCom->Begin(5);
	}
	else if (m_eBGType == UIBGTYPE::DEFAULT)
	{
		m_pShaderCom->Begin(6);
	}
    else if (m_eBGType == UIBGTYPE::TOP)
    {
        m_pShaderCom->Begin(7);
    }
    else if (m_eBGType == UIBGTYPE::CIRCLE)
    {
        m_pShaderCom->Begin(9);
    }
	else
	{
		m_pShaderCom->Begin(4);
	}
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
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pVIBufferCom);

}
