#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CCollider;
class CNavigation;
class CCharacterVirtual;
class CRigidBody;
NS_END

NS_BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	enum PLAYER_STATE {
		IDLE = 0x00000001,
		RUN = 0x00000002,
		ATTACK = 0x00000004,
	};
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;

private:
	_uint				m_iState = { };
	//class CRigidBody* m_pRigidBodyCom = { nullptr };
	class CCharacterVirtual* m_pCharVirCom = { nullptr };

#pragma region 상호 작용 맵 오브젝트 이벤트 임시 테스트용
	EventInteractType m_EventInteract = {};
#pragma endregion

private:


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_Collision();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END