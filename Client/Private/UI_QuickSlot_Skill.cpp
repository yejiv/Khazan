#include "UI_QuickSlot_Skill.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "UI_Atlas_Icon.h"

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
    m_pGameInstance->Subscribe_Event<EVENT_SKILL_SLOT>(ENUM_CLASS(EVENT_TYPE::SKILL_QUICKSLOT), [&](const EVENT_SKILL_SLOT& e) {Skill_Equip(e); });
    
    m_pGauge = &CClientInstance::GetInstance()->Get_PlayerData().fCulDoggedness;
    return S_OK;
}

void CUI_QuickSlot_Skill::Priority_Update(_float fTimeDelta)
{
}

void CUI_QuickSlot_Skill::Update(_float fTimeDelta)
{

}

void CUI_QuickSlot_Skill::Late_Update(_float fTimeDelta)
{
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
        if(*m_pGauge >= 1.f)
            m_pIcon->Set_Color({ 0.929f, 0.741f, 0.376f,1.f });
        else
            m_pIcon->Set_Color({ 1.f, 1.f, 1.f,1.f });

        m_pIcon->Late_Update(fTimeDelta);
    }
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);


}

HRESULT CUI_QuickSlot_Skill::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    CHECK_FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg), E_FAIL);
    FAILED_CHECK_RETURN(Ready_Children(), E_FAIL);

    if (m_szName == "HUD_Skill_Slot_0")
        m_iIndex = 0;
    if (m_szName == "HUD_Skill_Slot_1")
        m_iIndex = 1;
    if (m_szName == "HUD_Skill_Slot_2")
        m_iIndex = 2;
    if (m_szName == "HUD_Skill_Slot_3")
        m_iIndex = 3;
    if (m_szName == "HUD_Skill_Slot_4")
        m_iIndex = 4;
    if (m_szName == "HUD_Skill_Slot_5")
        m_iIndex = 5;


    return S_OK;
}

HRESULT CUI_QuickSlot_Skill::Ready_Children()
{
    CUI_Atlas_Icon::UIATLASICON_DESC AtlasDesc;

    AtlasDesc.fDepth = 7;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Icon";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    if (m_szName == "HUD_Item_Slot_0")
        AtlasDesc.vLocalSize = { m_vLocalSize.x * 0.9f, m_vLocalSize.y * 0.9f };
    else
        AtlasDesc.vLocalSize = { m_vLocalSize.x * 0.7f, m_vLocalSize.y * 0.7f };

    AtlasDesc.vUV = { 0.f, 0.f, 1.f, 1.f };
    AtlasDesc.iShaderPass = 2;
    AtlasDesc.iTexPass = 2;
    AtlasDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pIcon = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pIcon == nullptr)
        return E_FAIL;

    m_Children.push_back(m_pIcon);
    Safe_AddRef(m_pIcon);

    return S_OK;
}

void CUI_QuickSlot_Skill::Skill_Equip(const EVENT_SKILL_SLOT& e)
{
    if (e.iIndex != m_iIndex)
        return;
    
    if (e.isEquip)
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::ENABLE);
        m_iSkillIndex = e.iSkillIndex;

        SKILL_DB SkillDB = *CClientInstance::GetInstance()->Get_Data<SKILL_DB>(m_iSkillIndex);
        string strSkillIcon = WStringToAnsi(SkillDB.wstrIcon);
        _float4 vUV = CClientInstance::GetInstance()->Get_AtlasUV(strSkillIcon, SkillDB.iTexPass);

        m_pIcon->Set_Texture(vUV, SkillDB.iTexPass);
        m_pIcon->Set_Color({ 0.929f, 0.741f, 0.376f,1.f });
    }
    else
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM);
        m_iSkillIndex = -1;
    }
}

void CUI_QuickSlot_Skill::Update_State()
{

    if (m_iSkillIndex < 0 && m_iIndex != 0)
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM);
        m_vColor.w = 0.5f;
    }
    else
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::ENABLE);
        m_vColor.w = 1.f;
        m_pIcon->Set_Color(_float4{ 1.f, 1.f, 1.f, 1.f });
    }
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
    Safe_Release(m_pIcon);
}
