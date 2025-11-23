#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Khazan_GS_StateMachine.h"

NS_BEGIN(Client)

using   GS_COMMAND = CKhazan_GS_StateMachine::GS_COMMAND;
using   GS_MAIN = CKhazan_GS_StateMachine::GSSM_MAIN;
using   GS_MOVE = CKhazan_GS_StateMachine::GSSM_MOVE;
using   GS_ATTACK = CKhazan_GS_StateMachine::GSSM_ATTACK;
using   GS_GUARD = CKhazan_GS_StateMachine::GSSM_GUARD;
using   GS_WEAPON = CKhazan_GS_StateMachine::GSSM_WEAPON;
using   GS_CYCLE = CKhazan_GS_StateMachine::GSSM_CYCLE;


class CKhazan_GS_AnimationController final : public CBase
{
//public:
//    // ===== 애니메이션 정보 구조체 =====
//    struct ANIM_INFO
//    {
//        _uint       iAnimIndex = 0;
//        _bool       isEquipWeapon = false;
//        _bool       isLockOn = false;
//        _uint       iDirection = 0;
//        _uint       iCycle = 0;         // START, LOOP, END
//    };

private:
    CKhazan_GS_AnimationController();
    virtual ~CKhazan_GS_AnimationController() = default;

public:
    HRESULT                             Initialize(class CModel* pModel, _uint* pCurWaepon, _bool* pLockOn);
    void                                Update(_float fTimeDelta);

    /* 애니메이션 모듈별 실행 */ 
    void                                Execute_Move();
    void                                Execute_Attack();
    void                                Execute_Guard();
    void                                Execute_Damaged();
    void                                Execute_Skill();

    /* 상태 쿼리  */
    _bool                               Is_AnimationFinished() const;
    _bool                               Is_InComboWindow() const;
    _float                              Get_CurrentAnimProgress() const;

    /* Getters */ 
    class CKhazan_GS_Anim_Move*         Get_MoveAnim() { return m_pAnimMove; }
    class CKhazan_GS_Anim_Attack*       Get_AttackAnim() { return m_pAnimAttack; }
    class CKhazan_GS_Anim_Guard*        Get_GuardAnim() { return m_pAnimGuard; }
    class CKhazan_GS_Anim_Damaged*      Get_DamagedAnim() { return m_pAnimDamaged; }

    /* Setting */
    void                                Set_Command(GS_COMMAND eCommand) { m_eCommand = eCommand; }

private:
    class CModel*                       m_pModel = { nullptr };

    class CKhazan_GS_Anim_Move*         m_pAnimMove = { nullptr };
    class CKhazan_GS_Anim_Attack*       m_pAnimAttack = { nullptr };
    class CKhazan_GS_Anim_Guard*        m_pAnimGuard = { nullptr };
    class CKhazan_GS_Anim_Damaged*      m_pAnimDamaged = { nullptr };

    GS_COMMAND                          m_eCommand;
    _uint*                              m_pCurWeapon = { nullptr };       //현재 무기
    _bool*                              m_pLockOn = { nullptr };

    _uint                               m_iCurMainState = { GS_MAIN::GS_IDLE };
    _uint                               m_iPrevMainState = { 0 };
    _uint                               m_iCurSubState = { 0 };
    _uint                               m_iPrevSubState = { 0 };
    DIRECTION_INFO					    m_eCurDir;		//플레이어의 로컬 방향  dir(애니메이션 선택용)
    _uint						        m_iPrevDir = {};

//private:
//    ANIM_INFO&      Create_AnimInfo();

public:
    static CKhazan_GS_AnimationController* Create(class CModel* pModel, _uint* pCurWaepon, _bool* pLockOn);
    virtual void Free() override;
};

NS_END