#include "Level_Shader.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Shader.h"
#include "Prop_Test.h"
#include "JOH_EditorModelTest.h"

CLevel_Shader::CLevel_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Shader::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	m_pGameInstance->AddWidget(TEXT("Shader"), [&]()
	{
		ImGui::Begin("Shadow Light Settings");

		if (ImGui::CollapsingHeader("Frame Per Second"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			ImGui::SetWindowFontScale(2.f);
			ImGui::Text("%s", m_szFPS);
			ImGui::SetWindowFontScale(1.f);
		}

		if (ImGui::CollapsingHeader("Shadow Light"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			SHADOW_LIGHT_DESC Desc = m_pGameInstance->Get_ShadowLight();

			m_isChanged |= ImGui::DragFloat3("Light Eye", (float*)&Desc.vEye, 0.1f);
			m_isChanged |= ImGui::DragFloat3("Light At", (float*)&Desc.vAt, 0.1f);
			m_isChanged |= ImGui::SliderFloat("FOV", &Desc.fFovy, 1.f, 180.f);
			m_isChanged |= ImGui::InputFloat("Near Plane", &Desc.fNear, 0.1f, 1.f);
			m_isChanged |= ImGui::InputFloat("Far Plane", &Desc.fFar, 1.f, 1000.f);

			if (m_isChanged)
			{
				m_pGameInstance->Ready_ShadowLight(Desc);
			}
		}

		ImGui::End();
	});

	return S_OK;
}

void CLevel_Shader::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif

	return;
}

HRESULT CLevel_Shader::Render()
{
	SetWindowText(g_hWnd, TEXT("½¦ÀÌ´õÅø"));

#ifdef _DEBUG
	++m_iRenderCount;

	if (m_fTimeAcc >= 1.f)
	{
		sprintf_s(m_szFPS, "FPS:%d", m_iRenderCount);
		m_fTimeAcc = 0.f;
		m_iRenderCount = 0;
	}
	
	//	m_pGameInstance->DrawText(TEXT("Font_153"), m_szFPS, _float2(100.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
#endif

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Lights()
{
	// Directional
	LIGHT_DESC LightDesc = {};
	LightDesc.eType = LIGHT_DESC::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	if (FAILED(m_pGameInstance->Add_Light(TEXT("Directional_Shader"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
		return E_FAIL;

	// Point_Green
	//	LightDesc.eType = LIGHT_DESC::POINT;
	//	LightDesc.vPosition = _float4(20.f, 2.f, 10.f, 1.f);
	//	LightDesc.fRange = 10.f;
	//	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	//	LightDesc.vAmbient = _float4(0.1f, 0.6f, 0.1f, 1.f);
	//	LightDesc.vSpecular = LightDesc.vDiffuse;
	//	if (FAILED(m_pGameInstance->Add_Light(TEXT("Point_Green"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
	//		return E_FAIL;
	//	
	//	// Point_Red
	//	LightDesc.eType = LIGHT_DESC::POINT;
	//	LightDesc.vPosition = _float4(10.f, 2.f, 10.f, 1.f);
	//	LightDesc.fRange = 10.f;
	//	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	//	LightDesc.vAmbient = _float4(0.6f, 0.1f, 0.1f, 1.f);
	//	LightDesc.vSpecular = LightDesc.vDiffuse;
	//	if (FAILED(m_pGameInstance->Add_Light(TEXT("Point_Red"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
	//		return E_FAIL;

	// Shadow_Light
	SHADOW_LIGHT_DESC ShadowLightDesc{};
	ShadowLightDesc.vEye = _float4(-20.f, 20.f, -20.f, 1.f);
	ShadowLightDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	ShadowLightDesc.fFovy = XMConvertToRadians(60.f);
	ShadowLightDesc.fNear = 0.1f;
	ShadowLightDesc.fFar = 1000.f;
	if (FAILED(m_pGameInstance->Ready_ShadowLight(ShadowLightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_Camera()
{
	CCamera_Shader::CAMERA_EFFECT_DESC Desc{};

	Desc.vEye = _float4(0.f, 20.f, -20.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fMouseSensor = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_Camera"),
		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Camera_Shader"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_BackGround()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_BackGround"),
		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Terrain_Shader"))))
		return E_FAIL;

	//	CProp_Test::PROP_OBJECT_DESC ObjectDesc = {};
	//	memcpy(ObjectDesc.szModelName, TEXT("Prototype_Component_Model_WP_WOD_Ground_Base_004"), sizeof(ObjectDesc.szModelName));
	//	ObjectDesc.eLevel = LEVEL::SHADER;
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_MapObject"),
	//		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Prop_Test"), &ObjectDesc)))
	//		return E_FAIL;

	return S_OK;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_Player()
{
	//	CJOH_EditorModelTest::EDITORTESTMODEL_DESC Desc{};
	//	Desc.isAnim = true;
	//	Desc.strPrototypeTag = TEXT("Prototype_Component_Editor_Model_Test");
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_Player"),
	//		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Editor_Animation_TestModel"), &Desc)))
	//		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_Player"),
		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Player_Shader"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Shader* CLevel_Shader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Shader* pInstance = new CLevel_Shader(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Shader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Shader::Free()
{
	__super::Free();



}
