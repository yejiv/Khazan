#include "PlayerData_Manager.h"


CPlayerData_Manager::CPlayerData_Manager()
{
    UsedSpear();
    AllUnlock_Skill();
    UsedGSword();
    AllUnlock_Skill();

}

HRESULT CPlayerData_Manager::Initialize()
{
    /* weapon */
    m_CachedEquipments[4001] = TEXT("Meteor_GSword");
    m_CachedEquipments[4002] = TEXT("Execution_GSword");
    m_CachedEquipments[4011] = TEXT("Flash_Spear");
    m_CachedEquipments[4012] = TEXT("Punish_Spear");

    /* Injured Set*/
    m_CachedEquipments[0] = TEXT("Prisoner_Face1");
    m_CachedEquipments[1] = TEXT("Prisoner_Hair1");
    m_CachedEquipments[2] = TEXT("Prisoner_Torso1");
    m_CachedEquipments[3] = TEXT("Prisoner_Arm1");
    m_CachedEquipments[4] = TEXT("Prisoner_Leg3");
    m_CachedEquipments[5] = TEXT("Prisoner_Shoes1");

    /* Thief Set */
    m_CachedEquipments[5001] = TEXT("Thief_Head");
    m_CachedEquipments[5002] = TEXT("Thief_Torso");
    m_CachedEquipments[5003] = TEXT("Thief_Arm");
    m_CachedEquipments[5004] = TEXT("Thief_Leg");
    m_CachedEquipments[5005] = TEXT("Thief_Shoes");

    /* 현재 장비 초기화  */
    m_ePlayerEquipment.iFace = 0;
    m_ePlayerEquipment.iHead =1;
    m_ePlayerEquipment.iTorso = 2;
    m_ePlayerEquipment.iArm = 3;
    m_ePlayerEquipment.iLeg = 4;
    m_ePlayerEquipment.iShoes = 5;
    m_ePlayerEquipment.iSpear = 4011;
    m_ePlayerEquipment.iGSword = 4001;
    m_ePlayerEquipment.isSpear = true;
    m_ePlayerEquipment.isGSword = false;
    UsedSpear();

    return S_OK;
}

void CPlayerData_Manager::Change_PlayerEquipment(EQUIPMENTTYPE eType, _uint iEquipmentIndex)
{
    switch (eType)
    {
    case Client::EQUIPMENTTYPE::SPEAR:
        m_ePlayerEquipment.iSpear = iEquipmentIndex;
        m_ePlayerEquipment.isGSword = false;
        m_ePlayerEquipment.isSpear = true;
        UsedSpear();
        break;
    case Client::EQUIPMENTTYPE::GSWORD:
        m_ePlayerEquipment.iSpear = iEquipmentIndex;
        m_ePlayerEquipment.isGSword = true;
        m_ePlayerEquipment.isSpear = false;
        UsedGSword();
        break;
    case Client::EQUIPMENTTYPE::HEAD:
        m_ePlayerEquipment.iHead = iEquipmentIndex;
        break;
    case Client::EQUIPMENTTYPE::TORSO:
        m_ePlayerEquipment.iTorso = iEquipmentIndex;
        break;
    case Client::EQUIPMENTTYPE::ARM:
        m_ePlayerEquipment.iArm = iEquipmentIndex;
        break;
    case Client::EQUIPMENTTYPE::LEG:
        m_ePlayerEquipment.iLeg = iEquipmentIndex;
        break;
    case Client::EQUIPMENTTYPE::SHOES:
        m_ePlayerEquipment.iShoes = iEquipmentIndex;
        break;
    default:
        break;
    }

    m_OnChangePlayerEquipment(eType, m_CachedEquipments[iEquipmentIndex]);
}

void CPlayerData_Manager::Set_PlayerEquipment(EQUIPMENTTYPE eType, _uint iEquipmentIndex)
{
    switch (eType)
    {
    case Client::EQUIPMENTTYPE::SPEAR:
        m_ePlayerEquipment.iSpear = iEquipmentIndex;
        m_ePlayerEquipment.isGSword = false;
        m_ePlayerEquipment.isSpear = true;
        UsedSpear();
        break;
    case Client::EQUIPMENTTYPE::GSWORD:
        m_ePlayerEquipment.iSpear = iEquipmentIndex;
        m_ePlayerEquipment.isGSword = true;
        m_ePlayerEquipment.isSpear = false;
        UsedGSword();
        break;
    case Client::EQUIPMENTTYPE::HEAD:
        m_ePlayerEquipment.iHead = iEquipmentIndex;
        break;
    case Client::EQUIPMENTTYPE::TORSO:
        m_ePlayerEquipment.iTorso = iEquipmentIndex;
        break;
    case Client::EQUIPMENTTYPE::ARM:
        m_ePlayerEquipment.iArm = iEquipmentIndex;
        break;
    case Client::EQUIPMENTTYPE::LEG:
        m_ePlayerEquipment.iLeg = iEquipmentIndex;
        break;
    case Client::EQUIPMENTTYPE::SHOES:
        m_ePlayerEquipment.iShoes = iEquipmentIndex;
        break;
    default:
        break;
    }
}

const _wstring& CPlayerData_Manager::Get_EquipmentName(_uint iEquipmentIndex) const
{
    auto iter = m_CachedEquipments.find(iEquipmentIndex);
    if (iter != m_CachedEquipments.end())
        return iter->second;

    /* 없으면  */
    _uint iTemp = iEquipmentIndex % 10;
    if (iTemp == 0)
        return  TEXT("Prisoner_Face1");
    if (iTemp == 1)
        return  TEXT("Prisoner_Hair1");
    if (iTemp == 2)
        return  TEXT("Prisoner_Torso1");
    if (iTemp == 3)
        return  TEXT("Prisoner_Arm1");
    if (iTemp == 4)
        return  TEXT("Prisoner_Leg3");
    if (iTemp == 5)
        return  TEXT("Prisoner_Shoes1");
    else
        return  TEXT("Prisoner_Face1");  //다 틀렸으면 그냥 얼굴 박기,,
}


CPlayerData_Manager* CPlayerData_Manager::Create()
{
    CPlayerData_Manager* pInstance = new CPlayerData_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CPlayerData_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerData_Manager::Free()
{
	__super::Free();
}
