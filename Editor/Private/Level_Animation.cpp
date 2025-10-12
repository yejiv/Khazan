#include "Level_Animation.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_Animation::CLevel_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Animation::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Test(TEXT("Layer_Test"))))
		return E_FAIL;

	m_pGameInstance->AddWidget(TEXT("Animaition"), []() {
		ImGui::Begin("Debug");
		ImGui::Text("Hello, world!");
		ImGui::End();
		});


	return S_OK;
}

void CLevel_Animation::Update(_float fTimeDelta)
{

	return;
}

HRESULT CLevel_Animation::Render()
{
	SetWindowText(g_hWnd, TEXT("애니메이션툴"));

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::ANIMATION), strLayerTag,
	//	ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Animation::Ready_Layer_Test(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::ANIMATION), strLayerTag,
		ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_GameObject_JOH_TestModel"))))
		return E_FAIL;

	return S_OK;
}


CLevel_Animation* CLevel_Animation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Animation* pInstance = new CLevel_Animation(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Animation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Animation::Free()
{
	__super::Free();



}
