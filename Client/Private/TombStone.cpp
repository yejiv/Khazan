#include "TombStone.h"

#include "GameInstance.h"

#include "Interaction_Guide.h"

#include "ClientInstance.h"
#include "UI_BladeNexus.h"

CTombStone::CTombStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CTombStone::CTombStone(const CTombStone& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CTombStone::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CTombStone::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    CHECK_FAILED(Ready_Interaction_Guide(pArg), E_FAIL);

    CHECK_FAILED(Ready_DefaultSetting(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::BEFORE_IDLE;
    m_pModelCom->Set_Animation(ANIM_STATE::BEFORE_IDLE);
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(false);
    m_pModelCom->Play_Animation(0.f);
    m_pModelCom->Set_AnimationBlend(true);

    m_iEventID = m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e)
        {
            m_Event = e;
        });

    return S_OK;
}

void CTombStone::Priority_Update(_float fTimeDelta)
{
    if (false == m_isCollision)
    {
        m_Event.None();
    }
}

void CTombStone::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);
}

void CTombStone::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CTombStone::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        /*
        if (1 == i)     // 1 == 룬문자
        {
            _bool isEmissive = { true };
            m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_SPECULAR, 0);
            m_pShaderCom->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));

            m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &m_fEmissiveIntensity, sizeof(_float));
            m_pShaderCom->Bind_RawValue("g_isEnableEmissive", &m_isEnableEmissive, sizeof(_bool));
            m_pShaderCom->Bind_RawValue("g_isEnableBloom", &m_isEnableBloom, sizeof(_bool));
        }
        */

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CTombStone::Ready_Components(void* pArg)
{
    TOMBSTONE_DESC* pDesc = static_cast<TOMBSTONE_DESC*>(pArg);
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

HRESULT CTombStone::Ready_Collision(void* pArg)
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
    TriggerDesc.vExtent = _float3(1.6f, 1.f, 1.6f);
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
    //pCollDesc.pInfo = ?? // 작성하기
    TriggerDesc.pCollisionDesc = &m_TriggerCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger"), reinterpret_cast<CComponent**>(&m_pTriggerCom), &TriggerDesc)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CTombStone::Ready_Interaction_Guide(void* pArg)
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1] + 1.f), TEXT("샤르나크 산맥 일대"), 1.5f);

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

HRESULT CTombStone::Ready_DefaultSetting(void* pArg)
{
    TOMBSTONE_DESC* pDesc = static_cast<TOMBSTONE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    _int* pTombStoneID = static_cast<_int*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pTombStoneID, E_FAIL);

    m_iTombStoneID = *pTombStoneID;

    return S_OK;
}

HRESULT CTombStone::Bind_Materials(_uint iMeshIndex)
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

void CTombStone::Input_Interact_Event(_float fTimeDelta)
{
    if (ANIM_STATE::AFTER_START == m_eAnimState || ANIM_STATE::AFTER_LOOP == m_eAnimState || ANIM_STATE::AFTER_END == m_eAnimState ||
        ANIM_STATE::BEFORE_IDLE == m_eAnimState || ANIM_STATE::BEFORE_START == m_eAnimState)
        return;

    _bool isPressing = { false };

    if (m_pGameInstance->Key_Pressing(DIK_F, fTimeDelta))
    {
        isPressing = m_pGuide->IsPressing();
    }

    if (true == isPressing)
    {
        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::TOMBSTONE;

        InteractType.eState = EventInteractType::BEGIN;

        EventTombStone TSEvent = {};

        _matrix OffSetMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("IA_BeginLoc")) * m_pTransformCom->Get_WorldMatrix();

        XMStoreFloat4(&TSEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat4(&TSEvent.vPlayerPosition, OffSetMatrix.r[3]);

        InteractType.TSEvent = TSEvent;

        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
    }
}

void CTombStone::Animation_Update(_float fTimeDelta)
{
    // 경계의 틈 진입 후 일정 시간 후 AFTER 상태로 변경
    if (ANIM_STATE::BEFORE_IDLE == m_eAnimState)
    {
        m_fTimeAcc += fTimeDelta;

        if (10.f <= m_fTimeAcc)
        {
            m_eAnimState = ANIM_STATE::BEFORE_START;
            m_pModelCom->Set_Animation(m_eAnimState);
            m_pModelCom->Set_AnimationLoop(false);
        }

        return;
    }

    if (false == m_isCollision)
        return;

    Input_Interact_Event(fTimeDelta);

    if (m_Event.isOn())               // 켠다는 신호
    {
        // IDLE 상태
        if (ANIM_STATE::AFTER_IDLE == m_eAnimState)
        {
            m_pGuide->Update_Visible(false);

            m_fEmissiveIntensity = 1.5f;

            m_eAnimState = ANIM_STATE::AFTER_START;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);

            EventInteractType InteractType = {};

            InteractType.eInteractType = INTERACTIVE_TYPE::TOMBSTONE;
            InteractType.isEvent = true;

            EventTombStone TSEvent = {};

            _matrix OffSetMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("IA_BeginLoc")) * m_pTransformCom->Get_WorldMatrix();

            XMStoreFloat4(&TSEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
            XMStoreFloat4(&TSEvent.vPlayerPosition, OffSetMatrix.r[3]);
            TSEvent.isTSOpened = false;

            InteractType.TSEvent = TSEvent;

            // 툼스톤을 바라볼 수 있도록 포지션만 던짐 ( 툼스톤 애니메이션 아직 종료 X )
            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
        }
    }
    else if (m_Event.isOff())         // 끈다는 신호 ( 내가 받기만 하면 됨
    {
        if (ANIM_STATE::AFTER_LOOP == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::AFTER_END;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
        }
    }
}

void CTombStone::Animation_Change(_float fTimeDelta)
{
    // 툼스톤 가동 끝나면 ( 일정 시간 이후 )
    if (ANIM_STATE::BEFORE_START == m_eAnimState)       // BEFORE_START 가 끝나면 AFTER_IDLE
    {
        // 경계의 틈 진입 후 일정 시간 후 BEFORE_START 실행 -> 종료 후 AFTER_IDLE 로 변경
        m_eAnimState = ANIM_STATE::AFTER_IDLE;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(true);

        m_fEmissiveIntensity = 1.f;
    }
    // 툼스톤 가동 끝나면
    if (ANIM_STATE::AFTER_START == m_eAnimState)
    {
        // 툼스톤 애니메이션 끝나면 툼스톤 UI 창 팝업
        static_cast<CUI_BladeNexus*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("BladeNexus")))->On_Panel(CUI_BladeNexus::ONTYPE::DEFAULT, TEXT("하인마흐 구석진 으슥한 어떠한 곳"));

        // 애니메이션 루프로 전환
        m_eAnimState = ANIM_STATE::AFTER_LOOP;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(true);

        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::TOMBSTONE;
        InteractType.isEvent = true;

        EventTombStone TSEvent = {};

        XMStoreFloat4(&TSEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
        TSEvent.isTSOpened = true;              // 이제 툼스톤 UI 열리게

        InteractType.TSEvent = TSEvent;

        // 툼스톤을 바라볼 수 있도록 포지션만 던짐 ( 툼스톤 애니메이션 종료 O, UI 창 팝업? )
        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

        m_Event.None();
    }
    // 툼스톤 상호 작용 종료 후 ( 첫 해금 X )
    if (ANIM_STATE::AFTER_END == m_eAnimState)
    {
        if (true == m_isCollision)
            m_pGuide->Update_Visible(true);

        m_fEmissiveIntensity = 1.f;

        // 다회 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::AFTER_IDLE;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(true);

        m_Event.None();
    }
}

void CTombStone::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (ANIM_STATE::AFTER_IDLE == m_eAnimState)
        m_pGuide->Update_Visible(true);

    m_isCollision = true;
}

void CTombStone::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    m_isCollision = true;
}

void CTombStone::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    m_pGuide->Update_Visible(false);

    m_isCollision = false;
}

CTombStone* CTombStone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTombStone* pInstance = new CTombStone(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTombStone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTombStone::Clone(void* pArg)
{
    CTombStone* pInstance = new CTombStone(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CTombStone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTombStone::Free()
{
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), m_iEventID);

    __super::Free();

    Safe_Release(m_pStaticCom);
    Safe_Release(m_pTriggerCom);

    if (nullptr != m_pGuide)
    {
        m_pGuide->Set_IsDead(true);
        m_pGuide = nullptr;
    }
}
