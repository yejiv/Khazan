#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CBody_Gomdol final : public CPartObject
{
public:
	typedef struct tagBodyDesc : public PARTOBJECT_DESC
	{
		CTransform* pOwnerTransform = { nullptr };
		class CGomdol* pOwner = { nullptr };

	}BODY_DESC;

public:
	_float4*				Get_BonePointEX(const _char* pBoneName);
	_matrix					Get_BoneMatrix(const _char* pBoneName);

	void					Set_OnAttackCollision(_bool isToggle) { m_isOnAttackCollision = isToggle; }


private:
	CBody_Gomdol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Gomdol(const CBody_Gomdol& Prototype);
	virtual ~CBody_Gomdol() = default;

public:
public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void			Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	virtual void			Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	virtual void			Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;


private:
	HRESULT					Ready_Components();
	HRESULT					Bind_ShaderResources();
	void					Carculate_Matrix(_float fTimeDelta);

private:
	HRESULT					Ready_Colliders();

private:
	class CGomdol*			m_pOwner = { nullptr };

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CTransform*				m_pOwnerTransform = { nullptr };

private:
	CBody*					m_pRH_BodyCom = { nullptr }; // ¿À¸¥¼Õ
	CBody*					m_pLH_BodyCom = { nullptr }; // ¿Þ¼Õ

private:
	_float4					m_vLockOnPoint = {};


private:
	_float4x4				m_RightHandMatrix = {};
	_float4x4				m_LeftHandMatrix = {};

	_bool					m_isOnAttackCollision = { false };


public:
	static	CBody_Gomdol*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;

};

NS_END