#pragma once
#include "Monster.h"
#include "Client_Defines.h"
#include "BlackBoard.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CDragonian_Melee final : public CMonster
{
public:
    enum class MONSTATE { DEAD, GRORRY, BRUTAL, ATTACK, DAMAGE, LOCKON, SLEEP, TURN, END };

    typedef struct TagMonData{
        //Anim
        _int                iAnimIndex = {};
        _bool               isAnimFinash = {false};
        _bool               isSleep = {false};
        _bool               isStateFiash = {false};
        _bool               isSlowWalk = {false};
        _bool               isBland = { false };
        _float              fQuat = {};

        _float              fDecreaseAlpha = {};
        _float              fEdgeWidth = {};
        _float4             fEdgeColor = {};
        //BT
        _bool               isChase = { false };
        _bool               isDamage = { false };
        _bool               isAttack = { false };
        _bool               isTurn = { false };
        _bool               isLockOn = { false };
        _float              fAttackCool = {};
        _float              fAnimDeley = {};
        _int                iBrutalHit = {};
        HITREACTION         eHitType = { HITREACTION::END };
        //ETC
        _float              fGloggyTime = {};
        _bool               isAttack_Collision = { false };

        //State
        _float              fAttackDamage = {};
        
        _float*             pMaxHp = { nullptr };
        _float*             pCulHp = { nullptr };
        _float*             pMaxStamina = { nullptr };
        _float*             pCulStamina = { nullptr };

        _float              fWarkSpeed = { 10.f };
        CDragonian_Melee*   pOwner = { nullptr };
 
    }MONDATA;
private:
    CDragonian_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDragonian_Melee(const CDragonian_Melee& Prototype);
    virtual ~CDragonian_Melee() = default;

public:
    MONDATA&                        Get_Data();
    void                            LockOnLerp(_float fTimeDetla, _float fSpeed);
    void                            LockOn();
    void                            Hp_Visivle(_bool isVisivle);
    void                            Hp_Dead();

    void                            LookAt_Lerp(_float fTimeDelta);
    _bool                           Check_AttackRanage(string strKey);
    _bool                           Check_Ranage(_float fRange);
    TARGET_DIR                      Get_DIR();
public:
    virtual HRESULT					Initialize_Prototype(_int iLevel);
    virtual HRESULT					Initialize_Clone(void* pArg) override;
    virtual void					Priority_Update(_float fTimeDelta) override;
    virtual void					Update(_float fTimeDelta) override;
    virtual void					Late_Update(_float fTimeDelta) override;
    virtual void				    Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject = nullptr) override;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    HRESULT                         Ready_Prototype();
      
    HRESULT                         Ready_ETC();
    HRESULT							Ready_Components();
    HRESULT							Ready_PartObjects();
    HRESULT							Ready_AnimEvent();

    HRESULT							Ready_MonData();

    void                            Update_UIHp();
    void                            Update_WalkSpeed();

    void                            Attack_Move();
private:
    class CBody_Dragonian_Melee*    m_pBody = { nullptr };
    class CBlackBoard*              m_pBlackBoard = { nullptr };
    class CDragonian_Sword*         m_pWeapon = { nullptr };
    class CMon_HP*                  m_pUI_HP = { nullptr };

    _float4                         m_vHpPos = {};
    _float4x4*                      m_pHeadMatrix = { nullptr };
    MONDATA                         m_Data = {};

    _float                          m_fTimeDelta = {};

    _float4x4*                      m_pLockOnSocketMatrix = { nullptr };
    _float4                         m_vLockOnPos = {};

    _float4                         m_vSword_Start = {};
    _float4                         m_vSword_End = {};
public:
    static CDragonian_Melee*        Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END