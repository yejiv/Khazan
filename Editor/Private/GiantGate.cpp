#include "GiantGate.h"

#include "GameInstance.h"

#include "GiantGate_Part_L.h"
#include "GiantGate_Part_R.h"

CGiantGate::CGiantGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CGiantGate::CGiantGate(const CGiantGate& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CGiantGate::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CGiantGate::Initialize_Clone(void* pArg)
{
    GIANTGATE_DESC* pDesc = static_cast<GIANTGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::OPENNING;
    m_pModelCom->Set_Animation(ENUM_CLASS((m_eAnimState)));
    m_pModelCom->Set_AnimationLoop(false);
    m_pModelCom->Set_AnimationBlend(false);

    m_pModelCom->Play_Animation(0.f);

    return S_OK;
}

void CGiantGate::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CGiantGate::Update(_float fTimeDelta)
{
    if (false == m_isUnLock && m_pGameInstance->Key_Down(DIK_NUMPAD2))
    {
        m_isUnLock = true;
    }

    if (true == m_isUnLock)
        m_pModelCom->Play_Animation(fTimeDelta);

    if (ANIM_STATE::OPENNING != m_eAnimState)
        m_isState = true;
    else
        m_isState = false;

    __super::Update(fTimeDelta);
}

void CGiantGate::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CGiantGate::Render()
{
    return S_OK;
}

HRESULT CGiantGate::Ready_Components(void* pArg)
{
    GIANTGATE_DESC* pDesc = static_cast<GIANTGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CGiantGate::Ready_PartObjects(void* pArg)
{
    GIANTGATE_DESC* pDesc = static_cast<GIANTGATE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CGiantGate_Part_L::GIANTGATE_LEFT_DESC PartLeftDesc = {};

    PartLeftDesc.eLevel = eLevel;
    PartLeftDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PartLeftDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Door_Pivot_L");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gate_L"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_GiantGate_Part_L"), &PartLeftDesc), E_FAIL);

    CGiantGate_Part_R::GIANTGATE_RIGHT_DESC PartRightDesc = {};

    PartRightDesc.eLevel = eLevel;
    PartRightDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PartRightDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Door_Pivot_R");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gate_R"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_GiantGate_Part_R"), &PartRightDesc), E_FAIL);

    return S_OK;
}

CGiantGate* CGiantGate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGiantGate* pInstance = new CGiantGate(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGiantGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGiantGate::Clone(void* pArg)
{
    CGiantGate* pInstance = new CGiantGate(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CGiantGate"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGiantGate::Free()
{
    __super::Free();
}
