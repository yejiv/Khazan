#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Elamain.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CElamain_Sword final : public CPartObject
{
public:
    typedef struct tagWeaponDesc : public PARTOBJECT_DESC
    {
        CTransform* pOwnerTransform = { nullptr };
        _float4x4* pSocketMatrix = { nullptr };
        CElamain::MONDATA* pData = { nullptr };

    }WEAPON_DESC;

private:
    CElamain_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CElamain_Sword(const CElamain_Sword& Prototype);
    virtual ~CElamain_Sword() = default;

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
    HRESULT					    Ready_Components();
    HRESULT                     Ready_Collision();
    HRESULT					    Bind_ShaderResources();

private:
    CShader*                    m_pShaderCom = { nullptr };
    CModel*                     m_pModelCom = { nullptr };
    CTransform*                 m_pOwnerTransform = { nullptr };
    CBody*                      m_pBodyComp = { nullptr };

    _bool					    m_isOnAttackCollision = { false };
    _float4x4*                  m_pSocketMatrix = { nullptr };

    CElamain::MONDATA*          m_pData = { nullptr };
public:
    static CElamain_Sword*      Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void			    Free() override;
};

NS_END 