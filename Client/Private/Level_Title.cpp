#include "Level_Title.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Level_Loading.h"

#include "UIObject.h"
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
	return S_OK;
}

void CLevel_Title::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::STAGE1))))
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
	Desc.vLocalSize = { 48.f, 48.f };
	Desc.vLocalPos = { 0.f, 0.f };
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "Cursor";
	Desc.fDepth = 0;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Cursor"), &Desc)))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/HUD.json"))))
		return E_FAIL;

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/Inven.json"))))
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
