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

	MapDesc.fFar = 10000.f;
	MapDesc.fNear = 0.1f;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Camera_Map"), &MapDesc), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Prop_Static(const _wstring& strLayerTag)
{


	return S_OK;
}

HRESULT CLevel_Map::Ready_Temp_Prototypes()
{
	for (auto& Component : m_CustomJson)
	{
		_bool isInstance = (_bool)Component["isInstance"];

		_bool isObject = (_bool)Component["isObject"];

		wstring strModelName = AnsiToWString(Component["strModelName"]);
				
		if (true == isInstance)				// 인스턴싱 모델인 경우
		{
			_uint iNumInstances = (_uint)Component["iNumInstances"];

			CModelMesh_Instance::MODELMESH_INSTANCE_DESC ModelMeshDesc = {};
			ModelMeshDesc.InstanceData.reserve(iNumInstances);

			auto& position = Component["vInstancePosition"];
			auto& scale = Component["vInstanceScale"];
			auto& quat = Component["vInstanceQuaternion"];

			for (_uint i = 0; i < iNumInstances; ++i)
			{
				MESH_INSTANCE_DATA MeshInstanceData = {};

				FLOAT3_DATA vPos = FLOAT3_DATA((_float)position[i]["x"], (_float)position[i]["y"], (_float)position[i]["z"]);
				FLOAT3_DATA vScale = FLOAT3_DATA((_float)scale[i]["x"], (_float)scale[i]["y"], (_float)scale[i]["z"]);
				FLOAT4_DATA vQuaternion = FLOAT4_DATA((_float)quat[i]["x"], (_float)quat[i]["y"], (_float)quat[i]["z"], (_float)quat[i]["w"]);

				XMMATRIX matScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
				XMMATRIX matRot = XMMatrixRotationQuaternion(XMVectorSet(vQuaternion.x, vQuaternion.y, vQuaternion.z, vQuaternion.w));
				XMMATRIX matTrans = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

				MeshInstanceData.InstanceMatrix = XMMatrixTranspose(matScale * matRot * matTrans);

				ModelMeshDesc.InstanceData.push_back(MeshInstanceData);
			}

			ModelMeshDesc.iNumInstance = iNumInstances;

			_matrix PreTransformMatrix = XMMatrixIdentity();

			//CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), strModelName,
				//CModel_Instance::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, "../../Client/Bin/Resources/Models/Prop/Static/Plant/WP_VFS_Plants_Dry_008.fbx", &ModelMeshDesc, PreTransformMatrix)), E_FAIL);

			return S_OK;
		}
		else if (true == isObject)				// 단일 오브젝트인 경우
		{
			_matrix PreTransformMatrix = XMMatrixIdentity();

			//CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), strModelName,
				//CModel::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, "strModelName과 일치하는 경로", PreTransformMatrix)), E_FAIL);
		}
		else
		{
			MSG_BOX(TEXT("있어서는 안되는 else"));
		}
	}

	return S_OK;
}

HRESULT CLevel_Map::Ready_Temp_Layers(const _wstring& strLayerTag)
{
	CProp_Static::PROP_STATIC_DESC StaticDesc = {};
	//StaticDesc.szModelName = TEXT("WP_VFS_Plants_Dry_008");

	//CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		//ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Static")), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_DefaultImGui_For_MapTool()
{
	CHECK_FAILED(Ready_Main_Window(), E_FAIL);

	CHECK_FAILED(Ready_CustomJson_Edit_Window(), E_FAIL);

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

			if (ImGui::Button("JSON TO CUSTOM"))
			{
				Get_Directory_Files(m_szJsonPath);

				m_isJsonWindow = !m_isJsonWindow;
			}
			SAMELINE;
			if (ImGui::Button("CUSTOMJSON WINDOW"))
			{
				Get_Directory_Files(m_szJsonCustomPath);

				m_isCustomJsonWindow = !m_isCustomJsonWindow;
			}

			SEPARATOR;
			if (ImGui::Button("STATIC"))		m_isPropWindow[ENUM_CLASS(PROP_SPECIES::STATIC)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::STATIC)];
			SAMELINE;
			if (ImGui::Button("ANIMATED"))		m_isPropWindow[ENUM_CLASS(PROP_SPECIES::ANIMATED)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::ANIMATED)];
			SAMELINE;
			if (ImGui::Button("INTERACTIVE"))	m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)];
			SAMELINE;
			if (ImGui::Button("DESTRUCTIBLE"))	m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)];
			SEPARATOR;

			ImGui::Text("ETC FUNC");

			if (ImGui::Button("CLEAR JSON LIST"))
			{
				m_isJsonExport = false;
				m_isJsonConverted = false;

				m_JsonList.clear();
				m_JsonFiles.clear();

				m_iJsonFilesIndex = 0;
				m_iJsonListIndex = 0;
			}

			ImGui::End();
		}
		});

	return S_OK;
}

HRESULT CLevel_Map::Ready_CustomJson_Edit_Window()
{
#pragma region WIDGET : CumtomJson 용 위젯

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isCustomJsonWindow)
		{
			ImGui::Begin("CUSTOM JSON WINDOW", &m_isCustomJsonWindow, ImGuiWindowFlags_AlwaysAutoResize);

#pragma region CustomJson 로드
			
			ImGui::Text("CUSTOM JSON LOAD PATH : ");
			SAMELINE; ITEMWIDTH(350.f);
			ImGui::InputText("##json_custom_path", m_szJsonCustomPath, IM_ARRAYSIZE(m_szJsonCustomPath));

			ImGui::Text("MAP NAME : "); SAMELINE;

			if (ImGui::Button("HEINMACH"))
			{
				m_eMapType = MAPEDIT_MAP::HEINMACH;

				Get_Directory_Files(m_szJsonCustomPath);
			} SAMELINE;
			if (ImGui::Button("STORMPASS"))
			{
				m_eMapType = MAPEDIT_MAP::STORMPASS;

				Get_Directory_Files(m_szJsonCustomPath);
			} SAMELINE;
			if (ImGui::Button("THECREVICE"))
			{
				m_eMapType = MAPEDIT_MAP::THECREVICE;

				Get_Directory_Files(m_szJsonCustomPath);
			} SAMELINE;
			if (ImGui::Button("EMBARS"))
			{
				m_eMapType = MAPEDIT_MAP::EMBARS;

				Get_Directory_Files(m_szJsonCustomPath);
			}
			ImGui::Text("JSON FILE NAME : ");
			SAMELINE;

			vector<const _char*> JsonFileNames;
			JsonFileNames.reserve(m_JsonFiles.size());
			for (auto& String : m_JsonFiles)
				JsonFileNames.push_back(String.c_str());

			ImGui::Combo("##json_file_list", &m_iJsonFilesIndex, JsonFileNames.data(), static_cast<_int>(m_JsonFiles.size()));

			SAMELINE;
			if (ImGui::Button("LOAD##customjson"))
			{
				string strPath = m_szJsonCustomPath;

				strPath += m_szJsonFolderPath[ENUM_CLASS(m_eMapType)];
				strPath += m_JsonFiles[m_iJsonFilesIndex];
				strPath += ".json";

				ifstream ifs(strPath);

				if (true == ifs.is_open())
				{
					ifs >> m_CustomJson;
					ifs.close();

					m_isCustomJsonLoaded = true;
				}
				else
					ifs.close();
			}

#pragma endregion

#pragma region CustomJson을 이용한 프로토 타입 및 레이어 생성

			if (true == m_isCustomJsonLoaded)
			{
				if (ImGui::Button("CREATE PROTOTYPES"))
					Ready_Temp_Prototypes();

				if (ImGui::Button("CREATE LAYERS"))
					Ready_Temp_Layers(TEXT("Layer_MapObject"));
			}

#pragma endregion

			ImGui::End();
		}
		});

#pragma endregion

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

				ImGui::EndListBox();
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
#pragma region WIDGET : JSON TO CUSTOM

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isJsonWindow)
		{
			ImGui::Begin("JSON LOAD", &m_isJsonWindow, ImGuiWindowFlags_AlwaysAutoResize);

#pragma region JSON 로드

			ImGui::Text("DEFAULT LOAD PATH : ");
			SAMELINE; ITEMWIDTH(350.f);
			ImGui::InputText("##json_default_path", m_szJsonPath, IM_ARRAYSIZE(m_szJsonPath));

			ImGui::Text("MAP NAME : "); SAMELINE;

			if (ImGui::Button("HEINMACH"))
			{
				m_eMapType = MAPEDIT_MAP::HEINMACH;

				Get_Directory_Files(m_szJsonPath);
			} SAMELINE;
			if (ImGui::Button("STORMPASS"))
			{
				m_eMapType = MAPEDIT_MAP::STORMPASS;

				Get_Directory_Files(m_szJsonPath);
			} SAMELINE;
			if (ImGui::Button("THECREVICE"))
			{
				m_eMapType = MAPEDIT_MAP::THECREVICE;

				Get_Directory_Files(m_szJsonPath);
			} SAMELINE;
			if (ImGui::Button("EMBARS"))
			{
				m_eMapType = MAPEDIT_MAP::EMBARS;

				Get_Directory_Files(m_szJsonPath);
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
				string strPath = m_szJsonPath;

				strPath += m_szJsonFolderPath[ENUM_CLASS(m_eMapType)];
				strPath += m_JsonFiles[m_iJsonFilesIndex];
				strPath += ".json";

				ifstream ifs(strPath);

				if (true == ifs.is_open())
				{
					ifs >> m_Json;
					ifs.close();

#pragma region MOVE
					for (auto& Component : m_Json)
					{
						string strModelName = {};

						_uint iNumInstance = {};

						size_t start = {};
						size_t end = {};

						string Type = Component["Type"];

						_bool isContain = Component.contains("Properties") &&
							Component["Properties"].contains("StaticMesh") &&
							Component["Properties"]["StaticMesh"].contains("ObjectName") &&
							Component["Properties"]["StaticMesh"]["ObjectName"].is_string();

						if (false == isContain)
							continue;

						if (Type != "FoliageInstancedStaticMeshComponent" && Type != "xxStaticMeshComponent")
							continue;

						strModelName = Component["Properties"]["StaticMesh"]["ObjectName"];

						start = strModelName.find('\'');
						end = strModelName.rfind('\'');

						if (start != string::npos && end != string::npos && end > start)
							strModelName = strModelName.substr(start + 1, end - start - 1);

						auto iter = find_if(m_JsonList.begin(), m_JsonList.end(), [&](const JSON_MAP_DATA& CheckData) {
							return CheckData.strModelName == strModelName;
							});

						if (iter == m_JsonList.end() || Type == "xxStaticMeshComponent")
						{
							JSON_MAP_DATA NewData = {};
							NewData.strModelName = strModelName;
							m_JsonList.push_back(NewData);
							iter = prev(m_JsonList.end());
						}

						JSON_MAP_DATA& Data = *iter;

						if (Component["Type"] == "FoliageInstancedStaticMeshComponent")
						{
							auto& Instances = Component["PerInstanceSMData"];
							for (auto& Instance : Instances)
							{
								++Data.iNumInstances;

								auto pos = Instance["TransformData"]["Translation"];
								auto scale = Instance["TransformData"]["Scale3D"];
								auto quat = Instance["TransformData"]["Rotation"];

								FLOAT3_DATA vPos = FLOAT3_DATA((_float)pos["X"], (_float)pos["Y"], (_float)pos["Z"]);
								FLOAT3_DATA vScale = FLOAT3_DATA((_float)scale["X"], (_float)scale["Y"], (_float)scale["Z"]);
								FLOAT4_DATA vQuat = FLOAT4_DATA((_float)quat["X"], (_float)quat["Y"], (_float)quat["Z"], (_float)quat["W"]);

								Data.vInstancePosition.push_back(vPos);
								Data.vInstanceScale.push_back(vScale);
								Data.vInstanceQuaternion.push_back(vQuat);
							}

							Data.isInstance = true;
						}
						else if (Component["Type"] == "xxStaticMeshComponent")
						{
							auto Prop = Component["Properties"];

							_bool isInvalid = { false };

							if (!Prop.contains("RelativeLocation") && !Prop["RelativeLocation"].contains("X") && !Prop["RelativeLocation"]["X"].is_number())
								isInvalid = true;
							if (!Prop.contains("RelativeScale3D") && !Prop["RelativeScale3D"].contains("X") && !Prop["RelativeScale3D"]["X"].is_number())
								isInvalid = true;
							if (!Prop.contains("RelativeRotation") && !Prop["RelativeRotation"].contains("Pitch") && !Prop["RelativeRotation"]["Pitch"].is_number())
								isInvalid = true;

							if (true == isInvalid)
							{
								m_JsonList.pop_back();
								continue;
							}

							auto pos = Component["Properties"]["RelativeLocation"];
							auto scale = Component["Properties"]["RelativeScale3D"];
							auto rot = Component["Properties"]["RelativeRotation"];

							FLOAT3_DATA vPos = FLOAT3_DATA((_float)pos["X"], (_float)pos["Y"], (_float)pos["Z"]);
							FLOAT3_DATA vScale = FLOAT3_DATA((_float)scale["X"], (_float)scale["Y"], (_float)scale["Z"]);
							FLOAT3_DATA vRot = FLOAT3_DATA((_float)rot["Pitch"], (_float)rot["Yaw"], (_float)rot["Roll"]);

							Data.vPosition = vPos;
							Data.vScale = vScale;
							Data.vRotation = vRot;

							Data.isObject = true;
						}
					}

					m_isJsonConverted = true;
#pragma endregion

				}
				else
					ifs.close();

			} SEPARATOR;

#pragma endregion

			ImGui::End();
		}
		});

#pragma endregion

	return S_OK;
}

HRESULT CLevel_Map::Ready_Json_List_Window()
{
#pragma region WIDGET : JSON 에서 로드해온 리스트

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isJsonConverted)
		{
			ImGui::Begin("JSON LIST", &m_isJsonConverted, ImGuiWindowFlags_AlwaysAutoResize);

			_uint iJsonListSize = m_JsonList.size();

			ImGui::Text("LIST COUNT : %d", iJsonListSize);

			_uint iInstanceCnt = {};
			_uint iTotalInstanceCnt = {};
			_uint iObjCnt = {};

			for (_uint i = 0; i < iJsonListSize; ++i)
			{
				if (true == m_JsonList[i].isInstance)
				{
					++iInstanceCnt;

					iTotalInstanceCnt += m_JsonList[i].iNumInstances;
				}
				if (true == m_JsonList[i].isObject)
					++iObjCnt;
			}
			SEPARATOR;
			ImGui::Text("INSTANCE MODEL COUNT : %d", iInstanceCnt);

			ImGui::Text("TOTAL INSTANCE COUNT : %d", iTotalInstanceCnt);
			SEPARATOR;
			ImGui::Text("INDEPENDENT OBJECT COUNT : %d", iObjCnt);

			_uint iInstanceID = {};
			_uint iObjectID = {};

			if (ImGui::BeginListBox("##prop_static_list"))
			{
				for (_uint i = 0; i < m_JsonList.size(); ++i)
				{
					_bool isSelected = (m_iJsonListIndex == i);

					string strModelName = "ID-%d:";

					strModelName += m_JsonList[i].strModelName;

					_char szModelName[MAX_PATH] = {};

					sprintf_s(szModelName, strModelName.c_str(), i);

					if (ImGui::Selectable(szModelName, isSelected))
						m_iJsonListIndex = i;
				}

				ImGui::EndListBox();
			} SEPARATOR;

			if (ImGui::Button("EXPORT"))
				m_isJsonExport = !m_isJsonExport;

			ImGui::End();
		}
		});

#pragma endregion

#pragma region WIDGET : JSON 에서 로드해온 리스트들의 정보

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isJsonConverted)
		{
			ImGui::Begin("LIST INFO", &m_isJsonConverted, ImGuiWindowFlags_AlwaysAutoResize);

			if (0 == m_JsonList.size())
			{

			}
			else
			{
				ImGui::Text("MODEL NAME\n%s", m_JsonList[m_iJsonListIndex].strModelName.c_str());
				SEPARATOR;
				if (true == m_JsonList[m_iJsonListIndex].isInstance)
				{
					_uint iInstCnt = m_JsonList[m_iJsonListIndex].iNumInstances;
					ImGui::Text("INSTANCE COUNT : %d", iInstCnt);
					SEPARATOR;
					for (_uint i = 0; i < iInstCnt; ++i)
					{
						FLOAT3_DATA vPos = m_JsonList[m_iJsonListIndex].vInstancePosition[i];
						FLOAT3_DATA vScale = m_JsonList[m_iJsonListIndex].vInstanceScale[i];
						FLOAT4_DATA vQuat = m_JsonList[m_iJsonListIndex].vInstanceQuaternion[i];
						ImGui::Text("POSITION\nX : %.3f\nY : %.3f\nZ : %.3f", vPos.x, vPos.y, vPos.z);
						ImGui::Text("SCALE\nX : %.3f\nY : %.3f\nZ : %.3f", vScale.x, vScale.y, vScale.z);
						ImGui::Text("QUATERNION\nX : %.3f\nY : %.3f\nZ : %.3f\nW : %.3f", vQuat.x, vQuat.y, vQuat.z, vQuat.w);
						SEPARATOR;

					}
				}
				else if (true == m_JsonList[m_iJsonListIndex].isObject)
				{
					ImGui::Text("INDEPENDENT OBJECT");
					SEPARATOR;
					FLOAT3_DATA vPos = m_JsonList[m_iJsonListIndex].vPosition;
					FLOAT3_DATA vScale = m_JsonList[m_iJsonListIndex].vScale;
					FLOAT3_DATA vRot = m_JsonList[m_iJsonListIndex].vRotation;
					ImGui::Text("POSITION\nX : %.3f\nY : %.3f\nX : %.3f", vPos.x, vPos.y, vPos.z);
					ImGui::Text("SCALE\nX : %.3f\nY : %.3f\nX : %.3f", vScale.x, vScale.y, vScale.z);
					ImGui::Text("ROTATION\nPitch : %.3f\nYaw : %.3f\nRoll : %.3f", vRot.x, vRot.y, vRot.z);
					SEPARATOR;
				}
			}

			ImGui::End();
		}
		});

#pragma endregion

#pragma region WIDGET : JSON 에서 로드해온 리스트들을 CustomJson 으로 추출

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isJsonExport)
		{
			ImGui::Begin("CUSTOM JSON EXPORT", &m_isJsonExport, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("DEFAULT SAVE PATH : ");
			SAMELINE; ITEMWIDTH(350.f);
			ImGui::InputText("##json_default_save_path", m_szJsonCustomPath, IM_ARRAYSIZE(m_szJsonCustomPath));

			ImGui::Text("JSON FILE NAME : ");
			SAMELINE;
			ImGui::InputText("##json_save_name", m_szJsonSaveName, IM_ARRAYSIZE(m_szJsonSaveName));

			SAMELINE;
			if (ImGui::Button("SAVE##json"))
			{
				string strPath = m_szJsonCustomPath;

				strPath += m_szJsonFolderPath[ENUM_CLASS(m_eMapType)];
				strPath += m_szJsonSaveName;
				//strPath += m_JsonFiles[m_iJsonFilesIndex];
				strPath += ".json";

				ofstream ofs(strPath);

				if (true == ofs.is_open())
				{
					JSON OutJson = m_JsonList;

					ofs << OutJson.dump(4);

					ofs.close();

					if (false == ofs.good())
						MSG_BOX(TEXT("ofs 낫 굿"));
				}
				else
					ofs.close();

			} SEPARATOR;

			ImGui::End();
		}
		});

#pragma endregion

	return S_OK;
}

void CLevel_Map::Get_Directory_Files(const _char* pDirectoryPath)
{
	m_JsonFiles.clear();

	string strPath = pDirectoryPath;

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

void CLevel_Map::Load_ModelFolders(const string& strDefaultPath)
{
	try
	{
		for (const auto& entry : filesystem::directory_iterator(strDefaultPath))
		{
			if (entry.is_directory())
			{
				string strFolderName = entry.path().filename().string();

				int a = 10;
			}
		}
	}
	catch (const filesystem::filesystem_error& e)
	{
		wstring wMsg =
			L"파일 시스템 에러 발생!\n\n" +
			std::wstring(e.what(), e.what() + strlen(e.what())) + L"\n\n" +
			L"경로 1: " + e.path1().wstring() + L"\n" +
			(e.path2().empty() ? L"" : (L"경로 2: " + e.path2().wstring() + L"\n")) +
			L"오류 코드: " + std::to_wstring(e.code().value()) + L" (" +
			std::wstring(e.code().message().begin(), e.code().message().end()) + L")";

		MessageBox(nullptr, wMsg.c_str(), L"파일 시스템 예외", MB_ICONERROR | MB_OK);
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
