#include "Level_Editor.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_Editor::CLevel_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Editor::Initialize()
{
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Editor::Update(_float fTimeDelta)
{
	return;
}

HRESULT CLevel_Editor::Render()
{
	SetWindowText(g_hWnd, TEXT("에디터 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}


CLevel_Editor* CLevel_Editor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Editor* pInstance = new CLevel_Editor(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Editor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Editor::Free()
{
	__super::Free();

}
