#include "Skill_QuickSLot_List.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"

CSkill_QuickSlot_List::CSkill_QuickSlot_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Tap{ pDevice, pContext }
{
}

CSkill_QuickSlot_List::CSkill_QuickSlot_List(const CSkill_QuickSlot_List& Prototype)
    : CUI_Tap(Prototype)
{
}

void CSkill_QuickSlot_List::Update_Pos(_int iIndex, _float2 vPos, _float fOffSetY)
{
    m_vLocalPos.x = vPos.x;
    m_vLocalPos.y = vPos.y + iIndex * fOffSetY;

    __super::Update_Transform(nullptr, m_vWorldPos);
}

HRESULT CSkill_QuickSlot_List::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    return S_OK;
}

HRESULT CSkill_QuickSlot_List::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Componet(), E_FAIL);

    return S_OK;
}

void CSkill_QuickSlot_List::Priority_Update(_float fTimeDelta)
{
}

void CSkill_QuickSlot_List::Update(_float fTimeDelta)
{

}

void CSkill_QuickSlot_List::Late_Update(_float fTimeDelta)
{

    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    __super::Late_Update(fTimeDelta);
}

HRESULT CSkill_QuickSlot_List::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(m_iShaderPass);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSkill_QuickSlot_List::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);
    /*m_iShaderPass = 6;
    m_iState = ENUM_CLASS(UISTATE::ENABLE);*/

    //for (auto pChild : m_Children)
    //{
    //    string strName = pChild->Get_Name();
    //    if (strName == "Menu_Name")
    //    {
    //        m_pTextBox = static_cast<CUI_TextBox*>(pChild);
    //        Safe_AddRef(m_pTextBox);
    //    }
    //    else if (strName == "Menu_List_Deco")
    //    {
    //        m_pDeco = static_cast<CMainMune_Deco*>(pChild);
    //        Safe_AddRef(m_pDeco);
    //    }
    //}

    return S_OK;
}

HRESULT CSkill_QuickSlot_List::Ready_Componet()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_MenuList"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CSkill_QuickSlot_List* CSkill_QuickSlot_List::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CSkill_QuickSlot_List* pInstance = new CSkill_QuickSlot_List(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_QuickSlot_List"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_QuickSlot_List::Clone(void* pArg)
{
    CSkill_QuickSlot_List* pInstance = new CSkill_QuickSlot_List(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_QuickSlot_List"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_QuickSlot_List::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
