#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class CYetuga final : public CMonster
{
private:
	CYetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CYetuga(const CYetuga& Prototype);
	virtual ~CYetuga() = default;

public:
	class CBody_Yetuga*				Get_Body() const { return m_pBody; }
	virtual _float4*				Get_LockOnPosition();

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() { return S_OK; }

public:
	virtual void					Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	virtual void					Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	virtual void					Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;


private:
	HRESULT							Ready_Components();
	HRESULT							Ready_PartObjects();
	HRESULT							Ready_Projectiles();
	HRESULT							Ready_AnimEvent();

private:
	// ThrowBall
	void							Pick_Stone();
	void							Hold_Stone();
	void							Throw_Stone();


	// JumpGrab
	void							Grab_Check_Begin(const _char* BoneName);
	void							Grab_Check_End(const _char* pBoneName);

	// Armageddon
	void							Pick_Rock();
	void							Hold_Rock();
	void							Smash();

	// IceBreath
	void							Breath_Start();
	void							Breath_Loop();

private:
	class CBody_Yetuga*				m_pBody = { nullptr };
	class CHead_Yetuga*				m_pHead = { nullptr };
	class CProjectile_Yetuga*		m_pHoldStone = { nullptr };
	class CProjectile_Rock_Yetuga*	m_pHoldRock = { nullptr };
	class CProjectile_Breath_Yetuga* m_pBreath = { nullptr };


	_bool							m_isLookAt = { false };
	_float							m_fTurnSpeed = { false };
	_bool							m_isRockPlay = { false };
	_bool							m_isSmash = { false };
	_float3							m_vHoldRockOffset = {};
	_bool							m_isGrab = { false };


public:
	static CYetuga*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
 
NS_END
