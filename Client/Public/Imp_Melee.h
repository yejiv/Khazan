#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class CImp_Melee final : public CMonster
{
private:
    CImp_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CImp_Melee(const CImp_Melee& Prototype);
    virtual ~CImp_Melee() = default;

public:
    virtual _float4* Get_LockOnPosition();
    class   CBody_Imp_Melee* Get_Body() { return m_pBody; }


public:
    virtual HRESULT					Initialize_Prototype() override;
    virtual HRESULT					Initialize_Clone(void* pArg) override;
    virtual void					Priority_Update(_float fTimeDelta) override;
    virtual void					Update(_float fTimeDelta) override;
    virtual void					Late_Update(_float fTimeDelta) override;
    virtual HRESULT					Render() override;
    virtual HRESULT					Render_Shadow() { return S_OK; }

public:
    virtual void					Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void					Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void					Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

private:
    HRESULT							Ready_Components();
    HRESULT							Ready_PartObjects();
    HRESULT							Ready_AnimEvent();

public:
    void                            HPUI_Dead();

private:
    class CBody_Imp_Melee*          m_pBody = { nullptr };
    class CImp_Sword*               m_pWeapon = { nullptr };
    class CMon_HP*                  m_pUI_HP = { nullptr };

    _bool                           m_isDetected = { false };

public:
    static CImp_Melee*              Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END