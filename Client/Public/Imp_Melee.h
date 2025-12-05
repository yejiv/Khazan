#pragma once
#include "Monster.h"

NS_BEGIN(Engine)
class CModel;
class CGameInstance;
NS_END

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
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    void                            HPUI_Dead();

private:
    HRESULT							Ready_Components();
    HRESULT							Ready_PartObjects();
    HRESULT							Ready_AnimEvent();
    void                            Update_SwordPos();
    void                            Update_MeshTrail();

private:
    class CBody_Imp_Melee*          m_pBody = { nullptr };
    class CImp_Sword*               m_pWeapon = { nullptr };
    class CMon_HP*                  m_pUI_HP = { nullptr };
    class CMeshTrail*               m_pMeshTrail = { nullptr };

    _bool                           m_isDetected = { false };
    _bool                           m_isLookAt = { false };
    _float							m_fTurnSpeed = { 10.f };

    _float4                         m_vSwordStart{}, m_vSwordEnd{};
    COLLISION_DESC                  m_tImpMeleeColliderDesc = {};

public:
    static CImp_Melee*              Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END