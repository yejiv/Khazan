#include "Player_Manager.h"

CPlayer_Manager::CPlayer_Manager()
{
}

HRESULT CPlayer_Manager::Initialize()
{
	m_Data.iLevel = 1;
	m_Data.iGold = 1000;
	m_Data.iLachryma = 500;
	m_Data.iSkillLevel = 10;
	m_Data.fSkillLevel_EXP = 55.f;
	m_Data.iSkilPoint = 10;

	return S_OK;
}

void CPlayer_Manager::Add_SkillExp(_float fExp)
{
	m_Data.fSkillLevel_EXP += fExp;
	while (m_Data.fSkillLevel_EXP >= 100.f)
	{
		++m_Data.iSkillLevel;
		++m_Data.iSkilPoint;
		m_Data.fSkillLevel_EXP -= 100.f;
	}
}

_bool CPlayer_Manager::Add_SkillPoint(_int iPoint)
{
	if (m_Data.iSkilPoint - iPoint < 0)
		return false;
	
	m_Data.iSkilPoint += iPoint;

	return true;
}

CPlayer_Manager* CPlayer_Manager::Create()
{
	CPlayer_Manager* pInstance = new CPlayer_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Manager"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Manager::Free()
{
	__super::Free();
}
