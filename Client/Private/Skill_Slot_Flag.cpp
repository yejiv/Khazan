#include "Skill_Slot_Flag.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CSkill_Slot_Flag::CSkill_Slot_Flag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CSkill_Slot_Flag::CSkill_Slot_Flag(const CSkill_Slot_Flag& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CSkill_Slot_Flag::Initialize_Prototype(_int iLevel)
{
    return S_OK;
}

HRESULT CSkill_Slot_Flag::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Component(), E_FAIL);
    return S_OK;
}

void CSkill_Slot_Flag::Priority_Update(_float fTimeDelta)
{
}

void CSkill_Slot_Flag::Update(_float fTimeDelta)
{
    m_fWave += fTimeDelta;

}

void CSkill_Slot_Flag::Late_Update(_float fTimeDelta)
{
    if (m_isVisible)
        CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CSkill_Slot_Flag::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
        return E_FAIL;

    if (FAILED(m_pMaskTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", 7)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fValue", &m_fWave, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(16);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSkill_Slot_Flag::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    CHECK_FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg), E_FAIL);

    m_vColor = { 0.54f, 0.72f, 0.88f, 0.4f };
    return S_OK;
}

HRESULT CSkill_Slot_Flag::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Flag"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_FX_Mask"),
        TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CSkill_Slot_Flag* CSkill_Slot_Flag::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CSkill_Slot_Flag* pInstance = new CSkill_Slot_Flag(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_Slot_Flag"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_Slot_Flag::Clone(void* pArg)
{
    CSkill_Slot_Flag* pInstance = new CSkill_Slot_Flag(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_Slot_Flag"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_Slot_Flag::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pMaskTextureCom);
}
