#include "Level_Viper.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Level_Loading.h"

CLevel_Viper::CLevel_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Viper::Initialize()
{
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;



	return S_OK;
}

void CLevel_Viper::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::HEINMACH))))
			return;
	}


	return;
}

HRESULT CLevel_Viper::Render()
{
	SetWindowText(g_hWnd, TEXT("엠바스 유적지 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_UI()
{

	return S_OK;
}


CLevel_Viper* CLevel_Viper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Viper* pInstance = new CLevel_Viper(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Viper"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Viper::Free()
{
	__super::Free();



}
