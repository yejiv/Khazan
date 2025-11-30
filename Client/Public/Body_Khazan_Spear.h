#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "Player.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
class CMotionTrail;
NS_END

NS_BEGIN(Client)
class CBody_Khazan_Spear final : public CPartObject
{
public:
    typedef struct tagBodyKhazanSpearDesc : public CPartObject::PARTOBJECT_DESC
    {
        _uint* pState = { nullptr };
        _uint* pStatus = { nullptr };
        _uint* pHitReation = { nullptr };
        //_bool* pIsGuarding = { nullptr };
        _float4* pGuardRotationTarget = { nullptr };
        class CTransform* pParentTransform = { nullptr };

    }BODY_KHAZAN_SPEAR_DESC;

private:
    CBody_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_Khazan_Spear(const CBody_Khazan_Spear& Prototype);
    virtual ~CBody_Khazan_Spear() = default;

public:
    _float4x4* Get_BoneMatrix(const _char* pBoneName);
    _bool* Get_FinishedAnimation() { return &m_isFinishedAnimation; }
    void		Set_matSpearFX(_float4x4* mat) { m_pSpearFX_Matrix = mat; }
    void		Set_matSpearOffset(_matrix mat) { m_SpearOffset_Matrix = mat; }
    void        Set_IsGuarding(_bool* pIsGuarding) { m_pIsGuarding = pIsGuarding; }
    void        Set_Spear(class CSpear_Khazan_Spear* pSpear);
   // void        Set_Lantern(_bool* pIsLantern) { m_isEquipLantern = pIsLantern;}
    void	    Event_AttackTiming(_bool isAttackStart);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void    Priority_Update(_float fTimeDelta);
	virtual void    Update(_float fTimeDelta);
	virtual void    Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;
    virtual HRESULT Render_Outline() override;
    virtual HRESULT Render_MotionVector() override;
	void			Render_Part(CModel* pModel);
	void			Render_Part_Shadow(CModel* pModel);
    void            Render_Part_Outline(CModel* pModel);
    void            Render_Part_MotionVector(CModel* pModel);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    void                Search_BrutalTarget(_float fTimeDelta);
    _bool               Check_BrutalAttack(_float fTimeDelta);

public:
    CModel* Get_Model() { return m_pModelCom; }
    OUTLINE_CONFIG Get_OutlineConfig() { return m_OutlineConfig; }
    void Set_OutlineConfig(OUTLINE_CONFIG Config)
    {
        m_OutlineConfig.vColor = Config.vColor;
        m_OutlineConfig.fSize = Config.fSize;
    }
    // Shader
    void                        Set_EnableEdge(_bool isEnable) { m_isEnableEdge = isEnable; }
    const MOTIONTRAIL_CONFIG&   Get_MotionTrailConfig();
    void                        Set_MotionTrailConfig(const MOTIONTRAIL_CONFIG& Config);
    void                        Set_EnableMotionTrail(_bool isEnable);
    _bool                       isEnableMotionTrail();
    void                        Start_MotionTrail(_float fDuration);
    void                        Set_MotionTrailCallBack(function<void(const _wstring&, _bool)> callback) { m_OnMotionTrailCallBack = callback; }
    void                        Trigger_MotionTrail(const _wstring& strKey, _bool isActive) { if (m_OnMotionTrailCallBack)m_OnMotionTrailCallBack(strKey, isActive); }
    void                        On_MotionTrail(const _wstring strKey, _bool isActive) { m_pMotionTrailCom->Set_Config(strKey); m_isActiveMotionTrail = isActive; }


    _bool       Is_SpearFullExtension() const { return m_isSpearFullExtension; }
    _bool       Get_IsAttackCollisionActive() { return m_isSpearTipActive; }
    void        AllAttackCollisionActive_Off();

private:
    class CClientInstance*      m_pClientInstance = { nullptr };
	class CTransform*           m_pParentTransform = { nullptr };   
    class CSpear_Khazan_Spear*  m_pSpear = { nullptr };
    CMotionTrail*               m_pMotionTrailCom = { nullptr };

    CShader*            m_pShaderCom = { nullptr };
    CModel*             m_pModelCom = { nullptr }; // 매쉬없는 전체 모델
    unordered_map<_wstring, CModel*> m_AllParts; // 모든 모델 파츠 모음 <파츠이름, 모델클래스>
    unordered_map<EQUIPMENTTYPE, _wstring> m_EquippedParts;   // 현재 파츠 <파츠종류, 파츠 이름>
    vector<CModel*>     m_RenderParts;    // 빠른 렌더링을 위한 캐시 (렌더링할 파츠들만)

    CBody*              m_pBodyCom_SpearTip1 = { nullptr };
    CBody*              m_pBodyCom_SpearPole = { nullptr };
    CBody*              m_pBodyCom_Search = { nullptr };

    _float4x4*          m_pSpearFX_Matrix = { nullptr };
    _matrix				m_SpearOffset_Matrix = {};

    PLAYER_DATA*        m_pPlayerData;

    _uint*              m_pParentState = { nullptr };
    _uint*              m_pParentStatus = { nullptr };
    _uint*              m_pHitReaction = { nullptr };
    _uint				m_iCurState = {  };


    _bool				m_isFinishedAnimation = { false };
    _uint				m_iCurSetAnimIndex = { 0 };
    _bool               m_isSpearFullExtension = { false }; //창을 완전히 뻗는 타이밍부터 true 
    _bool*              m_pIsGuarding = { nullptr }; //가드중인지 체크

    /* 뼈 위치 */
    _float4x4*          m_pSpearTip1_Matrix = { nullptr };
    _float4x4			m_pSpearTip1_MatrixW;
    _float4x4*          m_pSpearPole_Matrix = { nullptr };
    _float4x4			m_pSpearPole_MatrixW;


    class CMeshTrail*   m_pTrail = { nullptr };
    _uint	            EffectID_SpearWind;
    _uint	            EffectID_SpiralSpear;
    OUTLINE_CONFIG      m_OutlineConfig = { _float3(1.f, 0.f, 1.f), 0.001f, 0.f, 0.f };
    function<void(const _wstring&, _bool)>  m_OnMotionTrailCallBack;
    _bool               m_isActiveMotionTrail = { false };

    _bool               m_isSpearTipActive = { true };
    _bool               m_isSpearPoleActive = { true };

    /* 가드 */
    _bool               m_isJustGuardOnce = { false };
    _float2             m_fJustGuardTime = { 0.f, 0.83f };
   // _bool               m_isGuardRotating = { false };
   // _float              m_fGuardRotationTime = { 0.f };
   // _float              m_fGuardRotationDuration = { 0.15f }; // 0.15초 동안 회전
   // _vector             m_vTargetRotationDir = {};
  //  _float              m_fStartAngle = { sd0.f };
   // _float              m_fTargetAngle = { 0.f };
    _float4*            m_pGuardRotationTarget = {nullptr};

    /* Monster Search, Brutal */
    _bool                       m_isBackBrutal = { false };
    _bool                       m_isGroggyBrutal = { false };
    vector< CGameObject*>       m_CollMonsters;
    _float2                     m_fOptimizationSearchTime = { 0.f,0.3f };
    CGameObject*                m_pBrutalmonster = { nullptr };
    const _float                m_fBrutalAttackSearchMaxDistance = { 2.f };
    const _float                m_fBrutalAttackMaxDistance = { 1.f };
    class CTarget_BrutalAttack* m_pBrutalAttack = { nullptr };

    /* 기타 */
    _bool*              m_isEquipSpear = { nullptr };
    _bool*              m_isEquipLantern = { nullptr };
    _bool               m_isNotifyAttacking = { false };    //어택중에 콜라이더 onoff 알림 

    _bool               m_isCollision;
    _float4             m_fCollisionPos;

    // Shader
    _bool               m_isEnableEdge = { true };
    _bool               m_isEnableMotionTrail = {};


    COLLISION_DESC      m_tSearchCollisionDesc = {};

    mutex m_CollMonsterMutex;

private:
    void				Update_Collider(_float fTimeDelta);                     
    void                Check_Guarding(_float fTimeDelta);
    void                Update_GuardRotation(_float fTimeDelta);
    void                Start_GuardRotation(_float3 vContactPoint);

private:
    HRESULT				Ready_Components();
    HRESULT				Ready_AnimationEvent();
    HRESULT				Ready_Collider();
    HRESULT				Bind_ShaderResources();
    HRESULT             Initialize_Equipment();
    void                Equip_Part(EQUIPMENTTYPE eType, const _wstring& strPartName); //파츠 갈아 입기
    void                Update_QuickRenderCache();  //빠른 랜더용 파츠모음 (모션트레일도 여기서 랜더용 파츠 갈아끼우기)


private:
    void	FX_Trail();
    void	UpdateSpearWind(_bool isEnableRadialBlur);
    void	SpawnSpearWind();
    void	FX_StrongAtk_Charge_Blust1(_fvector pos);
    void	FX_StrongAtk_Charge_Blust2(_fvector pos);
    void	FX_StrongAtk_Charge_Blust3(_fvector pos);
    void	FX_StrongAtk_Charge_Blust4(_fvector pos);
    void	FX_StrongAtk_Charge_Blust5(_fvector pos);
    void	FX_StrongAtk_Charge_Blust6(_fvector pos);
    void	FX_StrongAtk_Charge_BlustSmall(_fvector pos);
    void	Spear_Spike();

    //void	UpdateTrail();

private:
    inline void		Add_State(_uint i) { *m_pParentState |= i; }
    inline void		Toggle_State(_uint i) { *m_pParentState ^= i; }
    inline void		Remove_State(_uint i) { *m_pParentState &= ~i; }
    inline _bool	Has_State(_uint i) { return (*m_pParentState & i) != 0; }
    inline void		Clear_State() { *m_pParentState = 0; }

    inline void		Add_Status(_uint i) { *m_pParentStatus |= i; }
    inline void		Toggle_Status(_uint i) { *m_pParentStatus ^= i; }
    inline void		Remove_Status(_uint i) { *m_pParentStatus &= ~i; }
    inline _bool	Has_Status(_uint i) { return (*m_pParentStatus & i) != 0; }
  
public:
    static CBody_Khazan_Spear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END
