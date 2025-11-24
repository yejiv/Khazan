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
        _int                iAnimIndex = {};
        _bool               isAnimFinash;
        _bool               isSleep;
        _bool               isStateFiash;
        _bool               isSlowWalk;

        CDragonian_Melee*   pOwner = { nullptr };
    }MONDATA;
private:
    CDragonian_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDragonian_Melee(const CDragonian_Melee& Prototype);
    virtual ~CDragonian_Melee() = default;

public:
    MONDATA&                        Get_Data();
    void                            Move_F();
public:
    virtual HRESULT					Initialize_Prototype(_int iLevel);
    virtual HRESULT					Initialize_Clone(void* pArg) override;
    virtual void					Priority_Update(_float fTimeDelta) override;
    virtual void					Update(_float fTimeDelta) override;
    virtual void					Late_Update(_float fTimeDelta) override;

public:
    virtual void					Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void					Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void					Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

private:
    HRESULT                         Ready_Prototype();
      
    HRESULT							Ready_Components();
    HRESULT							Ready_PartObjects();
    HRESULT							Ready_AnimEvent();

private:
    class CBody_Dragonian_Melee*    m_pBody = { nullptr };
    class CDragonian_Sword*         m_pWeapon = { nullptr };
    MONDATA                         m_Data = {};

    _float                          m_fTimeDelta = {};

public:
    static CDragonian_Melee*        Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END