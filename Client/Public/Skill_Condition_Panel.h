#pragma once
#include "UI_Panel.h"

NS_BEGIN(Client)
class CSkill_Condition_Panel final : public CUI_Panel
{
private:
    CSkill_Condition_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSkill_Condition_Panel(const CSkill_Condition_Panel& Prototype);
    virtual ~CSkill_Condition_Panel() = default;

public:
    virtual HRESULT				        Initialize_Prototype();
    virtual HRESULT				        Initialize_Clone(void* pArg) override;
    virtual void				        Priority_Update(_float fTimeDelta) override;
    virtual void				        Update(_float fTimeDelta) override;
    virtual void				        Late_Update(_float fTimeDelta) override;

    virtual HRESULT				        Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

public:
    static CSkill_Condition_Panel*      Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void				        Free() override;
};
NS_END
