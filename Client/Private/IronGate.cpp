#include "IronGate.h"

#include "GameInstance.h"

#include "Interaction_Guide.h"

#include "IronGate_Lock.h"
#include "IronGate_Part_L.h"
#include "IronGate_Part_R.h"

CIronGate::CIronGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CIronGate::CIronGate(const CIronGate& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CIronGate::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CIronGate::Initialize_Clone(void* pArg)
{
    IRONGATE_DESC* pDesc = static_cast<IRONGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

#pragma region 범수 상호작용 슥슥

    CHECK_FAILED(Ready_Interaction_Guide(pArg), E_FAIL);

#pragma endregion

    m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e)
        {
            m_Event = e;
        });

    m_eAnimState = ANIM_STATE::ACTIVATION;
    m_pModelCom->Set_Animation(ENUM_CLASS((m_eAnimState)));
    m_pModelCom->Set_AnimationLoop(false);
    m_pModelCom->Set_AnimationBlend(false);

    m_pModelCom->Play_Animation(0.f);

    return S_OK;
}

void CIronGate::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CIronGate::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_isUnLock)
    {
        if (true == m_pModelCom->Play_Animation(fTimeDelta))
        {
            Animation_Change(fTimeDelta);
        }
    }

    __super::Update(fTimeDelta);
}

void CIronGate::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CIronGate::Render()
{
    return S_OK;
}

HRESULT CIronGate::Ready_Components(void* pArg)
{
    IRONGATE_DESC* pDesc = static_cast<IRONGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CIronGate::Ready_PartObjects(void* pArg)
{
    IRONGATE_DESC* pDesc = static_cast<IRONGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CIronGate_Lock::IRONGATE_LOCK_DESC PartLockDesc = {};

    PartLockDesc.eLevel = eLevel;
    PartLockDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PartLockDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("LockLever");

    PartLockDesc.pUnLock = &m_isUnLock;

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Lock"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_IronGate_Lock"), &PartLockDesc), E_FAIL);

    CIronGate_Part_L::IRONGATE_PART_LEFT_DESC PartLeftDesc = {};

    PartLeftDesc.eLevel = eLevel;
    PartLeftDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PartLeftDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Door_Pivot_L");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gate_L"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_IronGate_Part_L"), &PartLeftDesc), E_FAIL);

    CIronGate_Part_R::IRONGATE_PART_RIGHT_DESC PartRightDesc = {};

    PartRightDesc.eLevel = eLevel;
    PartRightDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PartRightDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Door_Pivot_R");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gate_R"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_IronGate_Part_R"), &PartRightDesc), E_FAIL);

    return S_OK;
}

HRESULT CIronGate::Ready_Collision(void* pArg)
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
    return S_OK;
#pragma endregion
}

HRESULT CIronGate::Ready_Interaction_Guide(void* pArg)
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    Safe_AddRef(m_pGuide);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, 10.f), TEXT("열어이"), 1.f);

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::EMBARS), TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

void CIronGate::Input_Interact_Event(_float fTimeDelta)
{
    if (true == m_isUnLock)
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

        InteractType.eInteractType = INTERACTIVE_TYPE::IRONGATE;

        InteractType.eState = EventInteractType::BEGIN;

        EventIronGate IronGateEvent = {};

        XMStoreFloat4(&IronGateEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        InteractType.IronGateEvent = IronGateEvent;

        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
    }
}

void CIronGate::Animation_Update(_float fTimeDelta)
{
    if (false == m_isCollision)
        return;

    Input_Interact_Event(fTimeDelta);

    if (m_Event.isOn())               // 켠다는 신호
    {
        m_pStaticCom->Collision_Active(false);
        m_pTriggerCom->Collision_Active(false);

        m_pGuide->Update_Visible(false);

        _bool isEventSetting = { false };

        if (false == m_isUnLock)
        {
            m_isUnLock = true;

            // 조각상 상호작용 시
            EventInteractType InteractType = {};

            InteractType.eInteractType = INTERACTIVE_TYPE::IRONGATE;
            InteractType.isEvent = true;

            EventIronGate IronGateEvent = {};

            _matrix OffSetMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Position_Ch")) * m_pTransformCom->Get_WorldMatrix();

            XMStoreFloat4(&IronGateEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
            XMStoreFloat4(&IronGateEvent.vPlayerPosition, OffSetMatrix.r[3]);

            InteractType.IronGateEvent = IronGateEvent;

            // OPENING 중에는 UI, Player 용 Active 변수는 false, 상자 앞 위치랑 상자 위치 던지기
            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

            m_Event.None();
        }
    }
}

void CIronGate::Animation_Change(_float fTimeDelta)
{
}

void CIronGate::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA))
        return;

    if (false == m_isUnLock)
        m_pGuide->Update_Visible(true);

    m_isCollision = true;
}

void CIronGate::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA))
        return;

    m_isCollision = true;
}

void CIronGate::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA))
        return;

    m_pGuide->Update_Visible(false);

    m_isCollision = false;
}

CIronGate* CIronGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CIronGate* pInstance = new CIronGate(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CIronGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CIronGate::Clone(void* pArg)
{
    CIronGate* pInstance = new CIronGate(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CIronGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CIronGate::Free()
{
    __super::Free();
}
