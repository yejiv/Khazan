#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Dragonian_Rampage.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CBody_Dragonian_Rampage final : public CPartObject
{
public:
    typedef struct tagBodyDesc : public PARTOBJECT_DESC
    {
        CTransform* pOwnerTransform = { nullptr };
        CDragonian_Rampage::MONDATA* pData = { nullptr };
    }BODY_DESC;

public:
    _float4x4*              Get_BoneMatrix_Ptr(const _char* pBoneName);
    CModel*                 Get_Model() { return m_pModelCom; }
private:
    CBody_Dragonian_Rampage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_Dragonian_Rampage(const CBody_Dragonian_Rampage& Prototype);
    virtual ~CBody_Dragonian_Rampage() = default;

public:
    virtual HRESULT			        Initialize_Prototype(_int iLevel);
    virtual HRESULT			        Initialize_Clone(void* pArg) override;
    virtual void			        Priority_Update(_float fTimeDelta) override;
    virtual void			        Update(_float fTimeDelta) override;
    virtual void			        Late_Update(_float fTimeDelta) override;
    virtual HRESULT			        Render() override;

public:
    virtual void			        Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void			        Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void			        Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

private:
    HRESULT					        Ready_Components();
    HRESULT					        Bind_ShaderResources();

private:
    CShader*                        m_pShaderCom = { nullptr };
    CModel*                         m_pModelCom = { nullptr };
    
    CTransform*                     m_pOwnerTransform = { nullptr };
    
    _int                            m_iPreAnim = { -1 };
    CDragonian_Rampage::MONDATA*      m_pData = { nullptr };

public:
    static CBody_Dragonian_Rampage*   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void			        Free() override;
};

NS_END