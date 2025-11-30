
#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)


class CPlayerData_Manager final: public CBase
{

public:
    enum SPEARSKILL : _uint
    {
        MOONLIGHT_SLASH = 1 << 0,   // 달빛 베기                            (패시브)        
        MOONLIGHT_STANCE = 1 << 1,   // 달빛 태세                           (패시브)  
        MOONLIGHT_STANCE_VITALITY = 1 << 2,   // 달빛 태세: 활력             (패시브)
        FULL_MOON = 1 << 3,   // 보름달                                     (액티브 @@)      
        SHADOW_SLASH = 1 << 4,   // 그림자 베기                              (패시브) 
        SPIRAL_THRUST = 1 << 5,   // 나선 찌르기                             (액티브 @@)
        SPIRAL_THRUST_WHIRLWIND = 1 << 6,   // 나선 찌르기: 소용돌이         (패시브)
        PURSUIT = 1 << 7,   // 추격                                           (패시브)                                 
        PURSUIT_DEVOUR = 1 << 8,   // 추격: 포식                            (패시브)         
        ASSAULT = 1 << 9,   // 강습                                       (조작키 고정되어있음)
        MOMENT_SLASH = 1 << 10,   // 찰나 베기                              (패시브) 
        AGILITY = 1 << 11,   // 기민함                                     (패시브)                
        CRITICAL_STRIKE = 1 << 12,   // 급소 타격                           (패시브)
        SHADOW_CLEAVE = 1 << 13,   // 그림자 참격                            (액티브 @@ )
        SMASH_DOWN = 1 << 14,   // 무너뜨리기                                (패시브)             
        ENDURANCE = 1 << 15,   // 인내심                                   (패시브)              
        WILL_EXTRACTION = 1 << 16,   // 투지 추출                        (패시브)        
        COMBATIVE_SPIRIT = 1 << 17,   // 호전적인 투지                    (패시브)       
        BRUTAL_ATTACK_EXECUTION = 1 << 18,   // 브루탈 어택: 처형          (?)
        BRUTAL_ATTACK_HARVEST = 1 << 19,   // 브루탈 어택: 수확            (패시브)  
        SPEAR_THROW = 1 << 20,   // 투창                              (추후 추가)
        SPEAR_THROW_REPOSE = 1 << 21,   // 투창: 안식                   (추후 추가)
        COUNTER_ATTACK = 1 << 22,   // 카운터 어택                       (?)
        COUNTER_ATTACK_ONSLAUGHT = 1 << 23,   // 카운터 어택: 공세     (패시브)  

        SPEAR_END = 1 << 24,
    };

    enum GSWORDSKILL : _uint
    {
        MOMENTUM                        = 1 << 0,           //거센기세
        BREATHTAKING                    = 1 << 1,           //숨통끊기
        BREATHTAKING_EMBRYONIC          = 1 << 2,           //숨통끊기 : 태동
        BREATHTAKING_BLOODSHED          = 1 << 3,           //숨통끊기 : 선혈
        MANIFESTSTRENGTH                = 1 << 4,           //강기발현
        GIANTHUNT                       = 1 << 5,           //거인사냥 command
        WILLPOWER_UP                    = 1 << 6,           //투지증폭
        PHANTOM_SHADOWOFDARKNESS        = 1 << 7,           //귀신 : 어둠의 그림자 command
        LIMIT_BREAK                     = 1 << 8,           //한계극복
        HEATOFBATTLEFIEDLD              = 1 << 9,           //전장의 열기
        BREAK_THROUGH                   = 1 << 10,          //정면돌파 command
        WARCRY                          = 1 << 11,          //거대한 포효 command
        INNER_FURY                      = 1 << 12,          //내제된분노 command
        BRIDLOFBATTLE                   = 1 << 13,          //전투의 굴레
        DODGE_ENERGY                    = 1 << 14,          //회피 : 기력
        COUNTER_OPPORTUNITY             = 1 << 15,          //역공의 기회
        DEFENSE_ENERGY                  = 1 << 16,          //방어 : 기력
        DEFENSE_FIGHT                   = 1 << 17,          //방어 : 투지

        GSWORD_END                      = 1 << 18,
    };

    typedef struct tagPlayerEquipmentDesc
    {
        _uint   iSpear;
        _uint   iGSword;
        _uint   iHead;
        _uint   iTorso;
        _uint   iArm;
        _uint   iLeg;
        _uint   iShoes;
        _uint   iFace;
        _bool   isSpear = { false };
        _bool   isGSword = { false };
    }PLAYER_EQUIPMENT;

   // enum    EQUIPMENTTYPE { NONE, SPEAR, GSWORD, HEAD, TORSO, ARM, LEG, SHOES, FACE, /*END*/ };

    

private:
	CPlayerData_Manager();
	virtual ~CPlayerData_Manager() = default;

public:
    HRESULT Initialize();

public:
    //해당 스킬이 있는지
    _bool   Check_Skill(_uint skill)
    { 
        if(m_isCurSpear)
            return (m_iSpearSkill & skill) != 0;
        if(m_isCurGSword)
            return  (m_iGSwordSkill & skill) != 0;
    }

    _bool   Check_Skills(_uint skills)
    {
        if (m_isCurSpear)
            return (m_iSpearSkill & skills) == skills;
        if (m_isCurGSword)
            return (m_iGSwordSkill & skills) == skills;
    }

    //모든 스킬 잠금 해제
    void    AllUnlock_Skill() 
    {
        if (m_isCurSpear)
            m_iSpearSkill = m_iSpearSkill | (SPEAR_END - 1);
        if (m_isCurGSword)
            m_iGSwordSkill = m_iGSwordSkill | (GSWORD_END - 1);
    }

    // 단일 스킬 잠금 해제
    void    Unlock_Skill(_uint skill)
    {
        if (m_isCurSpear)
            m_iSpearSkill = m_iSpearSkill | skill;
        if (m_isCurGSword)
            m_iGSwordSkill = m_iGSwordSkill | skill;
    }

    // 모든 스킬 잠금
    void    Alllock_Skill()
    { 
        if (m_isCurSpear)
            m_iSpearSkill &= ~(SPEAR_END - 1);
        if (m_isCurGSword)
            m_iGSwordSkill &= ~(GSWORD_END - 1);
    }

    // 단일 스킬 잠금 
    void    Lock_Skill(_uint skill)
    {
        if (m_isCurSpear)
            m_iSpearSkill &= ~skill;
        if (m_isCurGSword)
            m_iGSwordSkill &= ~skill;
    }

    void    UsedSpear() {  m_isCurSpear = true; m_isCurGSword = false; }
    void    UsedGSword() { m_isCurSpear = false; m_isCurGSword = true; }
    _bool   Is_CurrentSpear() const { return m_isCurSpear; }
    _bool   Is_CurrentGSword() const { return m_isCurGSword; }

    /* Equipment */
    void                        Set_ChangePlayerEquipmentCallBack(function<void(EQUIPMENTTYPE, const _wstring&)> callback) { m_OnChangePlayerEquipment = callback; }
    void                        Change_PlayerEquipment(EQUIPMENTTYPE eType, _uint iEquipmentIndex);  // ui 장착버튼같은거 누를 시 사용해주세요 EQUIPMENTTYPE  enum class로 뺐음!!
    const PLAYER_EQUIPMENT&     Get_PlayerEquipment() const { return m_ePlayerEquipment; }
    void                        Set_PlayerEquipment(EQUIPMENTTYPE eType, _uint iEquipmentIndex);  // 테스트용 Setter 
    const _wstring&             Get_EquipmentName(_uint iEquipmentIndex) const;

private:
    /* Skill */
    _uint                                       m_iSpearSkill = { };
    _uint                                       m_iGSwordSkill = { };
    _bool                                       m_isCurSpear = { false };
    _bool                                       m_isCurGSword = { false };

    /* Equipment */
    PLAYER_EQUIPMENT                            m_ePlayerEquipment{};
    unordered_map<_uint, _wstring>              m_CachedEquipments;
    function<void(EQUIPMENTTYPE, const _wstring&)>      m_OnChangePlayerEquipment;   
public:
	static CPlayerData_Manager* Create();
	virtual void Free() override;

};


NS_END
