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
public:
    void                            Set_Index(_int iIndex) { m_iIndex = iIndex; };
    _int                            Get_Index() { return m_iIndex; }
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
private:
    HRESULT					        Ready_Components();
    HRESULT					        Bind_ShaderResources();

public:
    static CRandomBox*   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void			        Free() override;
};

NS_END