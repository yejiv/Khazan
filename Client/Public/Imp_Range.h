#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class CImp_Range final : public CMonster
{
private:
    CImp_Range(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CImp_Range(const CImp_Range& Prototype);
    virtual ~CImp_Range() = default;

public:
    virtual _float4*                Get_LockOnPosition();
    class   CBody_Imp_Range*        Get_Body() { return m_pBody; }


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

private:
    class CBody_Imp_Range*          m_pBody = { nullptr };

public:
    static CImp_Range*              Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END