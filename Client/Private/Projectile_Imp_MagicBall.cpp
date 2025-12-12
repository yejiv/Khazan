#include "Projectile_Imp_MagicBall.h"
#include "GameInstance.h"
#include "Creature.h"
#include "LineTrail.h"

CProjectile_Imp_MagicBall::CProjectile_Imp_MagicBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CProjectile{pDevice,pContext}
{
}

CProjectile_Imp_MagicBall::CProjectile_Imp_MagicBall(const CProjectile_Imp_MagicBall& Protptype)
    :CProjectile{Protptype}
{
}

HRESULT CProjectile_Imp_MagicBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProjectile_Imp_MagicBall::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    m_isActive = false;

    //m_pModelCom->Set_Animation(0);
    m_pBody->Collision_Active(false);


    m_fEffect = dynamic_cast<CEffect_Prefab*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("MagicBall")));
    if (m_fEffect)
        m_fEffect->ResetChildren();

    CLineTrail::LINE_TRAIL_DESC Desc{};
    Desc.fOffset = 3.f;
    Desc.fLifeTime = 2.f;
    Desc.iDivisionCount = 5.f;
    Desc.iTextureIdx = 28;
    Desc.vColor = _float4(1.084f, 1.f, 4.f, 1.f);
    m_pLineTrail = static_cast<CLineTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LineTrail"), &Desc));

    return S_OK;
}

void CProjectile_Imp_MagicBall::Priority_Update(_float fTimeDelta)
{
    m_fEffect->Priority_Update(fTimeDelta);
    m_pLineTrail->Priority_Update(fTimeDelta);
}

void CProjectile_Imp_MagicBall::Update(_float fTimeDelta)
{
    m_fCurrentTime += fTimeDelta;

    if (m_fCurrentTime >= m_fLifeTime)
    {
        m_pBody->Collision_Active(false);
        // 풀로 돌아가고
        m_isDead = true;
        // Active 끄고
        m_isActive = false;
        m_isCrashed = true;
    }

    if (m_isActive)
    {
        m_pTransformCom->Go_Straight(fTimeDelta);
        // 콜라이더를 갱신시킨다.
        if (!m_isCrashed)
        {
            m_pBody->Sync_Update(m_pTransformCom);
            m_pBody->Update(fTimeDelta, m_pTransformCom);
        }
    }


    if (CRASHED == m_eState)
    {
        m_pBody->Collision_Active(false);
        m_isDead = true;
        Enter_State(PRJSTATE::END);
    }

    m_fEffect->UpdatePosition(m_pTransformCom->Get_State(STATE::POSITION));
    m_fEffect->Update(fTimeDelta);
    m_pLineTrail->Add_ControlPoint(m_pTransformCom->Get_State(STATE::POSITION));
    m_pLineTrail->Update(fTimeDelta);
    
}

void CProjectile_Imp_MagicBall::Late_Update(_float fTimeDelta)
{
    if (m_isVisible)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);

    m_fEffect->Late_Update(fTimeDelta);
    m_pLineTrail->Late_Update(fTimeDelta);
}

HRESULT CProjectile_Imp_MagicBall::Render()
{
    return S_OK;
}

void CProjectile_Imp_MagicBall::Reset()
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



    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vSpawnPoint), 1.f));
}

void CProjectile_Imp_MagicBall::Enter_State(PRJSTATE eNextState)
{
    m_eState = eNextState;

    switch (m_eState)
    {
    case Client::CProjectile::LOOP:
        m_pGameInstance->PlaySoundLoop(TEXT("Mon_DemonImpWizard_GuidedMagic_Obj_Loop (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 20.f);
        break;
    case Client::CProjectile::CRASHED:
    {

        _uint iSoundIndex = m_pGameInstance->Rand(0, 3);

        if (iSoundIndex == 0)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Obj_Exp_01 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 20.f);

        else if (iSoundIndex == 1)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Obj_Exp_02 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 20.f);

        else if (iSoundIndex == 2)
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_DemonImpWizard_GuidedMagic_Obj_Exp_03 (SFX).wav"), Get_Position(), Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 20.f);
        m_isActive = false;
        break;
    }
    case Client::CProjectile::END:
        m_isDead = true;
        break;
    }
}

void CProjectile_Imp_MagicBall::StopSound()
{
    m_pGameInstance->StopByChannel(Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)));
}

HRESULT CProjectile_Imp_MagicBall::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga_Stone"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CProjectile_Imp_MagicBall::Ready_Colliders()
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

    BodyDesc.fRadius = 1.f;    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::LinearCast;
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);

    XMStoreFloat3(&BodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&BodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tMagicBallColliderDesc.pGameObject = this;
    m_tMagicBallColliderDesc.strName = TEXT("MagicBallCollider");
    m_tMagicBallColliderDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.pCollisionDesc = &m_tMagicBallColliderDesc;
    BodyDesc.bIsTrigger = true;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_ImpRange_MagicBall"), reinterpret_cast<CComponent**>(&m_pBody), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProjectile_Imp_MagicBall::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

void CProjectile_Imp_MagicBall::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CProjectile_Imp_MagicBall::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (PRJSTATE::LOOP == m_eState)
    {
        COLLISION_LAYER eType = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
        if (COLLISION_LAYER::PLAYER == eType)
        {
            Enter_State(PRJSTATE::CRASHED);
            m_isCrashed = true;
            CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
            if (nullptr == pTarget)
                return;
            pTarget->Take_Damage(10.f,HITREACTION::KNOCKBACK_NORMAL,nullptr);
            
        }

        else if (pDesc->strName == TEXT("GuardCollisionDesc"))
        {
            
        }


    }
}

void CProjectile_Imp_MagicBall::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

CProjectile_Imp_MagicBall* CProjectile_Imp_MagicBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProjectile_Imp_MagicBall* pInstance = new CProjectile_Imp_MagicBall(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CProjectile_Imp_MagicBall"));
    }

    return pInstance;
}

CGameObject* CProjectile_Imp_MagicBall::Clone(void* pArg)
{
    CProjectile_Imp_MagicBall* pInstance = new CProjectile_Imp_MagicBall(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CProjectile_Imp_MagicBall"));
    }

    return pInstance;
}

void CProjectile_Imp_MagicBall::Free()
{
    Safe_Release(m_pLineTrail);
    Safe_Release(m_pBody);
    Safe_Release(m_fEffect);

    __super::Free();
}
