#include "BladeNexus.h"

#include "GameInstance.h"

#include "Interaction_Guide.h"

#include "ClientInstance.h"
#include "UI_BladeNexus.h"

CBladeNexus::CBladeNexus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CBladeNexus::CBladeNexus(const CBladeNexus& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CBladeNexus::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CBladeNexus::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    CHECK_FAILED(Ready_Interaction_Guide(pArg), E_FAIL);

    CHECK_FAILED(Ready_DefaultSetting(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::BEFORE_IDLE;
    m_pModelCom->Set_Animation(ANIM_STATE::BEFORE_IDLE);
    m_pModelCom->Set_AnimationLoop(true);

    m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e)
        {
            m_Event = e;
        });

    return S_OK;
}

void CBladeNexus::Priority_Update(_float fTimeDelta)
{
    if (false == m_isCollision)
    {
        m_Event.None();
    }

    Find_Target();
}

void CBladeNexus::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);
}

void CBladeNexus::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CBladeNexus::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    // 0 칼손잡이 | 1 손 잘림 보호대 | 2 뭐 존나 작은 눈 | 3 밑에 작은 날카로운 | 4 밑에 큰 날카로운 | 5 눈
    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        _bool isBNEye = { 5 == i };
        m_pShaderCom->Bind_RawValue("g_isBNEye", &isBNEye, sizeof(_bool));

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(8), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CBladeNexus::Ready_Components(void* pArg)
{
    BLADENEXUS_DESC* pDesc = static_cast<BLADENEXUS_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CBladeNexus::Ready_Collision(void* pArg)
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
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    TriggerDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger"), reinterpret_cast<CComponent**>(&m_pTriggerCom), &TriggerDesc)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CBladeNexus::Ready_Interaction_Guide(void* pArg)
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    Safe_AddRef(m_pGuide);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1] + 1.f), TEXT("접촉"), 1.5f);

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

HRESULT CBladeNexus::Ready_DefaultSetting(void* pArg)
{
    BLADENEXUS_DESC* pDesc = static_cast<BLADENEXUS_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    _int* pBladeNexusID = static_cast<_int*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pBladeNexusID, E_FAIL);

    m_iBladeNexus_ID = *pBladeNexusID;

    switch (m_iBladeNexus_ID)
    {
    case static_cast<_int>(BLADENEXUS_ID::HEINMACH_ENTER):
        memcpy(m_szPlaceName, TEXT("눈보라 협곡"), sizeof(m_szPlaceName));
        break;
    case static_cast<_int>(BLADENEXUS_ID::HEINMACH_CAVE):
        memcpy(m_szPlaceName, TEXT("냉기 서린 동굴"), sizeof(m_szPlaceName));
        break;
    case static_cast<_int>(BLADENEXUS_ID::HEINMACH_CLIFF):
        memcpy(m_szPlaceName, TEXT("설인의 대지"), sizeof(m_szPlaceName));
        break;
    case static_cast<_int>(BLADENEXUS_ID::HEINMACH_YETUGA):
    {
        memcpy(m_szPlaceName, TEXT("물음표"), sizeof(m_szPlaceName));

        // 예투가 죽었다는 이벤트 받으면 위치 옮길 예정 ( 아래에서 뿅 )

        break;
    }
    case static_cast<_int>(BLADENEXUS_ID::EMBARS_UNDER):
        memcpy(m_szPlaceName, TEXT("잊혀진 사원의 지하"), sizeof(m_szPlaceName));
        break;
    case static_cast<_int>(BLADENEXUS_ID::EMBARS_DEEP):
        memcpy(m_szPlaceName, TEXT("잊혀진 사원의 깊은 곳"), sizeof(m_szPlaceName));
        break;
    case static_cast<_int>(BLADENEXUS_ID::EMBARS_CORE):
        memcpy(m_szPlaceName, TEXT("잊혀진 사원의 심장"), sizeof(m_szPlaceName));
        break;
    }

    return S_OK;
}

HRESULT CBladeNexus::Bind_Materials(_uint iMeshIndex)
{
    _bool isDiffuse = { false };
    _bool isNormal = { false };
    _bool isEmissive = { false };
    _bool isSpecular = { false };
    _bool isMetalic = { false };
    _bool isRoughness = { false };

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        isDiffuse = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        isNormal = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        isEmissive = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        isSpecular = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", iMeshIndex, aiTextureType_METALNESS, 0)))
        isMetalic = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", iMeshIndex, aiTextureType_SHININESS, 0)))
        isRoughness = true;

    m_pShaderCom->Bind_RawValue("g_isDiffuse", &isDiffuse, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isSpecular", &isSpecular, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isMetalic", &isMetalic, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isRoughness", &isRoughness, sizeof(_bool));

    return S_OK;
}

void CBladeNexus::Input_Interact_Event(_float fTimeDelta)
{
    if (ANIM_STATE::AFTER_START == m_eAnimState || ANIM_STATE::AFTER_LOOP == m_eAnimState|| ANIM_STATE::AFTER_END == m_eAnimState ||
        ANIM_STATE::BEFORE_START == m_eAnimState || ANIM_STATE::BEFORE_LOOP == m_eAnimState|| ANIM_STATE::BEFORE_END == m_eAnimState)
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

        InteractType.eInteractType = INTERACTIVE_TYPE::CHECKPOINT;

        InteractType.eState = EventInteractType::BEGIN;

        EventBladeNexus BNEvent = {};

        XMStoreFloat4(&BNEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        InteractType.BNEvent = BNEvent;

        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
    }
}

void CBladeNexus::Animation_Update(_float fTimeDelta)
{
    if (ANIM_STATE::BEFORE_IDLE != m_eAnimState && ANIM_STATE::BEFORE_START != m_eAnimState)
    {
        _vector vPos = m_pTargetCom->Get_State(STATE::POSITION);
        vPos.m128_f32[1] = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];

        m_pTransformCom->LookAt_Lerp(vPos, fTimeDelta, 2.f);
    }

    if (false == m_isCollision)
        return;

    Input_Interact_Event(fTimeDelta);

    if (m_Event.isOn())               // 켠다는 신호
    {
        // 해금 전 IDLE 상태
        if (ANIM_STATE::BEFORE_IDLE == m_eAnimState)
        {
            m_pGuide->Update_Visible(false);

            // 처음 상호 작용 시
            m_eAnimState = ANIM_STATE::BEFORE_START;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);

            m_isUnLock = true;

            EventInteractType InteractType = {};

            InteractType.eInteractType = INTERACTIVE_TYPE::CHECKPOINT;
            InteractType.isEvent = true;

            EventBladeNexus BNEvent = {};

            XMStoreFloat4(&BNEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
            BNEvent.isUnLock = true;
            BNEvent.isBNOpened = false;

            InteractType.BNEvent = BNEvent;

            // 귀검을 바라볼 수 있도록 포지션만 던짐 ( 귀검 애니메이션 아직 종료 X )
            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
        }
        // 해금 후 IDLE 상태
        else if (ANIM_STATE::AFTER_IDLE == m_eAnimState)
        {
            m_pGuide->Update_Visible(false);

            // 2번 이상의 상호 작용 시
            m_eAnimState = ANIM_STATE::AFTER_START;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);

            EventInteractType InteractType = {};

            InteractType.eInteractType = INTERACTIVE_TYPE::CHECKPOINT;
            InteractType.isEvent = true;

            EventBladeNexus BNEvent = {};

            XMStoreFloat4(&BNEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
            BNEvent.isUnLock = false;
            BNEvent.isBNOpened = false;

            InteractType.BNEvent = BNEvent;

            // 귀검을 바라볼 수 있도록 포지션만 던짐 ( 귀검 애니메이션 아직 종료 X )
            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
        }
    }
    else if (m_Event.isOff())         // 끈다는 신호 ( 내가 받기만 하면 됨
    {
        if (ANIM_STATE::BEFORE_LOOP == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::BEFORE_END;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
        }
        if (ANIM_STATE::AFTER_LOOP == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::AFTER_END;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
        }
    }
}

void CBladeNexus::Animation_Change(_float fTimeDelta)
{
    // 귀검 가동 끝나면 ( 첫 해금 O )
    if (ANIM_STATE::BEFORE_START == m_eAnimState)       // BEFORE_START 가 끝나면 BEFORE_LOOP ( 플레이어가 UI랑 상호 작용 )
    {
        // 귀검 애니메이션 끝나면 귀검 UI 창 팝업
        static_cast<CUI_BladeNexus*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("BladeNexus")))->On_Panel(CUI_BladeNexus::ONTYPE::DEFAULT, m_szPlaceName);

        // 처음 상호 작용 후 애니메이션 루프로 전환 및 이벤트 발생
        m_eAnimState = ANIM_STATE::BEFORE_LOOP;
        m_pModelCom->Set_Animation(ANIM_STATE::BEFORE_LOOP);
        m_pModelCom->Set_AnimationLoop(true);

        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::CHECKPOINT;
        InteractType.isEvent = true;

        EventBladeNexus BNEvent = {};

        XMStoreFloat4(&BNEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
        BNEvent.isUnLock = true;
        BNEvent.isBNOpened = true;              // 이제 귀검 UI 열리게

        InteractType.BNEvent = BNEvent;

        // 귀검을 바라볼 수 있도록 포지션만 던짐 ( 귀검 애니메이션 종료 O, UI 창 팝업? )
        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

        m_Event.None();

        CClientInstance::GetInstance()->Unlock_BladeNexus(static_cast<_uint>(m_iBladeNexus_ID));
    }
    // 귀검 상호 작용 종료 후 ( 첫 해금 O )
    if (ANIM_STATE::BEFORE_END == m_eAnimState)
    {
        if (true == m_isCollision)
            m_pGuide->Update_Visible(true);

        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::AFTER_IDLE;
        m_pModelCom->Set_Animation(ANIM_STATE::AFTER_IDLE);
        m_pModelCom->Set_AnimationLoop(true);

        m_Event.None();

        // 첫 해금 후 접촉 -> 결속 으로 변경
        m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1] + 1.f), TEXT("결속"), 1.5f);
    }
    // 귀검 가동 끝나면 ( 첫 해금 X )
    if (ANIM_STATE::AFTER_START == m_eAnimState)
    {
        // 귀검 애니메이션 끝나면 귀검 UI 창 팝업
        static_cast<CUI_BladeNexus*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("BladeNexus")))->On_Panel(CUI_BladeNexus::ONTYPE::DEFAULT, TEXT("하인마흐 구석진 으슥한 어떠한 곳"));

        // 다회 상호 작용 후 애니메이션 루프로 전환
        m_eAnimState = ANIM_STATE::AFTER_LOOP;
        m_pModelCom->Set_Animation(ANIM_STATE::AFTER_LOOP);
        m_pModelCom->Set_AnimationLoop(true);

        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::CHECKPOINT;
        InteractType.isEvent = true;

        EventBladeNexus BNEvent = {};

        XMStoreFloat4(&BNEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
        BNEvent.isUnLock = false;
        BNEvent.isBNOpened = true;              // 이제 귀검 UI 열리게

        InteractType.BNEvent = BNEvent;

        // 귀검을 바라볼 수 있도록 포지션만 던짐 ( 귀검 애니메이션 종료 O, UI 창 팝업? )
        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

        m_Event.None();
    }
    // 귀검 상호 작용 종료 후 ( 첫 해금 X )
    if (ANIM_STATE::AFTER_END == m_eAnimState)
    {
        if (true == m_isCollision)
            m_pGuide->Update_Visible(true);

        // 다회 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::AFTER_IDLE;
        m_pModelCom->Set_Animation(ANIM_STATE::AFTER_IDLE);
        m_pModelCom->Set_AnimationLoop(true);

        m_Event.None();
    }
}

void CBladeNexus::Find_Target()
{
    CHECK_TRUE(m_isFindTarget, );

    LEVEL eLevel = CClientInstance::GetInstance()->Get_CurrLevel();
    m_pTargetCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(ENUM_CLASS(eLevel), TEXT("Layer_Creature_Player"), TEXT("Com_Transform")));
    CHECK_NULLPTR(m_pTargetCom, );

    m_isFindTarget = true;
}

void CBladeNexus::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
     if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA))
        return;

    if (ANIM_STATE::AFTER_IDLE == m_eAnimState || ANIM_STATE::BEFORE_IDLE == m_eAnimState)
        m_pGuide->Update_Visible(true);

    m_isCollision = true;
}

void CBladeNexus::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA))
        return;

    m_isCollision = true;
}

void CBladeNexus::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA))
        return;

    m_pGuide->Update_Visible(false);

    m_isCollision = false;
}

CBladeNexus* CBladeNexus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBladeNexus* pInstance = new CBladeNexus(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBladeNexus"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBladeNexus::Clone(void* pArg)
{
    CBladeNexus* pInstance = new CBladeNexus(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBladeNexus"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBladeNexus::Free()
{
    __super::Free();

    Safe_Release(m_pStaticCom);
    Safe_Release(m_pTriggerCom);

    if (nullptr != m_pGuide)
    {
        m_pGuide->Set_IsDead(true);
        Safe_Release(m_pGuide);
    }
}
