#include "Statue.h"

#include "GameInstance.h"

#include "Statue_Plate.h"
#include "Statue_Deco.h"

CStatue::CStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CStatue::CStatue(const CStatue& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CStatue::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CStatue::Initialize_Clone(void* pArg)
{
    STATUE_DESC* pDesc = static_cast<STATUE_DESC*>(pArg);
    CHECK_NULLPTR(pArg, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    m_iUnLockRotation = pDesc->StatueRotation.iUnLockRotation;

    m_eAnimState = ANIM_STATE::IDLE_0;
    m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
    m_pModelCom->Set_AnimationBlend(false);

    return S_OK;
}

void CStatue::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CStatue::Update(_float fTimeDelta)
{
    if (false)//false == m_isActive && m_pGameInstance->Key_Down(DIK_6))
    {
        m_isActive = true;

        switch (m_eAnimState)
        {
        case ANIM_STATE::IDLE_0:
            m_eAnimState = ANIM_STATE::ROTATE_90;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            break;
        case ANIM_STATE::IDLE_90:
            m_eAnimState = ANIM_STATE::ROTATE_180;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            break;
        case ANIM_STATE::IDLE_180:
            m_eAnimState = ANIM_STATE::ROTATE_270;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            break;
        case ANIM_STATE::IDLE_270:
            m_eAnimState = ANIM_STATE::ROTATE_360;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            break;
        }
    }

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {

        switch (m_eAnimState)
        {
        case ANIM_STATE::ROTATE_90:
            m_eAnimState = ANIM_STATE::IDLE_90;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            m_iRotation = 1;
            m_isActive = false;
            break;
        case ANIM_STATE::ROTATE_180:
            m_eAnimState = ANIM_STATE::IDLE_180;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            m_iRotation = 2;
            m_isActive = false;
            break;
        case ANIM_STATE::ROTATE_270:
            m_eAnimState = ANIM_STATE::IDLE_270;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            m_iRotation = 3;
            m_isActive = false;
            break;
        case ANIM_STATE::ROTATE_360:
            m_eAnimState = ANIM_STATE::IDLE_0;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            m_iRotation = 0;
            m_isActive = false;
            break;
        }
    }

    __super::Update(fTimeDelta);
}

void CStatue::Late_Update(_float fTimeDelta)
{
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

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Plate"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Statue_Plate"), &PlateDesc), E_FAIL);

    CStatue_Deco::STATUE_DECO_DESC DecoDesc = {};

    DecoDesc.eLevel = eLevel;
    DecoDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    DecoDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("P_Body_01");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Deco"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_Statue_Deco"), &DecoDesc), E_FAIL);

    return S_OK;
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
    __super::Free();
}
