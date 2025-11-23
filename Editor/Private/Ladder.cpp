#include "Ladder.h"

#include "GameInstance.h"

#include "Ladder_Top.h"
#include "Ladder_Middle.h"
#include "Ladder_Bottom.h"
#include "Ladder_Support.h"

CLadder::CLadder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CLadder::CLadder(const CLadder& Prototype)
    : CProp_Interactive { Prototype }
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

_float CLadder::Get_TopPosition_Y()
{
    m_pTopObjTransform = static_cast<CTransform*>(m_pTopObj->Get_Component(TEXT("Com_Transform")));

    return m_pTopObjTransform->Get_State(STATE::POSITION).m128_f32[1];
}

void CLadder::Set_TopPosition_Y(_float fHeight)
{
    CHECK_NULLPTR(m_pTopObjTransform, );

    _vector vPosition = m_pTopObjTransform->Get_State(STATE::POSITION);

    vPosition.m128_f32[1] = fHeight;

    m_pTopObjTransform->Set_State(STATE::POSITION, vPosition);
}

_float CLadder::Get_MiddlePosition_Y()
{
    m_pMidObjTransform = static_cast<CTransform*>(m_pMidObj->Get_Component(TEXT("Com_Transform")));

    return m_pMidObjTransform->Get_State(STATE::POSITION).m128_f32[1];
}

void CLadder::Set_MiddlePosition_Y(_float fHeight)
{
    CHECK_NULLPTR(m_pMidObjTransform, );

    _vector vPosition = m_pMidObjTransform->Get_State(STATE::POSITION);

    vPosition.m128_f32[1] = fHeight;

    m_pMidObjTransform->Set_State(STATE::POSITION, vPosition);
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

#pragma region 사다리 위쪽

    CLadder_Top::LADDER_TOP_DESC TopDesc = {};

    TopDesc.eLevel = eLevel;
    TopDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    TopDesc.fOffSetHeight = pDesc->fOffSetHeight;

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Top"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Ladder_Top"), &TopDesc), E_FAIL);

#pragma endregion

#pragma region 사다리 중간 세그먼트들

    CLadder_Middle::LADDER_MIDDLE_DESC MidDesc = {};

    MidDesc.eLevel = eLevel;
    MidDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    MidDesc.fOffSetHeight = 2.8f;
    
    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Middle"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Ladder_Middle"), &MidDesc), E_FAIL);

#pragma endregion

#pragma region 사다리 아래쪽

    CLadder_Bottom::LADDER_BOTTOM_DESC BotDesc = {};

    BotDesc.eLevel = eLevel;
    BotDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Bottom"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Ladder_Bottom"), &BotDesc), E_FAIL);

#pragma endregion

#pragma region 사다리 아래쪽 발판

    CLadder_Support::LADDER_SUPPORT_DESC SupportDesc = {};

    SupportDesc.eLevel = eLevel;
    SupportDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Support"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Ladder_Support"), &SupportDesc), E_FAIL);

#pragma endregion

    m_pTopObj = static_cast<CLadder_Top*>(Find_PartObject(TEXT("Part_Top")));
    m_pMidObj = static_cast<CLadder_Middle*>(Find_PartObject(TEXT("Part_Middle")));
    m_pBotObj = static_cast<CLadder_Bottom*>(Find_PartObject(TEXT("Part_Bot")));

    return S_OK;
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
}
