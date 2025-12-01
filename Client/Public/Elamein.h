#pragma once
#include "Monster.h"
#include "Client_Defines.h"
#include "BlackBoard.h"

NS_BEGIN(Engine)
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CElamein final : public CMonster
{
public:
    enum class MONSTATE { DEAD, GRORRY, BRUTAL, ATTACK_LONG, ATTACK_ENCHANT, ATTACK_DEFAULT, ATTACK_MIDDLE, DAMAGE, TURN, LOCKON, SLEEP, DODGE, GUARD, END };
    enum class ATTACK_BODY : _uint { SHILED = 1 << 0, SWORD = 1 << 1, RIGHT_LEG = 1 << 2, END = 1 << 3};
    enum class ATTACKSTATE { DEFAULT, ENCHANT, MIDDLE, LONG, END };

    typedef struct TagMonData_Elamein{
        //애니메이션 관련
        _int                iAnimIndex = {};
        _bool               isAnimFinash = {false};
        _bool               isSleep = {false};
        _bool               isStateFiash = {false};
        _bool               isTurn = { false };
        _float              fQuat = {};
        _float              fLook = {};
        _bool               isBland = { false };

        _float              fDecreaseAlpha = {};
        _float              fEdgeWidth = {};
        _float4             fEdgeColor = {};

        //BT 판단용 변수
        _float              fDodgeCool = {};
        _bool               isDodge = { false };

        _float              fGuardCool = {};
        _bool               isGuard = { false };
        
        _bool               isDamage = { false };
        _bool               isWallCrushed = { false };

        _float              fLong_AttackCool = {};
        _float              fAttackCool = {};
        _float              fSpecial_AttackCool = {};
        _float              fDeltaSpeed = {1.f};

        HITREACTION         eHitType = { HITREACTION::END };
        _int                iBrutalHit = {};

        //ETC
        _float              fGloggyTime = {};
        _uint               iAttackBody_State = {};
    
        //스테이터스
        _float              fAttackDamage = {};
        ATTACKSTATE         eAttackState = {};
        _float*             pMaxHp = { nullptr };
        _float*             pCulHp = { nullptr };
        _float*             pMaxStamina = { nullptr };
        _float*             pCulStamina = { nullptr };

        _float              fWarkSpeed = { 10.f };
        CElamein* pOwner = { nullptr };

    }MONDATA;
private:
    CElamein(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CElamein(const CElamein& Prototype);
    virtual ~CElamein() = default;

public:
    void                            LockOnLerp(_float fTimeDetla, _float fSpeed);
    void                            LockOn();

    void                            Rush();

    MONDATA&                        Get_Data();
    void                            Move_F();
    void                            Hp_Visivle(_bool isVisivle);
    void                            Hp_Dead();
    _bool                           Check_Ranage(string strKey);
    _bool                           Check_Ranage(_float fRange);
    TARGET_DIR                      Get_DIR();

    void                            Add_Charge(_float fValue);
    void                            Reset_Charge();
    _float                          Get_TrackPotion();
    virtual void				    Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject = nullptr) override;
public:
    virtual void                    Creature_Release() override;
    virtual HRESULT					Initialize_Prototype(_int iLevel);
    virtual HRESULT					Initialize_Clone(void* pArg) override;
    virtual void					Priority_Update(_float fTimeDelta) override;
    virtual void					Update(_float fTimeDelta) override;
    virtual void					Late_Update(_float fTimeDelta) override;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    class CBody_Elamein*            m_pBody = { nullptr };
    class CElamein_Shield*          m_pShield = { nullptr };
    class CElamein_Sword*           m_pSword = { nullptr };
    class CBlackBoard*              m_pBlackBoard = { nullptr };
    class CMon_HP*                  m_pUI_HP = { nullptr };

    CBody*                          m_pHitBodyCom = { nullptr };
    CBody*                          m_pLeftLegCom = { nullptr };

    _float4x4*                      m_pBodySocketMatrix = { nullptr };
    _float4x4*                      m_pLeftLegSocketMatrix = { nullptr };
    _float4x4*                      m_pLockOnSocketMatrix = { nullptr };
    _float4                         m_vLockOnPos = {};

    _float4                         m_vHpPos = {};
    _float4x4*                      m_pHeadMatrix = { nullptr };
    MONDATA                         m_Data = {};

    _float                          m_fTimeDelta = {};
    _float                          m_fAccTime = {};

    _bool                           m_isHit = { true };

    //UI테스트
    class CUI_Talk_Danjinjar*       m_pTalk = { nullptr };
private:
    HRESULT                         Ready_Prototype();

    HRESULT                         Ready_ETC();
    HRESULT							Ready_Components();
    HRESULT							Ready_PartObjects();
    HRESULT							Ready_AnimEvent();

    HRESULT							Ready_MonData();

    void                            Update_UIHp();
    void                            Update_Body(_float fTimeDelta);
public:
    static CElamein*      Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END