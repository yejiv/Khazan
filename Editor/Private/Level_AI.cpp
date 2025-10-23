#include "Level_AI.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_AI::CLevel_AI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_AI::Initialize()
{
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_AI::Update(_float fTimeDelta)
{
	return;
}

HRESULT CLevel_AI::Render()
{
	SetWindowText(g_hWnd, TEXT("AI 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_AI::Ready_Layer_BackGround(const _wstring& strLayerTag)
{

	return S_OK;
}


CLevel_AI* CLevel_AI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_AI* pInstance = new CLevel_AI(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_AI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_AI::Free()
{
	__super::Free();

}
