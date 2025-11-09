#pragma once
#include "UI_Panel.h"

NS_BEGIN(Client)
class CSkill_Info_Panel final : public CUI_Panel
{
private:
    CSkill_Info_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSkill_Info_Panel(const CSkill_Info_Panel& Prototype);
    virtual ~CSkill_Info_Panel() = default;
public:
    void                        Setting_Info(_wstring wstrInfo, _float2 vOffsetPos);

public:
    virtual HRESULT				Initialize_Prototype();
    virtual HRESULT				Initialize_Clone(void* pArg) override;
    virtual void				Priority_Update(_float fTimeDelta) override;
    virtual void				Update(_float fTimeDelta) override;
    virtual void				Late_Update(_float fTimeDelta) override;

    virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
    class CUI_TextBox*          m_pSKillInfo = { nullptr };

public:
    static CSkill_Info_Panel*   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void				Free() override;
};
NS_END
