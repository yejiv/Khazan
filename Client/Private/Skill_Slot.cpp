#include "Skill_Slot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"

CSkill_Slot::CSkill_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Slot{ pDevice , pContext}
{
}

CSkill_Slot::CSkill_Slot(const CSkill_Slot& Prototype)
    :  CUI_Slot(Prototype)
{
}

void CSkill_Slot::Setting_Skill()
{
    if (CClientInstance::GetInstance()->Get_PlayerData().iSkillLevel >= m_iLevel)
        m_isLock = false;
    else
        m_isLock = true;
    
    if (m_isLock)
    {
        m_pIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Skill_Lock_UI.png", 4), 4);
        m_pIcon->Update_Scaling(1.5f);
        m_pIcon->Set_Color({ 1.f,1.f,1.f,0.8f });
        m_pLine->Set_Color({ 1.f,1.f,1.f,0.8f });
        m_vColor = { 1.f,1.f,1.f, 0.8f };
    }
    else
    {
        m_pIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV(WStringToAnsi(m_pSkilData->wstrIcon), m_pSkilData->iTexPass), m_pSkilData->iTexPass);
        m_pIcon->Update_Scaling(1.f);
    }
}

HRESULT CSkill_Slot::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
	return S_OK;
}

HRESULT CSkill_Slot::Initialize_Clone(void* pArg)
{
    UISKILLSLOT_DESC* pDesc = static_cast<UISKILLSLOT_DESC*>(pArg);

    m_iSkillIndex = pDesc->iSkillIndex;

    m_pSkilData = CClientInstance::GetInstance()->Get_Data<SKILL_DB>(m_iSkillIndex);

    if (m_pSkilData->iSkillType == 1)
    {
        m_vUV.push_back(CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_BgBlack03_UI.png", 4));
        m_vUV.push_back(CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_BgBlack03_UI.png", 4));
    }
    else if (m_pSkilData->iSkillType == 2)
    {
        m_vUV.push_back(CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_BgBlack_UI.png", 4));
        m_vUV.push_back(CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_BgBlack_UI.png", 4));
    }
    else if (m_pSkilData->iSkillType == 3)
    {
        m_vUV.push_back(CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_BgBlack02_UI.png", 4));
        m_vUV.push_back(CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_BgBlack02_UI.png", 4));
    }
    m_iTexPass = 4;
    m_iShaderPass = 1;
    m_vColor = { 1.f, 1.f,1.f,1.f };

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Child(m_pSkilData), E_FAIL);
    m_iState = ENUM_CLASS(UISTATE::ENABLE);

    Update_Transform(this, m_vWorldPos);

    switch (m_pSkilData->iLevel)
    {
    case 0: m_iLevel = 0; break;
    case 1: m_iLevel = 2; break;
    case 2: m_iLevel = 10; break;
    case 3: m_iLevel = 18; break;
    case 4: m_iLevel = 26; break;
    case 5: m_iLevel = 34; break;
    }

    m_pGameInstance->Subscribe_Event<EVENT_SKILL_OPEN>(ENUM_CLASS(EVENT_TYPE::SKILL_EVENT), [&](const EVENT_SKILL_OPEN& e)
        { Setting_Skill();});

	return S_OK;
}

void CSkill_Slot::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CSkill_Slot::Update(_float fTimeDelta)
{
    if (m_isLock)
        return;

    if (IsPick(g_hWnd))
    {
        if (CClientInstance::GetInstance()->Get_PlayerData().iSkilPoint > 0 && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB, INPUT_TYPE::UI))
        {
            if (m_iSkillPoint + 1 <= m_pSkilData->iMaxPoint)
            {
                ++m_iSkillPoint;
                CClientInstance::GetInstance()->Add_SkillPoint(-1);
            }
        }
        else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB, INPUT_TYPE::UI))
        {
            if (m_iSkillPoint - 1 >= 0)
            {
                --m_iSkillPoint;
                CClientInstance::GetInstance()->Add_SkillPoint(1);
            }
        }

    }

    if (m_iSkillPoint <= 0)
    {
        m_pIcon->Set_Color({ 1.f,1.f,1.f,0.6f });
        m_pLine->Set_Color({ 1.f,1.f,1.f,0.6f });
        m_vColor = { 1.f,1.f,1.f, 0.6f };
    }
    else
    {
        if (m_pSkilData->iSkillType == 1)
            m_pIcon->Set_Color({ 0.898f, 0.965f, 1.f,0.8f });
        else
            m_pIcon->Set_Color({ 0.929f, 0.741f, 0.376f,1.f });
        m_pLine->Set_Color({ 1.f,1.f,1.f,1.f });
        m_vColor = { 1.f,1.f,1.f, 1.f };
    }
    __super::Update(fTimeDelta);
}

void CSkill_Slot::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
    __super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Slot::Ready_Child(const SKILL_DB* pData)
{
    CUI_Atlas_Icon::UIATLASICON_DESC AtlasDesc;

    AtlasDesc.fDepth = m_fDepth - 1;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Line";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    AtlasDesc.vLocalSize = { 64.f, 64.f };

    if (pData->iSkillType == 1)
        AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_Bg_Deco02_UI.png", 4);
    else if (pData->iSkillType == 2)
        AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_Bg_Deco_UI.png", 4);
    else if (pData->iSkillType == 3)
        AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_Bg_SpecialLine_UI.png", 4);

    AtlasDesc.iShaderPass = 1;
    AtlasDesc.iTexPass = 4;
    AtlasDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pLine = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pLine == nullptr)
        return E_FAIL;
    m_Children.push_back(m_pLine);
    Safe_AddRef(m_pLine);

    AtlasDesc.iShaderPass = 2;
    AtlasDesc.szName = "Item_Icon";
    AtlasDesc.vLocalSize = { 44.f, 44.f };
    AtlasDesc.iTexPass = pData->iTexPass;
    AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV(WStringToAnsi(pData->wstrIcon), pData->iTexPass);

    m_pIcon = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pIcon == nullptr)
        return E_FAIL;
    m_Children.push_back(m_pIcon);
    Safe_AddRef(m_pIcon);
   
    _int iPreIndex = m_pSkilData->iPreSkill;

    if (m_pSkilData->iType == 0 && iPreIndex > 0)
    {
        const SKILL_DB* pPreData = CClientInstance::GetInstance()->Get_Data<SKILL_DB>(iPreIndex);

        _int iValueY = m_pSkilData->iLevel - pPreData->iLevel;
        //_int iValueX = m_pSkilData->iSlotX - pPreData->iSlotX;

        for (; iValueY > 0; --iValueY)
        {
            AtlasDesc.fDepth = m_fDepth + 1;
            AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
            AtlasDesc.szName = "PreSkill_Line";
            AtlasDesc.vLocalPos = _float2{ 0.f,32 + (-32.f * iValueY) };
            AtlasDesc.vLocalSize = { 4.f, 32.f };
            AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Common_ListScroll_Con_UI.png", 4);
            AtlasDesc.iShaderPass = 1;
            AtlasDesc.iTexPass = 4;
            AtlasDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
            CUI_Atlas_Icon* pAtlas = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));
            m_pPreSkillLine.push_back(pAtlas);
            m_Children.push_back(pAtlas);
            Safe_AddRef(pAtlas);
        }
    }
    return S_OK;
}

CSkill_Slot* CSkill_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CSkill_Slot* pInstance = new CSkill_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_Slot::Clone(void* pArg)
{
    CSkill_Slot* pInstance = new CSkill_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_Slot::Free()
{
	__super::Free();
    Safe_Release(m_pLine);
    Safe_Release(m_pIcon);
    
    for (auto pLine : m_pPreSkillLine)
        Safe_Release(pLine);
    m_pPreSkillLine.clear();
}   