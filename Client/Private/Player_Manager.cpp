#include "Player_Manager.h"

CPlayer_Manager::CPlayer_Manager()
{
}

HRESULT CPlayer_Manager::Initialize()
{
    m_Data.fMaxHp = 1000;
    m_Data.fCulHp = 1000;
    m_Data.fMaxStamina = 1000;
    m_Data.fCulStamina = 1000;
    m_Data.fStaminaRegen = 100.f;
    m_Data.iMaxDoggednessCount = 5;
    m_Data.fCulDoggedness = 5;

    m_Data.fDamage = 50;
    m_Data.fGuard = 0;

	m_Data.iLevel = 1;
	m_Data.iGold = 1000;
	m_Data.iLachryma = 500;
    m_Data.iStone = 0;

	m_Data.iSkillLevel = 10;
	m_Data.iSkilPoint = 10;
	m_Data.fSkillLevel_EXP = 0.f;
    
    m_Data.iVitality = 10;
    m_Data.iEndurance = 10;
    m_Data.iPower = 10;
    m_Data.iCompetency = 10;
    m_Data.iWill = 10;

    m_Data.fWeight = 0.f;

    m_UsedSkill.assign(GetBitPosition(CPlayerData_Manager::SPEAR_END), false);

    /* 임시  */
    BindSkillToButton(Q, CPlayerData_Manager::FULL_MOON);
    BindSkillToButton(E, CPlayerData_Manager::SPIRAL_THRUST);
    BindSkillToButton(R, CPlayerData_Manager::SHADOW_CLEAVE);

	return S_OK;
}

PLAYER_DATA* CPlayer_Manager::Get_pInitailizePlayerData()
{
    m_Data.fCulHp = m_Data.fMaxHp;
    m_Data.fCulStamina = m_Data.fMaxStamina;
    m_Data.fCulDoggedness = static_cast<_float>(m_Data.iMaxDoggednessCount);

    return &m_Data;
}

void CPlayer_Manager::Initialize_PlayerData()
{
    m_Data.fCulHp = m_Data.fMaxHp;
    m_Data.fCulStamina = m_Data.fMaxStamina;
    m_Data.fCulDoggedness = static_cast<_float>(m_Data.iMaxDoggednessCount);
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

void CPlayer_Manager::BindSkillToButton(CONTROL_BUTTON eButton, _uint iSkill)
{
    m_ButtonToSkill[eButton] = iSkill;
}

void CPlayer_Manager::UnBindSkillToButton(CONTROL_BUTTON eButton)
{
    m_ButtonToSkill.erase(eButton);
}

_uint CPlayer_Manager::Get_ButtonSkill(CONTROL_BUTTON eButton)
{
    unordered_map<CONTROL_BUTTON, _uint>::iterator  it = m_ButtonToSkill.find(eButton);
    if (it != m_ButtonToSkill.end())  return it->second;
    return 0; // 바인딩된 스킬 없음
}

void CPlayer_Manager::Set_UsedSkill(_uint iSkill, _bool isUsed)
{
    if (0 >= iSkill && iSkill >= GetBitPosition(CPlayerData_Manager::SPEAR_END))
        return;

    m_UsedSkill[GetBitPosition(iSkill)] = isUsed;
}

_bool CPlayer_Manager::Is_UsedSkill(_uint iSkill)
{
    if (0 >= iSkill && iSkill >= GetBitPosition(CPlayerData_Manager::SPEAR_END))
        return false;

    return  m_UsedSkill[GetBitPosition(iSkill)];
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
