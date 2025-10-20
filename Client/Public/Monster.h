#pragma once

#include "Client_Defines.h"
#include "Creature.h"

NS_BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CRigidBody;
class CAI_Controller;
NS_END

NS_BEGIN(Client)

class CMonster abstract : public CCreature
{
public:
	typedef struct tagMonsterDesc : public CREATURE_DESC
	{
		
	}MONSTER_DESC;


protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	CAI_Controller*				Get_Controller() const { return m_pController; }


public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Shadow() { return S_OK; }

protected:
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };
	//CRigidBody* m_pRigidBodyCom = { nullptr };

protected:
	class CAI_Controller*		m_pController = { nullptr };
	class CGameObject*			m_pTarget = { nullptr };


//private:
//	HRESULT Ready_Components();

protected:
	HRESULT Bind_ShaderResources();

public:
	//static CMonster*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;
};

NS_END