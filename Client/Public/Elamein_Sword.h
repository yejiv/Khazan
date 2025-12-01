#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Elamein.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
class CTexture;
NS_END

NS_BEGIN(Client)

class CElamein_Sword final : public CPartObject
{
public:
    typedef struct tagWeaponDesc : public PARTOBJECT_DESC
    {
        CTransform* pOwnerTransform = { nullptr };
        _float4x4* pSocketMatrix = { nullptr };
        CElamein::MONDATA* pData = { nullptr };

    }WEAPON_DESC;

public:
    
    void                        Add_Charge(_float fValue) { m_fChageValue = fValue; }
    void                        Reset_Charge() { m_isReset = true; }
private:
    CElamein_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CElamein_Sword(const CElamein_Sword& Prototype);
    virtual ~CElamein_Sword() = default;

public:
    virtual HRESULT			    Initialize_Prototype(_int iLevel);
    virtual HRESULT			    Initialize_Clone(void* pArg) override;
    virtual void			    Priority_Update(_float fTimeDelta) override;
    virtual void			    Update(_float fTimeDelta) override;
    virtual void			    Late_Update(_float fTimeDelta) override;
    virtual HRESULT			    Render() override;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    CShader*                    m_pShaderCom = { nullptr };
    CModel*                     m_pModelCom = { nullptr };
    CTransform*                 m_pOwnerTransform = { nullptr };
    CBody*                      m_pBodyComp = { nullptr };
    CTexture*                   m_pTextureCom = { nullptr };

    _bool					    m_isOnAttackCollision = { false };
    _float4x4*                  m_pSocketMatrix = { nullptr };

    CElamein::MONDATA*          m_pData = { nullptr };

    _float                      m_fChageValue = {};
    _bool                       m_isReset = { false };

private:
    HRESULT					    Ready_Components();
    HRESULT                     Ready_Collision();
    HRESULT					    Bind_ShaderResources();
    HRESULT                     Bind_Dissolve();
public:
    static CElamein_Sword*      Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void			    Free() override;
};

NS_END 