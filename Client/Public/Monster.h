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
	virtual _float4				Get_LockOnPosition() const { return _float4(0.f, 0.f, 0.f, 0.f); };
	void						RequestRecoveryStamina() { m_isRequestRecoveryStamina = true; }

public:
	virtual void				Take_Damage(_float fDamage, HITREACTION eHitreaction, _float fValidTime ,CGameObject* pGameObject = nullptr);
	virtual void				Consume_Stamina(_float fAmout) override;
	virtual void				Recovery_Stamina(_float fTimeDelta) override;
	virtual void				Look_Target();
public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Shadow() { return S_OK; }


protected:
	//CRigidBody*				m_pRigidBodyCom = { nullptr };

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

//private:
//	HRESULT Ready_Components();

protected:

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END