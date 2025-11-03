#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CRigidBody : public CComponent
{
protected:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& Prototype);
	virtual ~CRigidBody() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);

	// CTranform의 값을 RigidBody에 적용시킨다.
	virtual void Sync_Update(class CTransform* pTransform) {};

	// 그이후 물리 연산값을 계산에 CTransform값에 더해준다.
	virtual void Update(_float fTimeDelta, class CTransform* pTransform) {};

	virtual void Sync_Update(_matrix WorldMatirx) {};
	virtual void Update(_float fTimeDelta, _vector& outQuatRotation, _vector& outPosition) {};

protected:
	virtual void Set_PosRot(_vector vPos, _vector vRot) {};
	virtual void Set_Position(_vector vPos) {};
	virtual void Set_Rotation(_vector vRot) {};
	virtual void Set_Velocity(_vector vVelocity) {};

public:
	static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END