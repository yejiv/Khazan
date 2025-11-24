#include "ElevatorL.h"

#include "GameInstance.h"

#include "Elevator_Inner.h"
#include "Elevator_Mid.h"
#include "Elevator_Outer.h"

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

    LARGE_ELEVATOR_POS* pElevatorPos = static_cast<LARGE_ELEVATOR_POS*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pElevatorPos, E_FAIL);

    m_vUpPos = pElevatorPos->vUp;
    m_vMidPos = pElevatorPos->vMid;
    m_vDownPos = pElevatorPos->vDown;

    m_eAnimState = ANIM_STATE::ALL;
    m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->AnimationLoop(true);

    m_pModelCom->Set_AnimationBlend(true);

    m_pGameInstance->Subscribe_Event<EventHallElevator>(ENUM_CLASS(EVENT_TYPE::HALL_ELEVATOR_UNLOCK), [&](const EventHallElevator& e) { m_Event = e; });

    return S_OK;
}

void CElevatorL::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CElevatorL::Update(_float fTimeDelta)
{
    if (m_Event.isEventOn) // m_pGameInstance->Key_Down(DIK_H) && ANIM_STATE::IDLE != m_eAnimState) // 어떤 조건이 들어오면 애니메이션 Loop 중단 후 슥슥 샥샥
    {
        m_Event.EventOff();

        m_isAnimChange = true;
        m_pModelCom->AnimationLoop(false);
    }
    else if (ANIM_STATE::IDLE == m_eAnimState)
    {
        if (m_Event.IsThirdStep()) // m_pGameInstance->Key_Down(DIK_L)) // 아이들 상태 되면 이제 슥슥 이동을 시작
        {
            switch (m_eMoveState)
            {
            case MOVE_STATE::MID:
                m_eMoveState = MOVE_STATE::MIDTODOWN;
                break;
            case MOVE_STATE::DOWN:
                m_eMoveState = MOVE_STATE::DOWNTOUP;
                break;
            default:
                break;
            }
        }

        switch (m_eMoveState)
        {
        case MOVE_STATE::MIDTODOWN:
            Lerp_ElevatorMove(fTimeDelta, m_vMidPos, m_vDownPos, 5.f);
            break;
        case MOVE_STATE::DOWNTOUP:
            Lerp_ElevatorMove(fTimeDelta, m_vDownPos, m_vUpPos, 5.f);
            break;
        default:
            break;
        }
    }

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        if (ANIM_STATE::INNER_STOPPING == m_eAnimState)
        {
            m_isAnimChange = false;
            m_eAnimState = ANIM_STATE::OUTER_STOPPING;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->AnimationLoop(true);
        }
        else if (true == m_isAnimChange)
        {
            m_isAnimChange = false;
            switch (m_eAnimState)
            {
            case ANIM_STATE::ALL:
                m_eAnimState = ANIM_STATE::MID_STOP;
                m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
                m_pModelCom->AnimationLoop(true);
                break;
            case ANIM_STATE::MID_STOP:
                m_eAnimState = ANIM_STATE::INNER_STOPPING;
                m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
                m_pModelCom->AnimationLoop(false);
                break;
            case ANIM_STATE::OUTER_STOPPING:
                m_eAnimState = ANIM_STATE::IDLE;
                m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
                m_pModelCom->AnimationLoop(true);
                break;
            case ANIM_STATE::IDLE:
                m_eAnimState = ANIM_STATE::ALL;
                m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
                m_pModelCom->AnimationLoop(true);
                break;
            }
        }
    }

    __super::Update(fTimeDelta);
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

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

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
            m_eMoveState = MOVE_STATE::DOWN;
            break;
        case MOVE_STATE::DOWNTOUP:
            m_eMoveState = MOVE_STATE::UP;
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

    return S_OK;
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
    __super::Free();


}
