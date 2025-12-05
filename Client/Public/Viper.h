#pragma once
#include "Monster.h"

NS_BEGIN(Client)



class CViper final : public CMonster
{
private:
    CViper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CViper(const CViper& Prototype);
    virtual ~CViper() = default;

public:
    enum class PHASE                { PHASE1,CINEMATIC,PHASE2, END };
    enum class TWINBLADE            { LEFT, RIGHT, END };
    enum class TWINBLADE_R          { SWORD, FINGER0, FINGER1, FINGER2, END };
    enum class EYE                  { LEFT, RIGHT, END };

public:
    class CBody_Viper*              Get_Body() const { return m_pBody; }
    class CBody_Cinematic_Viper*    Get_Cinematic_Body() const { return m_pCinematicBody; }
    class CBody_Phase2_Viper*       Get_P2Body() const { return m_pPahse2Body; }
    class CCore_Viper*              Get_Core() { return m_pCore; }

    virtual _float4*                Get_LockOnPosition();
    PHASE                           Get_Phase() const { return m_ePhase; }
    _vector                         Get_CutSceneLook() const { return XMLoadFloat3(&m_vCutSceneLook); }

    void                            Set_Phase(PHASE ePhase);
    void                            Set_Weapon_Phase1();
    void                            Set_PhaseWeapon_Cinematic();
    void                            Set_PhaseWeapon_Phase2();
    void                            Set_WeaponOff();
    void                            Set_CutSceneLook(_float3 vLook) { m_vCutSceneLook = vLook; }
    void                            Viper_Land(_fvector vGoalPosition, _float fSpeed);
    void                            Reset_Viper_Gravity();
    



    class CAS_CutScene_Start_Viper*         Get_Viper_CutSceneState();
    class CAS_CutScene_2Phase_Viper*        Get_Phase2_Viper_CutSceneState();
    class CFSM_Viper*                       Get_Viper_FSM();

public:
    TRAIL_CONFIG                    Get_TrailConfig() const;
    void                            Set_TrailConfig(const TRAIL_CONFIG& Config);
    _uint                           Get_NumTrailTextures();
    ID3D11ShaderResourceView*       Get_TrailTexture(_uint iIndex);

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

    void                            Update_Landing(_float fTimeDelta);
                    
    void                            Grab_Check_Begin();
    void                            Grab_Check_End();
    void                            Set_ViperPosition(_fvector vPosition);
    

    void Set_Teleport(_fvector vPos);

private:
    HRESULT							Ready_Components();
    HRESULT							Ready_PartObjects();
    HRESULT							Ready_Projectiles();
    HRESULT							Ready_AnimEvent();
    HRESULT						    Ready_AnimEffectEvent();

private:
    void							Pick_ViperRock();
    void							Hold_ViperRock();
    void							Throw_ViperRock();
     
private:
    _vector                         Decompose_Rotation(_matrix W, _vector localRot = { 0.f, 0.f, 0.f, 0.f }, _vector offset = { 0.f, 0.f, 0.f, 1.f });
    void                            FX_1PhaseTrail();
    void                            FX_2PhaseHandTrail();
    void                            FX_2PhaseSwordTrail();
    void                            FX_2PhaseEyeTrail();

private:
    class CBody_Viper*              m_pBody = { nullptr };
    class CBody_Cinematic_Viper*    m_pCinematicBody = { nullptr };
    class CBody_Phase2_Viper*       m_pPahse2Body = { nullptr };
    class CTwinBlade_Viper*         m_pWeapon = { nullptr };
    class CCore_Viper*              m_pCore = { nullptr };
    class CTwinBlade_R_Viper*       m_pP2Weapon = { nullptr };
    class CProjectile_Rock_Viper*   m_pRock = { nullptr };
    class CMeshTrail*               m_p1PhaseTrail[ENUM_CLASS(TWINBLADE::END)] = {};
    class CMeshTrail*               m_p2PhaseTrail[ENUM_CLASS(TWINBLADE_R::END)] = {};
    class CLineTrail*               m_pLineTrail[ENUM_CLASS(EYE::END)] = {};

    _bool							m_isLookAt = { false };
    _float							m_fTurnSpeed = { 8.f };
    _bool							m_isGrab = { false };


    PHASE                           m_ePhase = { PHASE::END };

    _float4x4*                      m_pThrowMatrix = {};
    COLLISION_DESC                  m_tViperCollisionDesc = {};
    _float3                         m_vCutSceneLook = {};


private:
    _uint                           m_iRotFX_Idx;
    _uint                           tmpIdx;

public:
    static CViper*                  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;
};

NS_END
