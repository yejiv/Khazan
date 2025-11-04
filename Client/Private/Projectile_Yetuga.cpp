#include "Projectile_Yetuga.h"
#include "GameInstance.h"

CProjectile_Yetuga::CProjectile_Yetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CProjectile{pDevice,pContext}
{
}

CProjectile_Yetuga::CProjectile_Yetuga(const CProjectile_Yetuga& Protptype)
	:CProjectile{ Protptype }
{
}

HRESULT CProjectile_Yetuga::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Yetuga::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = false;

 	m_pModelCom->Set_Animation(0);

	return S_OK;
}

void CProjectile_Yetuga::Priority_Update(_float fTimeDelta)
{

}

void CProjectile_Yetuga::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_isActive)
	{
		_float3 vPos{};
			XMStoreFloat3(&vPos,m_pTransformCom->Get_State(STATE::POSITION)); // _float3
		//std::cout << "Pos: " << vPos.x << ", " << vPos.y << ", " << vPos.z << std::endl;

		if (m_fCurrentTime >= m_fLifeTime)
		{
			// 풀로 돌아가고
			m_isDead = true;
			// Active 끄고
			m_isActive = false;
		}
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (m_pModelCom->Play_Animation(fTimeDelta))
	{
		int a = 10;
	}


}

void CProjectile_Yetuga::Late_Update(_float fTimeDelta)
{
	if(m_isVisible)
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND,this);
}

HRESULT CProjectile_Yetuga::Render()
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


void CProjectile_Yetuga::Reset()
{
	m_fCurrentTime = 0.f;
	_vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vSpawnDir));

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));
	vUp = XMVector3Cross(vDir, vRight);

	m_pTransformCom->Set_State(STATE::RIGHT, vRight);
	m_pTransformCom->Set_State(STATE::UP, vUp);
	m_pTransformCom->Set_State(STATE::LOOK, vDir);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vSpawnPoint), 1.f));
}

HRESULT CProjectile_Yetuga::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga_Stone"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
		return E_FAIL;


	return S_OK;
}

HRESULT CProjectile_Yetuga::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	return S_OK;
}

CProjectile_Yetuga* CProjectile_Yetuga::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	CProjectile_Yetuga* pInstance = new CProjectile_Yetuga(pDevice,pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CProjectile_Yetuga"));
	}

	return pInstance;
}

CGameObject* CProjectile_Yetuga::Clone(void* pArg)
{
	CProjectile_Yetuga* pInstance = new CProjectile_Yetuga(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CProjectile_Yetuga"));
	}

	return pInstance;
}

void CProjectile_Yetuga::Free()
{
	__super::Free();
}
