#include "NPC_Gacha.h"

#include "GameInstance.h"

#include "Interaction_Guide.h"

#include "UI_Talk_Gacha.h"

#include "ClientInstance.h"

CNPC_Gacha::CNPC_Gacha(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
    , m_pClientInstance { CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

CNPC_Gacha::CNPC_Gacha(const CNPC_Gacha& Prototype)
    : CProp_Interactive { Prototype }
    , m_pClientInstance { Prototype.m_pClientInstance }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CNPC_Gacha::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CNPC_Gacha::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    CHECK_FAILED(Ready_Interaction_Guide(pArg), E_FAIL);

    CHECK_FAILED(Ready_3D_Talk_UI(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::IDLE;
    m_pModelCom->Set_Animation(m_eAnimState);
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(true);

    m_iEventID = m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e)
        {
            m_Event = e;
        });

#pragma region 3D UI

#pragma endregion

    return S_OK;
}

void CNPC_Gacha::Priority_Update(_float fTimeDelta)
{
    if (false == m_isCollision)
    {
        m_Event.None();
    }

    m_pGachaTalkUI->Priority_Update(fTimeDelta);
}

void CNPC_Gacha::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);

    m_pGachaTalkUI->Update(fTimeDelta);
}

void CNPC_Gacha::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );

    m_pGachaTalkUI->Late_Update(fTimeDelta);

    m_pGachaTalkUI->Update_UITransform(m_pTransformCom->Get_State(STATE::POSITION));
}

HRESULT CNPC_Gacha::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _float fEdgeIntensity = 0.2f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.2f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float))))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(35), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CNPC_Gacha::Ready_Components(void* pArg)
{
    DANJIN_DESC* pDesc = static_cast<DANJIN_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CNPC_Gacha::Ready_Collision(void* pArg)
{
#pragma region 스태틱 몸체
    CBody::BODY_BOXSHAPE_DESC StaticBodyDesc{};
    StaticBodyDesc.vExtent = _float3(0.3f, 1.f, 0.3f);
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
    vPos.y += StaticBodyDesc.vExtent.y;
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
    TriggerDesc.vExtent = _float3(2.f, 1.f, 2.f);
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
    TriggerDesc.vPos.y += TriggerDesc.vExtent.y;
    XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
    TriggerDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_TriggerCollisionDesc.pGameObject = this;
    m_TriggerCollisionDesc.isForceVaildation = true;
    TriggerDesc.pCollisionDesc = &m_TriggerCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger"), reinterpret_cast<CComponent**>(&m_pTriggerCom), &TriggerDesc)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CNPC_Gacha::Ready_Interaction_Guide(void* pArg)
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1] + 1.f), TEXT("대화"), 1.5f);

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

HRESULT CNPC_Gacha::Ready_3D_Talk_UI(void* pArg)
{
    CUIObject::UIOBJECT_DESC Desc;

    Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
    Desc.vLocalPos = { 0.f, 0.f };
    Desc.vLocalSize = { 1.7f, 1.7f };
    Desc.szName = "Gacha_TalkUI";
    m_pGachaTalkUI = static_cast<CUI_Talk_Gacha*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Talk_Gacha"), &Desc));
    CHECK_NULLPTR(m_pGachaTalkUI, E_FAIL);

    return S_OK;
}

HRESULT CNPC_Gacha::Ready_DefaultSetting(void* pArg)
{
    return S_OK;
}

HRESULT CNPC_Gacha::Bind_Materials(_uint iMeshIndex)
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
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", iMeshIndex, aiTextureType_METALNESS, 0)))
        m_iMtrlFlags |= M_METALIC;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", iMeshIndex, aiTextureType_SHININESS, 0)))
        m_iMtrlFlags |= M_ROUGHNESS;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

void CNPC_Gacha::Input_Interact_Event(_float fTimeDelta)
{
    if (ANIM_STATE::TALK_START == m_eAnimState || ANIM_STATE::TALK_IDLE == m_eAnimState || ANIM_STATE::TALK_END == m_eAnimState)
        return;

    _bool isPressing = { false };

    if (m_pGameInstance->Key_Pressing(DIK_F, fTimeDelta))
    {
        isPressing = m_pGuide->IsPressing();
    }

    if (true == isPressing)
    {
        m_pGuide->Update_Visible(false);

        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::GACHANPC;

        InteractType.eState = EventInteractType::BEGIN;

        EventNPC NPCEvent = {};

        NPCEvent.vPlayerPosition = _float4(-60.172f, -92.26f, -37.281f, 1.f);
        XMStoreFloat4(&NPCEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        InteractType.NPCEvent = NPCEvent;

        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
    }
}

void CNPC_Gacha::Animation_Update(_float fTimeDelta)
{
    if (false == m_isCollision)
        return;

    Input_Interact_Event(fTimeDelta);

    if (m_Event.isOn())               // 켠다는 신호
    {
        // 해금 전 IDLE 상태
        if (ANIM_STATE::IDLE == m_eAnimState)
        {
            m_pGuide->Update_Visible(false);

            // 처음 상호 작용 시
            m_eAnimState = ANIM_STATE::TALK_START;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);

            m_pGachaTalkUI->On_Panel();

            EventInteractType InteractType = {};

            InteractType.eInteractType = INTERACTIVE_TYPE::GACHANPC;
            InteractType.isEvent = true;

            EventNPC NPCEvent = {};

            NPCEvent.vPlayerPosition = _float4(-60.172f, -92.26f, -37.281f, 1.f);
            XMStoreFloat4(&NPCEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

            InteractType.NPCEvent = NPCEvent;

            // NPC를 바라볼 수 있도록 포지션만 던짐 ( 귀검 애니메이션 아직 종료 X )
            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

            m_pClientInstance->Camera_Set_NpcTalk(true, _float3(-61.155f, -91.044f, -38.491f), _float3(-0.871f, 0.126f, 0.475f));
        }
    }
    else if (m_Event.isOff())         // 끈다는 신호 ( 내가 받기만 하면 됨
    {
        if (ANIM_STATE::TALK_IDLE == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::TALK_END;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);

            m_pClientInstance->Camera_Set_NpcTalk(false, _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f));
        }
    }
}

void CNPC_Gacha::Animation_Change(_float fTimeDelta)
{
    // NPC 가동 끝나면
    if (ANIM_STATE::TALK_START == m_eAnimState)       // BEFORE_START 가 끝나면 BEFORE_LOOP ( 플레이어가 UI랑 상호 작용 )
    {
        // 처음 상호 작용 후 애니메이션 루프로 전환 및 이벤트 발생
        m_eAnimState = ANIM_STATE::TALK_IDLE;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(true);
    }
    // NPC 상호 작용 종료 후 ( 첫 해금 O )
    if (ANIM_STATE::TALK_END == m_eAnimState)
    {
        if (true == m_isCollision)
            m_pGuide->Update_Visible(true);

        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::IDLE;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(true);

        m_Event.None();
    }
}

void CNPC_Gacha::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (ANIM_STATE::IDLE == m_eAnimState)
        m_pGuide->Update_Visible(true);

    m_isCollision = true;
}

void CNPC_Gacha::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    m_isCollision = true;
}

void CNPC_Gacha::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
    if (ANIM_STATE::TALK_END != m_eAnimState && ANIM_STATE::IDLE != m_eAnimState)
    {
        m_eAnimState = ANIM_STATE::TALK_END;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(true);
    }

    m_pGuide->Update_Visible(false);

    m_isCollision = false;
}

CNPC_Gacha* CNPC_Gacha::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Gacha* pInstance = new CNPC_Gacha(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CNPC_Gacha"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNPC_Gacha::Clone(void* pArg)
{
    CNPC_Gacha* pInstance = new CNPC_Gacha(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CNPC_Gacha"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Gacha::Free()
{
    __super::Free();

    Safe_Release(m_pStaticCom);
    Safe_Release(m_pTriggerCom);
    Safe_Release(m_pGachaTalkUI);

    Safe_Release(m_pClientInstance);

    if (nullptr != m_pGuide)
    {
        m_pGuide->Set_IsDead(true);
        m_pGuide = nullptr;
    }
}
