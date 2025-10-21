#pragma once
#include "ContainerObject.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CCreature abstract : public CContainerObject
{
public:
	typedef struct tagCreatureDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float				fMaxHP = {};
		_float				fMaxStamina = {};
		_float				fMoveSpeed = {};
		_float				fAttack = {};

	}CREATURE_DESC;

protected:
	CCreature(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCreature(const CCreature& Prototype);
	virtual ~CCreature() = default;

public:
	_float						Get_CurrentHP() const{ return m_fCurrentHP; }
	_float						Get_MaxHP() const{ return m_fMaxHP; }
	_float						Get_Attack() const{ return m_fAttack; }
	_float						Get_CurrentStamina() const{ return m_fCurrentStamina; }
	_float						Get_MaxStamina() const { return m_fMaxStamina; }
	_float						Get_MoveSpeed() const{ return m_fMoveSpeed; }

	// Set함수는 다르게 작동할수도 있을 것 같아서  일단 Get만 만들었습니다.

public:
	virtual void				Take_Damage(CCreature* pGameObject, _float fDamage) {};

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Shadow() { return S_OK; }

protected:
	_float						m_fCurrentHP = {};
	_float						m_fMaxHP = {};
	_float						m_fAttack = {};
	_float						m_fCurrentStamina = {};
	_float						m_fMaxStamina = {};
	_float						m_fMoveSpeed = {};

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;

};

NS_END