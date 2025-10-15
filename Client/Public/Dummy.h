#pragma once

#include "Client_Defines.h"
#include "Pool.h"

NS_BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CRigidBody;
class CCharacterVirtual;
NS_END

NS_BEGIN(Client)

class CDummy final : public CPool
{
private:
	CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy(const CDummy& Prototype);
	virtual ~CDummy() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	//CRigidBody* m_pRigidBodyCom = { nullptr };
	class CCharacterVirtual* m_pCharVirCom = { nullptr };


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	virtual void Reset() override;

public:
	static CDummy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END