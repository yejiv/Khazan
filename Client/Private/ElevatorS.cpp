#include "ElevatorS.h"

#include "GameInstance.h"

#include "Elevator_Gear.h"
#include "Slate_Switch.h"
#include "Body.h"

CElevatorS::CElevatorS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CElevatorS::CElevatorS(const CElevatorS& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CElevatorS::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CElevatorS::Initialize_Clone(void* pArg)
{
    SMALL_ELEVATOR_DESC* pDesc = static_cast<SMALL_ELEVATOR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    m_pModelCom->Set_Animation(0);

    ELEVATOR_POS* pPos = static_cast<ELEVATOR_POS*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pPos, E_FAIL);

    m_vUpPos = pPos->vUp;
    m_vDownPos = pPos->vDown;

    _float fUpLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vUpPos) - m_pTransformCom->Get_State(STATE::POSITION)));
    _float fDownLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vDownPos) - m_pTransformCom->Get_State(STATE::POSITION)));

    if (fUpLength > fDownLength)
        m_eState = ELEVATOR_STATE::DOWN;
    else
        m_eState = ELEVATOR_STATE::UP;

    return S_OK;
}

void CElevatorS::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CElevatorS::Update(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);

    if (true == m_isActiveElevator)
    {
        if (false == m_isStart)
        {
            m_isStart = true;

            //SoundOnce(TEXT("IP_Elevator_Start"), m_fInteract_Volume);
        }
        
        if (IsPlayingSound(TEXT("IP_Elevator_Start")))
        {

        }
        else
        {
            if (false == m_isLoop)
            {
                m_isLoop = true;

                SoundLoop(TEXT("IP_Elevator_Loop"), m_fInteract_Volume);
            }
        }

        if (true == m_isLoop)
        {
            if (ELEVATOR_STATE::UP == m_eState)
            {
                Lerp_ElevatorMove(fTimeDelta, m_vUpPos, m_vDownPos, 25.f);
            }
            else if (ELEVATOR_STATE::DOWN == m_eState)
            {
                Lerp_ElevatorMove(fTimeDelta, m_vDownPos, m_vUpPos, 25.f);
            }
        }
    }

    __super::Update(fTimeDelta);
    m_pBodyCom->MoveKinematic(fTimeDelta, m_pTransformCom);
    m_pTriggerCom->MoveKinematic(fTimeDelta, m_pTransformCom);
    //m_pBodyCom->Sync_Update(m_pTransformCom);
    //m_pBodyCom->Update(fTimeDelta, m_pTransformCom);

    //m_pTriggerCom->Sync_Update(m_pTransformCom);
    //m_pTriggerCom->Update(fTimeDelta, m_pTransformCom);
}

void CElevatorS::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );

    __super::Late_Update(fTimeDelta);
}

HRESULT CElevatorS::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CElevatorS : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

void CElevatorS::Lerp_ElevatorMove(_float fTimeDelta, _float4 vStartPos, _float4 vTargetPos, _float fDuration)
{
    _float4 vPos = vTargetPos;

    m_fTimeAcc += fTimeDelta;

    _float fLerpTime = m_fTimeAcc / fDuration;

    _float4 vLerpPos = Lerp(vStartPos, vPos, fLerpTime);

    _float fDistance = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vPos) - XMLoadFloat4(&vLerpPos)));

    if (0.1f > fDistance)
    {
        SoundStop_FadeOut(TEXT("IP_Elevator_Loop"), 0.5f);
        SoundOnce(TEXT("IP_Elevator_End"), m_fInteract_Volume);

        m_isActiveElevator = false;
        m_isLoop = false;
        m_isStart = false;

        m_fTimeAcc = 0.f;

        m_isAvailableSwitch = true;

        if (ELEVATOR_STATE::UP == m_eState)
            m_eState = ELEVATOR_STATE::DOWN;
        else
            m_eState = ELEVATOR_STATE::UP;
    }
    else
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vLerpPos));
    }
}

HRESULT CElevatorS::Ready_Components(void* pArg)
{
    SMALL_ELEVATOR_DESC* pDesc = static_cast<SMALL_ELEVATOR_DESC*>(pArg);
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

HRESULT CElevatorS::Ready_PartObjects(void* pArg)
{
    LEVEL eLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    SMALL_ELEVATOR_DESC* pDesc = static_cast<SMALL_ELEVATOR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CSlate_Switch::SLATE_SWITCH_DESC SlateDesc = {};

    SlateDesc.eLevel = eLevel;
    SlateDesc.eType = CSlate_Switch::ELEVATOR_TYPE::SMALL;
    SlateDesc.pActiveElevator = &m_isActiveElevator;
    SlateDesc.pAvailableSwitch = &m_isAvailableSwitch;
    SlateDesc.pSwitchPressed = &m_isSwitchPressed;
    SlateDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Switch"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Slate_Switch"), &SlateDesc), E_FAIL);

    CElevator_Gear::ELEVATOR_GEAR_DESC GearDesc = {};

    GearDesc.eLevel = eLevel;
    GearDesc.fOffsetRotation = XMConvertToRadians(45.f);
    GearDesc.pActiveElevator = &m_isActiveElevator;
    GearDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gear_0"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Elevator_Gear"), &GearDesc), E_FAIL);

    GearDesc.fOffsetRotation = XMConvertToRadians(135.f);

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gear_1"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Elevator_Gear"), &GearDesc), E_FAIL);

    GearDesc.fOffsetRotation = XMConvertToRadians(225.f);

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gear_2"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Elevator_Gear"), &GearDesc), E_FAIL);

    GearDesc.fOffsetRotation = XMConvertToRadians(315.f);

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gear_3"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_Elevator_Gear"), &GearDesc), E_FAIL);

    return S_OK;
}

HRESULT CElevatorS::Ready_Collision(void* pArg)
{
    CBody::BODY_CYLINDERSHAPE_DESC BodyDesc{};
    BodyDesc.fRadius = 7.7f;
    BodyDesc.fHeight = 1.f;
    BodyDesc.bIsTrigger = false;
    BodyDesc.bStartActive = true;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::LinearCast;
    BodyDesc.eShapeType = SHAPE::CYLINDER;
    BodyDesc.fFriction = 0.8f;
    BodyDesc.fMass = 1.0f;
    BodyDesc.fRestitution = 0.0f;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_MOVE_PLATFORM);

    XMStoreFloat3(&BodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * 1.f);
    //BodyDesc.vPos.y += BodyDesc.vExtent.y;

    XMStoreFloat4(&BodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    BodyDesc.vShapeOffset = _float3(0.f, -0.57f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    m_tCollisionDesc.isMovePlatform = true;
    m_tCollisionDesc.isForceVaildation = true;
    //pCollDesc.pInfo = ?? // 작성하기
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;

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

void CElevatorS::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (pMyDesc->iObjectLayer == ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT))
    {
        if (false == m_isSwitchPressed)
        {
            m_isSwitchPressed = true;
        }
    }
}

void CElevatorS::Collision_Stay(COLLISION_DESC * pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC * pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;
}

void CElevatorS::Collision_Exit(COLLISION_DESC * pDesc, _uint iOtherObjectLayer, COLLISION_DESC * pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    m_isCollision = false;
}

CElevatorS* CElevatorS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CElevatorS* pInstance = new CElevatorS(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CElevatorS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CElevatorS::Clone(void* pArg)
{
    CElevatorS* pInstance = new CElevatorS(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CElevatorS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CElevatorS::Free()
{
    __super::Free();

    Safe_Release(m_pBodyCom);
    Safe_Release(m_pTriggerCom);
}
