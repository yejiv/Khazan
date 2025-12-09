#pragma once
#include "Client_Defines.h"
#include "WeaponObject.h"
#include "Dragonian_Rampage.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBody_Dragonian_Rampage final : public CWeaponObject
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
    _float                  Get_CulTrack();
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
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    CShader*                        m_pShaderCom = { nullptr };
    CModel*                         m_pModelCom = { nullptr };
    CTexture*                       m_pTextureCom = { nullptr };

    CTransform*                     m_pOwnerTransform = { nullptr };

    CBody*                          m_pBodyComp = { nullptr };
    _bool					        m_isOnAttackCollision = { false };
    _float4x4*                      m_pSocketMatrix = { nullptr };

    _int                            m_iPreAnim = { -1 };
    CDragonian_Rampage::MONDATA*    m_pData = { nullptr };

private:
    HRESULT					        Ready_Components();
    HRESULT					        Bind_ShaderResources();
    HRESULT                         Bind_Dissolve();
    void                            Update_Body(_float fTimedelta);
public:
    static CBody_Dragonian_Rampage*   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void			        Free() override;
};

NS_END