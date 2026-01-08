#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Halberd.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
class CClothBody;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBody_Halberd final : public CPartObject
{
public:
    typedef struct tagBodyDesc : public PARTOBJECT_DESC
    {
        CTransform* pOwnerTransform = { nullptr };
        CHalberd::MONDATA* pData = { nullptr };
    }BODY_DESC;

public:
    _float4x4*              Get_BoneMatrix_Ptr(const _char* pBoneName);
    CModel*                 Get_Model() { return m_pModelCom; }
    _float                  Get_CulTrack();
private:
    CBody_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_Halberd(const CBody_Halberd& Prototype);
    virtual ~CBody_Halberd() = default;

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
    
    _int                            m_iPreAnim = { -1 };
    CHalberd::MONDATA*               m_pData = { nullptr };

    class CClothBody*               m_pCapeBody = { nullptr };
    COLLISION_DESC                  m_tCapeCollDesc = {};
    class CBody*                    m_pClothBody = { nullptr };
    COLLISION_DESC                  m_tClothBodyCollDesc = {};
    _float4x4*                      m_pClothBodyMatrix = { nullptr };
    _float4x4                       m_pClothCombinedMatrix;

private:
    HRESULT					        Ready_Components();
    HRESULT					        Bind_ShaderResources();
    HRESULT                         Bind_Dissolve();
public:
    static CBody_Halberd*   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void			        Free() override;
};

NS_END