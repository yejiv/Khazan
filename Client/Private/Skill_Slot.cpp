#include "Skill_Slot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"
#include "Skill_Info.h"
#include "Skill_QuickSlot.h"

#include "UI_Slot_Over_Fx.h"
#include "UI_Slot_Selete_Fx.h"
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
    if ((_int)CClientInstance::GetInstance()->Get_PlayerData().iSkillLevel >= m_iLevel)
        m_isLock = false;
    else
        m_isLock = true;
    
    if (m_isLock)
    {
        m_pIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Skill_Lock_UI.png", 4), 4);
        m_pIcon->Update_Scaling(1.5f);
        m_pIcon->Set_Color({ 1.f,1.f,1.f,0.8f });
        m_pLine->Set_Color({ 1.f,1.f,1.f,0.8f });
       
        if (m_pSkilData->iMaxPoint >= 2)
        {
            m_pPointBG->Update_Visible(false);
            m_pSkillPointText->Update_Visible(false);
        }
    }
    else
    {
        m_pIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV(WStringToAnsi(m_pSkilData->wstrIcon), m_pSkilData->iTexPass), m_pSkilData->iTexPass);
        m_pIcon->Update_Scaling(1.f);
        if (m_pSkilData->iMaxPoint >= 2)
        {
            m_pPointBG->Update_Visible(true);
            m_pSkillPointText->Update_Visible(true);
        }
    }
}

void CSkill_Slot::On_PreSkill(const EVENT_SKILL_ON& e)
{
    if (m_iPreSkillIndex == e.SkillIndex)
    {
        m_isPreSkillOn = e.isUp;
        if (!e.isUp && m_iSkillPoint >= 1)
        {
            CClientInstance::GetInstance()->Add_SkillPoint(m_iSkillPoint);
            m_pGameInstance->Emit_Event< EVENT_SKILL_ON>(ENUM_CLASS(EVENT_TYPE::PreSKILL_On), { false, m_iSkillIndex });
            m_iSkillPoint = 0;
        }
    }
}

void CSkill_Slot::Render_SkillInfo()
{
    CSkill_Info::SKILLINFO_DESC Desc = {};
    Desc.iSkillIndex = m_iSkillIndex;

    if (m_pSkilData->iType < 2)
    {
        switch (m_pSkilData->iSubID)
        {
        case 0: Desc.iOffsetPos = { 720.f, 595.f }; break;
        case 1: Desc.iOffsetPos = { 1020.f, 595.f }; break;
        case 2: Desc.iOffsetPos = { 1320.f, 595.f }; break;
        case 3: Desc.iOffsetPos = { 920.f, 595.f }; break;
        case 4: Desc.iOffsetPos = { 1220.f, 595.f }; break;
        }
    }
    else
    {
        switch (m_pSkilData->iSubID)
        {
        case 0: Desc.iOffsetPos = { 900.f, 595.f }; break;
        case 1: Desc.iOffsetPos = { 1200.f, 595.f }; break;
        case 2: Desc.iOffsetPos = { 760.f, 595.f }; break;
        case 3: Desc.iOffsetPos = { 1060.f, 595.f }; break;
        }
    }
 
    Desc.isEquip = false;
    m_iSkillPoint > 0 ? Desc.isGet = true : Desc.isGet = false;
    m_iSkillPoint >= m_iMaxCount ? Desc.isMaxLevel = true : Desc.isMaxLevel = false;
    m_isPreSkillOn ? Desc.isOnPreSkill = true : Desc.isOnPreSkill = false;

    CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("SkillInfo"), &Desc);
}

void CSkill_Slot::Reset_Slot()
{

    CClientInstance::GetInstance()->Add_SkillPoint(m_iSkillPoint);
    m_iSkillPoint = 0;
    m_pSlot_Selete->Update_Visible(false);
    
    m_pGameInstance->Emit_Event<EVENT_SKILL_ON>(ENUM_CLASS(EVENT_TYPE::PreSKILL_On), { false, m_iSkillIndex });
    
    if (m_pSkilData->iType == 0)
    {
        CClientInstance::GetInstance()->Lock_Skill(1 << m_pSkilData->iIndex);
        static_cast<CSkill_QuickSlot*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("SkillSlot_Quick")))->Equip_Check(m_iSkillIndex);
    }
        
}

void CSkill_Slot::Setting_Data(_int iSkillIndex, _int SKillPoint)
{
    if (m_iSkillIndex != iSkillIndex)
        return;

    m_iSkillPoint = SKillPoint;

    m_pSlot_Selete->Update_Visible(true);
    m_pGameInstance->Emit_Event< EVENT_SKILL_ON>(ENUM_CLASS(EVENT_TYPE::PreSKILL_On), { true, m_iSkillIndex });
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

    m_pGameInstance->Subscribe_Event<EVENT_SKILL_ON>(ENUM_CLASS(EVENT_TYPE::PreSKILL_On), [&](const EVENT_SKILL_ON& e)
        { On_PreSkill(e); });
    
    m_pGameInstance->Subscribe_Event<EVENT_SKILL_RESET>(ENUM_CLASS(EVENT_TYPE::SKILL_RESET), [&](const EVENT_SKILL_RESET& e)
        { Reset_Slot(); });

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

    if (IsPick(g_hWnd) && m_pGameInstance->Get_InputType() == INPUT_TYPE::UI)
    {
        if (!m_isOver)
        {
            m_isOver = true;
            _int iRand = m_pGameInstance->Rand(1, 4);
            _wstring wstrSound = TEXT("UI_common_mouse_over_0") + std::to_wstring(iRand) + TEXT(" (SFX).wav");
            m_pGameInstance->PlaySoundOnce(wstrSound.c_str());
        }
        m_pSlot_Over->Anim_On();
        if(m_pGameInstance->Get_InputType() == INPUT_TYPE::UI)
            Render_SkillInfo();

        if (CClientInstance::GetInstance()->Get_PlayerData().iSkilPoint > 0 && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB, INPUT_TYPE::UI))
        {
            m_pGameInstance->StopByKey(TEXT("UI_common_click2 (SFX).wav"));
            m_pGameInstance->PlaySoundOnce(TEXT("UI_common_click2 (SFX).wav"));

            if (m_iPreSkillIndex == 0 || m_isPreSkillOn)
            {
                if (m_iSkillPoint + 1 <= m_pSkilData->iMaxPoint)
                {
                    m_pGameInstance->StopByKey(TEXT("UI_common_click2 (SFX).wav"));
                    m_pGameInstance->PlaySoundOnce(TEXT("UI_common_click2 (SFX).wav"));
                    ++m_iSkillPoint;
                    CClientInstance::GetInstance()->Add_SkillPoint(-1);
                    if (m_iSkillPoint == 1)
                    {
                        m_pSlot_Selete->Update_Visible(true);

                        m_pGameInstance->Emit_Event< EVENT_SKILL_ON>(ENUM_CLASS(EVENT_TYPE::PreSKILL_On), { true, m_iSkillIndex });
                        if (m_pSkilData->iSkillType == 3)
                        {
                            CSkill_QuickSlot::SKILLQUICK_DESC Desc = {};
                            Desc.iSkillIndex = m_iSkillIndex;
                            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("SkillSlot_Quick"),&Desc);
                        }
                        if (m_pSkilData->iType == 0)
                            CClientInstance::GetInstance()->Unlock_Skill(1 << m_pSkilData->iIndex);
                    
                    }
                }
            }
        }
        else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB, INPUT_TYPE::UI))
        {


            if (m_iSkillPoint - 1 >= 0)
            {
                m_pGameInstance->StopByKey(TEXT("UI_skilllevel_countup_01_01 (SFX).wav"));
                m_pGameInstance->PlaySoundOnce(TEXT("UI_skilllevel_countup_01_01 (SFX).wav"));
                --m_iSkillPoint;
                CClientInstance::GetInstance()->Add_SkillPoint(1);
                if (m_iSkillPoint == 0)
                {
                    m_pSlot_Selete->Update_Visible(false);

                    m_pGameInstance->Emit_Event< EVENT_SKILL_ON>(ENUM_CLASS(EVENT_TYPE::PreSKILL_On), { false, m_iSkillIndex });

                    if (m_pSkilData->iType == 0)
                    {
                        CClientInstance::GetInstance()->Lock_Skill(1 << m_pSkilData->iIndex);
                        static_cast<CSkill_QuickSlot*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("SkillSlot_Quick")))->Equip_Check(m_iSkillIndex);
                    }
                }
            }
        }

    }
    else
    {
        m_isOver = false;
    }

    if (m_iSkillPoint <= 0)
    {
        m_pIcon->Set_Color({ 0.898f, 0.965f, 1.f,0.6f });
        m_pLine->Set_Color({ 1.f,1.f,1.f,0.6f });

        for (auto pLine : m_pPreSkillLine)
        {
            pLine->Set_Color({ 0.f,0.f,0.f,0.6f });
            pLine->Set_Depth(m_fDepth + 1.f);
        }
    }
    else
    {
        if (m_pSkilData->iSkillType == 1)
            m_pIcon->Set_Color({ 0.898f, 0.965f, 1.f, 1.0f });
        else
            m_pIcon->Set_Color({ 0.929f, 0.741f, 0.376f,1.f });
        m_pLine->Set_Color({ 1.f,1.f,1.f,1.f });

        for (auto pLine : m_pPreSkillLine)
        {
            pLine->Set_Color({ 1.f,1.f,1.f,1.f });
            pLine->Set_Depth(m_fDepth + 0.9f);
        }
    }

    if (m_pSkilData->iMaxPoint >= 2)
    {
        _wstring wstrPoint = to_wstring(m_iSkillPoint) + m_wstrMaxCount;
        m_pSkillPointText->Set_Text(wstrPoint);
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
   
    m_iPreSkillIndex = m_pSkilData->iPreSkill;


    if (m_pSkilData->iType == 0 && m_iPreSkillIndex > 0)
    {
        const SKILL_DB* pPreData = CClientInstance::GetInstance()->Get_Data<SKILL_DB>(m_iPreSkillIndex);

        _int iValueY = m_pSkilData->iLevel - pPreData->iLevel;
        _int iValueX = m_pSkilData->iSlotX - pPreData->iSlotX;

        if (iValueX < 0)
        {
            AtlasDesc.fDepth = m_fDepth + 1;
            AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
            AtlasDesc.szName = "PreSkill_Line";
            AtlasDesc.vLocalPos = _float2{ 35.f, -80.f + (-80.f * (iValueY - 1)) };
            AtlasDesc.vLocalSize = { 74.f, 8.f };
            AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_TreeConnect_Bg02_UI.png", 4);
            AtlasDesc.iShaderPass = 2;
            AtlasDesc.iTexPass = 4;
            AtlasDesc.vColor = { 0.f,0.f,0.f,1.f };
            CUI_Atlas_Icon* pAtlas = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));
            pAtlas->Update_Rotation(-90.f);
            m_pPreSkillLine.push_back(pAtlas);
            m_Children.push_back(pAtlas);
            Safe_AddRef(pAtlas);
        }
        else if(iValueX > 0)
        {
            AtlasDesc.fDepth = m_fDepth + 1;
            AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
            AtlasDesc.szName = "PreSkill_Line";
            AtlasDesc.vLocalPos = _float2{ -35.f, -80.f + (-80.f * (iValueY - 1)) };
            AtlasDesc.vLocalSize = { 74.f, 8.f };
            AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_TreeConnect_Bg02_UI.png", 4);
            AtlasDesc.iShaderPass = 2;
            AtlasDesc.iTexPass = 4;
            AtlasDesc.vColor = { 0.f,0.f,0.f,1.f };
            CUI_Atlas_Icon* pAtlas = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));
            pAtlas->Update_Rotation(-90.f);
            m_pPreSkillLine.push_back(pAtlas);
            m_Children.push_back(pAtlas);
            Safe_AddRef(pAtlas);
        }

        for (; iValueY > 0; --iValueY)
        {
            AtlasDesc.fDepth = m_fDepth + 1;
            AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
            AtlasDesc.szName = "PreSkill_Line";
            AtlasDesc.vLocalPos = _float2{ 0.f, -40.f + (-80.f * (iValueY - 1)) };
            AtlasDesc.vLocalSize = { 8.f, 84.f };
            AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_TreeConnect_Bg02_UI.png", 4);
            AtlasDesc.iShaderPass = 2;
            AtlasDesc.iTexPass = 4;
            AtlasDesc.vColor = { 0.f,0.f,0.f,0.8f };
            CUI_Atlas_Icon* pAtlas = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));
            m_pPreSkillLine.push_back(pAtlas);
            m_Children.push_back(pAtlas);
            Safe_AddRef(pAtlas);
        }

 
    }
    else if (m_pSkilData->iType != 0 && m_iPreSkillIndex > 0)
    {
        const SKILL_DB* pPreData = CClientInstance::GetInstance()->Get_Data<SKILL_DB>(m_iPreSkillIndex);

        _int iValueY = m_pSkilData->iLevel - pPreData->iLevel;
        _int iValueX = m_pSkilData->iSlotX - pPreData->iSlotX;

      
        for (; iValueY > 0; --iValueY)
        {
            AtlasDesc.fDepth = m_fDepth + 1;
            AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
            AtlasDesc.szName = "PreSkill_Line";
            if(iValueX < 0)
                AtlasDesc.vLocalPos = _float2{ 70.f, -40.f + (-80.f * (iValueY - 1)) };
            else if (iValueX > 0)
                AtlasDesc.vLocalPos = _float2{ -70.f, -40.f + (-80.f * (iValueY - 1)) };
            else
                AtlasDesc.vLocalPos = _float2{ 0.f, -40.f + (-80.f * (iValueY - 1)) };

            AtlasDesc.vLocalSize = { 8.f, 84.f };
            AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_TreeConnect_Bg02_UI.png", 4);
            AtlasDesc.iShaderPass = 2;
            AtlasDesc.iTexPass = 4;
            AtlasDesc.vColor = { 0.f,0.f,0.f,0.8f };
            CUI_Atlas_Icon* pAtlas = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));
            m_pPreSkillLine.push_back(pAtlas);
            m_Children.push_back(pAtlas);
            Safe_AddRef(pAtlas);
        }

        if (iValueX < 0)
        {
            AtlasDesc.fDepth = m_fDepth + 1;
            AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
            AtlasDesc.szName = "PreSkill_Line";
            AtlasDesc.vLocalPos = _float2{ 35.f, -80.f + (-80.f * (iValueY - 1)) };
            AtlasDesc.vLocalSize = { 74.f, 8.f };
            AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_TreeConnect_Bg02_UI.png", 4);
            AtlasDesc.iShaderPass = 2;
            AtlasDesc.iTexPass = 4;
            AtlasDesc.vColor = { 0.f,0.f,0.f,1.f };
            CUI_Atlas_Icon* pAtlas = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));
            pAtlas->Update_Rotation(-90.f);
            m_pPreSkillLine.push_back(pAtlas);
            m_Children.push_back(pAtlas);
            Safe_AddRef(pAtlas);
        }
        else if (iValueX > 0)
        {
            AtlasDesc.fDepth = m_fDepth + 1;
            AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
            AtlasDesc.szName = "PreSkill_Line";
            AtlasDesc.vLocalPos = _float2{ -35.f, -80.f + (-80.f * (iValueY - 1)) };
            AtlasDesc.vLocalSize = { 74.f, 8.f };
            AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_TreeConnect_Bg02_UI.png", 4);
            AtlasDesc.iShaderPass = 2;
            AtlasDesc.iTexPass = 4;
            AtlasDesc.vColor = { 0.f,0.f,0.f,1.f };
            CUI_Atlas_Icon* pAtlas = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));
            pAtlas->Update_Rotation(-90.f);
            m_pPreSkillLine.push_back(pAtlas);
            m_Children.push_back(pAtlas);
            Safe_AddRef(pAtlas);
        }
    }

    AtlasDesc.fDepth = m_fDepth - 0.1f;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "PreSkill_Line";
    AtlasDesc.vLocalPos = _float2{ 0.f, 28.f };
    AtlasDesc.vLocalSize = { 70.f, 24.f };
    AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_SlotLevelInfo_Bg.png", 4);
    AtlasDesc.iShaderPass = 2;
    AtlasDesc.iTexPass = 4;
    AtlasDesc.vColor = { 1.f,1.f,1.f,1.f };
    m_pPointBG = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));
    m_Children.push_back(m_pPointBG);
    Safe_AddRef(m_pPointBG);



    CUIObject::UIOBJECT_DESC TextDesc = {};
    TextDesc.fDepth = m_fDepth - 1.1f;
    TextDesc.iUIType = ENUM_CLASS(UITYPE::TEXT);
    TextDesc.szName = "Skill_Count";
    TextDesc.vLocalPos = _float2{ 0.f, 0.f };
    TextDesc.vLocalSize = { m_vLocalSize.x, m_vLocalSize.y };
    TextDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pSkillPointText = static_cast<CUI_TextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_TextBox"), &TextDesc));

    if (m_pSkillPointText == nullptr)
        return E_FAIL;

    CUI_TextBox::TEXTBOX_DESC TextSet = {};
    TextSet.bIsTextBox = false;
    TextSet.eTextAlign = TEXT_ALIGN::RIGHT_CENTER;
    TextSet.fMaxWidth = 0;
    TextSet.fOffsetHeight = 0;
    TextSet.iPivotX = 30;
    TextSet.iPivotY = 42;
    TextSet.wstrTexttag = TEXT("Blade_Medium_18");
    TextSet.wstrText = TEXT("1 / 3");
    TextSet.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pSkillPointText->Setting_Text(TextSet);
    m_Children.push_back(m_pSkillPointText);
    Safe_AddRef(m_pSkillPointText);

    m_iMaxCount = m_pSkilData->iMaxPoint;
    m_wstrMaxCount = TEXT(" / ") + to_wstring(m_pSkilData->iMaxPoint);
    if (m_pSkilData->iMaxPoint >= 2)
    {
        m_pPointBG->Update_Visible(true);
        m_pSkillPointText->Update_Visible(true);
    }
    else
    {
        m_pPointBG->Update_Visible(false);
        m_pSkillPointText->Update_Visible(false);
    }
    Update_Transform(this, m_vWorldPos);


    CUIObject::UIOBJECT_DESC FXDesc = {};
    FXDesc.fDepth = m_fDepth + 1.f;
    FXDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    FXDesc.szName = "Over";
    FXDesc.vLocalPos = { 0.f, 0.f };
    FXDesc.vLocalSize = { 64.f, 64.f };

    m_pSlot_Over = static_cast<CUI_Slot_Over_Fx*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Slot_Over_Fx"), &FXDesc));
    
    if (m_pSlot_Over == nullptr)
        return E_FAIL;

    m_Children.push_back(m_pSlot_Over);
    Safe_AddRef(m_pSlot_Over);
    
    FXDesc.fDepth = m_fDepth + 1.1f;
    FXDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    FXDesc.szName = "Selete";
    FXDesc.vLocalPos = { 0.f, 0.f };
    FXDesc.vLocalSize = { 90.f, 90.f };

    m_pSlot_Selete = static_cast<CUI_Slot_Selete_Fx*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Slot_Selete_Fx"), &FXDesc));

    if (m_pSlot_Selete == nullptr)
        return E_FAIL;

    m_Children.push_back(m_pSlot_Selete);
    Safe_AddRef(m_pSlot_Selete);

    m_pSlot_Selete->Update_Visible(false);
    
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

    Safe_Release(m_pPointBG);
    Safe_Release(m_pSkillPointText);
    Safe_Release(m_pSlot_Over);
    Safe_Release(m_pSlot_Selete);
    m_pSkilData = nullptr;
}   