#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class CYetuga final : public CMonster
{
private:
	CYetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CYetuga(const CYetuga& Prototype);
	virtual ~CYetuga() = default;

public:
	class CBody_Yetuga*				Get_Body() const { return m_pBody; }
    class CHead_Yetuga*             Get_Head() const { return m_pHead; }

	virtual _float4*				Get_LockOnPosition();
    void                            Yetuga_Jump(_fvector vGoalPos, _float fHeight, _float fSpeed );
    void                            Yetuga_Land(_fvector vGoalPosition, _float fSpeed);
    class CAS_CutScene_Yetuga*      Get_Yetuga_CutSceneState();
    vector<const _tchar*>           Get_IceBreathSound() { return m_IceBreathSounds; }
    virtual void				    KnockBack(_vector vDir, _float fPower, _float fLoss) override;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() { return S_OK; }

public:
    virtual void                    Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void                    Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void                    Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

    void                            Update_Landing(_float fTimeDelta);

private:
	HRESULT							Ready_Components();
	HRESULT							Ready_PartObjects();
	HRESULT							Ready_Projectiles();
	HRESULT							Ready_AnimEvent();
	HRESULT							Ready_AnimEffectEvent(class CModel* pModel);
    HRESULT                         Ready_SFX();

public:
    void                            SFX_Move(_uint iIndex);
    void                            SFX_HIT(_uint iHitIndex);

private:
	// ThrowBall
	void							Pick_Stone();
	void							Hold_Stone();
	void							Throw_Stone();


	// JumpGrab
	void							Grab_Check_Begin(const _char* BoneName);
	void							Grab_Check_End(const _char* pBoneName);

	// Armageddon
	void							Pick_Rock();
	void							Hold_Rock();
	void							Smash();

	// IceBreath
	void							Breath_Start();
	void							Breath_Loop();

    void                            Abort_Node(_bool isToggle);


private:
    void                            Start_DefaultRadialBlur();

private:
	class CBody_Yetuga*				m_pBody = { nullptr };
	class CHead_Yetuga*				m_pHead = { nullptr };
	class CProjectile_Yetuga*		m_pHoldStone = { nullptr };
	class CProjectile_Rock_Yetuga*	m_pHoldRock = { nullptr };
	class CProjectile_Breath_Yetuga* m_pBreath = { nullptr };

	_bool							m_isLookAt = { false };
	_float							m_fTurnSpeed = { 3.f };
	_bool							m_isRockPlay = { false };
	_bool							m_isSmash = { false };
	_float3							m_vHoldRockOffset = {};
	_bool							m_isGrab = { false };
    _uint                           m_iFX_ID_Dampsey;
    _uint                           m_iFX_ID_Roar;
    _uint                           m_iBreathCount;
    _float                          m_iBreathRotation;

    COLLISION_DESC                  m_tYetugaChaVir = {};
private:
    _bool   m_isLanding = false;

    _float3 m_vLandingStartPos;      
    _float3 m_vLandingTargetPos;   

    _float  m_fLandingHorizontalSpeed = 0.f;   
    _float  m_fLandingVerticalSpeed = 0.f;    
    _float  m_fGravity = -10.f;

    vector<const _tchar*>            m_IceBreathSounds;


public:
	static CYetuga*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
 
NS_END
