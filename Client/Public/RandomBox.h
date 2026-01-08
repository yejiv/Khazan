#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CRandomBox final : public CGameObject
{
private:
    enum class ANIM_STATE
    {
        DANCE1_ACTIVE, DANCE1_LOOP, FLIPPING_ACTIVE, FLIPPING_LOOP, LIE_ACTIVE, LIE_LOOP, WALK_ACTIVE, WALK_LOOP, LEAN_ACTIVE, LEAN_LOOP,
        SHADOWBOXING_ACTIVE, SHADOWBOXING_LOOP, DANCE2_ACTIVE, DANCE2_LOOP, DANCE3_ACTIVE, DANCE3_LOOP, DRSTRANGE_ACTIVE, DRSTRANGE_LOOP, DEACTIVE, DEACTIVE_IDLE, END
    };
    enum SKIP_MESH_TYPE
    {
        MESH_BODY, MESH_HEAD, MESH_LEFT, MESH_RIGHT, MESH_CENTER
    };
public:
    void                            Set_Index(_int iIndex) { m_iIndex = iIndex; };
    _int                            Get_Index() { return m_iIndex; }
    CModel*                         Get_Model() {return m_pModelCom;}
    _bool                           isAnimFinish() { return m_bisAnimFinish; }
private:
    CRandomBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRandomBox(const CRandomBox& Prototype);
    virtual ~CRandomBox() = default;

public:
    virtual HRESULT			        Initialize_Prototype(_int iLevel);
    virtual HRESULT			        Initialize_Clone(void* pArg) override;
    virtual void			        Priority_Update(_float fTimeDelta) override;
    virtual void			        Update(_float fTimeDelta) override;
    virtual void			        Late_Update(_float fTimeDelta) override;
    virtual HRESULT			        Render() override;

private:
    CShader*                        m_pShaderCom = { nullptr };
    CModel*                         m_pModelCom = { nullptr };
    _int                            m_iIndex = {};
    _bool                           m_bisAnimFinish = { false };
    _int                            m_iAnimIndex = {};
private:
    HRESULT					        Ready_Components();
    HRESULT					        Bind_ShaderResources();
    _bool                           Skip_Mesh(_uint iMeshIndex);
public:
    static CRandomBox*   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void			        Free() override;
};

NS_END