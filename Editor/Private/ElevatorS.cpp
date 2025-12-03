#include "ElevatorS.h"

#include "GameInstance.h"

#include "Elevator_Gear.h"
#include "Slate_Switch.h"

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
    CHECK_NULLPTR(pArg, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    m_pModelCom->Set_Animation(0);

    m_vUpPos = pDesc->ElevatorPos.vUp;
    m_vDownPos = pDesc->ElevatorPos.vDown;

    _float fUpLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vUpPos) - m_pTransformCom->Get_State(STATE::POSITION)));
    _float fDownLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vDownPos) - m_pTransformCom->Get_State(STATE::POSITION)));

    if (fUpLength > fDownLength)
        m_eState = ELEVATOR_STATE::IDLE_DOWN;
    else
        m_eState = ELEVATOR_STATE::IDLE_UP;

    m_pModelCom->Set_AnimationBlend(false);

    return S_OK;
}

void CElevatorS::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CElevatorS::Update(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);

    if (true == m_isActive)
    {
        if (ELEVATOR_STATE::IDLE_UP == m_eState)
        {
            Lerp_ElevatorMove(fTimeDelta, m_vUpPos, m_vDownPos, 5.f);
        }
        if (ELEVATOR_STATE::IDLE_DOWN == m_eState)
        {
            Lerp_ElevatorMove(fTimeDelta, m_vDownPos, m_vUpPos, 5.f);
        }
    }

    __super::Update(fTimeDelta);
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
        m_isActive = false;
        m_fTimeAcc = 0.f;

        if (ELEVATOR_STATE::IDLE_UP == m_eState)
            m_eState = ELEVATOR_STATE::IDLE_DOWN;
        else
            m_eState = ELEVATOR_STATE::IDLE_UP;
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
    SMALL_ELEVATOR_DESC* pDesc = static_cast<SMALL_ELEVATOR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CSlate_Switch::SLATE_SWITCH_DESC SlateDesc = {};

    SlateDesc.eLevel = eLevel;
    SlateDesc.pActive = &m_isActive;
    SlateDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Switch"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Slate_Switch"), &SlateDesc), E_FAIL);

    CElevator_Gear::ELEVATOR_GEAR_DESC GearDesc = {};

    GearDesc.eLevel = eLevel;
    GearDesc.pActive = &m_isActive;
    GearDesc.fOffsetRotation = XMConvertToRadians(45.f);
    GearDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gear_0"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Elevator_Gear"), &GearDesc), E_FAIL);

    GearDesc.fOffsetRotation = XMConvertToRadians(135.f);

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gear_1"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Elevator_Gear"), &GearDesc), E_FAIL);

    GearDesc.fOffsetRotation = XMConvertToRadians(225.f);

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gear_2"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Elevator_Gear"), &GearDesc), E_FAIL);

    GearDesc.fOffsetRotation = XMConvertToRadians(315.f);

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gear_3"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Elevator_Gear"), &GearDesc), E_FAIL);

    return S_OK;
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


}
