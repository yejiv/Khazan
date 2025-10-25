#include "Level_Camera.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Controller.h"

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

	m_pCamera_Controller = CCamera_Controller::Create();

	return S_OK;
}

void CLevel_Camera::Update(_float fTimeDelta)
{
	m_pCamera_Controller->Update(fTimeDelta);

	return;
}

HRESULT CLevel_Camera::Render()
{
	SetWindowText(g_hWnd, TEXT("카메라 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Camera::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CAMERA), TEXT("Camera_Terrain"),
		ENUM_CLASS(LEVEL::CAMERA), TEXT("Prototype_GameObject_Camera_Terrain")), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Camera::Ready_Layer_Player()
{
	//	CJOH_EditorModelTest::EDITORTESTMODEL_DESC Desc{};
	//	Desc.isAnim = true;
	//	Desc.strPrototypeTag = TEXT("Prototype_Component_Editor_Model_Test");
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_Player"),
	//		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Editor_Animation_TestModel"), &Desc)))
	//		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CAMERA), TEXT("Layer_Player"),
		ENUM_CLASS(LEVEL::CAMERA), TEXT("Prototype_GameObject_Player_Camera"))))
		return E_FAIL;

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
	Safe_Release(m_pCamera_Controller);

}
