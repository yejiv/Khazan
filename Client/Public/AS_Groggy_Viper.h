#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

enum class GROGGY { START, LOOP, RECOVERY, BRUTALATTACK,  END };

NS_BEGIN(Client)

class CAS_Groggy_Viper final : public CAI_State
{
private:
    CAS_Groggy_Viper();
    virtual ~CAS_Groggy_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
    virtual void OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:
    _float						m_fCurrentTime = {};
    _float						m_fGroggyTime = {};
    _float                      m_fBrutalAttackDelayTime = {};
    _float                      m_fBrutalAcc = {};

    _float4*                    m_vBonePosition = {};

    _bool						m_isBrutalAttackSuccess = { false };
    _bool                       m_isCheckBrutalCnt = { false };
    _bool                       m_isBlockAnimState = { false };

    GROGGY						m_eState = { GROGGY::START };


    class CTarget_BrutalAttack* m_pBrutalAttack = { nullptr };



public:
    static CAS_Groggy_Viper*    Create();
    virtual void				Free() override;

};

NS_END