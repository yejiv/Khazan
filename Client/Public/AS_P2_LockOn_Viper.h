#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_P2_LockOn_Viper final : public CAI_State
{
private:
    CAS_P2_LockOn_Viper();
    virtual ~CAS_P2_LockOn_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


private:
    void          Update_Direction(class CTransform* pOwnerTransform, CTransform* pTargetTransfrom, class CModel* pModel);
    void          Move_To_Direction(class CTransform* pOwnerTransform, _float fTimeDelta);



private:
    DIRECTION                               m_eDirState = { DIRECTION::END };
    _float						            m_fDotThreshold = {};
    _float						            m_fMoveSpeed = {};
    _float                                  m_fTimeAcc = {}; // 락온 유지 시간
    _float                                  m_fMinLockTime = {}; // 최소 유지시간
    _float                                  m_fMaxLockTime = {} ; // 최대 유지 시간
    _float                                  m_fEndDist = {}; // 거리 탈출용

public:
    static CAS_P2_LockOn_Viper*             Create();
    virtual void                            Free() override;

};

NS_END