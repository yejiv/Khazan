#include "Illusion_Wall.h"

#include "GameInstance.h"

CIllusion_Wall::CIllusion_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CIllusion_Wall::CIllusion_Wall(const CIllusion_Wall& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CIllusion_Wall::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CIllusion_Wall::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    m_pTransformCom->Scale(_float3(0.02f, 0.02f, 0.02f));

    return S_OK;
}

void CIllusion_Wall::Priority_Update(_float fTimeDelta)
{
}

void CIllusion_Wall::Update(_float fTimeDelta)
{
    CHECK_TRUE(m_isDead, );
    CHECK_FALSE(m_isCollision, );

    if (m_isCollision)
    {
        m_fTimeAcc += fTimeDelta;
    }

    if (1.f <= m_fTimeAcc)
    {
        m_fDecreaseAlpha += fTimeDelta * 0.2f;
    }

    if (false == m_isRemoveWall && 0.5f <= m_fDecreaseAlpha)
    {
        SoundOnce(TEXT("IP_Statue_Done"), m_fInteract_Volume);

        m_isRemoveWall = true;

        m_pStaticCom->Collision_Active(false);
    }

    if (1.f <= m_fDecreaseAlpha)
    {
        Set_IsDead(true);
    }
}

void CIllusion_Wall::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this), );
}

HRESULT CIllusion_Wall::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        Bind_DissolveValues();

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(14), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CIllusion_Wall::Ready_Components(void* pArg)
{
    ILLUSION_WALL_DESC* pDesc = static_cast<ILLUSION_WALL_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Dissolve"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CIllusion_Wall::Ready_Collision(void* pArg)
{
#pragma region 스태틱 몸체
    CBody::BODY_BOXSHAPE_DESC StaticBodyDesc{};
    StaticBodyDesc.vExtent = _float3(5.f, 3.f, 0.2f);
    StaticBodyDesc.bIsTrigger = false;
    StaticBodyDesc.bStartActive = true;
    StaticBodyDesc.eMotion = EMotionType::Static;
    StaticBodyDesc.eQuality = EMotionQuality::LinearCast;
    StaticBodyDesc.eShapeType = SHAPE::BOX;
    StaticBodyDesc.fFriction = 0.8f;
    StaticBodyDesc.fMass = 1.0f;
    StaticBodyDesc.fRestitution = 0.0f;
    StaticBodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_STATIC_TRIGGER);
    _float3 vPos{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    vPos.z += StaticBodyDesc.vExtent.z + 3.f;
    vPos.y += StaticBodyDesc.vExtent.y + 1.f;
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    StaticBodyDesc.vPos = vPos;
    StaticBodyDesc.vQuat = vQuat;
    StaticBodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    StaticBodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Static"), reinterpret_cast<CComponent**>(&m_pStaticCom), &StaticBodyDesc)))
        return E_FAIL;
#pragma endregion

#pragma region 트리거 영역
    CBody::BODY_BOXSHAPE_DESC TriggerDesc{};
    TriggerDesc.vExtent = _float3(5.f, 3.f, 1.2f);
    TriggerDesc.bIsTrigger = true;
    TriggerDesc.bStartActive = true;
    TriggerDesc.eMotion = EMotionType::Kinematic;
    TriggerDesc.eQuality = EMotionQuality::LinearCast;
    TriggerDesc.eShapeType = SHAPE::BOX;
    TriggerDesc.fFriction = 0.8f;
    TriggerDesc.fMass = 1.0f;
    TriggerDesc.fRestitution = 0.0f;
    TriggerDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT);

    XMStoreFloat3(&TriggerDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    TriggerDesc.vPos.z += TriggerDesc.vExtent.z + 3.f;
    TriggerDesc.vPos.y += TriggerDesc.vExtent.y + 1.f;
    XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
    TriggerDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_TriggerCollisionDesc.pGameObject = this;
    m_TriggerCollisionDesc.isForceVaildation = true;
    //pCollDesc.pInfo = ?? // 작성하기
    TriggerDesc.pCollisionDesc = &m_TriggerCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger"), reinterpret_cast<CComponent**>(&m_pTriggerCom), &TriggerDesc)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CIllusion_Wall::Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        m_iMtrlFlags |= M_DIFFUSE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

HRESULT CIllusion_Wall::Bind_DissolveValues()
{
    CHECK_FAILED(m_pDissolveTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0), E_FAIL);

    _float fEdgeWidth = { 0.06f };
    _float4 fEdgeColor = _float4(1.f, 1.f, 1.f, 1.f);

    m_pShaderCom->Bind_RawValue("g_fDecreaseAlpha", &m_fDecreaseAlpha, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeWidth", &fEdgeWidth, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeColor", &fEdgeColor, sizeof(_float4));

    return S_OK;
}

void CIllusion_Wall::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (false == m_isCollision && ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT) == pMyDesc->iObjectLayer)
    {
        m_isCollision = true;
    }
}

void CIllusion_Wall::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;
}

void CIllusion_Wall::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;
}

CIllusion_Wall* CIllusion_Wall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CIllusion_Wall* pInstance = new CIllusion_Wall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CIllusion_Wall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CIllusion_Wall::Clone(void* pArg)
{
    CIllusion_Wall* pInstance = new CIllusion_Wall(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CIllusion_Wall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CIllusion_Wall::Free()
{
    __super::Free();

    Safe_Release(m_pStaticCom);
    Safe_Release(m_pTriggerCom);

    Safe_Release(m_pDissolveTextureCom);
}
