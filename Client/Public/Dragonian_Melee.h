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
    enum class MONSTATE { DEAD, GRORRY, BRUTAL, ATTACK, DAMAGE, LOCKON, SLEEP, WALK, END };

    typedef struct TagMonData{
        //Anim
        _int                iAnimIndex = {};
        _bool               isAnimFinash = {false};
        _bool               isSleep = {false};
        _bool               isStateFiash = {false};
        _bool               isSlowWalk = {false};

        //BT
        _bool               isDamage = { false };
        _bool               isAttack = { false };
        _float              fAttackCool = {};
        
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

        CDragonian_Melee*   pOwner = { nullptr };
    }MONDATA;
private:
    CDragonian_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDragonian_Melee(const CDragonian_Melee& Prototype);
    virtual ~CDragonian_Melee() = default;

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
    HRESULT                         Ready_Prototype();
      
    HRESULT                         Ready_ETC();
    HRESULT							Ready_Components();
    HRESULT							Ready_PartObjects();
    HRESULT							Ready_AnimEvent();

    HRESULT							Ready_MonData();

    void                            Update_UIHp();
private:
    class CBody_Dragonian_Melee*    m_pBody = { nullptr };
    class CBlackBoard*              m_pBlackBoard = { nullptr };
    class CDragonian_Sword*         m_pWeapon = { nullptr };
    class CMon_HP*                  m_pUI_HP = { nullptr };

    _float4                         m_vHpPos = {};
    _float4x4*                      m_pHeadMatrix = { nullptr };
    MONDATA                         m_Data = {};

    _float                          m_fTimeDelta = {};

public:
    static CDragonian_Melee*        Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END