#include "Projectile_Rock_Yetuga.h"
#include "GameInstance.h"
#include "Creature.h"

CProjectile_Rock_Yetuga::CProjectile_Rock_Yetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CProjectile{ pDevice,pContext }
{
}

CProjectile_Rock_Yetuga::CProjectile_Rock_Yetuga(const CProjectile_Rock_Yetuga& Protptype)
	:CProjectile{ Protptype }
{
}

HRESULT CProjectile_Rock_Yetuga::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Rock_Yetuga::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    m_pBody->Collision_Active(false);

    m_isVisible = true;
    m_isPicked = false;

	m_pTransformCom->Rotation(XMConvertToRadians(90.f),0.f,0.f);


    /*_matrix PreTransformMatrix = XMMatrixIdentity();
    PreTransformMatrix = XMMatrixScaling(0.00053f, 0.00053f, 0.00053f);
    _float4x4 TempMatrix = {};
    XMStoreFloat4x4(&TempMatrix, PreTransformMatrix);
    m_pAnimModelCom->Set_PreTransformMatrix(TempMatrix);
    m_pAnimModelCom->Set_Animation(0);*/



	return S_OK;
}

void CProjectile_Rock_Yetuga::Priority_Update(_float fTimeDelta)
{
}

void CProjectile_Rock_Yetuga::Update(_float fTimeDelta)
{

    if (m_isPicked)
    {
        m_pBody->Sync_Update(m_pTransformCom);
        m_pBody->Update(fTimeDelta, m_pTransformCom);
    }
    else
    {
        m_pBody->Collision_Active(false);
    }
    
   
    //if (m_pAnimModelCom->Play_Animation(fTimeDelta))
    //{

        if (CProjectile::PRJSTATE::CRASHED == m_eState)
        {
            Enter_State(PRJSTATE::END);
        }
    //}
}

void CProjectile_Rock_Yetuga::Late_Update(_float fTimeDelta)
{
    if (m_isVisible)
    {
    /*    if(m_eState == CProjectile::PRJSTATE::CRASHED)
            m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
        else*/
            m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);

    }

}

HRESULT CProjectile_Rock_Yetuga::Render()
{

   /* if (m_eState == CProjectile::PRJSTATE::CRASHED)
    {
        if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pAnimShaderCom, "g_WorldMatrix")))
            return E_FAIL;

        if (FAILED(m_pAnimShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
            return E_FAIL;

        if (FAILED(m_pAnimShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
            return E_FAIL;

        _uint           iNumMeshes = m_pAnimModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; i++)
        {
            _uint           iNumMeshes = m_pAnimModelCom->Get_NumMeshes();

            for (size_t i = 0; i < iNumMeshes; i++)
            {
                m_pModelCom->Bind_Materials(m_pAnimShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

                m_pModelCom->Bind_Materials(m_pAnimShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);

                if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pAnimShaderCom, "g_BoneMatrices", i)))
                    return E_FAIL;

                m_pAnimShaderCom->Begin(0);
                m_pAnimModelCom->Render(i);
            }


        }
    }*/

    //else
    //{
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
    //}

    
	return S_OK;
}

void CProjectile_Rock_Yetuga::Reset()
{
 
    if (m_eState == CProjectile::PRJSTATE::CRASHED)
        return;

    m_isPicked = true;
    m_pBody->Collision_Active(true);
	m_fCurrentTime = 0.f;
	_vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vSpawnDir));

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));
	vUp = XMVector3Cross(vDir, vRight);

	m_pTransformCom->Set_State(STATE::RIGHT, vRight);
	m_pTransformCom->Set_State(STATE::UP, vUp);
	m_pTransformCom->Set_State(STATE::LOOK, vDir);

	//m_pTransformCom->Scale(_float3(1.5f, 1.5f, 1.5f));

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vSpawnPoint), 1.f));


}

void CProjectile_Rock_Yetuga::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        
        // 데미지 주고
        //pTarget->Take_Damage();
        // 넉백주고
        Enter_State(PRJSTATE::CRASHED);
        
    }
}

void CProjectile_Rock_Yetuga::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CProjectile_Rock_Yetuga::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

void CProjectile_Rock_Yetuga::Enter_State(PRJSTATE eNextState)
{
    if (m_eState == eNextState)
        return;

    m_eState = eNextState;

    switch (m_eState)
    {
    case CProjectile::PRJSTATE::IDLE:

        break;
    case CProjectile::PRJSTATE::LOOP:

        break;
    case CProjectile::PRJSTATE::CRASHED:
        /*_vector vCurrentPos = m_pTransformCom->Get_State(STATE::POSITION);
        _vector vOffset = vCurrentPos + XMVectorSet(0.f,100.f,0.f,0.f);*/
        //m_pTransformCom->Set_State(STATE::POSITION, vOffset);
        //m_pAnimModelCom->Set_Animation(2);
        break;
    case CProjectile::PRJSTATE::END:
        //m_pAnimModelCom->Set_Animation(0);
        m_pBody->Collision_Active(false);
        m_pBody->Set_Pos(XMVectorSet(0.f, 0.f, 0.f, 1.f));
        m_isDead = true;
        m_isVisible = false;
        m_isPicked = false;
        break;
    }



}

HRESULT CProjectile_Rock_Yetuga::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga_Rock"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
		return E_FAIL;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 /*   if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_AnimShader"), reinterpret_cast<CComponent**>(&m_pAnimShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga_Stone"),
        TEXT("Com_AnimModel"), reinterpret_cast<CComponent**>(&m_pAnimModelCom), nullptr)))
        return E_FAIL;*/

	return S_OK;
}

HRESULT CProjectile_Rock_Yetuga::Ready_Colliders()
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

    BodyDesc.fRadius = 5.f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::LinearCast; // 기본 모드
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);


    XMStoreFloat3(&BodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&BodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;
    BodyDesc.bIsTrigger = true;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_Yetuga_Rock"), reinterpret_cast<CComponent**>(&m_pBody), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProjectile_Rock_Yetuga::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;


	return S_OK;
}


CProjectile_Rock_Yetuga* CProjectile_Rock_Yetuga::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProjectile_Rock_Yetuga* pInstance = new CProjectile_Rock_Yetuga(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : Projectile_Rock_Yetuga"));
	}

	return pInstance;
}

CGameObject* CProjectile_Rock_Yetuga::Clone(void* pArg)
{
	CProjectile_Rock_Yetuga* pInstance = new CProjectile_Rock_Yetuga(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Clone : CProjectile_Rock_Yetuga"));
	}

	return pInstance;
}

void CProjectile_Rock_Yetuga::Free()
{
	__super::Free();

    Safe_Release(m_pBody);
    //Safe_Release(m_pAnimModelCom);
    //Safe_Release(m_pAnimShaderCom);
}
