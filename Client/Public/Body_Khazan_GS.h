#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
class CMotionTrail;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBody_Khazan_GS final: public CPartObject
{
public:
    typedef struct tagBodyKhazanGSwordDesc : public CPartObject::PARTOBJECT_DESC
    {
        _uint* pState = { nullptr };
        _uint* pStatus = { nullptr };
        _uint* pHitReation = { nullptr };
        _float4* pGuardRotationTarget = { nullptr };
        class CTransform* pParentTransform = { nullptr };

    }BODY_KHAZAN_GS_DESC;

private:
    enum GS_COLLISION
    {
        COLL_ATTACK = 1 << 0,
        COLL_RANGEATTACK = 1 << 1,
        COLL_BODYATTACK = 1 << 2,
        COLL_GUARD = 1 << 3,
    };

private:
    CBody_Khazan_GS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_Khazan_GS(const CBody_Khazan_GS& Prototype);
    virtual ~CBody_Khazan_GS() = default;


public:
    virtual HRESULT     Initialize_Prototype();
    virtual HRESULT     Initialize_Clone(void* pArg);
    virtual void        Priority_Update(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Late_Update(_float fTimeDelta);
    virtual HRESULT     Render();
    virtual HRESULT     Render_Shadow() override;
    virtual HRESULT     Render_Outline() override;
    virtual HRESULT     Render_MotionVector() override;
    void			    Render_Part(CModel* pModel);
    void			    Render_Part_Shadow(CModel* pModel);
    void                Render_Part_Outline(CModel* pModel);
    void                Render_Part_MotionVector(CModel* pModel);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    _float4x4*          Get_BoneMatrix(const _char* pBoneName);
    _bool*              Get_FinishedAnimation() { return &m_isFinishedAnimation; }
    CModel*             Get_Model() { return m_pModelCom; }
    OUTLINE_CONFIG      Get_OutlineConfig() { return m_OutlineConfig; }

public:
    void                Set_IsGuarding(_bool* pIsGuarding) { m_pIsGuarding = pIsGuarding; }
    void                Set_GSword(class CGSword_Khazan_GS* pGS);
    void                Set_OutlineConfig(OUTLINE_CONFIG Config){ m_OutlineConfig.vColor = Config.vColor; m_OutlineConfig.fSize = Config.fSize; }
    void		        Set_matGSword(_float4x4* mat) { m_pGSword_Matrix = mat; }

public:
   // _bool               Is_SpearFullExtension() const { return m_isSpearFullExtension; }

public:
    void                Search_BrutalTarget(_float fTimeDelta); //부르탈 타겟 찾기 
    _bool               Check_BrutalAttack(_float fTimeDelta);  //부르탈 체크
    void                AllAttackCollisionActive_Off();         //어택 콜리젼 다 끄기 

    /* Shader */
    void                        Set_EnableEdge(_bool isEnable) { m_isEnableEdge = isEnable; }
    const MOTIONTRAIL_CONFIG&   Get_MotionTrailConfig();
    void                        Set_MotionTrailConfig(const MOTIONTRAIL_CONFIG& Config);
    void                        Set_EnableMotionTrail(_bool isEnable);
    _bool                       isEnableMotionTrail();
    void                        Start_MotionTrail(_float fDuration);
    void                        Set_MotionTrailCallBack(function<void(const _wstring&, _bool)> callback) { m_OnMotionTrailCallBack = callback; }
    void                        Trigger_MotionTrail(const _wstring& strKey, _bool isActive) { if (m_OnMotionTrailCallBack)m_OnMotionTrailCallBack(strKey, isActive); }
    void                        On_MotionTrail(const _wstring strKey, _bool isActive) { m_pMotionTrailCom->Set_Config(strKey); m_isActiveMotionTrail = isActive; }

public:
    const TRAIL_CONFIG&         Get_TrailConfig() const;
    void                        Set_TrailConfig(const TRAIL_CONFIG& Config);
    _uint                       Get_NumTrailTextures();
    ID3D11ShaderResourceView*   Get_TrailTexture(_uint iIndex);

private:
    class CClientInstance*      m_pClientInstance = { nullptr };
    class CTransform*           m_pParentTransform = { nullptr };
    class CGSword_Khazan_GS*    m_pGSword = { nullptr };
    class CTarget_BrutalAttack* m_pBrutalAttack = { nullptr };
    CMotionTrail*               m_pMotionTrailCom = { nullptr };

    CShader*                    m_pShaderCom = { nullptr };
    CTexture*                   m_pTextureCom = { nullptr };

    CModel*                                 m_pModelCom = { nullptr }; // 매쉬없는 전체 모델
    unordered_map<_wstring, CModel*>        m_AllParts; // 모든 모델 파츠 모음 <파츠이름, 모델클래스>
    unordered_map<EQUIPMENTTYPE, _wstring>  m_EquippedParts;   // 현재 파츠 <파츠종류, 파츠 이름>
    vector<CModel*>                         m_RenderParts;    // 빠른 렌더링을 위한 캐시 (렌더링할 파츠들만)

    CBody*                      m_pBodyCom_Attack = { nullptr };            //검 공격시 사용하는 졸트 바디
    CBody*                      m_pBodyCom_RangeAttack = { nullptr };   //범위 공격에 졸트 바디
    CBody*                      m_pBodyCom_BodyAttack = { nullptr };   //플레이어 바디에 붙이는 졸트바디 (공격용)
    CBody*                      m_pBodyCom_Guard = { nullptr };         //검 방어에 사용하는 졸트 바디
    CBody*                      m_pBodyCom_Search = { nullptr };        //몬스터 서치에 사용하는 졸트 바디 

    PLAYER_DATA*                m_pPlayerData;
    COLLISION_DESC				m_tAttackCollisionDesc = {};
    COLLISION_DESC				m_tRangeAttackCollisionDesc = {};
    COLLISION_DESC				m_tBodyAttackCollisionDesc = {};
    COLLISION_DESC				m_tGuardCollisionDesc = {};
    COLLISION_DESC              m_tSearchCollisionDesc = {};

    _bool                       m_isCollAttack_Active = { true };
    _bool                       m_isCollRangeAttack_Active = { true };
    _bool                       m_isCollBodyAttack_Active = { true };
    _bool                       m_isCollGuard_Active = { true };

    _float4x4*                  m_pGSword_Matrix = { nullptr };

    _uint*                      m_pParentState = { nullptr };
    _uint*                      m_pParentStatus = { nullptr };
    _uint*                      m_pHitReaction = { nullptr };
    _uint				        m_iCurState = {  };
    _uint                       m_iCollState = {};          //어떤 콜라이더가 켜질 것인가에대한 상태 

    _bool				        m_isFinishedAnimation = { false };
    _uint				        m_iCurSetAnimIndex = { 0 };
   // _bool                       m_isSpearFullExtension = { false }; //창을 완전히 뻗는 타이밍부터 true 
    _bool*                      m_pIsGuarding = { nullptr }; //가드중인지 체크4
    _bool                       m_isNotifyAttacking = { false };    //어택중에 콜라이더 onoff 알림 
    _bool                       m_isBrutalSuccess = { false };


     /* 뼈 위치 */
    _float4x4*                  m_pMatGSwordBody = { nullptr }; // 칼 중앙 로컬행렬
    _float4x4			        m_matWorldGSwordBody;                // 칼 중앙 월드행렬
    _float4x4                   m_matWorldGSwordBody_nJolt;
    _float4x4*                  m_pMatGSwordTip = { nullptr };  // 칼 끝 로컬행렬
    _float4x4			        m_matWorldGSwordTip;                 // 칼 끝 월드행렬
    _float4x4			        m_matWorldGSwordTip_nJolt;


    _matrix				        m_Offset_Matrix = {};   //블랜더와 축이 달라서 사용.

    class CMeshTrail*           m_pTrail = { nullptr };
    _uint	                    EffectID_SpearWind;
    _uint	                    EffectID_SpiralSpear;


    /* 가드 */
    _bool                       m_isJustGuardOnce = { false };
    _float2                     m_fJustGuardTime = { 0.f, 0.83f };
    _float4*                    m_pGuardRotationTarget = { nullptr };

    /* Monster Search, Brutal */
    _bool                       m_isBackBrutal = { false };
    _bool                       m_isGroggyBrutal = { false };
    vector< CGameObject*>       m_CollMonsters;
    _float2                     m_fOptimizationSearchTime = { 0.f,0.3f };
    CGameObject*                m_pBrutalmonster = { nullptr };
    const _float                m_fBrutalAttackSearchMaxDistance = { 2.f };
    const _float                m_fBrutalAttackMaxDistance = { 1.f };

    /* 기타 */
    _bool*                      m_isEquipSpear = { nullptr };
    _bool*                      m_isEquipLantern = { nullptr };

    _bool                       m_isCollision;
    _float4                     m_fCollisionPos;

    // Shader
    _bool                       m_isEnableEdge = { true };
    _bool                       m_isActiveMotionTrail = { false };
    _bool                       m_isEnableAnimEvent = {};
    _uint                       m_iCurAnimEventIndex = {};
    OUTLINE_CONFIG              m_OutlineConfig = { _float3(1.f, 0.f, 1.f), 0.001f, 0.f, 0.f };
    function<void(const _wstring&, _bool)>  m_OnMotionTrailCallBack;

    /* event */
    _bool                       m_isEableGiantHuntEvent= { false };
    //_bool                       m_isEableWeakAtk1Event = { false };

    /* Dissolve */
    _float                      m_fDissolveEdgeWidth = { 0.2f };
    _float2                      m_fDissolveDecreaseAlphaTime = { 0.01f, 8.f };
    _float                      m_fDissolveDecreaseAlphaValue = { 0.f };
    const _float4               m_fDissolveColor = { 0.8f, 0.65f, 0.4f, 1.0f };


    /*  mutex */
    mutex                       m_CollMonsterMutex;

private :
    _uint                       m_iFXIdx_Spining;
    _uint                       m_iFXIdx_BodyWind;
    _uint                       m_iFXIdx_Trail;
    _bool                       m_bGuradFX[2];

private:
    _bool            Update_Dead(_float fTimeDelta);
    void			Update_Colliders(_float fTimeDelta);
    void            Check_Guarding(_float fTimeDelta);              //부모클래스가 가드를 했다고 알려줌
    void            Update_GuardRotation(_float fTimeDelta);        //가드시 충돌방향으로 회전
    void            Start_GuardRotation(_float3 vContactPoint);     //가드시 충돌방향으로 회전을 위한 초기화
    void            Exception_Animaition(); // 애니메이션 이상한 것들 처리 

    /* notify */
 private:
     void	        Event_AttackTiming(GS_COLLISION eColl, _bool isAttackStart);


private:
    HRESULT             Bind_ShaderResources();
    HRESULT             Bind_Dissolve();
    HRESULT             Ready_Components();
    HRESULT             Ready_Colliders();
    HRESULT             Ready_AnimationEvents();
    HRESULT             Ready_Equipment();
    void                Equip_Part(EQUIPMENTTYPE eType, const _wstring& strPartName); //파츠 갈아 입기
    void                Update_QuickRenderCache();  //빠른 랜더용 파츠모음 (모션트레일도 여기서 랜더용 파츠 갈아끼우기)


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


    inline void		Add_CollState(_uint i) { m_iCollState |= i; }
    inline void		Toggle_CollState(_uint i) { m_iCollState ^= i; }
    inline void		Remove_CollState(_uint i) { m_iCollState &= ~i; }
    inline _bool	Has_CollState(_uint i) { return (m_iCollState & i) != 0; }
    inline void		Clear_CollState() { m_iCollState = 0; }

private: 
    _vector Decompose_Rotation(_matrix W, _vector localRot = { 0.f, 0.f, 0.f, 0.f },_vector offset = { 0.f, 0.f, 0.f, 1.f });

    // Screen Effect
    void Spawn_EmissiveDecal(_bool isUseOffset);
    void Spawn_CrackDecal();
    void Spawn_CircleBloodDecal();
    void Spawn_LinearBloodDecal();
    void Start_DefaultVignette();
    void Start_DefaultRadialBlur();
    void Start_LongRadialBlur();
    void Start_LongDistortion();
    void Start_DefaultDistortion();
    void Start_FullScreenDistortion();
    void FX_Trail();
    void Set_BaseTrail();
    void Set_BrightTrail();
    void Spawn_Guard_FX();

public:
    static CBody_Khazan_GS* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*    Clone(void* pArg) override;
    virtual void            Free() override;

};

NS_END