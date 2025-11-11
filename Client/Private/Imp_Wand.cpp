#include "Imp_Wand.h"

_float4* CImp_Wand::Get_BonePointEX(const _char* pBoneName)
{
	_float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
	_matrix ConvertMatrix = XMLoadFloat4x4(&BoneMatrix);
	_matrix WorldMatrix = m_pOwnerTransform->Get_WorldMatrix();

	_matrix MulMatrix = ConvertMatrix*WorldMatrix;

	_float4x4 ThrowMatrix{};

	XMStoreFloat4x4(&ThrowMatrix, MulMatrix);

	m_vLockOnPoint.x = ThrowMatrix.m[ 3 ][ 0 ];
	m_vLockOnPoint.y = ThrowMatrix.m[ 3 ][ 1 ];
	m_vLockOnPoint.z = ThrowMatrix.m[ 3 ][ 2 ];

	return &m_vLockOnPoint;
}

_matrix CImp_Wand::Get_BoneMatrix(const _char* pBoneName)
{
	_float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
	_matrix BoneWorld = XMLoadFloat4x4(&BoneMatrix)*XMLoadFloat4x4(&m_CombinedWorldMatrix);

	return BoneWorld;
}

CImp_Wand::CImp_Wand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject{ pDevice,pContext }
{
}

CImp_Wand::CImp_Wand(const CImp_Wand& Prototype)
	:CPartObject{ Prototype }
{
}

HRESULT CImp_Wand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CImp_Wand::Initialize_Clone(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC* >( pArg );

	m_pOwnerTransform = pDesc->pOwnerTransform;
	if ( nullptr==m_pOwnerTransform )
		return E_FAIL;

	Safe_AddRef(m_pOwnerTransform);

	m_pOwner = pDesc->pOwner;
	if ( nullptr==m_pOwner )
		return E_FAIL;

    m_pSocketMatrix = pDesc->pSocketMatrix;

	if ( FAILED(__super::Initialize_Clone(pArg)) )
		return E_FAIL;


	if ( FAILED(Ready_Components()) )
		return E_FAIL;

    m_pTransformCom->Rotation(XMConvertToRadians(-90.f),0.f,0.f);


	return S_OK;
}

void CImp_Wand::Priority_Update(_float fTimeDelta)
{
}

void CImp_Wand::Update(_float fTimeDelta)
{
    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (_uint i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CImp_Wand::Late_Update(_float fTimeDelta)
{
	if ( FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)) )
		return;
}

HRESULT CImp_Wand::Render()
{
	if ( FAILED(Bind_ShaderResources()) )
		return E_FAIL;

	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for ( size_t i = 0; i<iNumMeshes; i++ )
	{
		m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

		m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);

		if ( FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)) )
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CImp_Wand::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CImp_Wand::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CImp_Wand::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
}

HRESULT CImp_Wand::Ready_Components()
{
	if ( FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast< CComponent** >( &m_pShaderCom ), nullptr)) )
		return E_FAIL;

	if ( FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_ImpWand"),
		TEXT("Com_Model"), reinterpret_cast< CComponent** >( &m_pModelCom ), nullptr)) )
		return E_FAIL;

	m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);


	return S_OK;
}

HRESULT CImp_Wand::Bind_ShaderResources()
{
	if ( FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)) )
		return E_FAIL;

	if ( FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))) )
		return E_FAIL;

	if ( FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))) )
		return E_FAIL;

	return S_OK;
}

CImp_Wand* CImp_Wand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImp_Wand* pInstance = new CImp_Wand(pDevice, pContext);
	if ( FAILED(pInstance->Initialize_Prototype()) )
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CImp_Wand"));
	}
	return pInstance;
}

CGameObject* CImp_Wand::Clone(void* pArg)
{
	CImp_Wand* pInstance = new CImp_Wand(*this);
	if ( FAILED(pInstance->Initialize_Clone(pArg)) )
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Clone : CImp_Wand"));
	}

	return pInstance;
}

void CImp_Wand::Free()
{
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pOwnerTransform);

	__super::Free();
}
