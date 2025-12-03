#pragma once
#include "Client_Defines.h"
#include "AI_State.h"

NS_BEGIN(Client)

enum class HIT_DIR { FRONT, BACK, LEFT, RIGTH, END };

class CAS_Hit_Yetuga final : public CAI_State
{
private:
	CAS_Hit_Yetuga();
	virtual ~CAS_Hit_Yetuga() = default;

public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


    virtual void OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;



private:
	HIT_DIR		Convert_HitFlag(DIRECTION_INFO Info);
	_uint		Make_AnimIndex(HITREACTION eHitreaction, HIT_DIR eHitDir);


public:
	static CAS_Hit_Yetuga* Create();
	virtual void Free() override;

};

NS_END