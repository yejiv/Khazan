#include "Level_Map.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "MapEditor_Header.h"

#define SAMELINE ImGui::SameLine()
#define ITEMWIDTH(_float) ImGui::SetNextItemWidth(_float)
#define SEPARATOR ImGui::Separator()

CLevel_Map::CLevel_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Map::Initialize()
{
	CHECK_FAILED(Ready_Lights(), E_FAIL);

	CHECK_FAILED(Ready_Layer_Camera(TEXT("Layer_Camera_Map")), E_FAIL);

	// CHECK_FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);

	// CHECK_FAILED(Ready_Layer_Prop_Static(TEXT("Layer_Prop_Static")), E_FAIL);

	CHECK_FAILED(Ready_DefaultImGui_For_MapTool(), E_FAIL);

	return S_OK;
}

void CLevel_Map::Update(_float fTimeDelta)
{

	return;
}

HRESULT CLevel_Map::Render()
{
	SetWindowText(g_hWnd, TEXT("맵툴"));

	return S_OK;
}

HRESULT CLevel_Map::Ready_Lights()
{
	LIGHT_DESC LightDesc = {};

	LightDesc.eType = LIGHT_DESC::DIRECTIONAL;

	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Map::CAMERA_MAP_DESC MapDesc = {};

	MapDesc.fSpeedPerSec = 5.f;
	MapDesc.fRotationPerSec = XMConvertToRadians(30.f);

	MapDesc.fMouseSensor = 0.2f;

	MapDesc.fFovy = XMConvertToRadians(45.f);

	MapDesc.vEye = _float4(0.f, 5.f, 0.f, 1.f);
	MapDesc.vAt = _float4(0.f, 5.f, 1.f, 1.f);

	MapDesc.fFar = 1000.f;
	MapDesc.fNear = 0.1f;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Camera_Map"), &MapDesc), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Prop_Static(const _wstring& strLayerTag)
{
	/*
	string strModelName = {};
	CProp_Static::PROP_STATIC_DESC StaticDesc = {};

	lstrcpy(StaticDesc.szModelName, TEXT("Prototype_Component_Model_"));
	lstrcat(StaticDesc.szModelName, TEXT("Bantu_LightObj"));

	strModelName = "Bantu_LightObj";
	m_JsonList.push_back(strModelName);

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Static"), &StaticDesc), E_FAIL);

	lstrcpy(StaticDesc.szModelName, TEXT("Prototype_Component_Model_"));
	lstrcat(StaticDesc.szModelName, TEXT("Bantu_Brick"));

	strModelName = "Bantu_Brick";
	m_JsonList.push_back(strModelName);

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Static"), &StaticDesc), E_FAIL);
	*/
	return S_OK;
}

HRESULT CLevel_Map::Ready_DefaultImGui_For_MapTool()
{
	Get_Directory_Files();

	CHECK_FAILED(Ready_Main_Window(), E_FAIL);

	CHECK_FAILED(Ready_Prop_Edit_Window(), E_FAIL);

	CHECK_FAILED(Ready_Json_Edit_Window(), E_FAIL);
	
	CHECK_FAILED(Ready_Json_List_Window(), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Main_Window()
{
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isMainWindow)
		{
			ImGui::Begin("MAIN WINDOW", &m_isMainWindow, ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::Button("STATIC"))		m_isPropWindow[ENUM_CLASS(PROP_SPECIES::STATIC)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::STATIC)];
			SAMELINE;
			if (ImGui::Button("ANIMATED"))		m_isPropWindow[ENUM_CLASS(PROP_SPECIES::ANIMATED)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::ANIMATED)];
			SAMELINE;
			if (ImGui::Button("INTERACTIVE"))	m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)];
			SAMELINE;
			if (ImGui::Button("DESTRUCTIBLE"))	m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)];
			SEPARATOR;

			ImGui::Text("INFORMATION");

			ImGui::Text("Hi-inh");

			ImGui::End();
		}
		});

	return S_OK;
}

HRESULT CLevel_Map::Ready_Prop_Edit_Window()
{
#pragma region PROP_STATIC EDIT WINDOW
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPropWindow[ENUM_CLASS(PROP_SPECIES::STATIC)])
		{
			ImGui::Begin("PROP STATIC WINDOW", &m_isPropWindow[ENUM_CLASS(PROP_SPECIES::STATIC)], ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::BeginListBox("##prop_static_list"))
			{
				/*
				for (_uint i = 0; i < m_JsonList.size(); ++i)
				{
					_bool isSelectPrototype = (m_iJsonListIndex == i);
					if (ImGui::Selectable(m_JsonList[i].strModelName.c_str(), isSelectPrototype))
					{
						m_iJsonListIndex = i;

						ImGui::Text("NUM INSTANCE : %d", m_JsonList[i].iNumInstances);

						//m_pProp_Static = dynamic_cast<CProp_Static*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Prop_Static"), m_iJsonListIndex));
						//CHECK_NULLPTR_MSG(m_pProp_Static, TEXT("pProp_Static == nullptr"), );
					}
				}
				*/
				ImGui::EndListBox();
			}

			//ImGui::Text("PATH : "); ImGui::SameLine();
			//ImGui::InputText("##jsoninputtext", m_szJsonFile, IM_ARRAYSIZE(m_szJsonFile));

			//ImGui::Text("JSON PATH : %s", m_szJsonFile);

			SEPARATOR;

			if (ImGui::Button("ADD INSTANCE"))
			{
				MESH_INSTANCE_DATA AddInstanceData = {};

				AddInstanceData.InstanceMatrix = XMMatrixIdentity();

				AddInstanceData.InstanceMatrix.r[3] = XMVectorSetW(XMLoadFloat4(m_pGameInstance->Get_CamPosition()), 1.f);
				AddInstanceData.InstanceID = 0;

				m_pProp_Static->Add_Instance(AddInstanceData);
			} SAMELINE;
			if (ImGui::Button("FIX INSTANCE"))
			{

			}

			ImGui::End();
		}
		});
#pragma endregion

#pragma region PROP_ANIMATED EDIT WINDOW
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPropWindow[ENUM_CLASS(PROP_SPECIES::ANIMATED)])
		{
			ImGui::Begin("PROP ANIMATED WINDOW", &m_isPropWindow[ENUM_CLASS(PROP_SPECIES::ANIMATED)], ImGuiWindowFlags_AlwaysAutoResize);

			//ImGui::Text("== PROP_ANIMATED_LIST");

			ImGui::End();
		}
		});
#pragma endregion

#pragma region PROP_INTERACTIVE EDIT WINDOW
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)])
		{
			ImGui::Begin("PROP INTERACTIVE WINDOW", &m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)], ImGuiWindowFlags_AlwaysAutoResize);

			//ImGui::Text("== PROP_INTERACTIVE_LIST");

			ImGui::End();
		}
		});
#pragma endregion

#pragma region PROP_DESTRUCTIBLE EDIT WINDOW
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)])
		{
			ImGui::Begin("PROP DESTRUCTIBLE WINDOW", &m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)], ImGuiWindowFlags_AlwaysAutoResize);

			//ImGui::Text("== PROP_DESTRUCTIBLE_LIST");

			ImGui::End();
		}
		});
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Map::Ready_Json_Edit_Window()
{
#pragma region JSON 컨버트
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		ImGui::Begin("JSON", &m_isJsonWindow, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Text("DEFAULT PATH : ");
		SAMELINE; ITEMWIDTH(350.f);
		ImGui::InputText("##json_default_path", m_szJsonDefaultPath, IM_ARRAYSIZE(m_szJsonDefaultPath));

		ImGui::Text("MAP NAME : "); SAMELINE;

		if (ImGui::Button("HEINMACH"))
		{
			m_eMapType = MAPEDIT_MAP::HEINMACH;
			
			Get_Directory_Files();
		} SAMELINE;
		if (ImGui::Button("STORMPASS"))
		{
			m_eMapType = MAPEDIT_MAP::STORMPASS;

			Get_Directory_Files();
		} SAMELINE;
		if (ImGui::Button("THECREVICE"))
		{
			m_eMapType = MAPEDIT_MAP::THECREVICE;

			Get_Directory_Files();
		} SAMELINE;
		if (ImGui::Button("EMBARS"))
		{
			m_eMapType = MAPEDIT_MAP::EMBARS;

			Get_Directory_Files();
		}
		ImGui::Text("JSON FILE NAME : ");
		SAMELINE;

		vector<const _char*> JsonFileNames;
		JsonFileNames.reserve(m_JsonFiles.size());
		for (auto& String : m_JsonFiles)
			JsonFileNames.push_back(String.c_str());

		ImGui::Combo("##json_file_list", &m_iJsonFilesIndex, JsonFileNames.data(), static_cast<_int>(m_JsonFiles.size()));

		//ImGui::InputText("##json_file_name", m_szJsonFile, IM_ARRAYSIZE(m_szJsonFile));

		SAMELINE;
		if (ImGui::Button("LOAD##json"))
		{
			m_isJsonConverted = false;
			m_JsonList.clear();

			string strPath = m_szJsonDefaultPath;

			strPath += m_szJsonFolderPath[ENUM_CLASS(m_eMapType)];
			strPath += m_JsonFiles[m_iJsonFilesIndex];
			strPath += ".json";

			ifstream ifs(strPath);

			if (true == ifs.is_open())
			{
				ifs >> m_Json;
				ifs.close();

				m_isJsonOpened = true;
			}
			else
				ifs.close();

		} SEPARATOR;

		// JSON 파일 열었을때
		if (true == m_isJsonOpened)
		{
			ImGui::Text("OPENED JSON : %s", m_JsonFiles[m_iJsonFilesIndex].c_str());

			if (ImGui::Button("JSON CONVERT"))
			{
				for (auto& Component : m_Json)
				{
					JSON_MAP_DATA Data = {};

					_uint iNumInstance = {};

					size_t start = {};
					size_t end = {};

					_bool isContain = Component.contains("Properties") && Component["Properties"].contains("StaticMesh") && Component["Properties"]["StaticMesh"].contains("ObjectName") && Component["Properties"]["StaticMesh"]["ObjectName"].is_string();

					if (Component["Type"] == "FoliageInstancedStaticMeshComponent")
					{
						if (true == isContain)
						{
							Data.strModelName = Component["Properties"]["StaticMesh"]["ObjectName"];

							start = Data.strModelName.find('\'');
							end = Data.strModelName.rfind('\'');

							if (start != string::npos && end != string::npos && end > start)
								Data.strModelName = Data.strModelName.substr(start + 1, end - start - 1);

							auto& instances = Component["PerInstanceSMData"];
							for (auto& inst : instances)
							{
								++iNumInstance;

								auto pos = inst["TransformData"]["Translation"];
								auto scale = inst["TransformData"]["Scale3D"];
								auto rot = inst["TransformData"]["Rotation"];

								_float3 vPos = _float3((_float)pos["X"], (_float)pos["Y"], (_float)pos["Z"]);
								_float3 vScale = _float3((_float)scale["X"], (_float)scale["Y"], (_float)scale["Z"]);
								_float4 vRot = _float4((_float)rot["X"], (_float)rot["Y"], (_float)rot["Z"], (_float)rot["W"]);

								Data.vInstancePos.push_back(vPos);
								Data.vInstanceScale.push_back(vScale);
								Data.vQuaternion.push_back(vRot);
							}

							Data.iNumInstances = iNumInstance;

							m_JsonList.push_back(Data);
						}
					}
					else if (Component["Type"] == "xxStaticMeshComponent")
					{
						if (isContain)
						{
							Data.strModelName = Component["Properties"]["StaticMesh"]["ObjectName"];

							start = Data.strModelName.find('\'');
							end = Data.strModelName.rfind('\'');

							if (start != string::npos && end != string::npos && end > start)
								Data.strModelName = Data.strModelName.substr(start + 1, end - start - 1);

							auto& instances = Component["PerInstanceSMData"];
							for (auto& inst : instances)
							{
								++iNumInstance;

								auto pos = inst["TransformData"]["Translation"];
								auto scale = inst["TransformData"]["Scale3D"];
								auto rot = inst["TransformData"]["Rotation"];

								Data.vInstancePos.push_back(_float3((_float)pos["X"], (_float)pos["Y"], (_float)pos["Z"]));
								Data.vInstanceScale.push_back(_float3((_float)scale["X"], (_float)scale["Y"], (_float)scale["Z"]));
								Data.vQuaternion.push_back(_float4((_float)pos["X"], (_float)pos["Y"], (_float)pos["Z"], (_float)pos["W"]));
							}

							Data.iNumInstances = iNumInstance;

							m_JsonList.push_back(Data);
						}
					}
				}

				m_isJsonConverted = true;
			}
		}

		ImGui::End();
		});
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Map::Ready_Json_List_Window()
{
		m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
			if (true == m_isJsonConverted)
			{
				ImGui::Begin("JSON LIST", &m_isJsonConverted, ImGuiWindowFlags_AlwaysAutoResize);

				for (_uint i = 0; i < m_JsonList.size(); ++i)
				{
					_bool isSelectPrototype = (m_iJsonListIndex == i);
					if (ImGui::Selectable(m_JsonList[i].strModelName.c_str(), isSelectPrototype))
						m_iJsonListIndex = i;
				}

				SEPARATOR;
				for (_uint i = 0; i < m_JsonList[m_iJsonFilesIndex].iNumInstances; ++i)
				{
					_float3 vPos = m_JsonList[m_iJsonListIndex].vInstancePos[i];
					_float3 vScale = m_JsonList[m_iJsonListIndex].vInstanceScale[i];
					_float4 vRot = m_JsonList[m_iJsonListIndex].vQuaternion[i];

					ImGui::Text("POSITION\nX : %.3f\nY : %.3f\nX : %.3f", vPos.x, vPos.y, vPos.z);
					ImGui::Text("SCALE\nX : %.3f\nY : %.3f\nX : %.3f", vScale.x, vScale.y, vScale.z);
					ImGui::Text("ROTATION\nX : %.3f\nY : %.3f\nX : %.3f", vRot.x, vRot.y, vRot.z);

				}

				ImGui::End();
			}
			});

	return S_OK;
}

void CLevel_Map::Get_Directory_Files()
{
	m_JsonFiles.clear();

	string strPath = m_szJsonDefaultPath;

	strPath += m_szJsonFolderPath[ENUM_CLASS(m_eMapType)];

	for (const auto& entry : std::filesystem::recursive_directory_iterator(strPath.c_str()))
	{
		if (!entry.is_regular_file())
			continue;

		if (entry.path().extension() != L".json")
			continue;

		// 파일 이름 (확장자 제거)
		const string strFileName = entry.path().stem().string(); // 예: Bar_Moxxis

		m_JsonFiles.push_back(strFileName);
	}
}

CLevel_Map* CLevel_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Map* pInstance = new CLevel_Map(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Map"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Map::Free()
{
	__super::Free();
}
