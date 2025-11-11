#include "Projectile_Imp_MagicBall.h"
#include "GameInstance.h"

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

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    m_isActive = false;

    m_pModelCom->Set_Animation(0);

    return S_OK;
}

void CProjectile_Imp_MagicBall::Priority_Update(_float fTimeDelta)
{

}

void CProjectile_Imp_MagicBall::Update(_float fTimeDelta)
{
    m_fCurrentTime += fTimeDelta;

    if (m_isActive)
    {
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
            m_pBody->Collision_Active(false);
            Enter_State(PRJSTATE::END);
        }
    }
}

void CProjectile_Imp_MagicBall::Late_Update(_float fTimeDelta)
{
    if (m_isVisible)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
}

HRESULT CProjectile_Imp_MagicBall::Render()
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

    m_pTransformCom->Scale(_float3(1.3f, 1.3f, 1.3f));

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vSpawnPoint), 1.f));
}

void CProjectile_Imp_MagicBall::Enter_State(PRJSTATE eNextState)
{
    m_eState = eNextState;

    switch (m_eState)
    {
    case Client::CProjectile::LOOP:
        m_pModelCom->Set_Animation(1);
        break;
    case Client::CProjectile::CRASHED:
        m_pModelCom->Set_Animation(2);
        m_isActive = false;
        break;
    case Client::CProjectile::END:
        m_isDead = true;
        break;
    }
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

    BodyDesc.fRadius = 0.3f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::LinearCast;
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.isCollideKinematicVsNonDynamic = true;

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

HRESULT CProjectile_Imp_MagicBall::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;
}

void CProjectile_Imp_MagicBall::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CProjectile_Imp_MagicBall::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (PRJSTATE::LOOP == m_eState)
    {
        COLLISION_LAYER eType = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
        if (COLLISION_LAYER::PLAYER == eType || COLLISION_LAYER::MAP_STATIC == eType)
        {
            Enter_State(PRJSTATE::CRASHED);
            m_isCrashed = true;

        }
    }
}

void CProjectile_Imp_MagicBall::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
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
    Safe_Release(m_pBody);

    __super::Free();
}
