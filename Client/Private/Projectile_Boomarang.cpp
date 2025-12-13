#include "Projectile_Boomarang.h"
#include "GameInstance.h"
#include "Creature.h"

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

    /*if (FAILED(Ready_Components()))
        return E_FAIL;*/

    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    m_isActive = false;
    m_pTarget = pDesc->pTarget;
    Safe_AddRef(m_pTarget);

    m_pBody->Collision_Active(false);

    
    m_fEffect = dynamic_cast<CEffect_Prefab*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Boomarang")));
    if (m_fEffect)
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


    m_pBody->Collision_Active(true);

    if (m_fCurrentTime >= m_fLifeTime)
    {
        m_pBody->Set_Pos(XMVectorSet(0.f, 0.f, 0.f, 1.f));
        m_isDead = true;
        m_isActive = false;
        m_pBody->Collision_Active(false);
        StopBoomarangSound();
    }

    if (m_isActive)
    {

        Enter_State(BOOMARANGSTATE::LOOP);

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

        m_pBody->Sync_Update(m_pTransformCom);
        m_pBody->Update(fTimeDelta, m_pTransformCom);


    }


     
    m_fEffect->UpdatePosition(m_pTransformCom->Get_State(STATE::POSITION));

    m_fEffect->Update(fTimeDelta);
}


void CProjectile_Boomarang::Late_Update(_float fTimeDelta)
{
    if (m_isVisible)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);

    if(m_isVisible)
        m_fEffect->Late_Update(fTimeDelta);

}

HRESULT CProjectile_Boomarang::Render()
{
    return S_OK;
}

void CProjectile_Boomarang::Reset()
{

    m_pBody->Collision_Active(true);
    m_isDamageForward = false;
    m_isDamageReturn = false;
    m_isClearSound = false;

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

void CProjectile_Boomarang::StopBoomarangSound()
{
    m_pGameInstance->StopByChannel(Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)));
}


HRESULT CProjectile_Boomarang::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga_Stone"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProjectile_Boomarang::Ready_Colliders()
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

    BodyDesc.fRadius = 0.3f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::LinearCast;
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.isCollideKinematicVsNonDynamic = true;

    XMStoreFloat3(&BodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&BodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tBoomanrangCollisionDesc.pGameObject = this;
    m_tBoomanrangCollisionDesc.strName = TEXT("BoomanrangCollision");
    m_tBoomanrangCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.pCollisionDesc = &m_tBoomanrangCollisionDesc;
    BodyDesc.bIsTrigger = true;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_ImpRange_Boomarang"), reinterpret_cast<CComponent**>(&m_pBody), &BodyDesc)))
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

    return S_OK;
}

void CProjectile_Boomarang::Enter_State(BOOMARANGSTATE eNextState)
{
    if (m_eState == eNextState)
        return;

    m_eState = eNextState;


    switch (m_eState)
    {
    case Client::BOOMARANGSTATE::LOOP:
        //m_pGameInstance->PlaySoundLoop(TEXT("Mon_DemonImpWizard_Boomerang_Obj_Cast_Spin (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 5.f);
        break;
    case Client::BOOMARANGSTATE::CRASHED:

        break;
    case Client::BOOMARANGSTATE::END:
        break;
    default:
        break;
    }

}

void CProjectile_Boomarang::BoomarangHitSFX()
{
     _uint iSoundIndex = m_pGameInstance->Rand(0, 3);

    if (iSoundIndex == 0)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_Boomerang_Obj_Exp_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 30.f);

    else if (iSoundIndex == 1)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_Boomerang_Obj_Exp_02 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 30.f);

    else if (iSoundIndex == 2)
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_Boomerang_Obj_Exp_03 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 30.f);
}

void CProjectile_Boomarang::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CProjectile_Boomarang::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eType = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
   

    if (COLLISION_LAYER::PLAYER == eType)
    {

        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        if (nullptr == pTarget)
            return;


        _float fCurrnetTIme = m_fCurrentTime;
        
        if (fCurrnetTIme < m_fReturnTime)
        {
            if (!m_isDamageForward)
            {
                pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL, nullptr);
                BoomarangHitSFX();
                m_isDamageForward = true;

            }
        }
        else if (fCurrnetTIme >= m_fReturnTime + m_fPauseTime)
        {
            if (!m_isDamageReturn)
            {
                pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL, nullptr);
                BoomarangHitSFX();
                m_isDamageReturn = true;
            }
        }
    }
}

void CProjectile_Boomarang::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
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
    __super::Free();

    Safe_Release(m_pTarget);
    Safe_Release(m_fEffect);
    Safe_Release(m_pBody);
}
