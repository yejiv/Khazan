#pragma once
#include "UI_Text.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Client)
class CUI_WorldTextBox final : public CUI_Text
{
private:
    CUI_WorldTextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_WorldTextBox(const CUI_WorldTextBox& Prototype);
    virtual ~CUI_WorldTextBox() = default;
public:
    void                        Update_UITransform(_matrix vParentMat);
    void                        Set_LocalPos(_vector vPos);
    void                        Set_LocalSize(_float3 vSize);
    void                        Set_TextAlign(TEXT_ALIGN eAlign);
public:
    virtual HRESULT				Initialize_Prototype() override;
    virtual HRESULT				Initialize_Clone(void* pArg) override;
    virtual void				Priority_Update(_float fTimeDelta) override;
    virtual void				Update(_float fTimeDelta) override;
    virtual void				Late_Update(_float fTimeDelta) override;
    virtual HRESULT				Render() override;
private:
    CShader*                    m_pShaderCom = { nullptr };
    _float4x4					m_CombinedWorldMatrix = {};
private:
    HRESULT						Ready_Component();

public:
    static CUI_WorldTextBox*    Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void				Free() override;
};
NS_END
