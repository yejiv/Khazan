#pragma once
#include "Monster.h"
#include "Client_Defines.h"
#include "BlackBoard.h"

NS_BEGIN(Engine)
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CDragonian_Rampage final : public CMonster
{
public:
    typedef struct tagDragonianRampageTag : public CMonster::MONSTER_DESC
    {
        _bool isSleep = { false };

    }DRAGON_RAMPAGE_MONSTER_DESC;

    enum class MONSTATE { ATTACK_DEFAULT, ATTACK_BACK, ATTACK_RUSH, DEAD, GRORRY, BRUTAL, DAMAGE, TURN, LOCKON, SLEEP, WALK, END };
    enum class ATTACKSTATE { DEFAULT, RUSH, BACK, END};
    enum class ATTACK_BODY : _uint { HAND_L = 1 << 0, HAND_R = 1 << 1, TAIL = 1 << 2, END = 1 << 3 };
    enum class CLAW { LEFT_1, LEFT_2, LEFT_3, RIGHT_1, RIGHT_2, RIGHT_3, END };

    typedef struct TagMonData_Rampage{
        //애니메이션 관련
        _int                iAnimIndex = {};
        _bool               isAnimFinash = {false};
        _bool               isSleep = {false};
        _bool               isStateFiash = {false};
        _bool               isSlowWalk = {false};
        _bool               isTurn = { false };
        _float              fQuat = {};
        _float              fLook = {};
        _bool               isBland = { false };

        _float              fDecreaseAlpha = {};
        _float              fEdgeWidth = {};
        _float4             fEdgeColor = {};
        //BT 판단용 변수
        _bool               isDamage = { false };
        _bool               isPageChange = { false };
        _bool               isLockOn = { false };
        _bool               isWallCrushed = { false };
        
        _float              fAttackCool = {};
        _float              fDeltaSpeed = {1.f};

        HITREACTION         eHitType = { HITREACTION::END };
        _int                iBrutalHit = {};
        //ETC
        _float              fGloggyTime = {};
        _uint               iAttackBody_State = {};
    
        //스테이터스
        _float              fAttackDamage = {};

        _float*             pMaxHp = { nullptr };
        _float*             pCulHp = { nullptr };
        _float*             pMaxStamina = { nullptr };
        _float*             pCulStamina = { nullptr };

        _float              fWarkSpeed = { 10.f };
        CDragonian_Rampage* pOwner = { nullptr };

        _bool               isStamina_Regen = {};
        ATTACKSTATE         eAttack_State = { ATTACKSTATE::END };

        _bool               isMotionSleep = {};

        _bool               isSearch = { false };
    }MONDATA;

private:
    CDragonian_Rampage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDragonian_Rampage(const CDragonian_Rampage& Prototype);
    virtual ~CDragonian_Rampage() = default;

public:
    void                            LockOnLerp(_float fTimeDetla, _float fSpeed);
    MONDATA&                        Get_Data();
    void                            Move_F();
    void                            Hp_Visivle(_bool isVisivle);
    void                            Hp_Dead();
    _bool                           Check_Ranage(string strKey);
    _bool                           Check_Ranage(_float fRange);
    TARGET_DIR                      Get_DIR();

    void                            BurutalUI_On(_float fTime);
    void                            BurutalUI_Off();
public:
    virtual void                    Creature_Release() override;
    virtual HRESULT					Initialize_Prototype(_int iLevel);
    virtual HRESULT					Initialize_Clone(void* pArg) override;
    virtual void					Priority_Update(_float fTimeDelta) override;
    virtual void					Update(_float fTimeDelta) override;
    virtual void					Late_Update(_float fTimeDelta) override;
    virtual HRESULT			        Render() override;
    virtual void				    Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject = nullptr) override;

public:
    const TRAIL_CONFIG&             Get_TrailConfig() const;
    void                            Set_TrailConfig(const TRAIL_CONFIG& Config);
    _uint                           Get_NumTrailTextures();
    ID3D11ShaderResourceView*       Get_TrailTexture(_uint iIndex);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    class CBody_Dragonian_Rampage*  m_pBody = { nullptr };
    class CBlackBoard*              m_pBlackBoard = { nullptr };
    class CDragonian_Claw_L*        m_pClaw_L = { nullptr };
    class CDragonian_Claw_R*        m_pClaw_R = { nullptr };
    class CMon_HP*                  m_pUI_HP = { nullptr };
    class CMeshTrail*               m_pMeshTrail[ENUM_CLASS(CLAW::END)] = {};

    CBody*                          m_pHitBodyCom = { nullptr };
    class CBody*                    m_pBodyComp = { nullptr };

    _float4x4*                      m_pBodySocketMatrix = { nullptr };
    _float4x4*                      m_pLockOnSocketMatrix = { nullptr };
    _float4                         m_vLockOnPos = {};

    _float4                         m_vHpPos = {};
    _float4x4*                      m_pHeadMatrix = { nullptr };
    MONDATA                         m_Data = {};

    _float                          m_fTimeDelta = {};
    _float                          m_fAccTime = {};

    _float4                         m_vClawL_1_Start = {};
    _float4                         m_vClawL_2_Start = {};
    _float4                         m_vClawL_3_Start = {};
    _float4                         m_vClawL_1_End = {};
    _float4                         m_vClawL_2_End = {};
    _float4                         m_vClawL_3_End = {};
    _float4                         m_vClawR_1_Start = {};
    _float4                         m_vClawR_2_Start = {};
    _float4                         m_vClawR_3_Start = {};
    _float4                         m_vClawR_1_End = {};
    _float4                         m_vClawR_2_End = {};
    _float4                         m_vClawR_3_End = {};

    class CTarget_BrutalAttack*     m_pBrutalAttack = { nullptr };
    COLLISION_DESC				    m_tSearchCollisionDesc = {};
    COLLISION_DESC				    m_tHitCollisionDesc = {};
private:
    HRESULT                         Ready_Prototype();

    HRESULT                         Ready_ETC();
    HRESULT							Ready_Components();
    HRESULT							Ready_PartObjects();
    HRESULT							Ready_AnimEvent();

    HRESULT							Ready_MonData();

    void                            Update_UIHp();
    void                            Update_Body(_float fTimeDelta);

    void                            Jump_Move_1();
    void                            Jump_Move_2();

    void                            Update_WeaponPos();
    void                            Update_MeshTrail_R();
    void                            Update_MeshTrail_L();

    void                            Move_Sound();
    void                            Run_Sound();
public:
    static CDragonian_Rampage*      Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END