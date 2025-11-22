#pragma once
#include "Client_Defines.h"
#include "Khazan_Spear_Animation.h"

NS_BEGIN(Client)

class CKhazan_GS_Anim_Fall final : public CKhazan_Spear_Animation
{
private:
    CKhazan_GS_Anim_Fall();
    virtual ~CKhazan_GS_Anim_Fall() = default;

public:
    void Enter() override;
    void Continue(_float fTimeDelta) override;
    void Exit() override;

public:
    _bool Force_StartFall();               // 일반 낙하 시작
    _bool Force_Landing();               // 일반 착지
    _bool Force_StartFallAttack();              // 낙하공격 시작
    _bool Force_AttackLanding();         // 낙공 착지 

public:
    _bool Is_Falling() const { return m_isFalling; }
    _bool Is_FallAttacking() const { return m_isFallAttacking; }

    void  Clear();

private:
    _bool m_isFallingStart = { false };
    _bool m_isFalling = { false };
    _bool m_isFallAttacking = { false };
    _bool m_isFallAttackingStart = { false };

public:
    static CKhazan_GS_Anim_Fall* Create();
    virtual void Free() override;
};

NS_END