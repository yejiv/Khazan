#include "ElevatorL.h"

#include "GameInstance.h"

#include "Elevator_Inner.h"
#include "Elevator_Mid.h"
#include "Elevator_Outer.h"
#include "Slate_Switch.h"

CElevatorL::CElevatorL(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CElevatorL::CElevatorL(const CElevatorL& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CElevatorL::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CElevatorL::Initialize_Clone(void* pArg)
{
    LARGE_ELEVATOR_DESC* pDesc = static_cast<LARGE_ELEVATOR_DESC*>(pArg);
    CHECK_NULLPTR(pArg, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    LARGE_ELEVATOR_POS* pElevatorPos = static_cast<LARGE_ELEVATOR_POS*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pElevatorPos, E_FAIL);

    m_vUpPos = pElevatorPos->vUp;
    m_vMidPos = pElevatorPos->vMid;
    m_vDownPos = pElevatorPos->vDown;

    m_eAnimState = ANIM_STATE::ALL;
    m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->AnimationLoop(true);

    m_pBodyCom->Collision_Active(false);

    m_pModelCom->Set_AnimationBlend(true);

    m_iEventID = m_pGameInstance->Subscribe_Event<EventHallElevator>(ENUM_CLASS(EVENT_TYPE::HALL_ELEVATOR_UNLOCK), [&](const EventHallElevator& e) { m_Event = e; });
    m_iSkipEventID = m_pGameInstance->Subscribe_Event<EventElevatorSkip>(ENUM_CLASS(EVENT_TYPE::ELEVATOR_SKIP), [&](const EventElevatorSkip& e) { m_SkipEvent = e; });
    return S_OK;
}
 
void CElevatorL::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CElevatorL::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        Animation_Change(fTimeDelta);
    }

    VerticalOnTime_Update(fTimeDelta);

    __super::Update(fTimeDelta);
    m_pBodyCom->Sync_Update(m_pTransformCom);
    m_pTriggerCom->Sync_Update(m_pTransformCom);

    m_pBodyCom->Update(fTimeDelta, m_pTransformCom);
    m_pTriggerCom->Update(fTimeDelta, m_pTransformCom);
}

void CElevatorL::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);

    __super::Late_Update(fTimeDelta);
}

HRESULT CElevatorL::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CLever : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    // 이미시브
    _float fEmissiveIntensity = 10.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(14), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

void CElevatorL::Lerp_ElevatorMove(_float fTimeDelta, _float4 vStartPos, _float4 vTargetPos, _float fDuration)
{
    _float4 vPos = vTargetPos;

    m_fTimeAcc += fTimeDelta;

    _float fLerpTime = m_fTimeAcc / fDuration;

    _float4 vLerpPos = Lerp(vStartPos, vPos, fLerpTime);

    _float fDistance = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vPos) - XMLoadFloat4(&vLerpPos)));

    if (0.1f > fDistance)
    {
        m_fTimeAcc = 0.f;

        switch (m_eMoveState)
        {
        case MOVE_STATE::MIDTODOWN:
        {
            m_eMoveState = MOVE_STATE::DOWN;
            m_pBodyCom->Collision_Active(true);

            EventGimmick Gimmick = {};
            Gimmick.Set_GimmickClear();
            Gimmick.Set_ActiveGate();
            m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(m_eGimmickType), Gimmick);
            break;
        }
        case MOVE_STATE::DOWNTOUP:
            m_eMoveState = MOVE_STATE::UP;

            m_isActiveElevator = false;

            m_isAvailableSwitch = true;
            break;
        default:
            break;
        }
    }
    else
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vLerpPos));
    }
}

HRESULT CElevatorL::Ready_Components(void* pArg)
{
    LARGE_ELEVATOR_DESC* pDesc = static_cast<LARGE_ELEVATOR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CElevatorL::Bind_Materials(_uint iMeshIndex)
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

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

HRESULT CElevatorL::Ready_PartObjects(void* pArg)
{
    LARGE_ELEVATOR_DESC* pDesc = static_cast<LARGE_ELEVATOR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CElevator_Inner::ELEVATOR_INNER_DESC InnerDesc = {};

    InnerDesc.eLevel = eLevel;
    InnerDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    InnerDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Dummy003");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Inner"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Elevator_Inner"), &InnerDesc), E_FAIL);

    CElevator_Mid::ELEVATOR_MID_DESC MidDesc = {};

    MidDesc.eLevel = eLevel;
    MidDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    MidDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Dummy002");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Mid"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Elevator_Mid"), &MidDesc), E_FAIL);

    CElevator_Outer::ELEVATOR_OUTER_DESC OuterDesc = {};

    OuterDesc.eLevel = eLevel;
    OuterDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    OuterDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Dummy001");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Outer"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Elevator_Outer"), &OuterDesc), E_FAIL);

    CSlate_Switch::SLATE_SWITCH_DESC SlateDesc = {};

    SlateDesc.eLevel = eLevel;
    SlateDesc.eType = CSlate_Switch::ELEVATOR_TYPE::LARGE;
    SlateDesc.pActiveElevator = &m_isActiveElevator;
    SlateDesc.pAvailableSwitch = &m_isAvailableSwitch;
    SlateDesc.pSwitchPressed = &m_isSwitchPressed;
    SlateDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    SlateDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Point_Elevator_Button");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Button"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Slate_Switch"), &SlateDesc), E_FAIL);

    return S_OK;
}

HRESULT CElevatorL::Ready_Collision(void* pArg)
{
#pragma region 스태틱 몸체
    CBody::BODY_BOXSHAPE_DESC BodyDesc{};
    BodyDesc.vExtent = _float3(17.f, 1.2f, 17.f);
    BodyDesc.bIsTrigger = false;
    BodyDesc.bStartActive = true;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::LinearCast;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.fFriction = 0.8f;
    BodyDesc.fMass = 1.0f;
    BodyDesc.fRestitution = 0.0f;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_MOVE_PLATFORM);

    XMStoreFloat3(&BodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));

    XMStoreFloat4(&BodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    BodyDesc.vShapeOffset = _float3(0.f, -0.95f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;
#pragma endregion

#pragma region 트리거 영역
    CBody::BODY_BOXSHAPE_DESC TriggerDesc{};
    TriggerDesc.vExtent = _float3(0.5f, 1.8f, 0.5f);
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

void CElevatorL::Animation_Update(_float fTimeDelta)
{
    if (ANIM_STATE::IDLE != m_eAnimState && true == m_SkipEvent.isSkip)
    {
        Gimmick_Event_Skip(fTimeDelta);
    }
    else if (true == m_Event.isEvent()) // m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_H) && ANIM_STATE::IDLE != m_eAnimState) // 어떤 조건이 들어오면 애니메이션 Loop 중단 후 슥슥 샥샥
    {
        m_Event.EventOff();

        m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        *m_pModelCom->Get_CurTrackPosition() = 50.f;
        m_isAnimChange = true;
        m_pModelCom->AnimationLoop(false);
    }
    else if (ANIM_STATE::IDLE == m_eAnimState)
    {
        if (m_eMoveState == MOVE_STATE::MID)
        {
            m_eMoveState = MOVE_STATE::MIDTODOWN;
        }

        if (m_isSwitchPressed && m_eMoveState == MOVE_STATE::DOWN)
        {
            m_isSwitchPressed = false;
            m_eMoveState = MOVE_STATE::DOWNTOUP;
        }

        if (m_Event.IsThirdStep()) // m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_L)) // 아이들 상태 되면 이제 슥슥 이동을 시작
        {
            if (MOVE_STATE::MID == m_eMoveState)
            {
                m_eMoveState = MOVE_STATE::MIDTODOWN;
            }
        }
        switch (m_eMoveState)
        {
        case MOVE_STATE::MIDTODOWN:
            Lerp_ElevatorMove(fTimeDelta, m_vMidPos, m_vDownPos, 20.f);
            break;
        case MOVE_STATE::DOWNTOUP:
            if (true == m_isActiveElevator)
                Lerp_ElevatorMove(fTimeDelta, m_vDownPos, m_vUpPos, 30.f);
            break;
        default:
            break;
        }
    }
}

void CElevatorL::Animation_Change(_float fTimeDelta)
{
    if (ANIM_STATE::INNER_STOPPING == m_eAnimState)
    {
        m_fLimitTimeAcc = 0.f;

        m_isAnimChange = false;

        m_eAnimState = ANIM_STATE::OUTER_STOPPING;
        m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        m_pModelCom->AnimationLoop(true);

        m_isVerticalActive = true;
        m_eGimmickType = EVENT_TYPE::EMBARS_GIMMICK1;

        EventGimmick Gimmick = {};
        Gimmick.Set_GimmickClear();
        m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(EVENT_TYPE::EMBARS_GIMMICK1), Gimmick);
    }
    else if (true == m_isAnimChange)
    {
        m_fLimitTimeAcc = 0.f;

        m_isAnimChange = false;

        switch (m_eAnimState)
        {
        case ANIM_STATE::ALL:
        {
            m_eAnimState = ANIM_STATE::MID_STOP;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->AnimationLoop(true);

            m_isVerticalActive = true;
            m_eGimmickType = EVENT_TYPE::EMBARS_GIMMICK0;

            EventGimmick Gimmick = {};
            Gimmick.Set_GimmickClear();
            m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(EVENT_TYPE::EMBARS_GIMMICK0), Gimmick);
            break;
        }
        case ANIM_STATE::MID_STOP:
        {
            m_eAnimState = ANIM_STATE::INNER_STOPPING;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->AnimationLoop(false);
            break;
        }
        case ANIM_STATE::OUTER_STOPPING:
        {
            m_eAnimState = ANIM_STATE::IDLE;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->AnimationLoop(true);

            m_eGimmickType = EVENT_TYPE::EMBARS_GIMMICK2;

            EventGimmick Gimmick = {};
            Gimmick.Set_GimmickClear();
            m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(EVENT_TYPE::EMBARS_GIMMICK2), Gimmick);
            break;
        }
        }
    }
}

void CElevatorL::Collision_Enter(COLLISION_DESC * pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (pMyDesc->iObjectLayer == ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT))
    {
        if (MOVE_STATE::UP == m_eMoveState)
            return;

        if (false == m_isSwitchPressed)
        {
            m_isSwitchPressed = true;
        }
    }
}

void CElevatorL::Collision_Stay(COLLISION_DESC * pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;


}

void CElevatorL::Collision_Exit(COLLISION_DESC * pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    m_isCollision = false;
}

void CElevatorL::VerticalOnTime_Update(_float fTimeDelta)
{
    if (true == m_isVerticalActive)
    {
        m_fVerticalTimeAcc += fTimeDelta;

        if (4.f < m_fVerticalTimeAcc)
        {
            m_fVerticalTimeAcc = 0.f;
            m_isVerticalActive = false;

            EventGimmick Gimmick = {};
            Gimmick.Set_GimmickClear();
            Gimmick.Set_ActiveGate();
            m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(m_eGimmickType), Gimmick);
        }
    }
}

void CElevatorL::Gimmick_Event_Skip(_float fTimeDelta)
{
    if (ANIM_STATE::IDLE == m_eAnimState)
        return;

    m_SkipEvent.isSkip = false;

    m_Event.EventOff();

    m_isAnimChange = false;
    m_fVerticalTimeAcc = 3.9f;
    m_isVerticalActive = true;

    switch (m_Event.eStep)
    {
    case EventHallElevator::UNLOCK_STATE::STEP_1:
    {
        m_eAnimState = ANIM_STATE::MID_STOP;
        m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        m_pModelCom->AnimationLoop(true);

        m_eGimmickType = EVENT_TYPE::EMBARS_GIMMICK0;

        EventGimmick Gimmick = {};
        Gimmick.Set_GimmickClear();
        m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(EVENT_TYPE::EMBARS_GIMMICK0), Gimmick);
        break;
    }
    case EventHallElevator::UNLOCK_STATE::STEP_2:
    {
        m_eAnimState = ANIM_STATE::OUTER_STOPPING;
        m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        m_pModelCom->AnimationLoop(true);

        m_eGimmickType = EVENT_TYPE::EMBARS_GIMMICK1;

        EventGimmick Gimmick = {};
        Gimmick.Set_GimmickClear();
        m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(EVENT_TYPE::EMBARS_GIMMICK1), Gimmick);
        break;
    }
    case EventHallElevator::UNLOCK_STATE::STEP_3:
    {
        m_pModelCom->AnimationLoop(false);

        m_isAnimChange = true;

        m_eGimmickType = EVENT_TYPE::EMBARS_GIMMICK2;

        EventGimmick Gimmick = {};
        Gimmick.Set_GimmickClear();
        m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(EVENT_TYPE::EMBARS_GIMMICK2), Gimmick);
        break;
    }
    }
}

CElevatorL* CElevatorL::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CElevatorL* pInstance = new CElevatorL(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CElevatorL"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CElevatorL::Clone(void* pArg)
{
    CElevatorL* pInstance = new CElevatorL(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CElevatorL"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CElevatorL::Free()
{
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::HALL_ELEVATOR_UNLOCK), m_iEventID);
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::ELEVATOR_SKIP), m_iSkipEventID);

    __super::Free();

    Safe_Release(m_pBodyCom);
    Safe_Release(m_pTriggerCom);
}
