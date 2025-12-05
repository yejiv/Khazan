#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CTwinBlade_Viper final : public CPartObject
{
public:
    typedef struct tagWeaponDesc : public PARTOBJECT_DESC
    {
        CTransform* pOwnerTransform = { nullptr };
        class CViper* pOwner = { nullptr };
        _float4x4* pSocketMatrix = { nullptr };

    }WEAPON_DESC;

//m_vRightTipPos
public:
    _float4*                Get_BonePointEX(const _char* pBoneName);
    _matrix					Get_BoneMatrix(const _char* pBoneName);
    _float4x4               Get_CombinedMatrix() const { return m_CombinedWorldMatrix; }
    void					Set_OnAttackCollision_L(_bool isToggle) { m_isLOnAttackCollision = isToggle; }
    void					Set_OnAttackCollision_R(_bool isToggle) { m_isROnAttackCollision = isToggle; }
    void					Set_OnAttackCollision(_bool isToggle) 
    { 
        m_isROnAttackCollision = isToggle;
        m_isLOnAttackCollision = isToggle;
    }
    _vector                 Get_RightSwordTip() const { return XMLoadFloat4(&m_vRightTipPos); }
    _vector                 Get_LeftSwordTip() const { return XMLoadFloat4(&m_vLeftTipPos); }
    _vector                 Get_RightSowrdStartPos() const { return XMLoadFloat4(&m_vRightBladeStartPos); }
    _vector                 Get_LeftSwordStartPos() const { return XMLoadFloat4(&m_vLeftBladeStartPos); }


    _float4                 Get_GrabPos() const { return m_vGrabPos; }

    _matrix                 Get_CombinedMatrixEX() const { return XMLoadFloat4x4(&m_CombinedWorldMatrix); }

private:
    CTwinBlade_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTwinBlade_Viper(const CTwinBlade_Viper& Prototype);
    virtual ~CTwinBlade_Viper() = default;

public:
    virtual HRESULT			Initialize_Prototype() override;
    virtual HRESULT			Initialize_Clone(void* pArg) override;
    virtual void			Priority_Update(_float fTimeDelta) override;
    virtual void			Update(_float fTimeDelta) override;
    virtual void			Late_Update(_float fTimeDelta) override;
    virtual HRESULT			Render() override;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;


private:
    HRESULT					Ready_Components();
    HRESULT                 Ready_Collision();
    HRESULT					Bind_ShaderResources();

private:
    class CViper*           m_pOwner = { nullptr };

private:
    CShader*                m_pShaderCom = { nullptr };
    CModel*                 m_pModelCom = { nullptr };
    CTransform*             m_pOwnerTransform = { nullptr };

    _bool					m_isLOnAttackCollision = { false };
    _bool                   m_isROnAttackCollision = { false };

    CBody*                  m_pRightBodyComp = { nullptr };
    CBody*                  m_pLeftBodyComp = { nullptr };
    
    _float4x4*              m_pSocketMatrix = { nullptr };

    _float4x4               m_RightMatrix = {};
    _float4x4               m_LeftMatrix = {};

    // 창 날 Offset
    _float4                 m_vRightTipPos = {};
    _float4                 m_vLeftTipPos = {};
    _float4                 m_vRightBladeStartPos = {};
    _float4                 m_vLeftBladeStartPos = {};



    _float4                 m_vGrabPos = {};
    _float3                 m_vLocalOffset = {};
    _float3                 m_VDebugRot = {};

    COLLISION_DESC          m_tRightBladeDesc = {};
    COLLISION_DESC          m_tLeftBladeDesc = {};

public:
    static CTwinBlade_Viper*    Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void			    Free() override;
};

NS_END