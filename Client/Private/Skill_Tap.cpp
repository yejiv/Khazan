#include "Skill_Tap.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "Skill_Slot_Panel.h"
#include "UI_SkillTree.h"
CSkill_Tap::CSkill_Tap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Tap{ pDevice, pContext }
{
}

CSkill_Tap::CSkill_Tap(const CSkill_Tap& Prototype)
	: CUI_Tap(Prototype)
{
}

HRESULT CSkill_Tap::Setting_Panel(_int iTapIndex)
{
	m_iTapIndex = iTapIndex;

	for (_int i = 0; i < m_pPanel.size(); ++i)
	{
		m_pPanel[i]->Setting_Slot(iTapIndex, i);
	}
	return S_OK;
}

void CSkill_Tap::Set_Selete(_bool isSelete)
{
    m_bIsSelete = isSelete;

    if (m_bIsSelete)
    {
        _int iRand = m_pGameInstance->Rand(1, 5);
        _wstring wstrSound = TEXT("UI_category_select_0") + std::to_wstring(iRand) + TEXT(" (SFX).wav");
        m_pGameInstance->PlaySoundOnce(wstrSound.c_str());
    }
}

HRESULT CSkill_Tap::Initialize_Prototype(_int iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT CSkill_Tap::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	return S_OK;
}

void CSkill_Tap::Priority_Update(_float fTimeDelta)
{
	for (auto Childe : m_Children)
		Childe->Priority_Update(fTimeDelta);

	if (m_bIsSelete)
		for (auto pPanel : m_pPanel)
			pPanel->Priority_Update(fTimeDelta);

}

void CSkill_Tap::Update(_float fTimeDelta)
{
	if (!m_bIsSelete)
	{
		if (ButtonClick(g_hWnd, false, true, INPUT_TYPE::UI))
		{
			CUI_SkillTree::SKillBUBBLE_DESC Desc = {};
			Desc.iIndex = m_iTapIndex;
			Bubble_EventCall(&Desc);
		}
		
        
        if (ButtonOver(g_hWnd))
        {
            if (!m_isOver)
            {
                m_isOver = true;
                m_Children[1]->Set_Color({ 1.f, 1.f, 1.f, 1.f });
                m_pGameInstance->StopByKey(TEXT("UI_common_mouse_over_01 (SFX).wav"));
                m_pGameInstance->PlaySoundOnce(TEXT("UI_common_mouse_over_01 (SFX).wav"));
            }
        }
        else
        {
            m_isOver = false;
            m_Children[1]->Set_Color({ 1.f, 1.f, 1.f, 0.5f });
        }
	}
	else
	{
		m_Children[1]->Set_Color({ 0.941f, 0.769f, 0.329f, 1.f });
	}

	for (auto Childe : m_Children)
		Childe->Update(fTimeDelta);

	if (m_bIsSelete)
		for (auto pPanel : m_pPanel)
			pPanel->Update(fTimeDelta);
}

void CSkill_Tap::Late_Update(_float fTimeDelta)
{
	if (!m_bIsSelete)
		m_Children[1]->Late_Update(fTimeDelta);
	else
	{
		m_Children[0]->Late_Update(fTimeDelta);
		m_Children[1]->Late_Update(fTimeDelta);
	}
	if (m_bIsSelete)
		for (auto pPanel : m_pPanel)
			pPanel->Late_Update(fTimeDelta);
}

HRESULT CSkill_Tap::Render()
{
	return S_OK;
}

HRESULT CSkill_Tap::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);
	if(m_szName == "Spear")
		m_iState = ENUM_CLASS(UISTATE::ENABLE);

	CHECK_FAILED(Ready_Children(), E_FAIL);

	return S_OK;
}

void CSkill_Tap::Update_Alpha(_float fAlpha)
{
	for (auto pPanel : m_pPanel)
		pPanel->Update_Alpha(fAlpha);
	
	__super::Update_Alpha(fAlpha);
}

HRESULT CSkill_Tap::Ready_Children()
{
	_bool isPublice = m_szName == "Public";
	_float2 vPos = {};

	isPublice ? vPos = { 505, 540 } : vPos = { 350, 540 };

	for (_int i = 0; i < 5; ++i)
	{
		if (isPublice && i == 4)
			continue;

		CSkill_Slot_Panel* pPanel = static_cast<CSkill_Slot_Panel*>(CClientInstance::GetInstance()->Load_UIObject(m_iLevel, TEXT("../Bin/Resources/UI/UIData/Skill_Slot_Panel.json")));
		if (pPanel == nullptr)
			return E_FAIL;

		m_pPanel.push_back(pPanel);
		pPanel->LocalPos_Set({ vPos.x + i * 310.f, vPos.y }, this);
	}
	return S_OK;
}

CSkill_Tap* CSkill_Tap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
	CSkill_Tap* pInstance = new CSkill_Tap(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CSkill_Tap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkill_Tap::Clone(void* pArg)
{
	CSkill_Tap* pInstance = new CSkill_Tap(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CSkill_Tap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_Tap::Free()
{
	__super::Free();
	for (auto pPanel : m_pPanel)
		Safe_Release(pPanel);
	m_pPanel.clear();
}
