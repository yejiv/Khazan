#include "Level_Shader.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Shader.h"
#include "Prop_Object.h"
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

	m_iNumCascades = m_pGameInstance->Get_NumCascades();

	m_pGameInstance->AddWidget(TEXT("Shader"), [&]()
	{
		if (!m_isInitShadow)
		{
			m_CascadeSplits.resize(m_iNumCascades);
			memcpy(m_CascadeSplits.data(), m_pGameInstance->Get_CascadeSplits(), sizeof(_float) * m_iNumCascades);
			m_fShadowBias = m_pGameInstance->Get_ShadowBias();
			m_fShadowLamda = m_pGameInstance->Get_ShadowLamda();
			m_vLightDir = m_pGameInstance->Get_ShadowLightDir();
			m_isInitShadow = true;
		}

		ImGui::Begin("Shader Settings");

		// isRenderShadow true일 때만 아래 애들 띄우기
		// true일 때만 렌더러에서 기록하는 거 켜주기
		// 컴바인드에서 안 켜지면 기본 명암 정도만 나타내도록 후처리 셰이더에 플래그 넘겨서 써야 할지도?
		//	_bool bCopyFlag = m_isRenderShadow;

		ImGui::Checkbox("Shadow", &m_isRenderShadow);

		if (m_isRenderShadow)
		{
			if (ImGui::CollapsingHeader("Frame Per Second"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				ImGui::SetWindowFontScale(2.f);
				ImGui::Text("%s", m_szFPS);
				ImGui::SetWindowFontScale(1.f);
			}

			if (ImGui::CollapsingHeader("Shadow Light"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				if (ImGui::SliderFloat3("Direction", reinterpret_cast<_float*>(&m_vLightDir), -1.f, 1.f))
					m_pGameInstance->Set_ShadowLightDir(m_vLightDir);
			}

			if (ImGui::CollapsingHeader("Cascade"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				ImGui::Text("Manual Split Adjustment");
				ImGui::Separator();

				for (_uint i = 0; i < m_iNumCascades; ++i)
				{
					_float fMin = (i == 0) ? m_fCameraNear : m_CascadeSplits[i - 1];
					_float fMax = (i == (m_iNumCascades - 1)) ? m_fCameraFar : m_CascadeSplits[i + 1];

					_char szLabel[64] = {};
					sprintf_s(szLabel, "Cascade %d Split Far", i);

					if (ImGui::SliderFloat(szLabel, &m_CascadeSplits[i], fMin, fMax))
						m_pGameInstance->Set_CascadeSplits(m_CascadeSplits.data());
				}

				ImGui::Separator();
				ImGui::Text("Auto Split Calculation");

				if (ImGui::SliderFloat("Cascade Mix Lamda", &m_fShadowLamda, 0.f, 1.f))
					m_pGameInstance->Set_ShadowLamda(m_fShadowLamda);

				ImGui::Separator();

				if (ImGui::SliderFloat("Shadow Bias", &m_fShadowBias, 0.0001f, 0.005f, "%.4f"))
					m_pGameInstance->Set_ShadowBias(m_fShadowBias);

				ImGui::Separator();
			}
		}

		ImGui::Checkbox("SSAO", &m_isRenderSSAO);

		if (m_isRenderSSAO)
		{

		}

		m_pGameInstance->Set_EnableShadow(m_isRenderShadow);
		m_pGameInstance->Set_EnableSSAO(m_isRenderSSAO);

		ImGui::End();
	});

	return S_OK;
}

void CLevel_Shader::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif
}

HRESULT CLevel_Shader::Render()
{
	SetWindowText(g_hWnd, TEXT("쉐이더툴"));

#ifdef _DEBUG
	++m_iRenderCount;

	if (m_fTimeAcc >= 1.f)
	{
		sprintf_s(m_szFPS, "FPS:%d", m_iRenderCount);
		m_fTimeAcc = 0.f;
		m_iRenderCount = 0;
	}
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
	Desc.fSpeedPerSec = 25.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fMouseSensor = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_Camera"),
		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Camera_Shader"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_BackGround()
{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_BackGround"),
	//		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Terrain_Shader"))))
	//		return E_FAIL;

	CProp_Object::PROP_OBJECT_DESC Desc = {};
	Desc.eLevel = LEVEL::SHADER;
	XMStoreFloat4x4(&Desc.WorldMatrix, XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixTranslation(0.f, -9.5f, 5.f));
	_tchar szPrototypeModelTag[MAX_PATH] = TEXT("Prototype_Component_Model_Rock");
	memcpy(Desc.szModelName, szPrototypeModelTag, MAX_PATH);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_BackGround"),
		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Prop_Object"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_Player()
{
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
