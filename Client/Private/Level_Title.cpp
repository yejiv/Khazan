#include "Level_Title.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Level_Loading.h"

#include "UIObject.h"
#include "Damage_Text.h"

#include "Sequence_Dummy.h"
#include "UI_Announce_MapName.h"

CLevel_Title::CLevel_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Title::Initialize()
{
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	CHECK_FAILED(Ready_Layer_UI(), E_FAIL);

	m_pGameInstance->Subscribe_Event<EventTest>(ENUM_CLASS(EVENT_TYPE::TEST), [&](const EventTest& e)
		{
			iTest = e.data;
		});

	return S_OK;
}

void CLevel_Title::Update(_float fTimeDelta)
{

	if (m_pGameInstance->Key_Down(DIK_B))
	{
		EVENT_ANNOUNCE_MAPNAME Desc = {};
		//화면에 표시할 시간
		Desc.fTime = 2.f;

		//표시할 지역 이름
		Desc.iMapType = ENUM_CLASS(CUI_Announce_MapName::MAP_TYPE::HEINMACH);

		//페이드 아웃 시간
		Desc.fFadeOutTime = 2.0f;

		//디죨부 처리 여부
		Desc.isDissovle = true;

		//이벤트 발생시키기	
		m_pGameInstance->Emit_Event<EVENT_ANNOUNCE_MAPNAME>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_MAPNAME), Desc);
	}

	if (m_pGameInstance->Key_Down(DIK_0))
	{
		m_pGameInstance->Emit_Event<EventTest>(ENUM_CLASS(EVENT_TYPE::TEST), EventTest{ 10 });
	}

	if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
	{
		iTest++;
	}

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::HEINMACH))))
			return;
	}


	return;
}

HRESULT CLevel_Title::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Title::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Title::Ready_Layer_UI()
{
	CUIObject::UIOBJECT_DESC Desc = {};
	Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "LogoBG";
	Desc.fDepth = 10;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), TEXT("Layer_UI"),
		ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_Logo_BG"), TIME_CHANNEL::WORLD, &Desc)))
		return E_FAIL;

	Desc.vLocalSize = { 48.f, 48.f };
	Desc.vLocalPos = { 0.f, 0.f };
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "Cursor";
	Desc.fDepth = 0;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Cursor"), TIME_CHANNEL::WORLD, &Desc)))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/HUD.json"))))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/Inven.json"))))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/MainMenu.json"))))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/BladeNexus.json"))))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/State.json"))))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/ItemInfo_Other.json"))))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/ItemInfo_Weapon.json"))))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/ItemInfo_Equip.json"))))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/BossHp.json"))))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/HUD_Amount.json"))))
		return E_FAIL;


	return S_OK;
}


CLevel_Title* CLevel_Title::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Title* pInstance = new CLevel_Title(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Title"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Title::Free()
{
	__super::Free();



}
