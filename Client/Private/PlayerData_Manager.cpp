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

    /* Nude Set*/
    m_CachedEquipments[10] = TEXT("Nude_Face");
    m_CachedEquipments[20] = TEXT("Nude_Hair1");   //긴 머리
    m_CachedEquipments[21] = TEXT("Nude_Hair2");  //짧은 머리
    m_CachedEquipments[30] = TEXT("Nude_Torso");
    m_CachedEquipments[40] = TEXT("Nude_Arm");
    m_CachedEquipments[50] = TEXT("Nude_Leg");
    m_CachedEquipments[60] = TEXT("Nude_Shoes");

    /* Injured Set*/
    m_CachedEquipments[9000] = TEXT("Prisoner_Face1");
    m_CachedEquipments[9001] = TEXT("Prisoner_Hair1");      //긴머리 
    m_CachedEquipments[9002] = TEXT("Prisoner_Torso1");
    m_CachedEquipments[9003] = TEXT("Prisoner_Arm1");
    m_CachedEquipments[9004] = TEXT("Prisoner_Leg3");
    m_CachedEquipments[9005] = TEXT("Prisoner_Shoes1");

    /* Thief Set */
    m_CachedEquipments[5001] = TEXT("Thief_Hair");      //긴머리 
    m_CachedEquipments[5002] = TEXT("Thief_Torso");
    m_CachedEquipments[5003] = TEXT("Thief_Arm");
    m_CachedEquipments[5004] = TEXT("Thief_Leg");
    m_CachedEquipments[5005] = TEXT("Thief_Shoes");

    /* ShadowLandFlow Set */
    m_CachedEquipments[5006] = TEXT("ShadowLandFlow_Hair");     //투구
    m_CachedEquipments[5007] = TEXT("ShadowLandFlow_Torso");
    m_CachedEquipments[5008] = TEXT("ShadowLandFlow_Arm");
    m_CachedEquipments[5009] = TEXT("ShadowLandFlow_Leg");
    m_CachedEquipments[5010] = TEXT("ShadowLandFlow_Shoes");

    /* Disc */
    m_CachedEquipments[5011] = TEXT("Danjin_Hair");   //단진 머리
    

    /* 현재 장비 초기화  */
    m_ePlayerEquipment.iFace = 9000;
    m_ePlayerEquipment.iHair = 21;
    m_ePlayerEquipment.iTorso = 9002;
    m_ePlayerEquipment.iArm = 9003;
    m_ePlayerEquipment.iLeg = 9004;
    m_ePlayerEquipment.iShoes = 9005;
    m_ePlayerEquipment.iSpear = 4011;
    m_ePlayerEquipment.iGSword = 4001;
    m_ePlayerEquipment.isSpear = true;
    m_ePlayerEquipment.isGSword = false;
    UsedSpear();

    //m_ePlayerEquipment.iFace = 10;
    //m_ePlayerEquipment.iHair = 5006;
    //m_ePlayerEquipment.iTorso = 5007;
    //m_ePlayerEquipment.iArm = 5008;
    //m_ePlayerEquipment.iLeg = 5009;
    //m_ePlayerEquipment.iShoes = 5010;
    //m_ePlayerEquipment.iSpear = 4011;
    //m_ePlayerEquipment.iGSword = 4002;
    //m_ePlayerEquipment.isSpear = false;
    //m_ePlayerEquipment.isGSword = true;
    //UsedGSword();

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
        m_OnChangePlayerEquipment[0](eType, m_CachedEquipments[iEquipmentIndex]);
        return;;
    case Client::EQUIPMENTTYPE::GSWORD:
        m_ePlayerEquipment.iSpear = iEquipmentIndex;
        m_ePlayerEquipment.isGSword = true;
        m_ePlayerEquipment.isSpear = false;
        UsedGSword();
        m_OnChangePlayerEquipment[0](eType, m_CachedEquipments[iEquipmentIndex]);
        return;
    case Client::EQUIPMENTTYPE::HEAD:
        m_ePlayerEquipment.iHair = iEquipmentIndex;
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
    case Client::EQUIPMENTTYPE::FACE:
        m_ePlayerEquipment.iFace = iEquipmentIndex;
        break;
    default:
        break;
    }

    m_OnChangePlayerEquipment[1](eType, m_CachedEquipments[iEquipmentIndex]);
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
        m_ePlayerEquipment.iHair = iEquipmentIndex;
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
    case Client::EQUIPMENTTYPE::FACE:
        m_ePlayerEquipment.iFace = iEquipmentIndex;
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
        return  TEXT("Nude_Face");
    if (iTemp == 1)
        return  TEXT("Nude_Hair2");
    if (iTemp == 2)
        return  TEXT("Nude_Torso");
    if (iTemp == 3)
        return  TEXT("Nude_Arm");
    if (iTemp == 4)
        return  TEXT("Nude_Leg");
    if (iTemp == 5)
        return  TEXT("Nude_Shoes");
    else
        return  TEXT("Nude_Face");  //다 틀렸으면 그냥 얼굴 박기,,
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
