#include "Item_Slot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"

CItem_Slot::CItem_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Slot{ pDevice, pContext }
{
}

CItem_Slot::CItem_Slot(const CItem_Slot& Prototype)
    : CUI_Slot(Prototype)
{
}

void CItem_Slot::Input_Slot()
{

}

HRESULT CItem_Slot::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    return S_OK;
}

HRESULT CItem_Slot::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM);
    return S_OK;
}

void CItem_Slot::Priority_Update(_float fTimeDelta)
{
}

void CItem_Slot::Update(_float fTimeDelta)
{

}

void CItem_Slot::Late_Update(_float fTimeDelta)
{

}

HRESULT CItem_Slot::Render()
{
    return S_OK;
}

HRESULT CItem_Slot::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    FAILED_CHECK_RETURN(__super::Load_UI(pInData, iPrototypeLevelID, pArg), E_FAIL);
    FAILED_CHECK_RETURN(Ready_Childer(), E_FAIL);

    return S_OK;
}

HRESULT CItem_Slot::Ready_Prototype()
{
    return S_OK;
}

HRESULT CItem_Slot::Ready_Childer()
{
    CUI_Atlas_Icon* pObject = { nullptr };
    CUI_Atlas_Icon::UIATLASICON_DESC Desc;
    Desc.fDepth = m_fDepth;
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "Quick_Slot_Item_Disable";
    Desc.vLocalPos = _float2{ 0.f, 0.f };
    Desc.vLocalSize = m_vLocalSize;

    Desc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Hud_Skill_Bg_Circle_Disable.png", 0);
    Desc.iShaderPass = 2;
    Desc.iTexPass = 0;
    Desc.vColor = { 1.f, 0.15f, 0.15f, 0.5f };
    //m_vFxColor = { 1.f, 0.15f, 0.15f, 0.5f };
    pObject = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &Desc));

    if (pObject == nullptr)
        return E_FAIL;
    
    m_Children.push_back(pObject);

    return S_OK;
}

CItem_Slot* CItem_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CItem_Slot* pInstance = new CItem_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CItem_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CItem_Slot::Clone(void* pArg)
{
    CItem_Slot* pInstance = new CItem_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CItem_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CItem_Slot::Free()
{
    __super::Free();
}
