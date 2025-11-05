#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END


NS_BEGIN(Client)

class CBody_Yetuga final : public CPartObject
{
public:
	typedef struct tagBodyDesc : public PARTOBJECT_DESC
	{
		CTransform* pOwnerTransform = { nullptr };
		class CYetuga*	pOwner = { nullptr };

	}BODY_DESC;

public:
	_float3					Get_BonePoint(const _char* BoneName);
	_float4*				Get_BonePointEX(const _char* BoneName);
	_matrix					Get_BoneMatrix(const _char* pBoneName);

private:
	CBody_Yetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Yetuga(const CBody_Yetuga& Prototype);
	virtual ~CBody_Yetuga() = default;

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
	class CYetuga*			m_pOwner = { nullptr };

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CTransform*				m_pOwnerTransform = { nullptr };

private:
	CBody*					m_pRH_BodyCom = { nullptr }; // 오른손
	CBody*					m_pLH_BodyCom = { nullptr }; // 왼손
	CBody*					m_pBack_BodyCom = { nullptr }; // 등


private:
	_float3					m_vThrowPoint = {};
	_float4					m_vLockOnPoint = {};

private:
	_float4x4				m_RightHandMatrix = {};
	_float4x4				m_LeftHandMatrix = {};
	_float4x4				m_BackMatrix = {};

public:
	static CBody_Yetuga*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END