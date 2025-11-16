#pragma once
#include "UI_Panel.h"

NS_BEGIN(Client)
class CItemInfo_RandomEffect final : public CUI_Panel
{
private:
    CItemInfo_RandomEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CItemInfo_RandomEffect(const CItemInfo_RandomEffect& Prototype);
    virtual ~CItemInfo_RandomEffect() = default;

public:
    virtual void                Set_LocalPos(_float2 vPos, CUIObject* pParent);
    virtual void                Set_Text(_int iEffect_Type, _int iValue);
public:
    virtual HRESULT				Initialize_Prototype(_uint iLevel);
    virtual HRESULT				Initialize_Clone(void* pArg) override;
    virtual void				Priority_Update(_float fTimeDelta) override;
    virtual void				Update(_float fTimeDelta) override;
    virtual void				Late_Update(_float fTimeDelta) override;

    virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
    class CUI_TextBox*          m_pEffect_Name = { nullptr };
    class CUI_TextBox*          m_pEffect_Value = { nullptr };

public:
    static CItemInfo_RandomEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void				Free() override;
};
NS_END
