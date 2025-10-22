#include "Level_UI.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_UI.h"
#include "Edit_Interface_UI.h"
CLevel_UI::CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_UI::Initialize()
{
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Obejct()))
		return E_FAIL;

	m_pUIInterface = CEdit_Interface_UI::Create(m_pDevice, m_pContext, LEVEL::UI);

	m_pGameInstance->AddWidget(TEXT("UI"), [this]() { this->Update_Interface(); });
	//m_pGameInstance->AddWidget(TEXT("UI"), [&]() {

	//	ImGui::Begin("UI TOOL", nullptr, ImGuiWindowFlags_MenuBar);

	//	static _char szDefaultName[MAX_PATH] = "DefaultName.dat";
	//	ImGui::InputText("FilePath", szDefaultName, IM_ARRAYSIZE(szDefaultName));

	//	ImGui::Separator();

	//	if (ImGui::BeginMenuBar())
	//	{
	//		if (ImGui::BeginMenu("File"))
	//		{
	//			if (ImGui::MenuItem("Save Layout")) {}
	//			if (ImGui::MenuItem("Load Layout")) {}
	//			ImGui::EndMenu();
	//		}
	//		ImGui::EndMenuBar();
	//	}

	//	if (ImGui::BeginTabBar("UITabs"))
	//	{
	//		//if (ImGui::BeginTabItem("Hierarchy"))
	//		//{
	//		//	Show_Hierarchy_Menu(szDefaultName);
	//		//	ImGui::EndTabItem();
	//		//}

	//		//if (ImGui::BeginTabItem("Inspector"))
	//		//{
	//		//	Show_Inspector_Menu();
	//		//	ImGui::EndTabItem();
	//		//}

	//		//if (ImGui::BeginTabItem("Create UI"))
	//		//{
	//		//	Show_CreateUI_Menu(szDefaultName);
	//		//	ImGui::EndTabItem();
	//		//}

	//		ImGui::EndTabBar();
	//	}

	//	ImGui::End();
	//	});

	return S_OK;
}

void CLevel_UI::Update(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;
	return;
}

HRESULT CLevel_UI::Render()
{
	SetWindowText(g_hWnd, TEXT("UIÅø"));

	return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_UI::CAMERA_FREE_DESC		CameraDesc{};

	CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.2f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::UI), strLayerTag,
		ENUM_CLASS(LEVEL::UI), TEXT("Prototype_GameObject_Camera_UI"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_UI::Ready_Obejct()
{
	return S_OK;
}

void CLevel_UI::Update_Interface()
{
	m_pUIInterface->Update_UIInterface(m_fTimeDelta);
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
	Safe_Release(m_pUIInterface);

}
