#include "Atlas_RenderGroup.h"
#include "GameInstance.h"

CAtlas_RenderGroup::CAtlas_RenderGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject{ pDevice,pDeviceContext }
{
}

CAtlas_RenderGroup::CAtlas_RenderGroup(const CAtlas_RenderGroup& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CAtlas_RenderGroup::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAtlas_RenderGroup::Initialize_Clone(void* pArg)
{
	ATLASGROUP_DESC* pDesc = static_cast<ATLASGROUP_DESC*>(pArg);

	__super::Initialize_Clone(pArg);

	//ATLASGROUP_DESC* pDesc = static_cast<ATLASGROUP_DESC*>(pArg);
	//m_fDepth = pDesc->fDepth;
	m_iShaderPass = pDesc->iShdaerPass;

	if (FAILED(Ready_Component()))
		return E_FAIL;
	return S_OK;
}

void CAtlas_RenderGroup::Priority_Update(_float fTimeDelta)
{
}

void CAtlas_RenderGroup::Update(_float fTimeDelta)
{
}

void CAtlas_RenderGroup::Late_Update(_float fTimeDelta)
{
}

HRESULT CAtlas_RenderGroup::Render()
{
	m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix");

	if (m_iShaderPass == 0)
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
			return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_Shader_AllTexture(m_pShaderCom, "g_DiffuseTextures")))
		return E_FAIL;

	m_pShaderCom->Begin(m_iShaderPass);

	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();
	m_isRender = false;
	return S_OK;
}

void CAtlas_RenderGroup::Add_Renderer()
{
	if (!m_isRender)
		return;

	m_pVIBufferCom->Update_Buffer();
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CAtlas_RenderGroup::Add_UIInstance(const VTXINSTANCE_UI* pUIData)
{
	m_isRender = true;
	return m_pVIBufferCom->Add_UIInstance(pUIData);
}

_float4 CAtlas_RenderGroup::Get_AtlasUV(const string pFrameName, _uint iTextureIndex)
{
	return m_pTextureCom->FindTexFrame(pFrameName, iTextureIndex);
}

HRESULT CAtlas_RenderGroup::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Point_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Instance_UI"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UI_Atlas"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CAtlas_RenderGroup* CAtlas_RenderGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CAtlas_RenderGroup* pInstance = new CAtlas_RenderGroup(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CAtlas_RenderGroup"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAtlas_RenderGroup::Clone(void* pArg)
{
	CAtlas_RenderGroup* pInstance = new CAtlas_RenderGroup(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CAtlas_RenderGroup"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAtlas_RenderGroup::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
