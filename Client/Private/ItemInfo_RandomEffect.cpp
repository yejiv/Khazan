#include "ItemInfo_RandomEffect.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"

CItemInfo_RandomEffect::CItemInfo_RandomEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CItemInfo_RandomEffect::CItemInfo_RandomEffect(const CItemInfo_RandomEffect& Prototype)
    : CUI_Panel(Prototype)
{
}

void CItemInfo_RandomEffect::Set_LocalPos(_float2 vPos, CUIObject* pParent)
{
    m_vLocalPos = vPos;
    __super::Update_Transform(pParent, vPos);
}

void CItemInfo_RandomEffect::Set_Text(_int iEffect_Type, _int iValue)
{
    switch (iEffect_Type)
    {
    case 1:
        m_pEffect_Name->Set_Text(TEXT("공격력"));
        m_pEffect_Value->Set_Text(to_wstring(iValue));
        break;
    case 2:
        m_pEffect_Name->Set_Text(TEXT("피해 증가"));
        m_pEffect_Value->Set_Text(to_wstring(iValue)+TEXT("%"));
        break;
    case 3:
        m_pEffect_Name->Set_Text(TEXT("기력 회복 속도"));
        m_pEffect_Value->Set_Text(to_wstring(iValue) + TEXT("%"));
        break;
    case 4:
        m_pEffect_Name->Set_Text(TEXT("방어력"));
        m_pEffect_Value->Set_Text(to_wstring(iValue));
        break;
    case 5:
        m_pEffect_Name->Set_Text(TEXT("최대 기력"));
        m_pEffect_Value->Set_Text(to_wstring(iValue));
        break;
    case 6:
        m_pEffect_Name->Set_Text(TEXT("최대 생명력"));
        m_pEffect_Value->Set_Text(to_wstring(iValue));
        break;
    case 7:
        m_pEffect_Name->Set_Text(TEXT("투지"));
        m_pEffect_Value->Set_Text(to_wstring(iValue));
        break;
    }
}

HRESULT CItemInfo_RandomEffect::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    return S_OK;
}

HRESULT CItemInfo_RandomEffect::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CItemInfo_RandomEffect::Priority_Update(_float fTimeDelta)
{
    m_IsUpdate = false;
}

void CItemInfo_RandomEffect::Update(_float fTimeDelta)
{
}

void CItemInfo_RandomEffect::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

}

HRESULT CItemInfo_RandomEffect::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);

    for (auto Child : m_Children)
    {
        string strName = Child->Get_Name();

        if (strName == "Rendom_Effect_Text")
        {
            m_pEffect_Name = static_cast<CUI_TextBox*>(Child);
            Safe_AddRef(m_pEffect_Name);
        }
        else if (strName == "Rendom_Effect_Value")
        {
            m_pEffect_Value = static_cast<CUI_TextBox*>(Child);
            Safe_AddRef(m_pEffect_Value);
        }
    }


    return S_OK;
}

CItemInfo_RandomEffect* CItemInfo_RandomEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CItemInfo_RandomEffect* pInstance = new CItemInfo_RandomEffect(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CItemInfo_RandomEffect"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CItemInfo_RandomEffect::Clone(void* pArg)
{
    CItemInfo_RandomEffect* pInstance = new CItemInfo_RandomEffect(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CItemInfo_RandomEffect"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CItemInfo_RandomEffect::Free()
{
    __super::Free();
    Safe_Release(m_pEffect_Name);
    Safe_Release(m_pEffect_Value);
}
