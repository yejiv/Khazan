#include "BigChest.h"

#include "GameInstance.h"

#include "Interaction_Guide.h"

CBigChest::CBigChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CBigChest::CBigChest(const CBigChest& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CBigChest::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CBigChest::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

#pragma region 범수 상호작용 슥슥

    CHECK_FAILED(Ready_Interaction_Guide(pArg), E_FAIL);

#pragma endregion

    PROP_INTERACTIVE_DESC* pDesc = static_cast<PROP_INTERACTIVE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    BOX_ITEMS* pItemBox = static_cast<BOX_ITEMS*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pItemBox, E_FAIL);

    m_Items = *pItemBox;

    m_eAnimState = ANIM_STATE::CLOSE;
    m_pModelCom->Set_Animation(ANIM_STATE::CLOSE);
    m_pModelCom->Set_AnimationLoop(true);

    m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e)
        {
            m_Event = e;
        });

    return S_OK;
}

void CBigChest::Priority_Update(_float fTimeDelta)
{
    if (false == m_isCollision)
    {
        m_Event.None();
    }
}

void CBigChest::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);
}

void CBigChest::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CBigChest::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(0), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CBigChest::Ready_Components(void* pArg)
{
    BIGCHEST_DESC* pDesc = static_cast<BIGCHEST_DESC*>(pArg);
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

HRESULT CBigChest::Ready_Collision(void* pArg)
{
#pragma region 스태틱 몸체
    CBody::BODY_BOXSHAPE_DESC StaticBodyDesc{};
    StaticBodyDesc.vExtent = _float3(1.f, 0.5f, 0.5f);
    StaticBodyDesc.bIsTrigger = false;
    StaticBodyDesc.bStartActive = true;
    StaticBodyDesc.eMotion = EMotionType::Static;
    StaticBodyDesc.eQuality = EMotionQuality::LinearCast;
    StaticBodyDesc.eShapeType = SHAPE::BOX;
    StaticBodyDesc.fFriction = 0.8f;
    StaticBodyDesc.fMass = 1.0f;
    StaticBodyDesc.fRestitution = 0.0f;
    StaticBodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_STATIC);
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
    TriggerDesc.vExtent = _float3(0.75f, 0.5f, 0.75f);
    TriggerDesc.bIsTrigger = true;
    TriggerDesc.bStartActive = true;
    TriggerDesc.eMotion = EMotionType::Kinematic;
    TriggerDesc.eQuality = EMotionQuality::LinearCast;
    TriggerDesc.eShapeType = SHAPE::BOX;
    TriggerDesc.fFriction = 0.8f;
    TriggerDesc.fMass = 1.0f;
    TriggerDesc.fRestitution = 0.0f;
    TriggerDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT);

    XMStoreFloat3(&TriggerDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * 1.f);
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

HRESULT CBigChest::Ready_Interaction_Guide(void* pArg)
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    Safe_AddRef(m_pGuide);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, 10.f), TEXT("열기"), 1.f);

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

void CBigChest::Input_Interact_Event(_float fTimeDelta)
{
    if (ANIM_STATE::OPENING == m_eAnimState || ANIM_STATE::CLOSING == m_eAnimState)
        return;

    _bool isPressing = { false };

    if (m_pGameInstance->Key_Pressing(DIK_F, fTimeDelta))
    {
        isPressing = m_pGuide->IsPressing();
    }
    else if (m_pGameInstance->Key_Down(DIK_LCONTROL))
    {
        EventInteractType InteractType = {};

        InteractType.eState = EventInteractType::END;

        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

        return;
    }

    if (true == isPressing)
    {
        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::CHEST;

        InteractType.eState = EventInteractType::BEGIN;

        EventChest ChestEvent = {};

        _matrix OffSetMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Position_Ch")) * m_pTransformCom->Get_WorldMatrix();

        XMStoreFloat3(&ChestEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat3(&ChestEvent.vPlayerPosition, OffSetMatrix.r[3]);

        InteractType.ChestEvent = ChestEvent;

        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
    }
}

void CBigChest::Animation_Update(_float fTimeDelta)
{
    if (false == m_isCollision)
        return;

    Input_Interact_Event(fTimeDelta);

    if (m_Event.isOn())               // 켠다는 신호
    {
        if (ANIM_STATE::CLOSE == m_eAnimState)
        {
            m_pGuide->Update_Visible(false);

            // 닫긴 상자 상호 작용 시
            m_eAnimState = ANIM_STATE::OPENING;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);

            EventInteractType InteractType = {};

            InteractType.eInteractType = INTERACTIVE_TYPE::CHEST;
            InteractType.isEvent = true;

            EventChest ChestEvent = {};

            _matrix OffSetMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Position_Ch")) * m_pTransformCom->Get_WorldMatrix();

            XMStoreFloat3(&ChestEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
            XMStoreFloat3(&ChestEvent.vPlayerPosition, OffSetMatrix.r[3]);
            ChestEvent.Items = m_Items;
            ChestEvent.isChestOpened = false;

            InteractType.ChestEvent = ChestEvent;

            // OPENING 중에는 UI, Player 용 Active 변수는 false, 상자 앞 위치랑 상자 위치 던지기
            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
        }
    }
    else if (m_Event.isOff())         // 끈다는 신호 ( 내가 받기만 하면 됨
    {
        if (ANIM_STATE::OPEN == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::CLOSING;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
        }
    }
}

void CBigChest::Animation_Change(_float fTimeDelta)
{
    if (ANIM_STATE::OPENING == m_eAnimState)
    {
        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::OPEN;
        m_pModelCom->Set_Animation(ANIM_STATE::OPEN);
        m_pModelCom->Set_AnimationLoop(true);

        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::CHEST;
        InteractType.isEvent = true;

        EventChest ChestEvent = {};

        _matrix OffSetMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Position_Ch")) * m_pTransformCom->Get_WorldMatrix();

        XMStoreFloat3(&ChestEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat3(&ChestEvent.vPlayerPosition, OffSetMatrix.r[3]);
        ChestEvent.Items = m_Items;
        ChestEvent.isChestOpened = true;

        InteractType.ChestEvent = ChestEvent;

        // OPENING 중에는 UI, Player 용 Active 변수는 false, 상자 앞 위치랑 상자 위치 던지기
        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
    }
    if (ANIM_STATE::CLOSING == m_eAnimState)
    {
        if (true == m_isCollision)
            m_pGuide->Update_Visible(true);

        // 처음 상호 작용 후 애니메이션 루프로 전환
        m_eAnimState = ANIM_STATE::CLOSE;
        m_pModelCom->Set_Animation(ANIM_STATE::CLOSE);
        m_pModelCom->Set_AnimationLoop(true);
    }
}

void CBigChest::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if(ANIM_STATE::CLOSE == m_eAnimState)
        m_pGuide->Update_Visible(true);

    m_isCollision = true;
}

void CBigChest::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    m_isCollision = true;
}

void CBigChest::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
    m_pGuide->Update_Visible(false);

    m_isCollision = false;
}

CBigChest* CBigChest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBigChest* pInstance = new CBigChest(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBigChest"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBigChest::Clone(void* pArg)
{
    CBigChest* pInstance = new CBigChest(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBigChest"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBigChest::Free()
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
