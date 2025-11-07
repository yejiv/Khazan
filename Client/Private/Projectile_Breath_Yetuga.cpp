#include "Projectile_Breath_Yetuga.h"
#include "GameInstance.h"

CProjectile_Breath_Yetuga::CProjectile_Breath_Yetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CProjectile{pDevice,pContext}
{
}

CProjectile_Breath_Yetuga::CProjectile_Breath_Yetuga(const CProjectile_Breath_Yetuga& Prototype)
	:CProjectile{Prototype}
{
}

HRESULT CProjectile_Breath_Yetuga::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Breath_Yetuga::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;


	return S_OK;
}

void CProjectile_Breath_Yetuga::Priority_Update(_float fTimeDelta)
{

}

void CProjectile_Breath_Yetuga::Update(_float fTimeDelta)
{

}

void CProjectile_Breath_Yetuga::Late_Update(_float fTimeDelta)
{
	if (m_isVisible)
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);
}

HRESULT CProjectile_Breath_Yetuga::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

		m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

void CProjectile_Breath_Yetuga::Reset()
{
	m_fCurrentTime = 0.f;
	_vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vSpawnDir));

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));
	vUp = XMVector3Cross(vDir, vRight);

	m_pTransformCom->Set_State(STATE::RIGHT, vRight);
	m_pTransformCom->Set_State(STATE::UP, vUp);
	m_pTransformCom->Set_State(STATE::LOOK, vDir);

	_vector vTempPos{};
	vTempPos = XMLoadFloat3(&m_vSpawnPoint);
	vTempPos = XMVectorSetY(vTempPos,XMVectorGetY(vTempPos) + 6.f);

	//m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vSpawnPoint), 1.f));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(vTempPos, 1.f));
}

HRESULT CProjectile_Breath_Yetuga::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga_Rock"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile_Breath_Yetuga::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CProjectile_Breath_Yetuga* CProjectile_Breath_Yetuga::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Breath_Yetuga* pInstance = new CProjectile_Breath_Yetuga(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CProjectile_Breath_Yetuga"));
	}

	return pInstance;
}

CGameObject* CProjectile_Breath_Yetuga::Clone(void* pArg)
{
	CProjectile_Breath_Yetuga* pInstance = new CProjectile_Breath_Yetuga(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CProjectile_Breath_Yetuga"));
	}

	return pInstance;
}

void CProjectile_Breath_Yetuga::Free()
{
	__super::Free();
}
