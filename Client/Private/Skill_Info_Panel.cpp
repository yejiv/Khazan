#include "Skill_Info_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"
#include "UI_Default_Tex.h"

CSkill_Info_Panel::CSkill_Info_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CSkill_Info_Panel::CSkill_Info_Panel(const CSkill_Info_Panel& Prototype)
    : CUI_Panel(Prototype)
{
}

void CSkill_Info_Panel::Setting_Info(_wstring wstrInfo, _float2 vOffsetPos)
{
    m_pSKillInfo->Set_Text(wstrInfo);
}

HRESULT CSkill_Info_Panel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_Info_Panel::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CSkill_Info_Panel::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CSkill_Info_Panel::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CSkill_Info_Panel::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Info_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);

    for (auto Child : m_Children)
    {
        string strName = Child->Get_Name();
        if (strName == "Skill_Effect_Text")
        {
            m_pSKillInfo = static_cast<CUI_TextBox*>(Child);
        }
    }

    return S_OK;
}

CSkill_Info_Panel* CSkill_Info_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkill_Info_Panel* pInstance = new CSkill_Info_Panel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_Info_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_Info_Panel::Clone(void* pArg)
{
    CSkill_Info_Panel* pInstance = new CSkill_Info_Panel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_Info_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_Info_Panel::Free()
{
    __super::Free();

    Safe_Release(m_pSKillInfo);

}
