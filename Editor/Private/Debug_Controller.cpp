#include "Debug_Controller.h"
#include "GameInstance.h"

CDebug_Controller::CDebug_Controller()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CDebug_Controller::Initialize()
{

	//m_vRenderTarget.reserve(12);
	//m_vRenderTarget.push_back(TEXT("Target_Diffuse"));
	//m_vRenderTarget.push_back(TEXT("Target_Normal"));
	//m_vRenderTarget.push_back(TEXT("Target_Depth"));
	//m_vRenderTarget.push_back(TEXT("Target_World"));
	//m_vRenderTarget.push_back(TEXT("Target_Shade"));
	//m_vRenderTarget.push_back(TEXT("Target_Specular"));
	//m_vRenderTarget.push_back(TEXT("Target_LightDepth"));
	//m_vRenderTarget.push_back(TEXT("Target_LightDepth_0"));
	//m_vRenderTarget.push_back(TEXT("Target_LightDepth_1"));
	//m_vRenderTarget.push_back(TEXT("Target_LightDepth_2"));
	//m_vRenderTarget.push_back(TEXT("Target_BackBuffer"));
	//m_vRenderTarget.push_back(TEXT("Target_BlurX"));

	Ready_ImGui();

	return S_OK;
}

void CDebug_Controller::Update()
{
	if (m_pGameInstance->Key_Down(DIK_Q))
	{
		m_vRenderTargetValue = m_pGameInstance->isPickRenderTargetPixel(CharToWString(m_szSelectRenderTarget));
	}
	
}

void CDebug_Controller::Ready_ImGui()
{
	m_pGameInstance->AddWidget(TEXT("Debug"), [this]() {
		ImGui::Begin("Debug");
		ImGui::Text("RenderTarget");

		ImGui::InputText("TargetName", m_szSelectRenderTarget, MAX_PATH);

		_char fX[64];
		snprintf(fX, sizeof(fX), "X = %.2f", m_vRenderTargetValue.x);
		ImGui::Text(fX);
		ImGui::SameLine();
		_char fY[64];
		snprintf(fY, sizeof(fY), "Y = %.2f", m_vRenderTargetValue.y);
		ImGui::Text(fY);
		ImGui::SameLine();
		_char fZ[64];
		snprintf(fZ, sizeof(fZ), "Z = %.2f", m_vRenderTargetValue.z);
		ImGui::Text(fZ);
		ImGui::SameLine();
		_char fW[64];
		snprintf(fW, sizeof(fW), "W = %.2f", m_vRenderTargetValue.w);
		ImGui::Text(fW);

		ImGui::End();
		});
}

Editor::CDebug_Controller* CDebug_Controller::Create()
{
	CDebug_Controller* pInstance = new CDebug_Controller();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CDebug_Controller"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Editor::CDebug_Controller::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
