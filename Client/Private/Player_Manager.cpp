#include "Player_Manager.h"
#include "ClientInstance.h"

CPlayer_Manager::CPlayer_Manager()
    :m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CPlayer_Manager::Initialize()
{
    m_Data.iSouleCount = 3;

    m_Data.fMaxHp = { 1000.f };
    m_Data.fCulHp = { 1000.f };
    m_Data.fMaxStamina = { 1000.f };
    m_Data.fCulStamina = { 1000.f };
    m_Data.fStaminaRegen = { 200.f };
    m_Data.iMaxDoggednessCount = { 5 };
    m_Data.fCulDoggedness = { 5.f };

    m_Data.fDamage = 10;
    m_Data.fGuard = 0;

	m_Data.iLevel = 1;
	m_Data.iGold = 1000;
	m_Data.iLachryma = 500;
    m_Data.iStone = 0;
    m_Data.iLevelStone = 0;

    m_Data.iSkillLevel = { 10 };
    m_Data.iSkilPoint = { 10 };
    m_Data.fSkillLevel_EXP = { 0.f };
    
    m_Data.iVitality = { 10 };
    m_Data.iEndurance = { 10 };
    m_Data.iPower = { 10 };
    m_Data.iCompetency = { 10 };
    m_Data.iWill = { 10 };

    m_Data.fWeight = { 0.f };

    m_UsedSpearSkill.assign(GetBitPosition(CPlayerData_Manager::SPEAR_END), false);

    m_UsedGSwordSkill.assign(GetBitPosition(CPlayerData_Manager::GSWORD_END), false);

    /* 임시  */
    m_pClientInstance->UsedSpear();
    BindSkillToButton(Q, CPlayerData_Manager::FULL_MOON);
    BindSkillToButton(E, CPlayerData_Manager::SPIRAL_THRUST);
    BindSkillToButton(R, CPlayerData_Manager::SHADOW_CLEAVE);

    /* 임시 */
    m_pClientInstance->UsedGSword();
    BindSkillToButton(Q, CPlayerData_Manager::GIANTHUNT);   /* 거인 사냥 */
    BindSkillToButton(E, CPlayerData_Manager::PHANTOM_SHADOWOFDARKNESS);    /* 귀신 : 어둠의 그림자*/
    BindSkillToButton(R, CPlayerData_Manager::BREAK_THROUGH);   /* 정면 돌파 */
    BindSkillToButton(CTRL_LB, CPlayerData_Manager::WARCRY);    /* 거대한 포효 */ 
    BindSkillToButton(CTRL_RB, CPlayerData_Manager::INNER_FURY);    /* 내재된 분노 */

    m_pClientInstance->UsedSpear();

	return S_OK;
}

PLAYER_DATA* CPlayer_Manager::Get_pInitailizePlayerData()
{
    m_Data.iSouleCount = 3;
    m_Data.fCulHp = m_Data.fMaxHp;
    m_Data.fCulStamina = m_Data.fMaxStamina;
    m_Data.fCulDoggedness = static_cast<_float>(m_Data.iMaxDoggednessCount);

    return &m_Data;
}

void CPlayer_Manager::Initialize_PlayerData()
{
    m_Data.iSouleCount = 3;
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
    if(m_pClientInstance->Is_CurrentSpear())
        m_ButtonToSpearSkill[eButton] = iSkill;
    
    if (m_pClientInstance->Is_CurrentGSword())
        m_ButtonToGSwordSkill[eButton] = iSkill;

}

void CPlayer_Manager::UnBindSkillToButton(CONTROL_BUTTON eButton)
{
    if (m_pClientInstance->Is_CurrentSpear())
        m_ButtonToSpearSkill.erase(eButton);

    if (m_pClientInstance->Is_CurrentGSword())
        m_ButtonToGSwordSkill.erase(eButton);
}

_uint CPlayer_Manager::Get_ButtonSkill(CONTROL_BUTTON eButton)
{
    if (m_pClientInstance->Is_CurrentSpear())
    {
        unordered_map<CONTROL_BUTTON, _uint>::iterator  it = m_ButtonToSpearSkill.find(eButton);
        if (it != m_ButtonToSpearSkill.end())  return it->second;
        return 0; // 바인딩된 스킬 없음
    }

    if (m_pClientInstance->Is_CurrentGSword()) 
    {
        unordered_map<CONTROL_BUTTON, _uint>::iterator  it = m_ButtonToGSwordSkill.find(eButton);
        if (it != m_ButtonToGSwordSkill.end())  return it->second;
        return 0; // 바인딩된 스킬 없음
    }
    return 0;
}

void CPlayer_Manager::Set_UsedSkill(_uint iSkill, _bool isUsed)
{
    if (m_pClientInstance->Is_CurrentSpear())
    {
        if (0 >= iSkill || GetBitPosition(iSkill) >= GetBitPosition(CPlayerData_Manager::SPEAR_END))
            return;
        m_UsedSpearSkill[GetBitPosition(iSkill)] = isUsed;
    }

    if (m_pClientInstance->Is_CurrentGSword())
    {
        if (0 >= iSkill || GetBitPosition(iSkill) >= GetBitPosition(CPlayerData_Manager::GSWORD_END))
            return;
        m_UsedGSwordSkill[GetBitPosition(iSkill)] = isUsed;
    }
}


void CPlayer_Manager::Set_UsedSkills(_uint iSkill, _bool isUsed)
{
    if (iSkill == 0)
        return;

    // 스킬 비트 전체 체크 (0~31)
    const _uint highest = GetHighestBitPosition(iSkill);

    if (m_pClientInstance->Is_CurrentSpear())
    {
        const _uint spearEnd = GetBitPosition(CPlayerData_Manager::SPEAR_END);

        for (_uint bit = 0; bit <= highest; ++bit)
        {
            if (iSkill & (1u << bit))
            {
                if (bit >= spearEnd)  continue; // 범위 밖이면 무시
                m_UsedSpearSkill[bit] = isUsed;
            }
        }
    }

    if (m_pClientInstance->Is_CurrentGSword())
    {
        const _uint gsEnd = GetBitPosition(CPlayerData_Manager::GSWORD_END);

        for (_uint bit = 0; bit <= highest; ++bit)
        {
            if (iSkill & (1u << bit))
            {
                if (bit >= gsEnd)   continue;
                m_UsedGSwordSkill[bit] = isUsed;
            }
        }
    }
}

void CPlayer_Manager::Set_UnUsedAllSkills()
{
    if (m_pClientInstance->Is_CurrentSpear())
    {
        for (_bool isUsed : m_UsedSpearSkill)
            isUsed = false; 
    }

    if (m_pClientInstance->Is_CurrentGSword())
    {
        for (_bool isUsed : m_UsedGSwordSkill)
            isUsed = false;
    }
}

_bool CPlayer_Manager::Is_UsedSkill(_uint iSkill)
{
    if (m_pClientInstance->Is_CurrentSpear()) 
    {
        if (0 >= iSkill && GetBitPosition(iSkill) >= GetBitPosition(CPlayerData_Manager::SPEAR_END))
            return false;
        return  m_UsedSpearSkill[GetBitPosition(iSkill)];
    }

    if (m_pClientInstance->Is_CurrentGSword())
    {
        if (0 >= iSkill && GetBitPosition(iSkill) >= GetBitPosition(CPlayerData_Manager::GSWORD_END))
            return false;
        return  m_UsedGSwordSkill[GetBitPosition(iSkill)];
    }
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
    Safe_Release(m_pClientInstance);
}
