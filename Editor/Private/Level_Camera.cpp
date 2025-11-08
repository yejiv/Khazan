#include "Level_Camera.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_Camera::CLevel_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Camera::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Camera::Update(_float fTimeDelta)
{
	return;
}

HRESULT CLevel_Camera::Render()
{
	SetWindowText(g_hWnd, TEXT("카메라 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Camera::Ready_Layer_BackGround(const _wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Camera::Ready_Layer_Player()
{
;

	return S_OK;
}

HRESULT CLevel_Camera::Ready_Layer_Camera(const _wstring& strLayerTag)
{

	return S_OK;
}


CLevel_Camera* CLevel_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Camera* pInstance = new CLevel_Camera(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Camera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Camera::Free()
{
	__super::Free();

}
