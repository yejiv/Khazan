#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
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
    void			    Render_Part(CModel* pModel);
    void			    Render_Part_Shadow(CModel* pModel);
    void                Render_Part_Outline(CModel* pModel);

public:
    virtual void        Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void        Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void        Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

public:
    _float4x4*          Get_BoneMatrix(const _char* pBoneName);
    _bool*              Get_FinishedAnimation() { return &m_isFinishedAnimation; }
    CModel*             Get_Model() { return m_pModelCom; }
    OUTLINE_CONFIG      Get_OutlineConfig() { return m_OutlineConfig; }

public:
    void                Set_IsGuarding(_bool* pIsGuarding) { m_pIsGuarding = pIsGuarding; }
    void                Set_GSword(class CGSword_Khazan_GS* pGS);
    void                Set_OutlineConfig(OUTLINE_CONFIG Config){ m_OutlineConfig.vColor = Config.vColor; m_OutlineConfig.fSize = Config.fSize; }
    void                Set_EnableEdge(_bool isEnable) { m_isEnableEdge = isEnable; }
    void		        Set_matGSword(_float4x4* mat) { m_pGSword_Matrix = mat; }

public:
    _bool               Is_SpearFullExtension() const { return m_isSpearFullExtension; }

public:
    void                Search_BrutalTarget(_float fTimeDelta);
    _bool               Check_BrutalAttack(_float fTimeDelta);
    void	            Event_AttackTiming(_bool isAttackStart);
  
private:
    class CClientInstance*      m_pClientInstance = { nullptr };
    class CTransform*           m_pParentTransform = { nullptr };
    class CGSword_Khazan_GS*    m_pWSword = { nullptr };
    class CTarget_BrutalAttack* m_pBrutalAttack = { nullptr };

    CShader*                    m_pShaderCom = { nullptr };
    CModel*                     m_pModelCom = { nullptr };
    CModel*                     m_pModelCom_Arm = { nullptr };
    CModel*                     m_pModelCom_Face = { nullptr };
    CModel*                     m_pModelCom_Hair = { nullptr };
    CModel*                     m_pModelCom_Leg = { nullptr };
    CModel*                     m_pModelCom_Shoes = { nullptr };
    CModel*                     m_pModelCom_Torso = { nullptr };

    CBody*                      m_pBodyCom_BodyAttack  = { nullptr }; //플레이어 바디에 붙이는 졸트바디 (공격용)
    CBody*                      m_pBodyCom_Attack = { nullptr }; //검 공격시 사용하는 졸트 바디
    CBody*                      m_pBodyCom_Guard  = { nullptr }; //검 방어에 사용하는 졸트 바디
    CBody*                      m_pBodyCom_Search = { nullptr }; //몬스터 서치에 사용하는 졸트 바디 

    PLAYER_DATA*                m_pPlayerData;
    COLLISION_DESC				m_tAttackCollisionDesc = {};
    COLLISION_DESC				m_tBodyAttackCollisionDesc = {};
    COLLISION_DESC				m_tGuardCollisionDesc = {};
    COLLISION_DESC              m_tSearchCollisionDesc = {};

    _float4x4*                  m_pGSword_Matrix = { nullptr };

    _uint*                      m_pParentState = { nullptr };
    _uint*                      m_pParentStatus = { nullptr };
    _uint*                      m_pHitReaction = { nullptr };
    _uint				        m_iCurState = {  };

    _bool				        m_isFinishedAnimation = { false };
    // _bool			        	m_isSetAnimation = { false };
    _uint				        m_iCurSetAnimIndex = { 0 };
    _bool                       m_isSpearFullExtension = { false }; //창을 완전히 뻗는 타이밍부터 true 
    _bool*                      m_pIsGuarding = { nullptr }; //가드중인지 체크

    // const _uint			    m_iSetAnimation[3] = { 3,2,1 };

     /* 뼈 위치 */
    _float4x4*                  m_pMatGSwordBody = { nullptr }; // 칼 중앙 로컬행렬
    _float4x4			        m_matWorldGSwordBody;                // 칼 중앙 월드행렬
    _float4x4*                  m_pMatGSwordTip = { nullptr };  // 칼 끝 로컬행렬
    _float4x4			        m_matWorldGSwordTip;                 // 칼 끝 월드행렬


    _matrix				        m_Offset_Matrix = {};   //블랜더와 축이 달라서 사용.

    class CMeshTrail*           m_pTrail = { nullptr };
    _uint	                    EffectID_SpearWind;
    _uint	                    EffectID_SpiralSpear;


    _bool                       m_isSpearTipActive = { true };
    _bool                       m_isSpearPoleActive = { true };

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
    OUTLINE_CONFIG              m_OutlineConfig = { _float3(1.f, 0.f, 1.f), 0.001f, 0.f, 0.f };

private:
    void			Update_Colliders(_float fTimeDelta);
    void            Check_Guarding(_float fTimeDelta);
    void            Update_GuardRotation(_float fTimeDelta);
    void            Start_GuardRotation(_float3 vContactPoint);

    void            Test_Attack(_float fTimeDelta);

private:
    HRESULT         Bind_ShaderResources();
    HRESULT         Ready_Components();
    HRESULT         Ready_Colliders();
    HRESULT         Ready_AnimationEvents();


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
    static CBody_Khazan_GS* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*    Clone(void* pArg) override;
    virtual void            Free() override;

};

NS_END




/*  
테스트 공격 키 
약공 3타 
    약공 1타       : 1
    약공 2타       : 1 + z
    약공 3타       : 1 + x

차지 약공
    차지          : 2
    공격          : 2 + z

차지 강공
    차지          : 3
    공격          : 3 + z

스킬 : 거센 기세
    모션          : 4

스킬 : 강기 발현
    차지          : 5 
    공격          : 5 + z

스킬 : 거인사냥 , 귀신  <-  비슷한 모션이 3가지 있는데 하실 때 같이 봅시당~ (일단 하나만 넣었음!)
    차지          : 6
    공격          : 6 + z

스킬 : 정면 돌파
    차지          : 7 
    공격          : 7 + z

스킬 : 거대한 포효
    모션1         : 9
    모션2         : 9 + ㅋ

스킬 : 내재된 분노
    모션          : 0
*/