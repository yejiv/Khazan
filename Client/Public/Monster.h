#pragma once

#include "Client_Defines.h"
#include "Creature.h"

NS_BEGIN(Engine)
class CCollider;
class CRigidBody;
class CAI_Controller;
NS_END

NS_BEGIN(Client)

class CMonster abstract : public CCreature
{
public:
	typedef struct tagMonsterDesc : public CCreature::CREATURE_DESC
	{
		string		strName;

        _float4x4   WorldMatrix{};
        
	}MONSTER_DESC;

	typedef struct tagMonsterInfo
	{
		enum MONSTERSTATE 
		{
			STATE_NONE = 0,
			WALK = 1 << 0,
			RUN = 1 << 1,
			SPRINT = 1 << 2,
		};

		void Add_State(unsigned int flag)			{ iStateFlag |= flag; }
		void Delete_State(unsigned int flag)		{ iStateFlag &= ~flag; }
		void Clear_State()							{ iStateFlag = 0; }
		bool Check_State(unsigned int flag) const	{ return (iStateFlag & flag) != 0; }

		unsigned int			iStateFlag = 0;

	}MONSTER_INFO;


protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
 	CAI_Controller*				Get_Controller() const { return m_pController; }
	_float						Get_CoolTimeAcc() const { return m_fCoolTimeAcc; }
	const string&				Get_Name() const { return m_strName; }
	virtual _float4*			Get_LockOnPosition() const { return m_vLockOnPosition; }
	void						RequestRecoveryStamina() { m_isRequestRecoveryStamina = true; }
    void                        Set_RequestRecoveryStamina(_bool isToggle) { m_isRequestRecoveryStamina = isToggle; }
    
    void						CheckMinDistanceWithPlayer(_float MinDist, _float fAnimRatio);
    _bool                       Get_isSleep();
    _bool                       Get_IsGroggy();

public:
	virtual void				Take_Damage(_float fDamage, HITREACTION eHitreaction,CGameObject* pGameObject = nullptr) override;
	virtual void				Consume_Stamina(_float fAmout) override;
	virtual void				Recovery_Stamina(_float fTimeDelta) override;
	virtual void				Look_Target();
	virtual void				Look_Target_Lerp(_float fTimeDleta, _float AnimRatio, _float fTurnSpeed);
	virtual void				Start_Decel(_float fDuration);
	virtual void				Update_Velocity(_float fTimeDelta);

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Shadow() { return S_OK; }


protected:
	class CAI_Controller*			m_pController = { nullptr };
	class CGameObject*				m_pTarget = { nullptr };

protected:
	unordered_map<string, _float>	m_CoolDowns;
	_float							m_fCoolTimeAcc = {};
	_float4*						m_vLockOnPosition = {};

	string							m_strName;
	_float							m_fRecoveryPerSec = {};
	_bool							m_isRequestRecoveryStamina = {};

	_float3							m_vVelocity = {}; // 현재 이동속도
	_float3							m_vVelocutyTarget = {}; // 목표 속도 (0)
	_float							m_fDecelTime = {}; // 감속 지속 시간
	_float							m_fDecelElapsed = {}; // 경과 시간
	_bool							m_isDecelerating = {}; // 감속 중 플래그

    _bool                           m_isDeadFlag = { false };

    _float2                         m_vDecalSize[ENUM_CLASS(DECALTYPE::END)] = {};

    _float                          m_fDissolveTime = {};
    _bool                           m_isDissolve = {};
//private:
//	HRESULT Ready_Components();

protected:

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END