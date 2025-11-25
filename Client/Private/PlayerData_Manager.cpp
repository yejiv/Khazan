#include "PlayerData_Manager.h"


CPlayerData_Manager::CPlayerData_Manager()
{
    UsedSpear();
    AllUnlock_Skill();
    UsedGSword();
    AllUnlock_Skill();

}


CPlayerData_Manager* CPlayerData_Manager::Create()
{
	return new CPlayerData_Manager;
}

void CPlayerData_Manager::Free()
{
	__super::Free();
}
