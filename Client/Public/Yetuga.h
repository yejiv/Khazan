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
	void						Pick_Rock();
	void						Hold_Rock();
	void						Throw_Rock();

	void						Grab_Check_Begin();
	void						Grab_Check_End();

	void						Smash(_float fTimeDelta);

public:
	class CBody_Yetuga*			m_pBody = { nullptr };
	class CProjectile_Yetuga*	m_pHoldRock = { nullptr };

	_bool						m_isSmash = { false };

public:
	static CYetuga*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END
