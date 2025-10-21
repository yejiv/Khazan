#include "UI_QuickSlot_Skill.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_QuickSlot_Skill::CUI_QuickSlot_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Slot{pDevice, pContext}
{
}

CUI_QuickSlot_Skill::CUI_QuickSlot_Skill(const CUI_QuickSlot_Skill& Prototype)
    : CUI_Slot( Prototype )
{
}

HRESULT CUI_QuickSlot_Skill::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    return S_OK;
}

HRESULT CUI_QuickSlot_Skill::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUI_QuickSlot_Skill::Priority_Update(_float fTimeDelta)
{
}

void CUI_QuickSlot_Skill::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_4))
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM);
    }
    if (m_pGameInstance->Key_Down(DIK_5))
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::DISABLE);
    }
    if (m_pGameInstance->Key_Down(DIK_6))
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::ENABLE);
    }

}

void CUI_QuickSlot_Skill::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
    if (m_iState == ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM))
    {
        m_vColor.w = 0.5f;
    }
    else if (m_iState == ENUM_CLASS(QUICKITMESLOTSTATE::DISABLE))
    {
        m_vColor.w = 1.f;
    }
    else if (m_iState == ENUM_CLASS(QUICKITMESLOTSTATE::ENABLE))
    {
        m_vColor.w = 1.f;
    }
}

HRESULT CUI_QuickSlot_Skill::Render()
{
    return S_OK;
}

HRESULT CUI_QuickSlot_Skill::Ready_Prototype()
{
    return S_OK;
}

CUI_QuickSlot_Skill* CUI_QuickSlot_Skill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CUI_QuickSlot_Skill* pInstance = new CUI_QuickSlot_Skill(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_QuickSlot_Skill"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_QuickSlot_Skill::Clone(void* pArg)
{
    CUI_QuickSlot_Skill* pInstance = new CUI_QuickSlot_Skill(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_QuickSlot_Skill"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_QuickSlot_Skill::Free()
{
    __super::Free();
}
