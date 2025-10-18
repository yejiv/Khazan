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
	CUIObject::UIOBJECT_DESC Desc = {};
	__super::Initialize_Clone(nullptr);

	ATLASGROUP_DESC* pDesc = static_cast<ATLASGROUP_DESC*>(pArg);
	m_fDepth = pDesc->fDepth;
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
	if (!m_isRender)
		return;

	m_pVIBufferCom->Update_Buffer();
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CAtlas_RenderGroup::Render()
{
	m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix");

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_AllTexture(m_pShaderCom, "g_DiffuseTextures")))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();
	m_isRender = false;
	return S_OK;
}

HRESULT CAtlas_RenderGroup::Add_UIInstance(const VTXINSTANCE_UI* pUIData)
{
	m_isRender = true;
	return m_pVIBufferCom->Add_UIInstance(pUIData);
}

HRESULT CAtlas_RenderGroup::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Shader_VtxInstance_PointUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_VIBuffer_UIInstance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Texture_UI_Atlas"),
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
