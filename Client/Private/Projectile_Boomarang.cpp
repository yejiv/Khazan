#include "Projectile_Boomarang.h"
#include "GameInstance.h"

CProjectile_Boomarang::CProjectile_Boomarang(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CProjectile{pDevice,pContext}
{
}

CProjectile_Boomarang::CProjectile_Boomarang(const CProjectile_Boomarang& Protptype)
    :CProjectile{ Protptype }
{
}

HRESULT CProjectile_Boomarang::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProjectile_Boomarang::Initialize_Clone(void* pArg)
{
    BOOMARANG_DESC* pDesc = static_cast<BOOMARANG_DESC*>(pArg);
    

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    m_isActive = false;

    m_pModelCom->Set_Animation(0);

    m_pTarget = pDesc->pTarget;
    Safe_AddRef(m_pTarget);
    
    m_fEffect = dynamic_cast<CEffect_Prefab*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Boomarang")));
    m_fEffect->ResetChildren();

    return S_OK;
}

void CProjectile_Boomarang::Priority_Update(_float fTimeDelta)
{
    m_fEffect->Priority_Update(fTimeDelta); 
}

void CProjectile_Boomarang::Update(_float fTimeDelta)
{
    m_fCurrentTime += fTimeDelta;

    if (m_fCurrentTime >= m_fLifeTime)
    {
        // 풀로 돌아가고
        m_isDead = true;
        // Active 끄고
        m_isActive = false;
        //m_isCrashed = true;
        //m_pBody->Collision_Active(false);
    }


    if (m_isActive)
    {
        if(m_fCurrentTime < m_fReturnTime)
            m_pTransformCom->Go_Straight(fTimeDelta);

        else if (m_fCurrentTime < m_fReturnTime + m_fPauseTime)
        {
            CTransform* pTargeTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
            _vector vTargetLoc = pTargeTransform->Get_State(STATE::POSITION);
            _vector vCurrentLoc = m_pTransformCom->Get_State(STATE::POSITION);

            vTargetLoc = XMVectorSetY(vTargetLoc,XMVectorGetY(vCurrentLoc));
            _vector vDir = XMVector3Normalize(vTargetLoc - vCurrentLoc);

            m_pTransformCom->LookAt(vCurrentLoc + vDir);
            m_pTransformCom->Set_State(STATE::POSITION, vCurrentLoc);
        }
        else if(m_fCurrentTime >= m_fReturnTime + m_fPauseTime)
            m_pTransformCom->Go_Straight(fTimeDelta);
    }

    if (m_pModelCom->Play_Animation(fTimeDelta))
    {
       
    }
    m_fEffect->UpdatePosition(m_pTransformCom->Get_State(STATE::POSITION));

    m_fEffect->Update(fTimeDelta);
}


void CProjectile_Boomarang::Late_Update(_float fTimeDelta)
{
    if (m_isVisible)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
    
    m_fEffect->Late_Update(fTimeDelta);
}

HRESULT CProjectile_Boomarang::Render()
{
    return S_OK;

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

void CProjectile_Boomarang::Reset()
{
    m_isCrashed = false;
    m_pBody->Collision_Active(true);
    Enter_State(PRJSTATE::IDLE);

    m_fCurrentTime = 0.f;
    _vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vSpawnDir));

    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));
    vUp = XMVector3Cross(vDir, vRight);

    m_pTransformCom->Set_State(STATE::RIGHT, vRight);
    m_pTransformCom->Set_State(STATE::UP, vUp);
    m_pTransformCom->Set_State(STATE::LOOK, vDir);

    //m_pTransformCom->Scale(_float3(0.5f, 0.5f, 0.5f));

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vSpawnPoint), 1.f));

}

void CProjectile_Boomarang::Enter_State(PRJSTATE eNextState)
{
}

HRESULT CProjectile_Boomarang::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga_Stone"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;
}

HRESULT CProjectile_Boomarang::Ready_Colliders()
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

    BodyDesc.fRadius = 0.3f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::LinearCast;
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    //BodyDesc.isCollideKinematicVsNonDynamic = true;

    XMStoreFloat3(&BodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&BodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;
    BodyDesc.bIsTrigger = true;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_Yetuga_Stone"), reinterpret_cast<CComponent**>(&m_pBody), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProjectile_Boomarang::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;
}

void CProjectile_Boomarang::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CProjectile_Boomarang::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CProjectile_Boomarang::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}

CProjectile_Boomarang* CProjectile_Boomarang::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProjectile_Boomarang* pInstance = new CProjectile_Boomarang(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CProjectile_Boomarang"));
    }

    return pInstance;
}

CGameObject* CProjectile_Boomarang::Clone(void* pArg)
{
    CProjectile_Boomarang* pInstance = new CProjectile_Boomarang(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CProjectile_Boomarang"));
    }

    return pInstance;
}

void CProjectile_Boomarang::Free()
{
    Safe_Release(m_pTarget);
    Safe_Release(m_fEffect);
}
