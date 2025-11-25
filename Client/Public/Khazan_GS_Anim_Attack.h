#pragma once

#include "Client_Defines.h"

#include "Khazan_GS_Animation.h"

NS_BEGIN(Client)


class CKhazan_GS_Anim_Attack final: public CKhazan_GS_Animation
{
public:
    typedef struct	tagKhazanWSwordAnimAttack
    {
        _bool				isEquipWeapon = {};
        _uint				iSubState;
        _uint				iSkill;

    }WSWORD_ATTACK;

private:
    CKhazan_GS_Anim_Attack();
    virtual ~CKhazan_GS_Anim_Attack() = default;

public:
    HRESULT			Initialize();
    void            Enter() override;
    void            Continue(_float fTimeDelta) override;
    void            Exit() override;

public:
    _bool   Try_FastAttack(); //combo
    _bool   Try_ChageFastAttack();
    void    Execute_ChargedFastAttack();

    _bool   Try_StrongAttack();
    _bool   Try_ChageStrongAttack();
    void    Execute_ChargedStrongAttack();

    _bool   Try_SprintAttack();
    void    Execute_SprintAttack();         

    _bool   Try_DodgeAttack(_uint iDir);
    void    Execute_DodgeAttack();     

    _bool	Try_GrappleAttack(); 

    _bool	Try_SkillAttack(_uint iSkill);




    _bool    Reserve_SkillAttack(_uint iSkill);
    void    Clear_Skill();
    //void    Clear_Attack();
    void    Clear_All();

    _bool	Is_Attacking() const { return m_isAttacking; }
    _bool   Is_FastAttacking() const { return m_isFastCombo; }
    _bool   Is_Skilling() const { return m_isSkilling; }
    _bool	Can_NextCombo() const { return m_isCanNextCombo; }
    _bool   Is_Reserve() const { return m_isReserveSkill; }
    _uint	Get_CurrentCombo() const { return m_iCurrentCombo; }

    _bool   Is_PossibleBreathtaking();              //스킬 숨통끊기 사용 가능한지?
    _bool   Is_PossibleBreathtaking_BloodShed();    //스킬 숨통끊기 : 선혈 사용 가능한지?    
    _bool   Is_PossibleBreathtaking_Embryonic();    //스킬 숨통끊기 : 태동 사용 가능한지?

private:
    class CClientInstance* m_pClientInstance = { nullptr };
    PLAYER_DATA* m_pPlayerData = { nullptr };


    _bool               m_isSkilling = { false };
    _bool			    m_isAttacking = { false };
    _bool               m_isReserveSkill = { false };
    _uint               m_iReserveSkillIndex = { 0 };

    /* Fast Combo */
    _uint			    m_iCurrentCombo = { 0 };
    _bool			    m_isFastCombo = { false };
    _bool			    m_isCanNextCombo = { false };
    const _float	    m_fFastAttackComboPossibleMaxFrame = { 40.f };

    /* Fast Charge */
    _uint               m_iLastFastComboIndex = { 0 };
    _bool               m_isInFastCombo = { false };        //패스트콤보중이었는지?
    _bool               m_isFastChargeStart = { false };    //차지 시작했는가?
    _bool               m_isFastChargeEnd = { false };      //차징이 끝났는가?
    _bool               m_isFastChargedAttack = { false };  //차징 공격을 했는지? 

    /* Strong */
    _bool               m_isStrongAttack = { false };

    /* Strong Charge */
    _bool               m_isStrongChargeStart = { false };   //차지 시작했는가?
    _bool               m_isStrongChargeEnd = { false };     //차징이 끝났는가?
    _bool               m_isStrongChargedAttack = { false };  //차징 공격을 했는지? 

    /* Sprint */
    _bool               m_isSprintChargeStart = { false };   //차지 시작했는가?
    _bool               m_isSprintChargeEnd = { false };     //차징이 끝났는가?
    _bool               m_isSprintChargedAttack = { false };  //차징 공격을 했는지? 

    /* Dodge */
    _uint               m_iDodgeDirection = { 0 };
    _bool               m_isDodgeChargeStart = { false };    //차지 시작했는가?
    _bool               m_isDodgeChargeEnd = { false };      //차징이 끝났는가?
    _bool               m_isDodgeChargedAttack = { false };  //차징 공격을 했는지? 

    /* Brutal */
    _bool               m_isAutoComboBrutal = { false };


    /* SKill */
    _uint               m_iCurSkillIndex = { 0 };
    _bool               m_isSkillChargeEnd = { false };             //스킬 차징이 끝났는지
    _bool               m_isSkillChargeAttack = { false };          //스킬 차징 공격을 했는지 
    _bool               m_isSkillSingle = { false };                //스킬 단일 공격
    _bool               m_isBreathaking = { false };                //숨통끊기 
    _bool               m_isBreathaking_Embryonic = { false };      //숨통끊기 태동
    _bool               m_isBreathaking_Bloodshed = { false };      //숨통끊기 선혈
    _bool               m_isGiantHunt = { false };                  // 거인사냥 
    _bool               m_isPhantom = { false };                    // 귀신 : 어둠의 그림자 
    _bool               m_isBreakThrough = { false };               // 정면 돌파
    _bool               m_isWarCry = { false };                     // 거대한 포효
    _bool               m_isInnerFury = { false };                  // 내재된 분노

    /* passive +  Assigned Skill Key*/
    _bool               m_isPossibleBreathtaking = { false };
    //_bool               m_isPossibleBreathtaking_BloodShed = { false };
    //_bool               m_isPossibleBreathtaking_Embryonic = { false };

    /* Caching */
    _uint               m_iCachedFastAttackAnimIndices[3];
    _uint               m_iCachedFastChargeAnimIndices[3];
    _uint               m_iCachedFastChargeAttackAnimIndices[3];

private:
    void            Update_FastAttackCombo();
    void            Update_FastAttackCharge();
    void            Update_StrongAttack();
    void            Update_StrongAttackCharge();
    void            Update_SprintAttackCharge();
    void            Update_DodgeAttackCharge();

    void            Update_Skill_Single();
    void            Update_Skill_Breathaking();
    void            Update_Skill_Breathaking_Embryonic();
    void            Update_Skill_Breathaking_Bloodshed();
    void            Update_Skill_GiantHunt();
    void            Update_Skill_Phantom();
    void            Update_Skill_BreakThrough();
    //void            Update_Skill_WarCry();
    void            Update_Skill_InnerFury();

    void            Execute_Skill_Breathaking();
    void            Execute_Skill_Breathaking_Embryonic();
    void            Execute_Skill_Breathaking_Bloodshed();
    void            Execute_Skill_GiantHunt();
    void            Execute_Skill_Phantom();
    void            Execute_Skill_BreakThrough();
    //void            Execute_Skill_WarCry();
    void            Execute_Skill_InnerFury();

public:
    static CKhazan_GS_Anim_Attack* Create();
    virtual void Free() override;


};


NS_END
