#include "UI_Loading.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "UI_Atlas_Icon.h"
#include "UI_Loading_Panel.h"
#include "UI_BackGround.h"

CUI_Loading::CUI_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_Loading::CUI_Loading(const CUI_Loading& Prototype)
	: CUI_Panel(Prototype)
{
}

void CUI_Loading::On_Panel()
{
	_float2 vPos = m_pPointIcon->Get_WolrdPos();
	m_IsUpdate = true;
	m_pPointIcon->Set_Pos({ vPos.x - 30.f * m_iSelete ,vPos.y });
	m_iSelete = 0;
	Update_Text();

	m_pIcon->Start_Load();
}

void CUI_Loading::Off_Panel()
{
	m_IsUpdate = false;
}

void CUI_Loading::Finsh_UI()
{
	m_pIcon->Finsh_Load();
}

HRESULT CUI_Loading::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_Loading::Initialize_Clone(void* pArg)
{
	__super::Initialize_Clone(pArg);

	return S_OK;
}

void CUI_Loading::Priority_Update(_float fTimeDelta)
{
}

void CUI_Loading::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	Input_Update();
}

void CUI_Loading::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Loading::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	for (auto child : m_Children)
	{
		string strName = child->Get_Name();

		if (strName == "Loading_Info_Point")
		{
			m_pPointIcon = static_cast<CUI_Atlas_Icon*>(child);
			Safe_AddRef(child);
		}
		else if (strName == "Loading_Icon")
		{
			m_pIcon = static_cast<CUI_Loading_Panel*>(child);
			Safe_AddRef(child);
		}
		else if (strName == "Loading_Info_Name")
		{
			m_pInfoName = static_cast<CUI_TextBox*>(child);
			Safe_AddRef(child);
		}
		else if (strName == "Loading_Info_Text")
		{
			m_pInfoText = static_cast<CUI_TextBox*>(child);
			Safe_AddRef(child);
		}
		
	}
	Ready_Object();
	Update_Text();
	return S_OK;
}

HRESULT CUI_Loading::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Loading_Panel"),
		CUI_Loading_Panel::Create(m_pDevice, m_pContext)), E_FAIL);

	return S_OK;
}

HRESULT CUI_Loading::Ready_Object()
{
	UIOBJECT_DESC Desc = {};
	Desc.fDepth = 5.5f;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "BackGround";
	Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
	if (m_pBackGround == nullptr)
		return E_FAIL;
	m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::LOADING);
	m_Children.push_back(m_pBackGround);
	Safe_AddRef(m_pBackGround);

	return S_OK;

}

void CUI_Loading::Input_Update()
{
	_float2 vPos = m_pPointIcon->Get_WolrdPos();
	_bool IsInputKey = false;
	if (m_pGameInstance->Key_Down(DIK_E, INPUT_TYPE::UI))
	{
        m_pGameInstance->StopByKey(TEXT("UI_Iteminfo_Close (SFX).wav"));
        m_pGameInstance->PlaySoundOnce(TEXT("UI_Iteminfo_Close (SFX).wav"));
		IsInputKey = true;
		++m_iSelete;
		if (m_iSelete > 3)
		{
			m_iSelete = 0;
			m_pPointIcon->Set_Pos({ vPos.x - 90.f ,vPos.y });
		}
		else
			m_pPointIcon->Set_Pos({ vPos.x + 30.f ,vPos.y });
	}
	else if (m_pGameInstance->Key_Down(DIK_Q, INPUT_TYPE::UI))
	{
        m_pGameInstance->StopByKey(TEXT("UI_Iteminfo_Close (SFX).wav"));
        m_pGameInstance->PlaySoundOnce(TEXT("UI_Iteminfo_Close (SFX).wav"));
		IsInputKey = true;
		--m_iSelete;
		if (m_iSelete < 0)
		{
			m_iSelete = 3;
			m_pPointIcon->Set_Pos({ vPos.x + 90.f ,vPos.y });
		}
		else
			m_pPointIcon->Set_Pos({ vPos.x - 30.f ,vPos.y });
	}

	if(IsInputKey)
		Update_Text();
}

void CUI_Loading::Update_Text()
{
	switch (m_iSelete)
	{
	case 0:
		m_pInfoName->Set_Text(TEXT("직전 가드"));
		m_pInfoText->Set_Text(TEXT("타이밍에 맞춰 가드를 하면\\직전 가드를 할 수 있고\\성공하면 기력 피해가 크게 감소한다"));
		break;
	case 1:
		m_pInfoName->Set_Text(TEXT("귀검"));
		m_pInfoText->Set_Text(TEXT("귀검과 상호작용을 하면 명계의 기운\\생명력 그리고 상태 이상을 회복할 수 있다"));
		break;
	case 2:
		m_pInfoName->Set_Text(TEXT("배후 공격"));
		m_pInfoText->Set_Text(TEXT("적의 후방에서 공격에 성공하면\\정면에서 공격할 때보다 피해량이 증가한다"));
		break;
	case 3:
		m_pInfoName->Set_Text(TEXT("기력 관리"));
		m_pInfoText->Set_Text(TEXT("공격 방어 회피 등 전투 중 액션을 사용하면\\기력이 소모되며 기력을 모두 소모하면\\기력 기반의 액션을 사용할 수 없다"));
		break;
	}
}


CUI_Loading* CUI_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_Loading* pInstance = new CUI_Loading(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Loading"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Loading::Clone(void* pArg)
{
	CUI_Loading* pInstance = new CUI_Loading(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Loading"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pInfoName);
	Safe_Release(m_pInfoText);
	Safe_Release(m_pIcon);
	Safe_Release(m_pPointIcon);

	Safe_Release(m_pBackGround);
}
