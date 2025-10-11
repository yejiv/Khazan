//#pragma once
//
//#include "Component.h"
//#ifdef new
//#pragma push_macro("new")
//#undef new
//#endif
//
//#include <Jolt/Physics/PhysicsSystem.h>
//#include <Jolt/Physics/Collision/RayCast.h>
//#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
//#include <Jolt/Core/TempAllocator.h>
//
//#ifdef new
//#pragma pop_macro("new") // DBG_NEW º¹¿ø
//#endif
//
//NS_BEGIN(Engine)
//
//class ENGINE_DLL CCharacter final : public CComponent
//{
//public:
//	typedef struct tagCharacterDesc {
//		class CPhysicsBody* pBody = nullptr;
//		class CTransform* pTransform = nullptr;
//		_float fMaxSlopeDeg = 50.f;
//		_float fStepOffset = 0.4f;
//		_float fGroundSnap = 0.1f;
//		_float fGravity = 9.81f;
//	}CHARACTER_DESC;
//private:
//	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	CCharacter(const CCharacter& Prototype);
//	virtual ~CCharacter() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype();
//	virtual HRESULT Initialize_Clone(void* pArg) override;
//	void Update(_float fTimeDelta, class CPhysicsBody* pBody, class CTransform* pTransform);
//
//public:
//	void    Set_DesiredVelocity(const _float3& v) { m_vDesired = v; }
//	void    Jump(_float v0) { m_bWantJump = true; m_fJumpV0 = v0; }
//
//private:
//	void    Update_Kinematic(_float fTimeDelta, class CPhysicsBody* pBody, class CTransform* pTransform);
//
//
//private:
//	class CGameInstance* m_pGameInstance = { nullptr };
//	_float3       m_vDesired{ 0,0,0 };
//	_float3       m_vActual{ 0,0,0 };
//	_bool         m_bGrounded = false;
//	_bool         m_bWantJump = false;
//	_float        m_fJumpV0 = 0.f;
//
//	_float        m_fMaxSlopeDeg = 50.f;
//	_float        m_fStepOffset = 0.4f;
//	_float        m_fGroundSnap = 0.1f;
//	_float        m_fGravity = 9.81f;
//
//public:
//	static CCharacter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual CComponent* Clone(void* pArg) override;
//	virtual void Free() override;
//
//};
//
//NS_END