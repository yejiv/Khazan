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
    void    Reserve_SkillAttack(_uint iSkill);
    void    Clear_Skill();
    void    Clear_Attack();
    void    Clear_All();

    _bool	Is_Attacking() const { return m_isAttacking; }
    _bool   Is_FastAttacking() const { return m_isFastCombo; }
    _bool   Is_Skilling() const { return m_isSkilling; }
    _bool	Can_NextCombo() const { return m_isCanNextCombo; }
    _bool   Is_Reserve() const { return m_isReserve; }
    _uint	Get_CurrentCombo() const { return m_iCurrentCombo; }


private:
    class CClientInstance* m_pClientInstance = { nullptr };
    PLAYER_DATA* m_pPlayerData = { nullptr };


    _bool           m_isSkilling = { false };
    _bool			m_isAttacking = { false };
    _bool			m_isCanNextCombo = { false };
    _bool           m_isReserve = { false };
    _uint           m_iReserveSkillIndex = {};


    /* Combo */
    _uint			m_iCurrentCombo = { };
    _float			m_fComboTime = { 0.f };
    _bool			m_isFastCombo = { false };
    _bool			m_isStrongCombo = { false };
    _bool			m_isStrongCharge = { false };
    const _float	m_fFastAttackComboPossibleMaxFrame = { 30.f };
    const _float	m_fStrongAttackComboPossibleMaxFrame = { 30.f };

    _bool           m_isAutoComboBrutal = { false };

    /* SKill */
    _uint           m_iCurSkillIndex = { };

public:
    static CKhazan_GS_Anim_Attack* Create();
    virtual void Free() override;


};


NS_END
