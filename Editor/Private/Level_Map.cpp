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
	SetWindowText(g_hWnd, TEXT("¸ÊÅø"));

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
	string strModelName = {};
	CProp_Static::PROP_STATIC_DESC StaticDesc = {};

	lstrcpy(StaticDesc.szModelName, TEXT("Prototype_Component_Model_"));
	lstrcat(StaticDesc.szModelName, TEXT("Bantu_LightObj"));

	strModelName = "Bantu_LightObj";
	m_StaticModels.push_back(strModelName);

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Static"), &StaticDesc), E_FAIL);

	lstrcpy(StaticDesc.szModelName, TEXT("Prototype_Component_Model_"));
	lstrcat(StaticDesc.szModelName, TEXT("Bantu_Brick"));

	strModelName = "Bantu_Brick";
	m_StaticModels.push_back(strModelName);

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Static"), &StaticDesc), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_DefaultImGui_For_MapTool()
{
	CHECK_FAILED(Ready_Main_Window(), E_FAIL);

	CHECK_FAILED(Ready_Prop_Edit_Window(), E_FAIL);

	CHECK_FAILED(Ready_Json_Edit_Window(), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Main_Window()
{
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isMainWindow)
		{
			ImGui::Begin("MAIN WINDOW", &m_isMainWindow, ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::Button("STATIC")) m_isPropWindow[ENUM_CLASS(PROP_SPECIES::STATIC)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::STATIC)];
			SAMELINE;
			if (ImGui::Button("ANIMATED")) m_isPropWindow[ENUM_CLASS(PROP_SPECIES::ANIMATED)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::ANIMATED)];
			SAMELINE;
			if (ImGui::Button("INTERACTIVE")) m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)];
			SAMELINE;
			if (ImGui::Button("DESTRUCTIBLE")) m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)];
			SEPARATOR;

			ImGui::Text("INFORMATION");
			ImGui::Text("Hi-ing");

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
				for (_uint i = 0; i < m_StaticModels.size(); ++i)
				{
					_bool isSelectPrototype = (m_iStatIndex == i);
					if (ImGui::Selectable(m_StaticModels[i].c_str(), isSelectPrototype))
					{
						m_iStatIndex = i;

						//m_pProp_Static = dynamic_cast<CProp_Static*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Prop_Static"), m_iStatIndex));
						//CHECK_NULLPTR_MSG(m_pProp_Static, TEXT("pProp_Static == nullptr"), );
					}
				}

				ImGui::EndListBox();
			}

			ImGui::Text("CURRENT INDEX : %d", m_iStatIndex);

			//ImGui::Text("PATH : "); ImGui::SameLine();
			//ImGui::InputText("##jsoninputtext", m_szJsonFile, IM_ARRAYSIZE(m_szJsonFile));

			//ImGui::Text("JSON PATH : %s", m_szJsonFile);

			ImGui::Text("================##0");

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
#pragma region JSON CONVERT FOR ME
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		ImGui::Begin("JSON", &m_isJsonWindow, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Text("JSON FILE NAME : ");
		SAMELINE; ITEMWIDTH(300.f);
		ImGui::InputText("##json_file_name", m_szJsonFile, IM_ARRAYSIZE(m_szJsonFile));

		SAMELINE;
		if (ImGui::Button("LOAD##json"))
		{
			string strPath = "../../Client/Bin/Resources/Models/Prop/Json/";

			strPath += m_szJsonFile;
			strPath += ".json";

			ifstream ifs(strPath);

			if (true == ifs.is_open())
			{
				ifs >> m_Json;
				ifs.close();

				m_isJsonOpened = true;
			}
			else
			{
				MSG_BOX(TEXT("JSON ¸ø¿­À½"));
			}

		} SEPARATOR;

		ImGui::End();
		});
#pragma endregion

#pragma region JSON OPENED	
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		ImGui::Begin("MAIN WINDOW", &m_isJsonOpened, ImGuiWindowFlags_AlwaysAutoResize);

		if (true == m_isJsonOpened)
		{
			ImGui::Text("%s", m_szJsonFile);

			if (ImGui::Button("SET MAPOBJECT"))
			{
				for (auto& Component : m_Json)
				{
					if (Component["Type"] == "FoliageInstancedStaticMeshComponent")
					{
						string strMeshName = Component["Properties"]["StaticMesh"]["ObjectName"];

						size_t start = strMeshName.find('\'');
						size_t end = strMeshName.rfind('\'');

						if (start != std::string::npos && end != std::string::npos && end > start)
							strMeshName = strMeshName.substr(start + 1, end - start - 1);

						m_StaticModels.push_back(strMeshName);

						auto& instances = Component["PerInstanceSMData"];
						for (auto& inst : instances)
						{
							auto pos = inst["TransformData"]["Translation"];
							auto scale = inst["TransformData"]["Scale3D"];
							auto rot = inst["TransformData"]["Rotation"];
							ImGui::Text("Pos: %.2f %.2f %.2f", (float)pos["X"], (float)pos["Y"], (float)pos["Z"]);
							ImGui::Text("Scale: %.2f", (float)scale["X"]);
							ImGui::Text("Rot(Quat): %.3f %.3f %.3f %.3f", (float)rot["X"], (float)rot["Y"], (float)rot["Z"], (float)rot["W"]);
							ImGui::Separator();
						}
					}
				}
			}
		}

		ImGui::End();
		});
#pragma endregion

	return S_OK;
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
