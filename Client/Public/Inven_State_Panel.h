#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CInven_State_Panel final : public CUI_Panel
{
private:
    enum TEXT_TYPE { LEVEL, VITALITY, ENDURANCE, STRENGTH, WILL, COMPETECNCY, MAXHP, MAXSTAMINA, ATK, DEF, END };
private:
    CInven_State_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CInven_State_Panel(const CInven_State_Panel& Prototype);
    virtual ~CInven_State_Panel() = default;

public:
    virtual HRESULT				Initialize_Prototype() override;
    virtual HRESULT				Initialize_Clone(void* pArg) override;
    virtual void				Priority_Update(_float fTimeDelta) override;
    virtual void				Update(_float fTimeDelta) override;
    virtual void				Late_Update(_float fTimeDelta) override;
    virtual HRESULT				Render() override;

    virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
    vector<class CUI_TextBox*>  m_pUIText;

    const _uint*                m_pLevel = { nullptr };
    
    const _uint*                m_pVitality = { nullptr };
    const _uint*                m_pEndurance = { nullptr };
    const _uint*                m_pPower = { nullptr };
    const _uint*                m_pCompetency = { nullptr };
    const _uint*                m_pWill = { nullptr };
    
    const _float*               m_pMaxHp = {nullptr};
    const _float*               m_pMaxStamina = { nullptr };
    const _float*               m_pAtk = { nullptr };
    const _float*               m_pDef = { nullptr };

private:
    void                        Text_Update();

public:
    static CInven_State_Panel*  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*        Clone(void* pArg) override;
    virtual void				Free() override;
};
NS_END
