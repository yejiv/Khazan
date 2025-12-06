#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CCore_Viper final : public CPartObject
{
public:
    typedef struct tagWeaponDesc : public PARTOBJECT_DESC
    {
        CTransform* pOwnerTransform = { nullptr };
        class CViper* pOwner = { nullptr };
        _float4x4* pSocketMatrix = { nullptr };

    }WEAPON_DESC;

public:
    _float4*                Get_BonePointEX(const _char* pBoneName);
    _matrix					Get_BoneMatrix(const _char* pBoneName);
    _float4x4               Get_CombinedMatrix() const { return m_CombinedWorldMatrix; }
    _vector                 Get_CoreCenter() const {return XMLoadFloat4(&m_vCoreCenterPos);}
    _vector                 Get_CoreTip();


private:
    CCore_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CCore_Viper(const CCore_Viper& Prototype);
    virtual ~CCore_Viper() = default;

public:
    virtual HRESULT			Initialize_Prototype() override;
    virtual HRESULT			Initialize_Clone(void* pArg) override;
    virtual void			Priority_Update(_float fTimeDelta) override;
    virtual void			Update(_float fTimeDelta) override;
    virtual void			Late_Update(_float fTimeDelta) override;
    virtual HRESULT			Render() override;

public:
    virtual void            Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void            Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void            Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;


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
    CBody*                  m_pBodyComp = { nullptr };

    _float4x4*              m_pSocketMatrix = { nullptr };
    _float3                 m_vLocalOffset = {};
    
    _float4                 m_vCoreCenterPos = {};

public:
    static CCore_Viper*         Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void			    Free() override;
};

NS_END