#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Camera.h"

NS_BEGIN(Engine)
class CCamera;
class CBody;
class CGameObject;
NS_END

NS_BEGIN(Client)
class CCamera_Compre final : public CCamera
{
public:
	typedef struct tagCameraCompreDesc : public CCamera::CAMERA_DESC
	{

	}CAMERA_COMPRE_DESC;

private:
	CCamera_Compre(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Compre(const CCamera_Compre& Prototype);
	virtual ~CCamera_Compre() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Update_Free(_float fTimeDelta);
	void Update_Spring(_float fTimeDelta);

public:
	HRESULT Ready_Camera(void* pArg);
	HRESULT Ready_Body();

public:
	HRESULT Spring(_float fTimeDelta);
	HRESULT RayCast(_float fTimeDelta);
	HRESULT LockOn(_float fTimeDelta);
    void Update_BlendBack(_float fTimeDelta);
public:
	void LockOn_Check(_float fTimeDelta);
	class CGameObject* Pick_ClosetTarget();
	_vector Cal_CamPos(_float fTimeDelta, _vector& vTargetPos, _vector& vDir);

    virtual void OnCameraAniEnd() override;

public:
	_float UpdateY_Stable(_float fCurrentY, _float fDesiredY, _float fTimeDelta);

	void  SetYSmoothTime(_float fTime) { m_fYSmoothTime = fTime; }
	void  SetYDeadZone(_float fDeadZone) { m_fDeadZone = fDeadZone; }
	void  SetYMaxRiseFall(_float fRise, _float fFall) { m_fMaxRise = fRise; m_fMaxFall = fFall; }
	void  ResetYState(_float fY = 0.f) { m_fYVel = 0.f; m_isInited = false; m_fSmoothY = fY; }

public:
	CAMERA_COMPRE_DESC  Get_Desc();
    _bool               Get_IsLockOn() const { return m_isLockOn; }
    _float4*            Get_LockOnPosition() { return m_pLockOnPos; }
private:
	_float m_fYVel = { 0.f };
	_bool m_isInited = { false };
	_float m_fSmoothY = { 0.f };

	_float m_fYSmoothTime = { 0.25f }; // 감쇠 시간
	_float m_fDeadZone = { 0.1f }; // 미세 요철 무시
	_float m_fMaxRise = { 5.f }; // 초당 상승 한도
	_float m_fMaxFall = { 9.f }; // 초당 하강한도

	CBody* m_pBody = { nullptr };
	_bool m_isLockOn = { false };
	_float m_fLockOnDelay = {};
	vector<class CGameObject*> m_CollMonsters;
	class CGameObject* m_pLockMonster = { nullptr };
	_float4* m_pLockOnPos = {};
	class CTarget_LockOn* m_pLockOnUI = { nullptr };
	
	_float m_fTargetHalfFovDegrees = { 50.f };
	_float m_fTargetHalfFovCos = 0.f;
	_float m_fTargetMaxDistance = 20.f;


    // 특정위치에서 강제 이동시 보간
    _bool m_isBlendBack = false;
    _float m_fBlendBackTime = 0.f;
    _float m_fBlendBackDuration = 1.5f;

    _vector m_vBlendStartPos = XMVectorZero();
    _vector m_vBlendStartRight = XMVectorZero();
    _vector m_vBlendStartUp = XMVectorZero();
    _vector m_vBlendStartLook = XMVectorZero();

public:
	void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;


public:
	static CCamera_Compre* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;

};

NS_END