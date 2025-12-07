#pragma once
#include "Client_Defines.h"
#include "Khazan_Spear_Animation.h"

NS_BEGIN(Client)

class CKhazan_Spear_Anim_Fall final : public CKhazan_Spear_Animation
{
private:
    CKhazan_Spear_Anim_Fall();
    virtual ~CKhazan_Spear_Anim_Fall() = default;

public:
    HRESULT Initialize();

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
    class CClientInstance* m_pClientInstance = { nullptr };
    PLAYER_DATA* m_pPlayerData = { nullptr };

    _bool m_isFallingStart = { false };
    _bool m_isFalling = { false };
    _bool m_isFallAttacking = { false };
    _bool m_isFallAttackingStart = { false };

public:
    static CKhazan_Spear_Anim_Fall* Create();
    virtual void Free() override;
};

NS_END