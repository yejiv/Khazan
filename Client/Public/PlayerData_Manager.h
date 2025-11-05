
#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)


class CPlayerData_Manager final: public CBase
{

public:
    enum SPEARSKILL : _uint
    {
        MOONLIGHT_SLASH = 1 << 0,   // 달빛 베기        
        MOONLIGHT_STANCE = 1 << 1,   // 달빛 태세 (패시브)  
        MOONLIGHT_STANCE_VITALITY = 1 << 2,   // 달빛 태세: 활력 (패시브)
        FULL_MOON = 1 << 3,   // 보름달              
        SHADOW_SLASH = 1 << 4,   // 그림자 베기           
        SPIRAL_THRUST = 1 << 5,   // 나선 찌르기          
        SPIRAL_THRUST_WHIRLWIND = 1 << 6,   // 나선 찌르기: 소용돌이
        PURSUIT = 1 << 7,   // 추격 (패시브)                
        PURSUIT_DEVOUR = 1 << 8,   // 추격: 포식 (패시브)         
        ASSAULT = 1 << 9,   // 강습                
        MOMENT_SLASH = 1 << 10,   // 찰나 베기           
        AGILITY = 1 << 11,   // 기민함 (패시브)                
        CRITICAL_STRIKE = 1 << 12,   // 급소 타격        
        SHADOW_CLEAVE = 1 << 13,   // 그림자 참격          
        SMASH_DOWN = 1 << 14,   // 무너뜨리기 (패시브)             
        ENDURANCE = 1 << 15,   // 인내심 (패시브)              
        WILL_EXTRACTION = 1 << 16,   // 투지 추출 (패시브)        
        COMBATIVE_SPIRIT = 1 << 17,   // 호전적인 투지 (패시브)       
        BRUTAL_ATTACK_EXECUTION = 1 << 18,   // 브루탈 어택: 처형
        BRUTAL_ATTACK_HARVEST = 1 << 19,   // 브루탈 어택: 수확 (패시브)  
        SPEAR_THROW = 1 << 20,   // 투창 (추후 추가)
        SPEAR_THROW_REPOSE = 1 << 21,   // 투창: 안식 (추후 추가)
        COUNTER_ATTACK = 1 << 22,   // 카운터 어택         
        COUNTER_ATTACK_ONSLAUGHT = 1 << 23,   // 카운터 어택: 공세 (패시브)  

        SPEAR_END = 1 << 24,
    };


private:
	CPlayerData_Manager();
	virtual ~CPlayerData_Manager() = default;

public:
    //해당 스킬이 있는지
    _bool   Check_SpearSkill(_uint skill) { return m_iSpearSkill & skill; }
   // _bool   Check_GSwordSkill(_uint skill) { return m_iGSwordSkill & skill; }

    //모든 스킬 잠금 해제
    void    AllUnlock_SpearSkill() { m_iSpearSkill = m_iSpearSkill | (SPEAR_END - 1); }
   //void    AllUnlock_GswordSkill( ){ m_iGSwordSkill = m_iGSwordSkill | (GSWORD_END - 1);}

    // 단일 스킬 잠금 해제
    void    Unlock_SpearSkill(_uint skill) { m_iSpearSkill = m_iSpearSkill | skill; }
   // void    Unlock_GswordSkill(_uint skill) { m_iGSwordSkill = m_iGSwordSkill | skill; }

    // 모든 스킬 잠금
    void    Alllock_SpearSkill() { m_iSpearSkill &= ~(SPEAR_END - 1); }
    //void    Alllock_GswordSkill() { m_iGSwordSkill &= ~(GSWORD_END - 1); }

    // 단일 스킬 잠금 
    void    lock_SpearSkill(_uint skill) { m_iSpearSkill &= ~skill; }
    //void    lock_GswordSkill(_uint skill) { m_iGSwordSkill &= ~skill; }



private:
    _uint   m_iSpearSkill = { };
    _uint   m_iGSwordSkill = { };


public:
	static CPlayerData_Manager* Create();
	virtual void Free() override;

};


NS_END
