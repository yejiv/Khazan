#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Pet_Danjinjar.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBody_Pet_Danjinjar final : public CPartObject
{
private:
    enum class ANIM_STATE
    {
        DANCE1_ACTIVE,DANCE1_LOOP, FLIPPING_ACTIVE, FLIPPING_LOOP, LIE_ACTIVE, LIE_LOOP, WALK_ACTIVE, WALK_LOOP, LEAN_ACTIVE, LEAN_LOOP,
        SHADOWBOXING_ACTIVE, SHADOWBOXING_LOOP, DANCE2_ACTIVE, DANCE2_LOOP, DANCE3_ACTIVE, DANCE3_LOOP, DRSTRANGE_ACTIVE,DRSTRANGE_LOOP, DEACTIVE, DEACTIVE_IDLE, END };
    enum SKIP_MESH_TYPE
    {
        MESH_BODY, MESH_HEAD, MESH_LEFT, MESH_RIGHT, MESH_CENTER   
    };
public:
    typedef struct tagBodyDesc : public PARTOBJECT_DESC
    {
        CTransform* pOwnerTransform = { nullptr };
        CPet_Danjinjar::MONDATA* pData = { nullptr };
    }BODY_DESC;

public:
    _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName);
    CModel* Get_Model() { return m_pModelCom; }
private:
    CBody_Pet_Danjinjar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_Pet_Danjinjar(const CBody_Pet_Danjinjar& Prototype);
    virtual ~CBody_Pet_Danjinjar() = default;

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
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };

    CTransform* m_pOwnerTransform = { nullptr };

    _int                            m_iPreAnim = { -1 };
    CPet_Danjinjar::MONDATA*        m_pData = { nullptr };

private:
    HRESULT					        Ready_Components();
    HRESULT					        Bind_ShaderResources();
    HRESULT                         Bind_Dissolve();
    _bool                           Skip_Mesh(_uint iMeshIndex);
public:
    static CBody_Pet_Danjinjar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void			        Free() override;
};

NS_END