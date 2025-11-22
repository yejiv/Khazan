#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CStore_Slot final : public CUI_Panel
{
private:
    CStore_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CStore_Slot(const CStore_Slot& Prototype);
    virtual ~CStore_Slot() = default;

public:
    void                    Setting_Slot(const ITEM_DATA* pData);
public:
    virtual HRESULT			Initialize_Prototype() override;
    virtual HRESULT			Initialize_Clone(void* pArg) override;
    virtual void			Priority_Update(_float fTimeDelta) override;
    virtual void			Update(_float fTimeDelta) override;
    virtual void			Late_Update(_float fTimeDelta) override;
    virtual HRESULT		    Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
    class CUI_Atlas_Icon*   m_pAtlasIcon = { nullptr };
public:
    static CStore_Slot*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*    Clone(void* pArg) override;
    virtual void			Free() override;
};

NS_END