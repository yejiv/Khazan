#include "Skill_Condition_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"
#include "UI_Default_Tex.h"

CSkill_Condition_Panel::CSkill_Condition_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CSkill_Condition_Panel::CSkill_Condition_Panel(const CSkill_Condition_Panel& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CSkill_Condition_Panel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_Condition_Panel::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CSkill_Condition_Panel::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CSkill_Condition_Panel::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CSkill_Condition_Panel::Late_Update(_float fTimeDelta)
{
    if(m_isVisible)
        __super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Condition_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);

    return S_OK;
}

CSkill_Condition_Panel* CSkill_Condition_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkill_Condition_Panel* pInstance = new CSkill_Condition_Panel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_Condition_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_Condition_Panel::Clone(void* pArg)
{
    CSkill_Condition_Panel* pInstance = new CSkill_Condition_Panel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_Condition_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_Condition_Panel::Free()
{
    __super::Free();

}
