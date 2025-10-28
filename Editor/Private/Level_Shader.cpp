#include "Level_Shader.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Shader.h"
#include "Prop_Object.h"
#include "JOH_EditorModelTest.h"
#include "Player_Shader.h"

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

	//if (FAILED(Ready_Lights()))
		//return E_FAIL;

#pragma region 테스트용 ( 박준영이 남기고 간거 )
	CHECK_FAILED(Ready_Lights(TEXT("Test"), LEVEL::SHADER), E_FAIL);

	CHECK_FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"), TEXT("Test"), LEVEL::SHADER), E_FAIL);

	CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Layer_MapObject_Inst"), TEXT("Test"), LEVEL::SHADER), E_FAIL);
#pragma endregion


	m_iNumCascades = m_pGameInstance->Get_NumCascades();

	m_pGameInstance->AddWidget(TEXT("Shader"), [&]()
	{
		if (!m_isInitShadow)
		{
			m_CascadeConfig.Splits.resize(m_iNumCascades);
			m_CascadeConfig = m_pGameInstance->Get_CascadeConfig();
			m_SSAOConfig = m_pGameInstance->Get_SSAOConfig();
			m_BlurConfig = m_pGameInstance->Get_BlurConfig();
			m_isInitShadow = true;
		}

		ImGui::Begin("Shader Settings");

		if (ImGui::CollapsingHeader("Frame Per Second"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			ImGui::SetWindowFontScale(2.f);
			ImGui::Text("%s", m_szFPS);
			ImGui::SetWindowFontScale(1.f);
			ImGui::Separator();
		}

		ImGui::Checkbox("Shadow", &m_isRenderShadow);

		if (m_isRenderShadow)
		{
			if (ImGui::CollapsingHeader("Shadow Light"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				if (ImGui::SliderFloat3("Direction", reinterpret_cast<_float*>(&m_CascadeConfig.vLightDir), -1.f, 1.f))
					m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);
			}

			if (ImGui::CollapsingHeader("Cascade"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				ImGui::Text("Manual Split Adjustment");
				ImGui::Separator();

				for (_uint i = 0; i < m_iNumCascades; ++i)
				{
					_float fMin = (i == 0) ? m_fCameraNear : m_CascadeConfig.Splits[i - 1];
					_float fMax = (i == (m_iNumCascades - 1)) ? m_fCameraFar : m_CascadeConfig.Splits[i + 1];

					_char szLabel[64] = {};
					sprintf_s(szLabel, "Cascade %d Split Far", i);

					if (ImGui::SliderFloat(szLabel, &m_CascadeConfig.Splits[i], fMin, fMax))
						m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);
				}

				ImGui::Separator();
				ImGui::Text("Auto Split Calculation");

				if (ImGui::SliderFloat("Cascade Mix Lamda", &m_CascadeConfig.fLamda, 0.f, 1.f))
					m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);

				ImGui::Separator();

				if (ImGui::SliderFloat("Shadow Bias", &m_CascadeConfig.fBias, 0.0001f, 0.005f, "%.4f"))
					m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);

				ImGui::Separator();
			}
		}

		ImGui::Checkbox("SSAO", &m_isRenderSSAO);

		if (m_isRenderSSAO)
		{
			_int iNumKernels = static_cast<_int>(m_SSAOConfig.iNumKernels);
			if (ImGui::InputInt("Sample Count", &iNumKernels, 4, 16))
			{
				m_SSAOConfig.iNumKernels = iNumKernels;
				m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);
			}

			if (ImGui::SliderFloat("Sample Radius", &m_SSAOConfig.fRadius, 0.1f, 2.f))
				m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);

			if (ImGui::SliderFloat("Intensity", &m_SSAOConfig.fIntensity, 0.5f, 4.f))
				m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);

			if (ImGui::SliderFloat("Contrast", &m_SSAOConfig.fConstrast, 0.5f, 2.f))
				m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);

			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader("Post Processing"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			ImGui::Checkbox("Emissive", &m_isEnableEmissive);

			if (m_isEnableEmissive)
			{
				if (ImGui::SliderFloat("Brightness", &m_fEmissiveIntensity, 0.f, 10.f))
				{
					dynamic_cast<CPlayer_Shader*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
						TEXT("Layer_Player"), 0))->Set_EmissiveIntensity(m_fEmissiveIntensity);
				}

				ImGui::Separator();
			}

			ImGui::Checkbox("Bloom", &m_isEnableBloom);

			if (m_isEnableBloom)
			{
				// 가우시안 블러 범위(반경)
				if (ImGui::InputInt("Blur Radius", &m_BlurConfig.iRadius, 2, 4))
					m_pGameInstance->Set_BlurConfig(m_BlurConfig);

				// 가우시안 블러 가중치 밀집도
				if (ImGui::SliderFloat("Concentration", &m_BlurConfig.fSigma, 1.f, 10.f))
					m_pGameInstance->Set_BlurConfig(m_BlurConfig);

				// 가우시안 블러 가중치 합 정규화 수치
				if (ImGui::SliderFloat("Normalization", &m_BlurConfig.fNormalization, 0.f, 15.f))
					m_pGameInstance->Set_BlurConfig(m_BlurConfig);

				ImGui::Separator();
			}
		}

		dynamic_cast<CPlayer_Shader*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER), 
			TEXT("Layer_Player"), 0))->Set_EnableEmissive(m_isEnableEmissive);
		dynamic_cast<CPlayer_Shader*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
			TEXT("Layer_Player"), 0))->Set_EnableBloom(m_isEnableBloom);
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
	XMStoreFloat4x4(&Desc.WorldMatrix, XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixTranslation(0.f, -5.f, 20.f));
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

HRESULT CLevel_Shader::Ready_Layer_MapObject(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
		break;
	case KHAZAN_MAP::YETUGA:
		pDataFilePath += TEXT("Yetuga/");
		break;
	case KHAZAN_MAP::THECREVICE:
		pDataFilePath += TEXT("TheCrevice/");
		break;
	case KHAZAN_MAP::EMBARS:
		pDataFilePath += TEXT("Embars/");
		break;
	case KHAZAN_MAP::VIPER:
		pDataFilePath += TEXT("Viper/");
		break;
	default:
		break;
	}

	pDataFilePath += pDataFileName;

	// 동일한 파일명의 _objects.dat 불러오기
	pDataFilePath += TEXT("_object.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("데이터 파일이 없거나 박준영 문제"), E_FAIL);

	// 1. 오브젝트의 총 개수
	_uint iObjectCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

	// 오브젝트 총 개수만큼 순회
	for (_uint i = 0; i < iObjectCnt; ++i)
	{
		CProp_Object::PROP_OBJECT_DESC ObjectDesc = {};

		ObjectDesc.eLevel = eCurrentLevel;

		// 2. 프로토 타입 태그 길이 불러오기
		_uint iPrototypeTagLen = {};
		CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 3. 프로토 타입 태그 이름 불러오기
		_tchar szPrototypeTag[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

		// 불러온 태그 카피
		memcpy(ObjectDesc.szModelName, szPrototypeTag, sizeof(ObjectDesc.szModelName));

		// 4. 객체당 월드 행렬 때오기
		_float4x4 WorldMatrix = {};
		CHECK_FALSE(ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr), E_FAIL);

		ObjectDesc.WorldMatrix = WorldMatrix;

		// 5. 객체의 속성 불러오기
		MAPOBJECT_PROPERTIES PropProperties = {};
		CHECK_FALSE(ReadFile(hFile, &PropProperties, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr), false);

		ObjectDesc.Properties = PropProperties;

		// 일단 단일 오브젝트로 배치하고 추후에 인스턴스, 인터렉티브, 다이나믹 으로 나누겠습니다.
		//	m_pGameInstance->Add_FireTask([this, objDesc = ObjectDesc, curLevel = eCurrentLevel]() mutable {
		//		});

		CHECK_FAILED(
			m_pGameInstance->Add_GameObject_ToLayer(
				ENUM_CLASS(ObjectDesc.eLevel),
				TEXT("Layer_MapObject"),
				ENUM_CLASS(eCurrentLevel),
				TEXT("Prototype_GameObject_Prop_Object"),
				&ObjectDesc // 캡처된 값의 주소 -> 안전
			),
			E_FAIL
		);

		/*CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc), E_FAIL);*/
	}

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
		break;
	case KHAZAN_MAP::YETUGA:
		pDataFilePath += TEXT("Yetuga/");
		break;
	case KHAZAN_MAP::THECREVICE:
		pDataFilePath += TEXT("TheCrevice/");
		break;
	case KHAZAN_MAP::EMBARS:
		pDataFilePath += TEXT("Embars/");
		break;
	case KHAZAN_MAP::VIPER:
		pDataFilePath += TEXT("Viper/");
		break;
	default:
		break;
	}

	pDataFilePath += pDataFileName;

	// 동일한 파일명의 _inst.dat 불러오기
	pDataFilePath += TEXT("_inst.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("데이터 파일이 없거나 박준영 문제"), E_FAIL);

	// 1. 오브젝트의 총 개수
	_uint iObjectCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

	// 오브젝트 총 개수만큼 순회
	for (_uint i = 0; i < iObjectCnt; ++i)
	{
		CProp_Object::PROP_OBJECT_DESC ObjectDesc = {};

		ObjectDesc.eLevel = eCurrentLevel;

		// 2. 프로토 타입 태그 길이 불러오기
		_uint iPrototypeTagLen = {};
		CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 3. 프로토 타입 태그 이름 불러오기
		_tchar szPrototypeTag[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

		// 불러온 태그 카피
		memcpy(ObjectDesc.szModelName, szPrototypeTag, sizeof(ObjectDesc.szModelName));

		// 4. 객체의 속성 불러오기
		MAPOBJECT_PROPERTIES PropProperties = {};
		CHECK_FALSE(ReadFile(hFile, &PropProperties, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr), false);

		ObjectDesc.Properties = PropProperties;

		// 인스턴스 객체 슈웃
		m_pGameInstance->Add_FireTask([this, objDesc = ObjectDesc, curLevel = eCurrentLevel]() mutable {
			CHECK_FAILED(
				m_pGameInstance->Add_GameObject_ToLayer(
					ENUM_CLASS(objDesc.eLevel),
					TEXT("Layer_MapObject_Inst"),
					ENUM_CLASS(curLevel),
					TEXT("Prototype_GameObject_Prop_Static"),
					&objDesc // 캡처된 값의 주소 -> 안전
				),
				E_FAIL
			);
			});
	}

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
	// Dat 기본 경로
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
		break;
	case KHAZAN_MAP::YETUGA:
		pDataFilePath += TEXT("Yetuga/");
		break;
	case KHAZAN_MAP::THECREVICE:
		pDataFilePath += TEXT("TheCrevice/");
		break;
	case KHAZAN_MAP::EMBARS:
		pDataFilePath += TEXT("Embars/");
		break;
	case KHAZAN_MAP::VIPER:
		pDataFilePath += TEXT("Viper/");
		break;
	default:
		break;
	}

	pDataFilePath += pDataFileName;

	pDataFilePath += TEXT("_lights.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL(INVALID_HANDLE_VALUE, hFile, E_FAIL);

	// 1. 조명의 총 개수
	_uint iLightCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iLightCnt, sizeof(_uint), &dwByte, nullptr), false);

	// 조명 총 개수만큼 순회
	for (_uint i = 0; i < iLightCnt; ++i)
	{
		LIGHT_DESC LightDesc = {};

		// 2. 조명 태그 길이 불러오기
		_uint iLightTagLen = {};
		CHECK_FALSE(ReadFile(hFile, &iLightTagLen, sizeof(_uint), &dwByte, nullptr), false);

		// 3. 조명 태그 이름 불러오기
		_tchar szLightTag[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szLightTag, sizeof(_tchar) * iLightTagLen, &dwByte, nullptr), false);

		// 4. 조명 구조체 불러오기
		CHECK_FALSE(ReadFile(hFile, &LightDesc, sizeof(LIGHT_DESC), &dwByte, nullptr), false);

		// 조명 등록
		m_pGameInstance->Add_FireTask([this, szLightTag = szLightTag, eCurrentLevel = ENUM_CLASS(eCurrentLevel), LightDesc = LightDesc]() mutable {
			m_pGameInstance->Add_Light(szLightTag, ENUM_CLASS(eCurrentLevel), LightDesc, true);
			return S_OK;
			});

	}

	CloseHandle(hFile);

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
