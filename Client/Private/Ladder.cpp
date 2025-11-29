#include "Ladder.h"

#include "GameInstance.h"

#include "Interaction_Guide.h"

#include "Ladder_Top.h"
#include "Ladder_Middle.h"
#include "Ladder_Bottom.h"
#include "Ladder_Support.h"

CLadder::CLadder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CLadder::CLadder(const CLadder& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CLadder::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CLadder::Initialize_Clone(void* pArg)
{
    LADDER_DESC* pDesc = static_cast<LADDER_DESC*>(pArg);
    CHECK_NULLPTR(pArg, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    CHECK_FAILED(Ready_Interaction_Guide(pArg), E_FAIL);

    return S_OK;
}

void CLadder::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CLadder::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CLadder::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CLadder::Render()
{
    return S_OK;
}

void CLadder::Set_GuideMatrix(_float4 vGuidePos)
{
    _vector vPos = XMLoadFloat4(&vGuidePos);
    _matrix matGuide = XMMatrixIdentity();
    vPos.m128_f32[3] = 1.f;
    matGuide.r[3] = vPos;
    XMStoreFloat4x4(&m_matGuide, matGuide);
}

void CLadder::Set_PlayerPosition()
{
    switch (m_eLadderPoint)
    {
    case LADDER_POINT::UP_POS_CH:
        m_vPlayerPosition = m_vUpPlayerPos;
        Set_GuideMatrix(m_vPlayerPosition);
        m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, &m_matGuide,
            _float2(0.f, 10.f), TEXT("타기"), 1.5f);
        break;
    case LADDER_POINT::DOWN_POS_CH:
        m_vPlayerPosition = m_vDownPlayerPos;
        Set_GuideMatrix(m_vPlayerPosition);
        m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, &m_matGuide,
            _float2(0.f, 10.f), TEXT("타기"), 1.5f);
        break;
    case LADDER_POINT::CLIMB_UP:
        break;
    case LADDER_POINT::CLIMB_DOWN:
        break;
    }

    m_vPlayerPosition.w = 1.f;
}

HRESULT CLadder::Ready_Components(void* pArg)
{
    return S_OK;
}

HRESULT CLadder::Ready_PartObjects(void* pArg)
{
    LADDER_DESC* pDesc = static_cast<LADDER_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    LADDER_OFFSET* pLadderOffset = static_cast<LADDER_OFFSET*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pLadderOffset, E_FAIL);

#pragma region 사다리 위쪽

    CLadder_Top::LADDER_TOP_DESC TopDesc = {};

    TopDesc.eLevel = eLevel;
    TopDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    TopDesc.fOffSetHeight = pLadderOffset->fOffSetHeight;

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Top"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Ladder_Top"), &TopDesc), E_FAIL);

    m_pLadderTop = static_cast<CLadder_Top*>(Find_PartObject(TEXT("Part_Top")));
    CHECK_NULLPTR(m_pLadderTop, E_FAIL);

#pragma endregion

#pragma region 사다리 중간 세그먼트들

    _int iNumSegment = pLadderOffset->iSegmentCount;

    _float fOffSetHeight = 2.8f;

    _tchar szPartTag[MAX_PATH] = {};

    for (_uint i = 0; i <= iNumSegment; ++i)
    {
        wsprintf(szPartTag, TEXT("Part_Middle_%d"), i);

        CLadder_Middle::LADDER_MIDDLE_DESC MidDesc = {};

        MidDesc.eLevel = eLevel;
        MidDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
        MidDesc.fOffSetHeight = fOffSetHeight;

        CHECK_FAILED(__super::Add_PartObject(szPartTag, ENUM_CLASS(eLevel),
            TEXT("Prototype_GameObject_Prop_Ladder_Middle"), &MidDesc), E_FAIL);

        fOffSetHeight += 0.4f;
    }

#pragma endregion

#pragma region 사다리 아래쪽

    CLadder_Bottom::LADDER_BOTTOM_DESC BotDesc = {};

    BotDesc.eLevel = eLevel;
    BotDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Bottom"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Ladder_Bottom"), &BotDesc), E_FAIL);

    m_pLadderBot = static_cast<CLadder_Bottom*>(Find_PartObject(TEXT("Part_Bottom")));
    CHECK_NULLPTR(m_pLadderBot, E_FAIL);

#pragma endregion

#pragma region 사다리 아래쪽 발판

    CLadder_Support::LADDER_SUPPORT_DESC SupportDesc = {};

    SupportDesc.eLevel = eLevel;
    SupportDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Support"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Ladder_Support"), &SupportDesc), E_FAIL);

#pragma endregion

    return S_OK;
}

HRESULT CLadder::Ready_Collision(void* pArg)
{
    LADDER_POINT eLadderPoint = { LADDER_POINT::END };

#pragma region 트리거 영역
    CBody::BODY_BOXSHAPE_DESC TriggerDesc{};
    TriggerDesc.bIsTrigger = true;
    TriggerDesc.bStartActive = true;
    TriggerDesc.eMotion = EMotionType::Kinematic;
    TriggerDesc.eQuality = EMotionQuality::LinearCast;
    TriggerDesc.eShapeType = SHAPE::BOX;
    TriggerDesc.fFriction = 0.8f;
    TriggerDesc.fMass = 1.0f;
    TriggerDesc.fRestitution = 0.0f;
    TriggerDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT);

#pragma region 위쪽 플레이어 보정 트리거
    TriggerDesc.vExtent = _float3(0.5f, 0.5f, 0.5f);

    _float4x4* pBoneMatrix = static_cast<CModel*>(m_pLadderTop->Get_Component(TEXT("Com_Model")))->Get_LocalBoneMatrix("IA_BeginLoc_Climb_Down");
    CHECK_NULLPTR(pBoneMatrix, E_FAIL);

    _matrix PartMatrix = static_cast<CTransform*>(m_pLadderTop->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
    _matrix BoneWorldMatrix = XMLoadFloat4x4(pBoneMatrix) * PartMatrix * m_pTransformCom->Get_WorldMatrix();

    XMStoreFloat3(&TriggerDesc.vPos, BoneWorldMatrix.r[3]);
    TriggerDesc.vPos.y += TriggerDesc.vExtent.y;

    XMStoreFloat4(&m_vUpPlayerPos, BoneWorldMatrix.r[3]);

    XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    TriggerDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    eLadderPoint = LADDER_POINT::UP_POS_CH;
    m_tCollisionDesc.pInfo = &eLadderPoint;
    TriggerDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger_Up"), reinterpret_cast<CComponent**>(&m_pTriggerCom[ENUM_CLASS(eLadderPoint)]), &TriggerDesc)))
        return E_FAIL;

#pragma endregion

#pragma region 위쪽으로 플레이어 다 올랐을때의 트리거
    TriggerDesc.vExtent = _float3(0.5f, 0.5f, 0.5f);

    pBoneMatrix = static_cast<CModel*>(m_pLadderTop->Get_Component(TEXT("Com_Model")))->Get_LocalBoneMatrix("ClimbEndLoc_Top");
    CHECK_NULLPTR(pBoneMatrix, E_FAIL);

    PartMatrix = static_cast<CTransform*>(m_pLadderTop->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
    BoneWorldMatrix = XMLoadFloat4x4(pBoneMatrix) * PartMatrix * m_pTransformCom->Get_WorldMatrix();

    XMStoreFloat3(&TriggerDesc.vPos, BoneWorldMatrix.r[3]);

    XMStoreFloat4(&m_vClimbUpPos, BoneWorldMatrix.r[3]);

    XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    TriggerDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    eLadderPoint = LADDER_POINT::CLIMB_UP;
    m_tCollisionDesc.pInfo = &eLadderPoint;
    TriggerDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger_Climb_Up"), reinterpret_cast<CComponent**>(&m_pTriggerCom[ENUM_CLASS(eLadderPoint)]), &TriggerDesc)))
        return E_FAIL;

#pragma endregion

#pragma region 아래쪽 플레이어 보정 트리거
    TriggerDesc.vExtent = _float3(1.5f, 0.25f, 1.5f);
    pBoneMatrix = static_cast<CModel*>(m_pLadderBot->Get_Component(TEXT("Com_Model")))->Get_LocalBoneMatrix("IA_BeginLoc_Climb_Up");
    CHECK_NULLPTR(pBoneMatrix, E_FAIL);

    PartMatrix = static_cast<CTransform*>(m_pLadderBot->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
    BoneWorldMatrix = XMLoadFloat4x4(pBoneMatrix) * PartMatrix * m_pTransformCom->Get_WorldMatrix();

    XMStoreFloat3(&TriggerDesc.vPos, BoneWorldMatrix.r[3]);
    TriggerDesc.vPos.y += TriggerDesc.vExtent.y;

    XMStoreFloat4(&m_vDownPlayerPos, BoneWorldMatrix.r[3]);

    XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    TriggerDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    eLadderPoint = LADDER_POINT::DOWN_POS_CH;
    m_tCollisionDesc.pInfo = &eLadderPoint;
    TriggerDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger_Down"), reinterpret_cast<CComponent**>(&m_pTriggerCom[ENUM_CLASS(eLadderPoint)]), &TriggerDesc)))
        return E_FAIL;

#pragma endregion

#pragma region 아래쪽으로 플레이어 다 내려왔을때의 트리거
    TriggerDesc.vExtent = _float3(1.5f, 0.25f, 1.5f);

    pBoneMatrix = static_cast<CModel*>(m_pLadderBot->Get_Component(TEXT("Com_Model")))->Get_LocalBoneMatrix("ClimbEndLoc_Bottom");
    CHECK_NULLPTR(pBoneMatrix, E_FAIL);

    PartMatrix = static_cast<CTransform*>(m_pLadderBot->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
    BoneWorldMatrix = XMLoadFloat4x4(pBoneMatrix) * PartMatrix * m_pTransformCom->Get_WorldMatrix();

    XMStoreFloat3(&TriggerDesc.vPos, BoneWorldMatrix.r[3]);

    XMStoreFloat4(&m_vClimbDownPos, BoneWorldMatrix.r[3]);

    XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    TriggerDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    eLadderPoint = LADDER_POINT::CLIMB_DOWN;
    m_tCollisionDesc.pInfo = &eLadderPoint;
    TriggerDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger_Climb_Down"), reinterpret_cast<CComponent**>(&m_pTriggerCom[ENUM_CLASS(eLadderPoint)]), &TriggerDesc)))
        return E_FAIL;

#pragma endregion

#pragma endregion

    return S_OK;
}

HRESULT CLadder::Ready_Interaction_Guide(void* pArg)
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1] + 1.f), TEXT("타기"), 1.5f);

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

void CLadder::Input_Interact_Event(_float fTimeDelta)
{
    _bool isPressing = { false };

    if (m_pGameInstance->Key_Pressing(DIK_F, fTimeDelta))
    {
        isPressing = m_pGuide->IsPressing();
    }

    if (true == isPressing)
    {
        m_pGuide->Update_Visible(false);

        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::UNLOCKGEAR;

        InteractType.eState = EventInteractType::BEGIN;

        EventLadder LadderEvent = {};

        XMStoreFloat4(&LadderEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

        LadderEvent.vPlayerPosition = m_vPlayerPosition;

        InteractType.LadderEvent = LadderEvent;

        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
    }
}

void CLadder::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA))
        return;

    m_eLadderPoint = *static_cast<LADDER_POINT*>(pMyDesc->pInfo);
    Set_PlayerPosition();
}

void CLadder::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA))
        return;


}

void CLadder::Collision_Exit(COLLISION_DESC * pDesc, _uint iOtherObjectLayer, COLLISION_DESC * pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA))
        return;

    m_pGuide->Update_Visible(false);

    m_isCollision = false;
}

CLadder* CLadder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLadder* pInstance = new CLadder(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLadder"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLadder::Clone(void* pArg)
{
    CLadder* pInstance = new CLadder(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLadder"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLadder::Free()
{
    __super::Free();

    for (auto& pTriggerCom : m_pTriggerCom)
        Safe_Release(pTriggerCom);

    if (nullptr != m_pGuide)
    {
        m_pGuide->Set_IsDead(true);
        m_pGuide = nullptr;
    }
}
