#include "Tutorial_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Guide_Icon.h"
#include "UI_TextBox.h"
CTutorial_Panel::CTutorial_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CTutorial_Panel::CTutorial_Panel(const CTutorial_Panel& Prototype)
	: CUI_Panel(Prototype)
{
}

void CTutorial_Panel::Setting_Panel(GUIDE_TYPE eType, _int iPage)
{
	if (eType == GUIDE_TYPE::LOCKON)
	{
		m_pKeyIcon_1->Update_Visible(true);
		m_pKeyIcon_1->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Mouse_Wheel.png", 3), 3);
		m_pKeyIcon_1->Set_Text(TEXT("락온"));

		m_pKeyIcon_2->Update_Visible(true);
		m_pKeyIcon_2->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Mouse_Rmb_Wheel.png", 3), 3);
		m_pKeyIcon_2->Set_Text(TEXT("락온 대상 변경"));

	}
	else if (eType == GUIDE_TYPE::GUARD)
	{
		if (iPage == 0)
		{
			m_pKeyIcon_1->Update_Visible(true);
			m_pKeyIcon_1->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_Shift.png", 3), 3);
			m_pKeyIcon_1->Set_Text(TEXT("가드"));
			m_pKeyIcon_2->Update_Visible(false);
		}
		else
		{
			m_pKeyIcon_1->Update_Visible(false);
			m_pKeyIcon_2->Update_Visible(false);
		}
	}
	else if (eType == GUIDE_TYPE::UNDERWORLD)
	{
		m_pKeyIcon_1->Update_Visible(true);
		m_pKeyIcon_1->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_One.png", 3), 3);
		m_pKeyIcon_1->Set_Text(TEXT("명계의 기운 사용"));
		m_pKeyIcon_2->Update_Visible(false);
	}
	else if (eType == GUIDE_TYPE::DODGE)
	{
		if (iPage == 0)
		{
			m_pKeyIcon_1->Update_Visible(true);
			m_pKeyIcon_1->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_Space.png", 3), 3);
			m_pKeyIcon_1->Set_Text(TEXT("회피"));
			m_pKeyIcon_2->Update_Visible(false);
		}
		else
		{
			m_pKeyIcon_1->Update_Visible(false);
			m_pKeyIcon_2->Update_Visible(false);
		}
	}
	else if (eType == GUIDE_TYPE::BURTALATTACK)
	{
		if (iPage == 0)
		{
			m_pKeyIcon_1->Update_Visible(true);
			m_pKeyIcon_1->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_F.png", 3), 3);
			m_pKeyIcon_1->Set_Text(TEXT("비전투 상태인 적 뒤에서"));

			m_pKeyIcon_2->Update_Visible(true);
			m_pKeyIcon_2->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_Alt.png", 3), 3);
			m_pKeyIcon_2->Set_Text(TEXT("걷기"));
		}
		else
		{
			m_pKeyIcon_1->Update_Visible(true);
			m_pKeyIcon_1->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_F.png", 3), 3);
			m_pKeyIcon_1->Set_Text(TEXT("탈진 상태인 적 상대로"));
			m_pKeyIcon_2->Update_Visible(false);

		}
	}
	else if (eType == GUIDE_TYPE::FALLATTACK)
	{
		m_pKeyIcon_1->Update_Visible(true);
		m_pKeyIcon_1->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Mouse_Lmb.png", 3), 3);
		m_pKeyIcon_1->Set_Text(TEXT("공중에서 낙하 중"));
		m_pKeyIcon_2->Update_Visible(false);
	}
	else if (eType == GUIDE_TYPE::IMPULSE)
	{
		m_pKeyIcon_1->Update_Visible(false);
		m_pKeyIcon_2->Update_Visible(false);
	}
	Text_Set(eType, iPage);
}

HRESULT CTutorial_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTutorial_Panel::Initialize_Clone(void* pArg)
{
	__super::Initialize_Clone(pArg);

	return S_OK;
}

void CTutorial_Panel::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CTutorial_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CTutorial_Panel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CTutorial_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	for (auto pChild : m_Children)
	{
		string strName = pChild->Get_Name();

		if (strName == "Guide_TexBox")
		{
			m_pText = static_cast<CUI_TextBox*>(pChild);
			Safe_AddRef(m_pText);
		}
	}

	CHECK_FAILED(Ready_Child(), E_FAIL);
	return S_OK;
}

HRESULT CTutorial_Panel::Ready_Child()
{
	UIOBJECT_DESC Desc = {};
	Desc.fDepth = 1.f;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXT);
	Desc.szName = "Icon_1";
	Desc.vLocalSize = { 32.f, 32.f };
	Desc.vLocalPos = { 0, 0 };

	CUI_Guide_Icon::TEXTBOX_DESC TextDesc = {};
	TextDesc.bIsTextBox = false;
	TextDesc.eTextAlign = TEXT_ALIGN::LEFT_CENTER;
	TextDesc.iPivotX = 20.f;
	TextDesc.iPivotY = 15.f;
	TextDesc.wstrTexttag = TEXT("Blade_Medium_18");
	TextDesc.wstrText = TEXT("닫기");
	TextDesc.vColor = { 1.f, 1.f, 1.f, 1.f };

	//아이콘 1번
	m_pKeyIcon_1 = static_cast<CUI_Guide_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Guide_Icon"), &Desc));
	if (m_pKeyIcon_1 == nullptr)
		return E_FAIL;

	m_pKeyIcon_1->Setting_Text(TextDesc);
	m_pKeyIcon_1->Set_Color({ 1.f,1.f,1.f,1.f });
	m_pKeyIcon_1->Set_LocalPos({ -210.f, -120.f }, this);
	m_Children.push_back(m_pKeyIcon_1);
	Safe_AddRef(m_pKeyIcon_1);

	//아이콘 2번
	Desc.szName = "Icon_2";
	m_pKeyIcon_2 = static_cast<CUI_Guide_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Guide_Icon"), &Desc));
	if (m_pKeyIcon_2 == nullptr)
		return E_FAIL;

	m_pKeyIcon_2->Setting_Text(TextDesc);
	m_pKeyIcon_2->Set_Color({ 1.f,1.f,1.f,1.f });
	m_pKeyIcon_2->Set_LocalPos({ -210.f, -90.f }, this);

	m_Children.push_back(m_pKeyIcon_2);
	Safe_AddRef(m_pKeyIcon_2);
	return S_OK;
}

void CTutorial_Panel::Text_Set(GUIDE_TYPE eType, _int iPage)
{
	if (eType == GUIDE_TYPE::LOCKON)
	{
		m_pText->Setting_Pivot(-220, -60);
		m_pText->Set_Text(TEXT("락온 버튼을 입력하거나 적을 공격하면 대상을 락온하여 주시할 수 있습니다\\락온 버튼을 재입력하면 락온을 해제합니다\\전방에 다수의 대상이 존재할 경우 카메라 조작을 통해\\락온 대상을 변경할 수 있습니다"));
	}
	else if (eType == GUIDE_TYPE::GUARD)
	{
		if (iPage == 0)
		{
			m_pText->Setting_Pivot(-220, -90);
			m_pText->Set_Text(TEXT("가드 버튼 입력 중 생명력 대신\\기력을 소모해 공격을 막는 가드 자세를 취합니다\\가드 자세일 때는 이동 속도와 기력 회복 속도가 느려집니다"));
		}
		else
		{
			m_pText->Setting_Pivot(-220, -120);
			m_pText->Set_Text(TEXT("적의 공격 시점에 맞춰 가드 버튼을 입력해 직전 가드에 성공하면 기력을 소량 소모해 공격을 막아내며\\적의 기력 혹은 괴력 게이지에 피해를 줍니다"));
		}
	}
	else if (eType == GUIDE_TYPE::UNDERWORLD)
	{
		m_pText->Setting_Pivot(-220, -90);
		m_pText->Set_Text(TEXT("카잔은 명계의 기운을 소모해 생명력을 회복합니다\\소모한 명계의 기운은 귀검과 상호작용을 하면 다시\\ 생성됩니다"));

	}
	else if (eType == GUIDE_TYPE::DODGE)
	{
		if (iPage == 0)
		{
			m_pText->Setting_Pivot(-220, -90);
			m_pText->Set_Text(TEXT("기력을 소모해 적의 공격을 피합니다\\회피를 실행한 후 일정 시간 동안은 적의 공격에 피격당하지 않습니다"));
		}
		else
		{
			m_pText->Setting_Pivot(-220, -120);
			m_pText->Set_Text(TEXT("적의 공격 시점에 맞춰 회피 버튼을 입력하면 직전 회피에\\성공하며 이 때 일정 시간 동안 회피 공격이 강화됩니다"));
		}
	}
	else if (eType == GUIDE_TYPE::BURTALATTACK)
	{
		if (iPage == 0)
		{
			m_pText->Setting_Pivot(-220, -60);
			m_pText->Set_Text(TEXT("걷기를 이용해 비전투 상태인 적에게 들키지 않고 접근하면\\브루탈 어택을 사용해 적에게 강한 피해를 줄 수 있습니다"));
		}
		else
		{
			m_pText->Setting_Pivot(-220, -90);
			m_pText->Set_Text(TEXT("브루탈 어택은 탈진 상태의 적에게 사용할 수 있습니다\\탈진 상태의 적 전방에서 브루탈 어택을 사용할 경우\\카잔은 기력을 회복합니다"));
		}
	}
	else if (eType == GUIDE_TYPE::FALLATTACK)
	{
		m_pText->Setting_Pivot(-220, -90);
		m_pText->Set_Text(TEXT("공중에서 낙하 중 공격 버튼을 입력하면\\낙하 공격을 사용합니다"));
	}
	else if (eType == GUIDE_TYPE::IMPULSE)
	{
		m_pText->Setting_Pivot(-220, -120);
		m_pText->Set_Text(TEXT("어떤 적들은 카잔의 공격에도 자세가 무너지지 않습니다\\이때 강한 공격을 사용하면 적의 자세를 무너뜨릴 수 있습니다"));
	}
}

CTutorial_Panel* CTutorial_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTutorial_Panel* pInstance = new CTutorial_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CTutorial_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTutorial_Panel::Clone(void* pArg)
{
	CTutorial_Panel* pInstance = new CTutorial_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CTutorial_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTutorial_Panel::Free()
{
	__super::Free();

	Safe_Release(m_pKeyIcon_1);
	Safe_Release(m_pKeyIcon_2);
	Safe_Release(m_pText);
}
