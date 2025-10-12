#include "Level_UI.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_UI::CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_UI::Initialize()
{

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;


	return S_OK;
}

void CLevel_UI::Update(_float fTimeDelta)
{

	return;
}

HRESULT CLevel_UI::Render()
{
	SetWindowText(g_hWnd, TEXT("UI툴"));

	return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}


CLevel_UI* CLevel_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_UI* pInstance = new CLevel_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_UI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_UI::Free()
{
	__super::Free();



}
