#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "Player.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CBody_Khazan_Spear final : public CPartObject
{
public:
    typedef struct tagBodyKhazanSpearDesc : public CPartObject::PARTOBJECT_DESC
    {
        _uint* pState = { nullptr };
        _uint* pStatus = { nullptr };
        _uint* pHitReation = { nullptr };
        //_bool* pIsGuarding = { nullptr };
        class CTransform* pParentTransform = { nullptr };

    }BODY_KHAZAN_SPEAR_DESC;

private:
    CBody_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_Khazan_Spear(const CBody_Khazan_Spear& Prototype);
    virtual ~CBody_Khazan_Spear() = default;

public:
    _float4x4* Get_BoneMatrix(const _char* pBoneName);
    _bool* Get_FinishedAnimation() { return &m_isFinishedAnimation; }
    void		Set_matSpearFX(_float4x4* mat) { m_pSpearFX_Matrix = mat; }
    void		Set_matSpearOffset(_matrix mat) { m_SpearOffset_Matrix = mat; }
    void        Set_IsGuarding(_bool* pIsGuarding) { m_pIsGuarding = pIsGuarding; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void    Priority_Update(_float fTimeDelta);
	virtual void    Update(_float fTimeDelta);
	virtual void    Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;
    virtual HRESULT Render_Outline() override;
	void			Render_Part(CModel* pModel);
	void			Render_Part_Shadow(CModel* pModel);
    void            Render_Part_Outline(CModel* pModel);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

public:
	CModel* Get_Model() { return m_pModelCom; }

public:
    OUTLINE_CONFIG Get_OutlineConfig() { return m_OutlineConfig; }
    void Set_OutlineConfig(OUTLINE_CONFIG Config)
    {
        m_OutlineConfig.vColor = Config.vColor;
        m_OutlineConfig.fSize = Config.fSize;
    }

    _bool       Is_SpearFullExtension() const { return m_isSpearFullExtension; }

private:
    class CClientInstance* m_pClientInstance = { nullptr };
	class CTransform*   m_pParentTransform = { nullptr };   
    CShader*            m_pShaderCom = { nullptr };
    CModel*             m_pModelCom = { nullptr };
    CModel*             m_pModelCom_Arm = { nullptr };
    CModel*             m_pModelCom_Face = { nullptr };
    CModel*             m_pModelCom_Hair = { nullptr };
    CModel*             m_pModelCom_Leg = { nullptr };
    CModel*             m_pModelCom_Shoes = { nullptr };
    CModel*             m_pModelCom_Torso = { nullptr };

    CBody*              m_pBodyCom_SpearTip1 = { nullptr };
    CBody*              m_pBodyCom_SpearPole = { nullptr };

    _float4x4*          m_pSpearFX_Matrix = { nullptr };
    _matrix				m_SpearOffset_Matrix = {};

    PLAYER_DATA*        m_pPlayerData;

    _uint*              m_pParentState = { nullptr };
    _uint*              m_pParentStatus = { nullptr };
    _uint*              m_pHitReaction = { nullptr };
    _uint				m_iCurState = {  };


    _bool				m_isFinishedAnimation = { false };
   // _bool				m_isSetAnimation = { false };
    _uint				m_iCurSetAnimIndex = { 0 };
    _bool               m_isSpearFullExtension = { false }; //창을 완전히 뻗는 타이밍부터 true 
    _bool*              m_pIsGuarding = { nullptr }; //가드중인지 체크

   // const _uint			m_iSetAnimation[3] = { 3,2,1 };

    /* 뼈 위치 */
    _float4x4*          m_pSpearTip1_Matrix = { nullptr };
    _float4x4			m_pSpearTip1_MatrixW;
    _float4x4*          m_pSpearPole_Matrix = { nullptr };
    _float4x4			m_pSpearPole_MatrixW;
   // _float4x4			m_pSpearPole_MatrixW_AxisCorrection; // 축보정한 창대  

    class CMeshTrail* m_pTrail = { nullptr };
    _uint	EffectID_SpearWind;

    OUTLINE_CONFIG      m_OutlineConfig = { _float3(1.f, 0.f, 1.f), 0.001f, 0.f, 0.f };

    _bool               m_isSpearTipActive = { false };
    _bool               m_isSpearPoleActive = { false };

    /* 가드 */
    _float2             m_fJustGuardTime = { 0.f, 0.83f };
    _bool               m_isGuardRotating = { false };
    _float              m_fGuardRotationTime = { 0.f };
    _float              m_fGuardRotationDuration = { 0.15f }; // 0.15초 동안 회전
    _vector             m_vTargetRotationDir = {};
    _float              m_fStartAngle = { 0.f };
    _float              m_fTargetAngle = { 0.f };



private:
    void				Update_Collider(_float fTimeDelta);                     
    void                Check_Guarding(_float fTimeDelta);
    void                Update_GuardRotation(_float fTimeDelta);
    void                Start_GuardRotation(_float3 vContactPoint);

private:
    HRESULT				Ready_Components();
    HRESULT				Ready_AnimationEvent();
    HRESULT				Ready_Collider();
    HRESULT				Bind_ShaderResources();

private:
    void	FX_StrongAtk01_Trail();
    void	FX_StrongAtk02_Trail();
    void	FX_StrongAtk03_Trail();
    void	FX_FastAtk01_Trail();
    void	FX_FastAtk02_Trail();
    void	FX_FastAtk03_Trail();
    void	FX_FastAtk_SpawnWind();
    void	FX_StrongAtk_Charge_Blust();
    void	FX_StrongAtk_Charge_Trail();

    void	Event_AttackTiming(_bool isAttackStart);


private:
    inline void		Add_State(_uint i) { *m_pParentState |= i; }
    inline void		Toggle_State(_uint i) { *m_pParentState ^= i; }
    inline void		Remove_State(_uint i) { *m_pParentState &= ~i; }
    inline _bool	Has_State(_uint i) { return (*m_pParentState & i) != 0; }
    inline void		Clear_State() { *m_pParentState = 0; }


public:
    static CBody_Khazan_Spear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END
