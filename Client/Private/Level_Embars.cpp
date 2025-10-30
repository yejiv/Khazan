#include "Level_Embars.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Level_Loading.h"

CLevel_Embars::CLevel_Embars(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Embars::Initialize()
{
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;



	return S_OK;
}

void CLevel_Embars::Update(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::HEINMACH))))
			return;
	}


	return;
}

HRESULT CLevel_Embars::Render()
{
	SetWindowText(g_hWnd, TEXT("엠바스 유적지 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Embars::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Embars::Ready_Layer_UI()
{
	
	return S_OK;
}


CLevel_Embars* CLevel_Embars::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Embars* pInstance = new CLevel_Embars(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Embars"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Embars::Free()
{
	__super::Free();



}
