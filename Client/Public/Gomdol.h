#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class CGomdol final : public CMonster
{
private:
	CGomdol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGomdol(const CGomdol& Prototype);
	virtual ~CGomdol() = default;

public:
	virtual _float4*				Get_LockOnPosition();
	class CBody_Gomdol*				Get_Body() { return m_pBody; }
    void                            Set_InitPosition();

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() { return S_OK; }

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
	HRESULT							Ready_Components();
	HRESULT							Ready_PartObjects();
	HRESULT							Ready_AnimEvent();

private:
	class CBody_Gomdol*				m_pBody = { nullptr };
    COLLISION_DESC                  m_tGomdolCharvir = {};


public:
	static CGomdol*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;

};

NS_END