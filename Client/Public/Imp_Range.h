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
    DIRECTION_INFO                  Get_HitDirInfo() const { return m_tHitDirInfo; }
    virtual _float4*                Get_LockOnPosition();
    class   CBody_Imp_Range*        Get_Body() { return m_pBody; }
    void                            Cast_Failed();

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
    void                            SFX_Move();
    void                            SFX_HIT(_uint iHitIndex);
    void                            SFX_DEAD();
    void                            SFX_SLEEP();
    void                            SFX_REALIZE();

private:
    HRESULT							Ready_Components();
    HRESULT							Ready_PartObjects();
    HRESULT							Ready_Projectiles();
    HRESULT							Ready_AnimEvent();
    HRESULT                         Ready_SFX();


private:
    void                            Cast_MagicBall(_uint iIndex);
    void                            Shoot_MagicBall(_uint iIndex);


    void                            Cast_Boomarang();
    void                            Hold_Boomarang();
    void                            Shoot_Boomarang();

public:
    void                            HPUI_Dead();
    void                            Dissolve_On();
                                

private:
    _bool							m_isLookAt = { false };
    _bool                           m_isDetected = { false };
    _bool                           m_isCastBoomarange = { false };
    _bool                           m_isCastMagicBall = { false };
    
    class CBody_Imp_Range*          m_pBody = { nullptr };
    class CImp_Wand*                m_pWeapon = { nullptr };
    class CMon_HP*                  m_pUI_HP = { nullptr };


    vector<class CProjectile_Imp_MagicBall*> m_MagicBalls;
    class CProjectile_Boomarang*    m_pBoomarang = { nullptr };

    COLLISION_DESC                  m_tImp_RangeColliderDesc = {};
    DIRECTION_INFO                  m_tHitDirInfo = {};


    _bool                           m_isDissolve = { false };
    _bool                           m_isDissolveEnd = { false };
    _float                          m_fDecreaseAlpha = {};
public:
    static CImp_Range*              Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END