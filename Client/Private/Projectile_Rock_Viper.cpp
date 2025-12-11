#include "Projectile_Rock_Viper.h"
#include "GameInstance.h"
#include "Creature.h"

CProjectile_Rock_Viper::CProjectile_Rock_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CProjectile{ pDevice,pContext }
{
}

CProjectile_Rock_Viper::CProjectile_Rock_Viper(const CProjectile_Rock_Viper& Protptype)
    :CProjectile{ Protptype }
{
}

HRESULT CProjectile_Rock_Viper::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProjectile_Rock_Viper::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    m_isActive = false;

    m_pModelCom->Set_Animation(1);

    return S_OK;
}

void CProjectile_Rock_Viper::Priority_Update(_float fTimeDelta)
{

}

void CProjectile_Rock_Viper::Update(_float fTimeDelta)
{
    m_fCurrentTime += fTimeDelta;

    if (m_isActive)
    {
        m_pBody->Collision_Active(true);

        if (m_fCurrentTime >= m_fLifeTime)
        {
            // 풀로 돌아가고
            m_isDead = true;
            // Active 끄고
            m_isActive = false;
            m_isCrashed = true;
            m_pBody->Collision_Active(false);
        }
        m_pTransformCom->Go_Straight(fTimeDelta);
        // 콜라이더를 갱신시킨다.
        if (!m_isCrashed)
        {
            m_pBody->Sync_Update(m_pTransformCom);
            m_pBody->Update(fTimeDelta, m_pTransformCom);
        }
    }

    if (m_pModelCom->Play_Animation(fTimeDelta))
    {
        if (CRASHED == m_eState)
        {
            m_isDead = true;
            m_pBody->Collision_Active(false);
            Enter_State(PRJSTATE::END);
        }
    }
}

void CProjectile_Rock_Viper::Late_Update(_float fTimeDelta)
{
    if (m_isVisible)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
}

HRESULT CProjectile_Rock_Viper::Render()
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


void CProjectile_Rock_Viper::Reset()
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

    m_pTransformCom->Scale(_float3(1.3f, 1.3f, 1.3f));

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vSpawnPoint), 1.f));
}

void CProjectile_Rock_Viper::Enter_State(PRJSTATE eNextState)
{
    if (m_eState == eNextState)
        return;

    m_eState = eNextState;

    switch (m_eState)
    {
    case Client::CProjectile::LOOP:
        m_pModelCom->Set_Animation(1);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_viper_p2_throwing_rock_projectile_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::EFFECT1)), 30.f);
        break;
    case Client::CProjectile::CRASHED:
        m_pModelCom->Set_Animation(0);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_viper_p2_throwing_rock_exp_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::EFFECT1)), 30.f);
        m_isActive = false;
        break;
    case Client::CProjectile::END:
        m_isDead = true;
        break;
    }
}

HRESULT CProjectile_Rock_Viper::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Viper_Rock"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CProjectile_Rock_Viper::Ready_Colliders()
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

    BodyDesc.fRadius = 1.f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::LinearCast;
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.isCollideKinematicVsNonDynamic = true;

    XMStoreFloat3(&BodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&BodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tVIperRock_CollisionDesc.pGameObject = this;
    m_tVIperRock_CollisionDesc.strName = TEXT("Viper_Rock");
    m_tVIperRock_CollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;
    BodyDesc.bIsTrigger = true;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_Viper_Rock"), reinterpret_cast<CComponent**>(&m_pBody), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}



HRESULT CProjectile_Rock_Viper::Bind_ShaderResources()
{

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

void CProjectile_Rock_Viper::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    

}

void CProjectile_Rock_Viper::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (PRJSTATE::LOOP == m_eState)
    {
        COLLISION_LAYER eType = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
        if (COLLISION_LAYER::PLAYER == eType || COLLISION_LAYER::MAP_STATIC == eType)
        {
            Enter_State(PRJSTATE::CRASHED);
            CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
            if (nullptr == pTarget)
                return;
            pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL, nullptr);
            m_isCrashed = true;

        }
    }
}

void CProjectile_Rock_Viper::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

CProjectile_Rock_Viper* CProjectile_Rock_Viper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

    CProjectile_Rock_Viper* pInstance = new CProjectile_Rock_Viper(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CProjectile_Rock_Viper"));
    }

    return pInstance;
}

CGameObject* CProjectile_Rock_Viper::Clone(void* pArg)
{
    CProjectile_Rock_Viper* pInstance = new CProjectile_Rock_Viper(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CProjectile_Rock_Viper"));
    }

    return pInstance;
}

void CProjectile_Rock_Viper::Free()
{
    Safe_Release(m_pBody);

    __super::Free();
}
