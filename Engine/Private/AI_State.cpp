#include "AI_State.h"
#include "GameInstance.h"
CAI_State::CAI_State()
	:m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

void CAI_State::Free()
{
	__super::Free();
	
	Safe_Release(m_pGameInstance);
}
