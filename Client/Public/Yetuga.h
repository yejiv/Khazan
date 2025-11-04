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
	class CBody_Yetuga*			Get_Body() const { return m_pBody; }
	virtual _float4				Get_LockOnPosition();
	inline _float				EaseInOutSine(_float t) { return -(cosf(XM_PI * t) - 1.f) * 0.5f; }

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Shadow() { return S_OK; }

private:
	HRESULT						Ready_Components();
	HRESULT						Ready_PartObjects();
	HRESULT						Ready_Projectiles();
	HRESULT						Ready_AnimEvent();


private:
	// ThrowBall
	void							Pick_Stone();
	void							Hold_Stone();
	void							Throw_Stone();


	// Rush
	void							Grab_Check_Begin();
	void							Grab_Check_End();

	// Armageddon
	void							Pick_Rock();
	void							Hold_Rock();
	void							Smash();


public:
	class CBody_Yetuga*				m_pBody = { nullptr };
	class CProjectile_Yetuga*		m_pHoldStone = { nullptr };
	class CProjectile_Rock_Yetuga*	m_pHoldRock = { nullptr };

	_bool							m_isRockPlay = { false };
	_bool							m_isSmash = { false };
	_float3							m_vHoldRockOffset = {};



public:
	static CYetuga*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
 
NS_END
