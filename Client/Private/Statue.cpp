#include "Statue.h"

#include "GameInstance.h"

#include "Interaction_Guide.h"

#include "Statue_Plate.h"
#include "Statue_Deco.h"
#include "Effect_Prefab.h"

CStatue::CStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CStatue::CStatue(const CStatue& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CStatue::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CStatue::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    PROP_INTERACTIVE_DESC* pDesc = static_cast<PROP_INTERACTIVE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    STATUE_DESC* pStatueDesc = static_cast<STATUE_DESC*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pStatueDesc, E_FAIL);

    m_iUnLockRotation = pStatueDesc->StatueRotation.iUnLockRotation;
    m_iEventID = pStatueDesc->iEventID;
    m_iStatueIndex = pStatueDesc->iStatueIndex;

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

#pragma region 범수 상호작용 슥슥

    CHECK_FAILED(Ready_Interaction_Guide(pArg), E_FAIL);

#pragma endregion

    m_eAnimState = ANIM_STATE::IDLE_0;
    m_pModelCom->Set_Animation(ENUM_CLASS((ANIM_STATE::IDLE_0)));
    m_pModelCom->Set_AnimationLoop(false);
    m_pModelCom->Set_AnimationBlend(false);
    m_pModelCom->Play_Animation(0.f);
    m_pModelCom->Set_AnimationBlend(true);

    m_iObjectInteractEventID = m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e)
        {
            m_Event = e;
        });

    switch (m_iEventID)
    {
    case 0:
        m_eEventType = EVENT_TYPE::EMBARS_GIMMICK0;
        break;
    case 1:
        m_eEventType = EVENT_TYPE::EMBARS_GIMMICK1;
        break;
    default:
        m_eEventType = EVENT_TYPE::END;
        break;
    }


    CHECK_FAILED(Ready_Effect(), E_FAIL);


    if (EVENT_TYPE::END != m_eEventType)
        m_iSubscribeEventID = m_pGameInstance->Subscribe_Event<EventGimmick>(ENUM_CLASS(m_eEventType), [&](const EventGimmick& e) { m_EventGimmick = e; });

    return S_OK;
}

void CStatue::Priority_Update(_float fTimeDelta)
{
    if (false == m_isCollision)
    {
        m_Event.None();
    }

    Check_Solved();
    m_pEffect->Priority_Update(fTimeDelta);

    __super::Priority_Update(fTimeDelta);
}

void CStatue::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        Animation_Change(fTimeDelta);        
    }
    m_pEffect->Update(fTimeDelta);

    __super::Update(fTimeDelta);
}

void CStatue::Late_Update(_float fTimeDelta)
{
    m_pEffect->Late_Update(fTimeDelta);

    __super::Late_Update(fTimeDelta);
}

HRESULT CStatue::Render()
{
    return S_OK;
}

HRESULT CStatue::Ready_Components(void* pArg)
{
    STATUE_DESC* pDesc = static_cast<STATUE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);
     
    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CStatue::Ready_PartObjects(void* pArg)
{
    STATUE_DESC* pDesc = static_cast<STATUE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CStatue_Plate::STATUE_PLATE_DESC PlateDesc = {};

    PlateDesc.eLevel = eLevel;
    PlateDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PlateDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("P_Body_01");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Plate"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Statue_Plate"), &PlateDesc), E_FAIL);

    CStatue_Deco::STATUE_DECO_DESC DecoDesc = {};

    DecoDesc.eLevel = eLevel;
    DecoDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    DecoDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("P_Body_01");

    if (1 == m_iEventID && 1 == m_iStatueIndex)
        DecoDesc.isOtherModel = true;

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Deco"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Statue_Deco"), &DecoDesc), E_FAIL);

    return S_OK;
}

HRESULT CStatue::Ready_Collision(void* pArg)
{
#pragma region 스태틱 몸체
    CBody::BODY_BOXSHAPE_DESC StaticBodyDesc{};
    StaticBodyDesc.vExtent = _float3(1.f, 1.f, 1.f);
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
    for (_uint i = 0; i < ENUM_CLASS(STATUE_TRIGGER::END); ++i)
    {
        CBody::BODY_BOXSHAPE_DESC TriggerDesc{};
        TriggerDesc.vExtent = _float3(0.5f, 0.5f, 0.5f);
        TriggerDesc.bIsTrigger = true;
        TriggerDesc.bStartActive = true;
        TriggerDesc.eMotion = EMotionType::Kinematic;
        TriggerDesc.eQuality = EMotionQuality::LinearCast;
        TriggerDesc.eShapeType = SHAPE::BOX;
        TriggerDesc.fFriction = 0.8f;
        TriggerDesc.fMass = 1.0f;
        TriggerDesc.fRestitution = 0.0f;
        TriggerDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT);
        TriggerDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        m_TriggerCollisionDesc.pGameObject = this;
        m_TriggerCollisionDesc.isForceVaildation = true;
        TriggerDesc.pCollisionDesc = &m_TriggerCollisionDesc;
        switch (i)
        {
        case ENUM_CLASS(STATUE_TRIGGER::FRONT):
            XMStoreFloat3(&TriggerDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * 1.f);
            TriggerDesc.vPos.y += TriggerDesc.vExtent.y;
            
            XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
            if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
                TEXT("Com_Trigger_Front"), reinterpret_cast<CComponent**>(&m_pTriggerCom[i]), &TriggerDesc)))
                return E_FAIL;
            break;
        case ENUM_CLASS(STATUE_TRIGGER::RIGHT):
            XMStoreFloat3(&TriggerDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT)) * 1.f);
            TriggerDesc.vPos.y += TriggerDesc.vExtent.y;

            XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

            if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
                TEXT("Com_Trigger_Right"), reinterpret_cast<CComponent**>(&m_pTriggerCom[i]), &TriggerDesc)))
                return E_FAIL;
            break;
        case ENUM_CLASS(STATUE_TRIGGER::BACK):
            XMStoreFloat3(&TriggerDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION) - XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * 1.f);
            TriggerDesc.vPos.y += TriggerDesc.vExtent.y;

            XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

            if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
                TEXT("Com_Trigger_Back"), reinterpret_cast<CComponent**>(&m_pTriggerCom[i]), &TriggerDesc)))
                return E_FAIL;
            break;
        case ENUM_CLASS(STATUE_TRIGGER::LEFT):
            XMStoreFloat3(&TriggerDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION) - XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT)) * 1.f);
            TriggerDesc.vPos.y += TriggerDesc.vExtent.y;

            XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
            if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
                TEXT("Com_Trigger_Left"), reinterpret_cast<CComponent**>(&m_pTriggerCom[i]), &TriggerDesc)))
                return E_FAIL;
            break;
        }
    }
#pragma endregion

    return S_OK;
}

HRESULT CStatue::Ready_Interaction_Guide(void* pArg)
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, 10.f), TEXT("조작"), 0.75f);

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::EMBARS), TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

HRESULT CStatue::Ready_Effect()
{
    m_pEffect = dynamic_cast<CEffect_Prefab*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::EMBARS), TEXT("Statue_Twinkle")));

    if (nullptr == m_pEffect)
        return E_FAIL;

    m_pEffect->UpdatePosition(m_pTransformCom->Get_State(STATE::POSITION)); 

    if (m_iUnLockRotation == m_iRotation)
        m_bPrevState = true;
    else
    {
        m_pEffect->ResetChildren();
        m_bPrevState = false; 
    }
    return S_OK;
}

void CStatue::Check_Solved()
{
    CHECK_TRUE(m_isLateInit, );

    m_isLateInit = true;

    if (m_iUnLockRotation == m_iRotation)
    {
        m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(m_eEventType), { EventGimmick::Set_InitSolve(m_iStatueIndex) });
    }
}

void CStatue::Input_Interact_Event(_float fTimeDelta)
{
    if (true == isRotateState())
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

        InteractType.eInteractType = INTERACTIVE_TYPE::STATUE;

        InteractType.eState = EventInteractType::BEGIN;

        EventStatue StatueEvent = {};

        XMStoreFloat4(&StatueEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        InteractType.StatueEvent = StatueEvent;

        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
    }
}

void CStatue::Animation_Update(_float fTimeDelta)
{
    if (0.f != m_fColTimeAcc)
    {
        m_fColTimeAcc += fTimeDelta;

        if (0.5f <= m_fColTimeAcc)
        {
            m_fColTimeAcc = 0.f;
            m_pStaticCom->Collision_Active(true);
        }
    }

    if (false == m_isCollision)
        return;

    Input_Interact_Event(fTimeDelta);

    if (m_Event.isOn())               // 켠다는 신호
    {
        m_pStaticCom->Collision_Active(false);

        m_pGuide->Update_Visible(false);

        _bool isEventSetting = { false };

        switch (m_eAnimState)
        {
        case ANIM_STATE::IDLE_0:
            m_eAnimState = ANIM_STATE::ROTATE_90;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            isEventSetting = true;
            break;
        case ANIM_STATE::IDLE_90:
            m_eAnimState = ANIM_STATE::ROTATE_180;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            isEventSetting = true;
            break;
        case ANIM_STATE::IDLE_180:
            m_eAnimState = ANIM_STATE::ROTATE_270;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            isEventSetting = true;
            break;
        case ANIM_STATE::IDLE_270:
            m_eAnimState = ANIM_STATE::ROTATE_360;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            isEventSetting = true;
            break;
        }

        SoundOnce(TEXT("IP_Statue_Rotation"), m_fInteract_Volume);

        // 예지 조각상 돌리는 먼지 ( 오프셋은 맞춰야 할 거 같습니다 )
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Statue_Dust"), m_pTransformCom->Get_State(STATE::POSITION));
        if (true == isEventSetting)
        {
            // 조각상 상호작용 시
            EventInteractType InteractType = {};

            InteractType.eInteractType = INTERACTIVE_TYPE::STATUE;
            InteractType.isEvent = true;

            EventStatue StatueEvent = {};

            XMStoreFloat4(&StatueEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
            XMStoreFloat4(&StatueEvent.vPlayerPosition, Calculate_BonePosition_MinLength(m_Event.vPosition).r[3]);

            InteractType.StatueEvent = StatueEvent;

            // OPENING 중에는 UI, Player 용 Active 변수는 false, 상자 앞 위치랑 상자 위치 던지기
            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

            m_Event.None();
        }
    }
}

void CStatue::Animation_Change(_float fTimeDelta)
{
    if (true == isIdleState())
        return;

    switch (m_eAnimState)
    {
    case ANIM_STATE::ROTATE_90:
        m_eAnimState = ANIM_STATE::IDLE_90;
        m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        m_pModelCom->Set_AnimationLoop(false);
        m_iRotation = 1;
        break;
    case ANIM_STATE::ROTATE_180:
        m_eAnimState = ANIM_STATE::IDLE_180;
        m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        m_pModelCom->Set_AnimationLoop(false);
        m_iRotation = 2;
        break;
    case ANIM_STATE::ROTATE_270:
        m_eAnimState = ANIM_STATE::IDLE_270;
        m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        m_pModelCom->Set_AnimationLoop(false);
        m_iRotation = 3;
        break;
    case ANIM_STATE::ROTATE_360:
        m_eAnimState = ANIM_STATE::IDLE_0;
        m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        m_pModelCom->Set_AnimationLoop(false);
        m_iRotation = 0;
        break;
    }

    m_fColTimeAcc += fTimeDelta;

    if (m_iUnLockRotation == m_iRotation)   //정답 맞췄을때
    {
        m_EventGimmick.Set_SolveStatue(m_iStatueIndex);
        m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(m_eEventType), m_EventGimmick);
        m_pEffect->SetClose();
        m_bPrevState = true;
    }
    else //돌렸는데 틀렸을때
    {
        m_EventGimmick.Reset_SolveStatue(m_iStatueIndex);
        m_pGameInstance->Emit_Event<EventGimmick>(ENUM_CLASS(m_eEventType), m_EventGimmick);
        if (true == m_bPrevState)
            m_pEffect->ResetChildren();
        m_bPrevState = false;
    }

    if (true == m_isCollision)
        m_pGuide->Update_Visible(true);
}

void CStatue::Collision_Enter(COLLISION_DESC * pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (m_EventGimmick.isUnLockGearAvailable(m_iEventID))
        return;

    if (true == isIdleState())
        m_pGuide->Update_Visible(true);

    m_isCollision = true;
}

void CStatue::Collision_Stay(COLLISION_DESC * pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (m_EventGimmick.isUnLockGearAvailable(m_iEventID))
    {
        m_pGuide->Update_Visible(false);
        return;
    }

    m_isCollision = true;
}

void CStatue::Collision_Exit(COLLISION_DESC * pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    m_pGuide->Update_Visible(false);

    m_isCollision = false;
}

_matrix CStatue::Calculate_BonePosition_MinLength(_float4 vPlayerPosition)
{
    const _char* BoneNames[4] = {
        "Position_Ch_F",
        "Position_Ch_R",
        "Position_Ch_B",
        "Position_Ch_L"
    };

    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

    _float fMinDistance = FLT_MAX;
    _matrix ClosestBoneMatrix = XMMatrixIdentity();

    for (_uint i = 0; i < ENUM_CLASS(STATUE_TRIGGER::END); ++i)
    {
        _float4x4 LocalBoneMatrix = {};
        XMStoreFloat4x4(&LocalBoneMatrix, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix(BoneNames[i])));

        _matrix BoneWorldMatrix = XMLoadFloat4x4(&LocalBoneMatrix) * WorldMatrix;

        _vector vBonePosition = BoneWorldMatrix.r[3];

        _float fDistance = XMVectorGetX(XMVector3Length(vBonePosition - XMLoadFloat4(&vPlayerPosition)));

        if (fDistance < fMinDistance)
        {
            fMinDistance = fDistance;
            ClosestBoneMatrix = BoneWorldMatrix;
        }
    }

    return ClosestBoneMatrix;
}

CStatue* CStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStatue* pInstance = new CStatue(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CStatue"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CStatue::Clone(void* pArg)
{
    CStatue* pInstance = new CStatue(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CStatue"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStatue::Free()
{
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), m_iObjectInteractEventID);

    if (EVENT_TYPE::END != m_eEventType)
        m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(m_eEventType), m_iSubscribeEventID);

    __super::Free();

    Safe_Release(m_pStaticCom);
    for (auto& pTrigger : m_pTriggerCom)
        Safe_Release(pTrigger);

    if (nullptr != m_pGuide)
    {
        m_pGuide->Set_IsDead(true);
        m_pGuide = nullptr;
    }
    Safe_Release(m_pEffect);
}
