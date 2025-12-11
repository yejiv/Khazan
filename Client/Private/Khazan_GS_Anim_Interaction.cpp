#include "Khazan_GS_Anim_Interaction.h"
#include "GameInstance.h"
#include "ClientInstance.h"


CKhazan_GS_Anim_Interaction::CKhazan_GS_Anim_Interaction()
    : m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CKhazan_GS_Anim_Interaction::Initialize_Prototype()
{
    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();

    return S_OK;
}

void CKhazan_GS_Anim_Interaction::Enter()
{
}

void CKhazan_GS_Anim_Interaction::Continue(_float fTimeDelta)
{
}

void CKhazan_GS_Anim_Interaction::Exit()
{
}

_bool CKhazan_GS_Anim_Interaction::Try_Lantern(_bool isEquip)
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    if (isEquip) m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_Com_Lantern_On"));
    else  m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_Com_Lantern_Off"));

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_TobStone(_bool isWeapon)
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    if (isWeapon)
    {
        if( m_pClientInstance->Is_CurrentGSword())
            m_pModel->Set_AnimationSet("Set_GSwordTobstone");
        else
            m_pModel->Set_AnimationSet("Set_SpearTobstone");
    }
    else 
        m_pModel->Set_AnimationSet("Set_Tobstone");

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_DamagedTS_Before(_bool isWeapon)
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    if (isWeapon)
    {
        if (m_pClientInstance->Is_CurrentGSword())
            m_pModel->Set_AnimationSet("Set_GSwordDamagedTS_Before");
        else
            m_pModel->Set_AnimationSet("Set_SpearDamagedTS_Before");
    }
    else 
        m_pModel->Set_AnimationSet("Set_DamagedTS_Before");

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_DamagedTS_After(_bool isWeapon)
{
    //if (!m_pModel->Check_MinAnimationTime())
    //    return false;

    if (isWeapon)
    {
        if (m_pClientInstance->Is_CurrentGSword())
            m_pModel->Set_AnimationSet("Set_GSwordDamagedTS_After");
        else
            m_pModel->Set_AnimationSet("Set_SpearDamagedTS_After");
    }
    else 
        m_pModel->Set_AnimationSet("Set_DamagedTS_After");

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_BoxOpen(_bool isUsedSet)
{

    //if (!m_pModel->Check_MinAnimationTime())
    //    return false;


    if (isUsedSet) {
        m_pModel->Set_AnimationSet("Set_OpenBox");  //무기 집어넣고 상자열고 다시 무기 들기
        m_pModel->AnimationSetIndexIncrease();      //무기 집어 넣는건 플레이어에서 관리.

    }
    else  m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_BigChest_Open_003")); //상자만 여는 모션

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_Lachryma()
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_LacrimaInteraction"));

    //노티파이에서 힐링 처리 
    //m_pPlayerData->fCulHp += m_pPlayerData->fLachrymaItemRegen;
    //if (m_pPlayerData->fCulHp > m_pPlayerData->fMaxHp)
    //    m_pPlayerData->fCulHp = m_pPlayerData->fMaxHp;

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_Heal()
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;
    m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_Heal_01"));

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_Lever(_bool isUsedSet)
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    if (isUsedSet) {
        if(m_pClientInstance->Get_PlayerEquipment().isGSword)
            m_pModel->Set_AnimationSet("Set_GSwordLeverPull"); 
        else
            m_pModel->Set_AnimationSet("Set_SpearLeverPull");

        m_pModel->AnimationSetIndexIncrease();    

    }
    else  m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_LeverPull")); 

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_Statue(_bool isUsedSet)
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    m_pClientInstance->Set_PlayerInput(true);

    if (isUsedSet) {
        m_pModel->Set_AnimationSet("Set_StatueRotate");
        m_pModel->AnimationSetIndexIncrease();

    }
    else 
    m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_StatueTable_Rotate_001"));

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_IronGate(_bool isUsedSet)
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    if (isUsedSet) {
        m_pModel->Set_AnimationSet("Set_OpenGateLock");
        m_pModel->AnimationSetIndexIncrease();

    }
    else  m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_MiddleDoor_Open_001_b"));

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_UnLockGear(_bool isUsedSet)
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    if (isUsedSet) {
        if (m_pClientInstance->Get_PlayerEquipment().isGSword)
            m_pModel->Set_AnimationSet("Set_GSwordGearSwitch");
        else
            m_pModel->Set_AnimationSet("Set_SpearGearSwitch");

        m_pModel->AnimationSetIndexIncrease();

    }
    else  m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_GearSwitch_001_Interation"));

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Try_GiantGate(_bool isUsedSet)
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    if (isUsedSet) {
        m_pModel->Set_AnimationSet("Set_OpenGiantGate");
        m_pModel->AnimationSetIndexIncrease();

    }
    else  m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_OpenDoor_Giant"));

    return true;
}

_bool CKhazan_GS_Anim_Interaction::Is_Heal() const
{
    return m_pModel->Get_CurAnimIndex() == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Heal_01");
}

CKhazan_GS_Anim_Interaction* CKhazan_GS_Anim_Interaction::Create()
{
    CKhazan_GS_Anim_Interaction* pInstance = new CKhazan_GS_Anim_Interaction();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_GS_Anim_Interaction"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_GS_Anim_Interaction::Free()
{
    __super::Free();
    Safe_Release(m_pClientInstance);

}
