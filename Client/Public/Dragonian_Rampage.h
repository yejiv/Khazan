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
    enum class MONSTATE { ATTACK_DEFAULT, ATTACK_BACK, ATTACK_JUMP, ATTACK_RUSH, DEAD, GRORRY, BRUTAL, DAMAGE, LOCKON, SLEEP, WALK, END };
    enum class ATTACKSTATE { DEFAULT, JUMP, RUSH, BACK, END};
    enum ATTACK_BODY : _uint {HAND_L = 1 << 0, HAND_R = 1 << 1, TAIL = 1 << 2, END = 1 << 3};
    typedef struct TagMonData_Rampage{
        //애니메이션 관련
        _int                iAnimIndex = {};
        _bool               isAnimFinash = {false};
        _bool               isSleep = {false};
        _bool               isStateFiash = {false};
        _bool               isSlowWalk = {false};

        //BT 판단용 변수
        _bool               isDamage = { false };
        _float              fAttackCool = {};
        
        HITREACTION         eHitType = { HITREACTION::END };
        
        //ETC
        _float              fGloggyTime = {};
        _uint               iAttack_State = {};

        //스테이터스
        _float*             pMaxHp = { nullptr };
        _float*             pCulHp = { nullptr };
        _float*             pMaxStamina = { nullptr };
        _float*             pCulStamina = { nullptr };

        CDragonian_Rampage*   pOwner = { nullptr };

        ATTACKSTATE         eAttack_State = { ATTACKSTATE::END };
    }MONDATA;
private:
    CDragonian_Rampage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDragonian_Rampage(const CDragonian_Rampage& Prototype);
    virtual ~CDragonian_Rampage() = default;

public:
    MONDATA&                        Get_Data();
    void                            Move_F();
    void                            Hp_Visivle(_bool isVisivle);
    void                            Hp_Dead();
    _bool                           Check_AttackRanage(string strKey);

public:
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
    class CBody_Dragonian_Rampage*  m_pBody = { nullptr };
    class CBlackBoard*              m_pBlackBoard = { nullptr };
    class CDragonian_Claw_L*        m_pClaw_L = { nullptr };
    class CDragonian_Claw_R*        m_pClaw_R = { nullptr };
    class CMon_HP*                  m_pUI_HP = { nullptr };
    CBody*                          m_pHitBodyCom = { nullptr };
    CBody*                          m_pTaileCom = { nullptr };

    _float4x4*                      m_pBodySocketMatrix = { nullptr };
    _float4x4*                      m_pTailSocketMatrix = { nullptr };

    _float4                         m_vHpPos = {};
    _float4x4*                      m_pHeadMatrix = { nullptr };
    MONDATA                         m_Data = {};

    _float                          m_fTimeDelta = {};

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
    static CDragonian_Rampage*      Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END