#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END


NS_BEGIN(Client)

class CHead_Yetuga final : public CPartObject
{
public:
	typedef struct tagBodyDesc : public PARTOBJECT_DESC
	{
		CTransform* pOwnerTransform = { nullptr };
		class CYetuga* pOwner = { nullptr };

	}HEAD_DESC;

	void					Set_OnAttackCollision(_bool isToggle) { m_isOnAttackCollision = isToggle; }

private:
	CHead_Yetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHead_Yetuga(const CHead_Yetuga& Prototype);
	virtual ~CHead_Yetuga() = default;

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
	//HRESULT					Ready_Components();
	//HRESULT					Bind_ShaderResources();
	void					Carculate_Matrix(_float fTimeDelta);


private:
	HRESULT					Ready_Colliders();

private:
	class CYetuga*			m_pOwner = { nullptr };

private:
	
	CTransform*				m_pOwnerTransform = { nullptr };

private:
	CBody*					m_pHeadBodyCom = { nullptr };

private:
	_float4x4				m_HeadMatrix = {};
	_bool					m_isOnAttackCollision = { false };

public:
	static CHead_Yetuga*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END