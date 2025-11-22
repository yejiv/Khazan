#include "Amount_Info.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"

CAmount_Info::CAmount_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CAmount_Info::CAmount_Info(const CAmount_Info& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CAmount_Info::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAmount_Info::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Componet(), E_FAIL);
    return S_OK;
}

void CAmount_Info::Priority_Update(_float fTimeDelta)
{
    m_IsUpdate = false;
}

void CAmount_Info::Update(_float fTimeDelta)
{
}

void CAmount_Info::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    __super::Late_Update(fTimeDelta);

}

HRESULT CAmount_Info::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(m_iShaderPass);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CAmount_Info::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);

    for (auto Child : m_Children)
    {
        string strName = Child->Get_Name();

        if (strName == "Line_0" || strName == "Line_1" || strName == "Line_2"
            || strName == "Line_3" || strName == "Line_4" || strName == "Line_5")
        {
            Child->Set_Color({ 1.f, 1.f, 1.f, 0.8f });
        }
        else if (strName == "Cul_Icon" || strName == "Get_Icon")
        {
            if (m_szName == "Gold_Info")
               static_cast<CUI_Atlas_Icon*>(Child)->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Item_Material_GraveStuff_UI.png",2), 2);
            else
                static_cast<CUI_Atlas_Icon*>(Child)->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Item_Potion_HP_1.png", 2), 2);

        }
        else if (strName == "Cul_Value")
        {
            m_pCulValue = static_cast<CUI_TextBox*>(Child);
            Safe_AddRef(m_pCulValue);
        }
        else if (strName == "Get_Value")
        {
            m_pGetValue = static_cast<CUI_TextBox*>(Child);
            Safe_AddRef(m_pGetValue);
        }
      
    }

    if (m_szName == "Gold_Info")
        m_pValue = &CClientInstance::GetInstance()->Get_PlayerData().iGold;
    else
        m_pValue = &CClientInstance::GetInstance()->Get_PlayerData().iLachryma;

    return S_OK;
}

HRESULT CAmount_Info::Update_Switch(void* pArg)
{
    m_IsUpdate = true;
    AMOUNTINFO_DESC* pDesc = static_cast<AMOUNTINFO_DESC*>(pArg);

    m_vLocalPos = pDesc->iOffsetPos;
    Update_Transform(nullptr, m_vLocalPos);

    m_pGetValue->Set_Text(IntToWstring(pDesc->iGetValue));
    m_pCulValue->Set_Text(IntToWstring(*m_pValue));
    return S_OK;
}

HRESULT CAmount_Info::Ready_Componet()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UI_BackGround"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

_wstring CAmount_Info::IntToWstring(_int iValue)
{
    _wstring wstrValue = {};
    if (iValue > 999)
    {
        _int iTemp = iValue * 0.001;
        wstrValue = to_wstring(iTemp);
        wstrValue += TEXT(",");
        iTemp = iValue - (1000 * iTemp);
        if (iTemp <= 0)
            wstrValue += TEXT("000");
        else
            wstrValue += to_wstring(iTemp);
    }
    else
    {
        wstrValue = to_wstring(iValue);
    }
    return wstrValue;
}

CAmount_Info* CAmount_Info::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAmount_Info* pInstance = new CAmount_Info(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CAmount_Info"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CAmount_Info::Clone(void* pArg)
{
    CAmount_Info* pInstance = new CAmount_Info(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CAmount_Info"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CAmount_Info::Free()
{
    __super::Free();

    Safe_Release(m_pCulValue);
    Safe_Release(m_pGetValue);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
