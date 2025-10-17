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
	CHECK_FAILED(Ready_Defaults(), E_FAIL);

	CHECK_FAILED(Ready_DefaultImGui_For_MapTool(), E_FAIL);

	return S_OK;
}

void CLevel_Map::Update(_float fTimeDelta)
{
	Clear_List();

	Select_Fix_Object(fTimeDelta);
	Select_Fix_Instance(fTimeDelta);
	Select_Add_LightPoint(fTimeDelta);

	return;
}

HRESULT CLevel_Map::Render()
{
	SetWindowText(g_hWnd, TEXT("맵 툴"));

	return S_OK;
}

HRESULT CLevel_Map::Ready_Defaults()
{
	CHECK_FAILED(Ready_Default_Lights(), E_FAIL);

	CHECK_FAILED(Ready_Layer_Camera(TEXT("Layer_Map_Camera")), E_FAIL);

	CHECK_FAILED(Ready_Layer_Terrain(TEXT("Layer_Map_Terrain")), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Default_Lights()
{
	// Shadow_Light
	SHADOW_LIGHT_DESC ShadowLightDesc{};
	ShadowLightDesc.vEye = _float4(-20.f, 20.f, -20.f, 1.f);
	ShadowLightDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	ShadowLightDesc.fFovy = XMConvertToRadians(60.f);
	ShadowLightDesc.fNear = 0.1f;
	ShadowLightDesc.fFar = 1000.f;

	CHECK_FAILED(m_pGameInstance->Ready_ShadowLight(ShadowLightDesc), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Map::CAMERA_MAP_DESC MapDesc = {};

	MapDesc.fSpeedPerSec = 10.f;
	MapDesc.fRotationPerSec = XMConvertToRadians(30.f);

	MapDesc.fMouseSensor = 0.2f;

	MapDesc.fFovy = XMConvertToRadians(45.f);

	MapDesc.vEye = _float4(0.f, 5.f, 0.f, 1.f);
	MapDesc.vAt = _float4(0.f, 5.f, 1.f, 1.f);

	MapDesc.fFar = 100000.f;
	MapDesc.fNear = 0.1f;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Camera_Map"), &MapDesc), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Map_Terrain")), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Prop_Static(const _wstring& strLayerTag)
{


	return S_OK;
}

HRESULT CLevel_Map::Add_Prototypes_FromJson()
{
	_matrix PreTransformMatrix = XMMatrixIdentity();

	// 스케일 변환 ( 1 / 100 )
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	for (auto& Component : m_CustomJson)
	{
		_bool isInstance = (_bool)Component["isInstance"];
		_bool isObject = (_bool)Component["isObject"];

		string strModelName = Component["strModelName"];
				
		if (true == isInstance)				// 인스턴싱 모델인 경우
		{
			// 모델명과 일치하는 경로 찾기
			string strLoadPath = Find_ModelPath(strModelName);

			if ("NOTFOUND" == strLoadPath)
			{
				string error = "Can't found load path\nModelName : " + strModelName;
				OutputDebugStringA(error.c_str());
				continue;
			}

			CEditor_ModelMesh_Instance::EDITOR_MODELMESH_INSTANCE_DESC InstanceDesc = {};

			InstanceDesc.iNumInstance = 0;

			// Instance 모델 프로토타입 등록
			CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), AnsiToWString(strModelName),
				CEditor_Model_Instance::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, strLoadPath.c_str(), PreTransformMatrix, &InstanceDesc)), E_FAIL);

			CProp_Static::PROP_STATIC_DESC StaticDesc = {};

			_wstring strTempName = AnsiToWString(strModelName);
			memcpy(StaticDesc.szModelName, strTempName.c_str(), sizeof(StaticDesc.szModelName));

			// Instance 는 바로 Layer 등록
			CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_InstObj"),
				ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Static"), &StaticDesc), E_FAIL);

			m_CheckPrototypes.emplace(strModelName, strLoadPath);
			m_Prototypes_Inst.push_back(strModelName);
		}
		else if (true == isObject)				// 단일 오브젝트인 경우
		{
			auto iter = m_CheckPrototypes.find(strModelName);

			if (iter == m_CheckPrototypes.end())
			{
				// 모델명과 일치하는 경로 찾기
				string strLoadPath = Find_ModelPath(strModelName);

				if ("NOTFOUND" == strLoadPath)
				{
					string error = "Can't found load path\nModelName : " + strModelName + "\n";
					OutputDebugStringA(error.c_str());
					continue;
				}

				// 단일 오브젝트는 바로 인스턴스 등록
				CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), AnsiToWString(strModelName),
					CEditor_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, strLoadPath.c_str(), PreTransformMatrix)), E_FAIL);

				m_CheckPrototypes.emplace(strModelName, strLoadPath);
				m_Prototypes_Obj.push_back(strModelName);
			}
		}
		else
		{
			MSG_BOX(TEXT("있어서는 안되는 else"));
		}
	}

	return S_OK;
}

HRESULT CLevel_Map::Convert_Json_To_Data()
{
	_matrix PreTransformMatrix = XMMatrixIdentity();

	// 스케일 변환 ( 1 / 100 )
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	for (auto& Component : m_CustomJson)
	{
		_bool isInstance = (_bool)Component["isInstance"];
		_bool isObject = (_bool)Component["isObject"];

		string strModelName = Component["strModelName"];

		if (true == isInstance)				// 인스턴싱 모델인 경우
		{
			// 모델명과 일치하는 경로 찾기
			string strLoadPath = Find_ModelPath(strModelName);

			if ("NOTFOUND" == strLoadPath)
			{
				string error = "Can't found load path\nModelName : " + strModelName;
				OutputDebugStringA(error.c_str());
				continue;
			}

			CEditor_ModelMesh_Instance::EDITOR_MODELMESH_INSTANCE_DESC InstanceDesc = {};

			InstanceDesc.iNumInstance = 0;

			// Instance 모델 프로토타입 등록
			CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), AnsiToWString(strModelName),
				CEditor_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, strLoadPath.c_str(), PreTransformMatrix)), E_FAIL);

			CProp_Export::PROP_EXPORT_DESC ExportDesc = {};

			_wstring strTempName = AnsiToWString(strModelName);
			memcpy(ExportDesc.szModelName, strTempName.c_str(), sizeof(ExportDesc.szModelName));

			ExportDesc.eLevel = LEVEL::MAP;

			// 추출할거는 바로 Layer 등록
			CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_ExportObj"),
				ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Export"), &ExportDesc), E_FAIL);

			m_CheckPrototypes.emplace(strModelName, strLoadPath);
			m_Prototypes_Inst.push_back(strModelName);
		}
		else if (true == isObject)				// 단일 오브젝트인 경우
		{
			auto iter = m_CheckPrototypes.find(strModelName);

			if (iter == m_CheckPrototypes.end())
			{
				// 모델명과 일치하는 경로 찾기
				string strLoadPath = Find_ModelPath(strModelName);

				if ("NOTFOUND" == strLoadPath)
				{
					string error = "Can't found load path\nModelName : " + strModelName + "\n";
					OutputDebugStringA(error.c_str());
					continue;
				}

				// 단일 오브젝트는 바로 인스턴스 등록
				CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), AnsiToWString(strModelName),
					CEditor_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, strLoadPath.c_str(), PreTransformMatrix)), E_FAIL);

				CProp_Export::PROP_EXPORT_DESC ExportDesc = {};

				_wstring strTempName = AnsiToWString(strModelName);
				memcpy(ExportDesc.szModelName, strTempName.c_str(), sizeof(ExportDesc.szModelName));

				ExportDesc.eLevel = LEVEL::MAP;

				// 추출할거는 바로 Layer 등록
				CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_ExportObj"),
					ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Export"), &ExportDesc), E_FAIL);

				m_CheckPrototypes.emplace(strModelName, strLoadPath);
				m_Prototypes_Obj.push_back(strModelName);
			}
		}
		else
		{
			MSG_BOX(TEXT("있어서는 안되는 else"));
		}
	}

	return S_OK;
}

void CLevel_Map::Clear_List()
{
	for (_uint i = 0; i < m_ObjectList.size(); )
	{
		if (nullptr == m_ObjectList[i])
		{
			swap(m_ObjectList[i], m_ObjectList.back());
			m_ObjectList.pop_back();
		}
		else
			++i;
	}
}

void CLevel_Map::Select_Fix_Object(_float fTimeDelta)
{
	if (true == m_isFixObjectWindow)
		return;

	if (m_pGameInstance->Key_Pressing(DIK_F1, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
	{
		_float3 vPosition = {};
		_uint iObjectID = {};

		if (m_pGameInstance->isPicked(&vPosition, &iObjectID))
		{
			for (auto& pObject : m_ObjectList)
			{
				if (nullptr != pObject)
				{
					if (iObjectID == pObject->Get_MapObjectID())
					{
						m_pFixPropObj = pObject;
						m_pFixTransformCom = static_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
						CHECK_NULLPTR_MSG(m_pFixTransformCom, TEXT("Fix Transform == nullptr"), );

						m_vFixScale = m_pFixTransformCom->Get_Scaled();
						XMStoreFloat3(&m_vFixPosition, m_pFixTransformCom->Get_State(STATE::POSITION));

						_vector vScale = {};
						_vector vRotation = {};
						_vector vTranslation = {};

						XMMatrixDecompose(&vScale, &vRotation, &vTranslation, m_pFixTransformCom->Get_WorldMatrix());

						XMStoreFloat3(&m_vFixRotation, vRotation);

						m_isFixObjectWindow = true;

						break;
					}
				}
			}

		}
	}
}

void CLevel_Map::Select_Fix_Instance(_float fTimeDelta)
{
	if (true == m_isFixObjectWindow)
		return;

	if (m_pGameInstance->Key_Pressing(DIK_F2, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
	{
		_float3 vPosition = {};
		_uint iInstanceID = {};

		if (m_pGameInstance->isPicked(&vPosition, &iInstanceID))
		{
			for (auto& pObject : m_ObjectList)
			{
				if (nullptr != pObject)
				{
					//if (iObjectID == pObject->Get_MapObjectID())
					//{
					//	m_pFixPropObj = pObject;
					//	m_pFixTransformCom = static_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
					//	CHECK_NULLPTR_MSG(m_pFixTransformCom, TEXT("Fix Transform == nullptr"), );

					//	m_vFixScale = m_pFixTransformCom->Get_Scaled();
					//	XMStoreFloat3(&m_vFixPosition, m_pFixTransformCom->Get_State(STATE::POSITION));
					//	//m_vFixRotation = m_pFixTransformCom->Get_Rotation_Quat();

					//	m_isFixObjectWindow = true;

					//	break;
					//}
				}
			}

		}
	}
}

void CLevel_Map::Select_Add_LightPoint(_float fTimeDelta)
{
	if (false == m_isLightSettingWindow)
		return;

	if (m_pGameInstance->Key_Pressing(DIK_F3, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
	{
		_float3 vPosition = {};

		if (m_pGameInstance->isPicked(&vPosition))
		{
			m_isAddLightPoint = true;
			m_vLightPoint = vPosition;
		}
	}
}

HRESULT CLevel_Map::Ready_DefaultImGui_For_MapTool()
{
	CHECK_FAILED(Ready_Main_Window(), E_FAIL);

	CHECK_FAILED(Ready_CustomJson_Edit_Window(), E_FAIL);

	CHECK_FAILED(Ready_CustomJson_List_Window(), E_FAIL);

	CHECK_FAILED(Ready_Prop_Edit_Window(), E_FAIL);

	CHECK_FAILED(Ready_Json_Edit_Window(), E_FAIL);

	CHECK_FAILED(Ready_Json_List_Window(), E_FAIL);

	CHECK_FAILED(Ready_Light_Window(), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Main_Window()
{
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isMainWindow)
		{
			ImGui::Begin("MAIN WINDOW", &m_isMainWindow, ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::Button("INFORMATION"))
				m_isInformation = !m_isInformation;

				if (true == m_isInformation)
				{
					ImGui::Text("F1 + LB      : SELECT OBJECT");
					ImGui::Text("F2 + LB      : SELECT INSTANCE");
					ImGui::Text("F3 + LB      : ADD LIGHT POSITION");
					ImGui::Text("F4 + LB      : SELECTED OBJECT MOVE");

					ImGui::Text("3      : EXPORT MODEL");
					ImGui::Text("4      : UPDATE JSON TO DAT");
					ImGui::Text("5      : DELETE");
					ImGui::Text("8      : MAPOBJECT WIREFRAME");
					ImGui::Text("9      : MAPOBJECT SOLID");
				}
				else
				{
					SEPARATOR;
					ImGui::Text("LIGHT");
					if (ImGui::Button("LIGHT EDIT"))
					{
						m_isLightSettingWindow = !m_isLightSettingWindow;
					}
					SEPARATOR;
					ImGui::Text("JSON");
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
					ImGui::Text("PROP LIST");
					if (ImGui::Button("OBJECT##active"))		m_isPropWindow[ENUM_CLASS(PROP_SPECIES::OBJECT)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::OBJECT)];
					SAMELINE;
					if (ImGui::Button("STATIC##active"))		m_isPropWindow[ENUM_CLASS(PROP_SPECIES::STATIC)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::STATIC)];
					SAMELINE;
					if (ImGui::Button("ANIMATED##active"))		m_isPropWindow[ENUM_CLASS(PROP_SPECIES::ANIMATED)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::ANIMATED)];
					SAMELINE;
					if (ImGui::Button("INTERACTIVE##active"))	m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)];
					SAMELINE;
					if (ImGui::Button("DESTRUCTIBLE##active"))	m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)] = !m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)];
					SEPARATOR;

					ImGui::Text("ETC FUNC");

					if (ImGui::Button("CLEAR JSON LIST"))
					{
						m_isMainWindow = { true };
						m_isJsonWindow = { false };
						m_isCustomJsonWindow = { false };
						m_isPrototypeWindow = { false };
						for (auto& bProp : m_isPropWindow)
							bProp = false;
						m_isLightSettingWindow = { false };

						ZeroMemory(&m_LightDesc, sizeof(LIGHT_DESC));
						m_LightDesc.eType = LIGHT_DESC::END;

						m_strLightTag.clear();

						m_LightTags.clear();
						m_iLightTagIndex = {};

						m_isAddLight = { false };
						m_isFixLight = { false };
						m_isFindFixLight = { false };

						ZeroMemory(&m_FixLightDesc, sizeof(LIGHT_DESC));

						m_szFixLightTag[MAX_PATH] = {};
						m_strFixLightTag.clear();

						m_isAddLightPoint = { false };
						m_vLightPoint = {};

						ZeroMemory(m_szJsonSaveName, sizeof(m_szJsonSaveName));		// Json 이름

						m_JsonFiles.clear();				// JsonFile 이름 명 ( Combo에서 볼 Json 폴더 경로의 .json 파일들 )
						m_iJsonFilesIndex = {};				// ImGui::BeginListBox 용 인덱스 변수

						m_Json = {};						// Original Json 정보 저장해놓을 JSON
						m_isJsonExport = { false };			// 추출 됬는지 확인용

						m_isJsonConverted = { false };		// 변환됬는지 확인 용

						m_JsonList.clear();					// Original Json 맵 데이터 용 벡터
						m_iJsonListIndex = {};				// ImGui::BeginListBox 용 인덱스 변수

						m_iObjCnt = {};						// 단일 객체 갯수 확인용

						m_CustomJson = {};					// Custom Json 정보 저장해놓을 JSON

						m_CustomJsonList.clear();			// Custom Json 맵 데이터 용 벡터
						m_iCustomJsonListIndex = {};		// ImGui::BeginListBox 용 인덱스 변수
						m_isCustomJsonInfoList = false;		// List Info 창 ON/OFF

						m_isCustomJsonLoaded = { false };	// Custom Json 로드 됬는지 확인 용

						m_CheckPrototypes.clear();			// 중복 프로토타입 체크 및 리스트 출력용

						m_Prototypes_Inst.clear();			// Prototype 목록 ( Instance 용 모델 )
						m_iIndex_PrtInst = {};				// Prototype Instance 용 인덱스

						m_Prototypes_Obj.clear();			// Prototype 목록 ( Object 용 모델 )
						m_iIndex_PrtObj = {};				// Prototype Object 용 인덱스
					}
					if (ImGui::Button("CLEAR LEVEL"))
					{
						CHECK_FAILED(m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::MAP)), );
					}
				}

			ImGui::End();
		}
		});

	return S_OK;
}

HRESULT CLevel_Map::Ready_Prop_Edit_Window()
{
#pragma region WIDGET : 모델 프로토타입 리스트 위젯

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPrototypeWindow)
		{
			ImGui::Begin("PROTOTYPE WINDOW", &m_isPrototypeWindow, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("MODEL_PROTOTYPES_INSTANCE");
			if (ImGui::BeginListBox("##prototype_instance_list"))
			{
				for (_uint i = 0; i < m_Prototypes_Inst.size(); ++i)
				{
					_bool isSelected = (m_iIndex_PrtInst == i);

					if (ImGui::Selectable(m_Prototypes_Inst[i].c_str(), isSelected))
						m_iIndex_PrtInst = i;
				}

				ImGui::EndListBox();
			} SEPARATOR;

			// 인스턴스 행렬 추가
			if (false == m_isLightSettingWindow && false == m_isFixObjectWindow && (ImGui::Button("ADD (T)") || m_pGameInstance->Key_Down(DIK_T)))
			{
				CEditor_Model_Instance* pModelInst = static_cast<CEditor_Model_Instance*>(m_pGameInstance->Find_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_InstObj"), TEXT("Com_Model"), m_iIndex_PrtInst));
				CHECK_NULLPTR(pModelInst, );

				MESH_INSTANCE_DATA TempData = {};

				TempData.InstanceMatrix = XMMatrixIdentity();
				TempData.InstanceID = m_iMapObjectCnt++;

				if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB))
				{
					_float3 vPickedPos = {};

					if (m_pGameInstance->isPicked(&vPickedPos))
						TempData.InstanceMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&vPickedPos), 1.f);
					else
						TempData.InstanceMatrix.r[3] = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
				}
				else
					TempData.InstanceMatrix.r[3] = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

				pModelInst->Add_Instance(TempData);
			} SEPARATOR;

			ImGui::Text("MODEL_PROTOTYPES_OBJECT");
			if (ImGui::BeginListBox("##prototype_object_list"))
			{
				for (_uint i = 0; i < m_Prototypes_Obj.size(); ++i)
				{
					_bool isSelected = (m_iIndex_PrtObj == i);

					if (ImGui::Selectable(m_Prototypes_Obj[i].c_str(), isSelected))
						m_iIndex_PrtObj = i;
				}

				ImGui::EndListBox();
			} SEPARATOR;

			// 단일 오브젝트 Layer 추가
			if (false == m_isLightSettingWindow && false == m_isFixObjectWindow && (ImGui::Button("ADD (Y)") || m_pGameInstance->Key_Down(DIK_Y)))
			{
				CProp_Object::PROP_OBJECT_DESC ObjectDesc = {};

				ObjectDesc.iMapObjectID = m_iMapObjectCnt++;
				ObjectDesc.eLevel = LEVEL::MAP;
				memcpy(ObjectDesc.szModelName, AnsiToWString(m_Prototypes_Obj[m_iIndex_PrtObj]).c_str(), sizeof(ObjectDesc.szModelName));

				_float4 vPos = {};

				if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB))
				{
					_float3 vPickedPos = {};

					if (m_pGameInstance->isPicked(&vPickedPos))
						vPos = _float4(vPickedPos.x, vPickedPos.y, vPickedPos.z, 1.f);
					else
						vPos = *m_pGameInstance->Get_CamPosition();
				}
				else
					vPos = *m_pGameInstance->Get_CamPosition();

				ObjectDesc.vPosition = _float3(vPos.x, vPos.y, vPos.z);
				ObjectDesc.vScale = _float3(0.01f, 0.01f, 0.01f);

				CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj"),
					ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc), );

				CProp* pObject_Prop = static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj")));
				CHECK_NULLPTR_MSG(pObject_Prop, TEXT("엥"), );

				m_ObjectList.push_back(pObject_Prop);

			} SEPARATOR;

			ImGui::End();
		}
		});

#pragma endregion

#pragma region WIDGET : 오브젝트 수정 위젯

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isFixObjectWindow)
		{
			ImGui::Begin("OBJECT FIX WINDOW", &m_isFixObjectWindow, ImGuiWindowFlags_AlwaysAutoResize);

			if (nullptr != m_pFixPropObj)
			{
				_char szModelName[MAX_PATH] = {};
				WideCharToMultiByte(CP_ACP, 0, m_pFixPropObj->Get_ModelName(), -1, szModelName, MAX_PATH, nullptr, nullptr);
				ImGui::Text("MODEL NAME : %s", szModelName); SEPARATOR;
			}

			if (ImGui::Button("DETAIL SCALE"))
			{
				m_eFixType = FIX_OBJECT::SCALE_DETAIL;

			} SAMELINE;
			if (ImGui::Button("ROTATION POSITION"))
			{
				m_eFixType = FIX_OBJECT::FIX_ALL;

			} SEPARATOR;

			if (FIX_OBJECT::SCALE_DETAIL == m_eFixType)
			{
				ImGui::Text("DETAIL SCALE FIX");
				SEPARATOR;

				ImGui::Text("X : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##scalex", &m_vFixScale.x);
				ImGui::SliderFloat("##sliderdetailscalex", &m_vFixScale.x, 0.001f, 10.f);
				ImGui::Text("Y : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##scaley", &m_vFixScale.y);
				ImGui::SliderFloat("##sliderdetailscaley", &m_vFixScale.y, 0.001f, 10.f);
				ImGui::Text("Z : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##scalez", &m_vFixScale.z);
				ImGui::SliderFloat("##sliderdetailscalez", &m_vFixScale.z, 0.001f, 10.f);

				if (0.001f > m_vFixScale.x) m_vFixScale.x = 0.001f;
				if (0.001f > m_vFixScale.y) m_vFixScale.y = 0.001f;
				if (0.001f > m_vFixScale.z) m_vFixScale.z = 0.001f;
				SEPARATOR;

				m_pFixTransformCom->Scale(m_vFixScale);
			}
			if (FIX_OBJECT::FIX_ALL == m_eFixType)
			{
				ImGui::Text("ROTATION FIX");
				SEPARATOR;

				ImGui::Text("X : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##rotationx", &m_vFixRotation.x, 1.f, 5.f);
				ImGui::SliderFloat("##sliderrotationx", &m_vFixRotation.x, 0.f, 360.f);
				ImGui::Text("Y : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##rotationy", &m_vFixRotation.y, 1.f, 5.f);
				ImGui::SliderFloat("##sliderrotationy", &m_vFixRotation.y, 0.f, 360.f);
				ImGui::Text("Z : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##rotationz", &m_vFixRotation.z, 1.f, 5.f);
				ImGui::SliderFloat("##sliderrotationz", &m_vFixRotation.z, 0.f, 360.f);
				SEPARATOR;

				m_pFixTransformCom->Rotation(XMConvertToRadians(m_vFixRotation.x), XMConvertToRadians(m_vFixRotation.y), XMConvertToRadians(m_vFixRotation.z));

				ImGui::Text("POSITION FIX");
				SEPARATOR;

				_bool isPicked = { false };

				if (m_pGameInstance->Key_Pressing(DIK_F4, 0.001f) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
				{
					_float3 vPos = {};

					if (m_pGameInstance->isPicked(&vPos))
					{
						m_vFixPosition = vPos;
					}
				}

				ImGui::Text("X : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##positionx", &m_vFixPosition.x, 0.2f, 1.f);
				ImGui::Text("Y : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##positiony", &m_vFixPosition.y, 0.2f, 1.f);
				ImGui::Text("Z : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##positionz", &m_vFixPosition.z, 0.2f, 1.f);
				SEPARATOR;

				m_pFixTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vFixPosition), 1.f));
			}

			if (ImGui::Button("DONE"))
			{
				ZeroMemory(m_szModelName, sizeof(m_szModelName));
				m_pFixPropObj = nullptr;
				m_pFixTransformCom = nullptr;
				m_isFixObjectWindow = false;
				m_eFixType = FIX_OBJECT::END;

			} SAMELINE;
			if (FIX_OBJECT::FIX_ALL != m_eFixType && FIX_OBJECT::SCALE_DETAIL != m_eFixType &&
				ImGui::Button("DELETE") || m_pGameInstance->Key_Down(DIK_DELETE))
			{
				if (nullptr != m_pFixPropObj)
				{
					m_pFixPropObj->Set_IsDead(true);
					m_pFixPropObj = nullptr;
				}

				ZeroMemory(m_szModelName, sizeof(m_szModelName));
				m_pFixTransformCom = nullptr;
				m_isFixObjectWindow = false;
				m_eFixType = FIX_OBJECT::END;
			}

			ImGui::End();
		}
		});

#pragma endregion

#pragma region 주석
		/*
#pragma region PROP_OBJECT EDIT WINDOW
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPropWindow[ENUM_CLASS(PROP_SPECIES::OBJECT)])
		{
			ImGui::Begin("PROP OBJECT WINDOW", &m_isPropWindow[ENUM_CLASS(PROP_SPECIES::OBJECT)], ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::BeginListBox("##prop_object_list"))
			{

				ImGui::EndListBox();
			}

			ImGui::End();
		}
		});
#pragma endregion

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

			if (ImGui::BeginListBox("##prop_animated_list"))
			{

				ImGui::EndListBox();
			}

			ImGui::End();
		}
		});
#pragma endregion

#pragma region PROP_INTERACTIVE EDIT WINDOW
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)])
		{
			ImGui::Begin("PROP INTERACTIVE WINDOW", &m_isPropWindow[ENUM_CLASS(PROP_SPECIES::INTERACTIVE)], ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::BeginListBox("##prop_interactive_list"))
			{

				ImGui::EndListBox();
			}

			ImGui::End();
		}
		});
#pragma endregion

#pragma region PROP_DESTRUCTIBLE EDIT WINDOW
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)])
		{
			ImGui::Begin("PROP DESTRUCTIBLE WINDOW", &m_isPropWindow[ENUM_CLASS(PROP_SPECIES::DESTRUCTIBLE)], ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::BeginListBox("##prop_destructible_list"))
			{

				ImGui::EndListBox();
			}

			ImGui::End();
		}
		});
#pragma endregion
*/
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Map::Ready_CustomJson_Edit_Window()
{
#pragma region WIDGET : CUSTOM JSON 로드 및 테스트용 위젯

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

					for (auto& Component : m_CustomJson)
					{
						string strModelName = {};

						_uint iNumInstance = {};

						size_t start = {};
						size_t end = {};

						_bool isInstance = Component["isInstance"];
						_bool isObject = Component["isObject"];

						JSON_MAP_DATA Data = {};

						Data.strModelName = Component["strModelName"];

						if (true == isInstance)
						{
							auto& positions = Component["vInstancePosition"];
							for (auto& poses : positions)
							{
								++Data.iNumInstances;

								auto& x = poses["x"];
								auto& y = poses["y"];
								auto& z = poses["z"];

								FLOAT3_DATA vPos = FLOAT3_DATA((_float)x, (_float)y, (_float)z);

								Data.vInstancePosition.push_back(vPos);
							}

							auto& quats = Component["vInstanceQuaternion"];
							for (auto& quas : quats)
							{
								auto& x = quas["x"];
								auto& y = quas["y"];
								auto& z = quas["z"];
								auto& w = quas["w"];

								FLOAT4_DATA vQuat = FLOAT4_DATA((_float)x, (_float)y, (_float)z, (_float)w);

								Data.vInstanceQuaternion.push_back(vQuat);
							}

							auto& scales = Component["vInstanceScale"];
							for (auto& scas : scales)
							{
								auto& x = scas["x"];
								auto& y = scas["y"];
								auto& z = scas["z"];

								FLOAT3_DATA vScale = FLOAT3_DATA((_float)x, (_float)y, (_float)z);

								Data.vInstanceScale.push_back(vScale);
							}

							Data.isInstance = true;
						}
						else if (true == isObject)
						{
							auto& positions = Component["vPosition"];
							for (auto& poses : positions)
							{
								auto& x = positions["x"];
								auto& y = positions["y"];
								auto& z = positions["z"];

								FLOAT3_DATA vPos = FLOAT3_DATA((_float)x, (_float)y, (_float)z);

								Data.vPosition = vPos;
							}

							auto& rotas = Component["vRotation"];
							for (auto& rots : rotas)
							{
								auto& x = rotas["x"];
								auto& y = rotas["y"];
								auto& z = rotas["z"];

								FLOAT3_DATA vRot = FLOAT3_DATA((_float)x, (_float)y, (_float)z);

								Data.vRotation = vRot;
							}

							auto& scales = Component["vScale"];
							for (auto& scas : scales)
							{
								auto& x = scales["x"];
								auto& y = scales["y"];
								auto& z = scales["z"];

								FLOAT3_DATA vScale = FLOAT3_DATA((_float)x, (_float)y, (_float)z);

								Data.vScale = vScale;
							}

							Data.isObject = true;
						}

						m_CustomJsonList.push_back(Data);
					}

					m_isCustomJsonLoaded = true;
				}
				else
					ifs.close();
			}

#pragma endregion

#pragma region CustomJson을 이용한 프로토 타입 및 레이어 생성

			if (true == m_isCustomJsonLoaded)
			{
				SEPARATOR;
				if (ImGui::Button("CONVERT JSON TO DAT"))
				{
					CHECK_FAILED_MSG(Convert_Json_To_Data(), TEXT("추출 실패"), );
				}
				SEPARATOR;
				if (ImGui::Button("CREATE PROTOTYPES"))
				{
					CHECK_FAILED_MSG(Add_Prototypes_FromJson(), TEXT("임시 프로토타입 생성 실패 or 임시 Layer 생성 실패"), );
					m_isPrototypeWindow = true;
				}
				SEPARATOR;
			}

#pragma endregion

			ImGui::End();
		}
		});

#pragma endregion

	return S_OK;
}

HRESULT CLevel_Map::Ready_CustomJson_List_Window()
{
#pragma region WIDGET : CUSTOM JSON 에서 로드해온 리스트

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isCustomJsonLoaded)
		{
			ImGui::Begin("CUSTOM JSON LIST", &m_isCustomJsonLoaded, ImGuiWindowFlags_AlwaysAutoResize);

			_uint iJsonListSize = m_CustomJsonList.size();

			ImGui::Text("LIST COUNT : %d", iJsonListSize);

			_uint iInstanceCnt = {};
			_uint iTotalInstanceCnt = {};
			_uint iObjCnt = {};

			for (_uint i = 0; i < iJsonListSize; ++i)
			{
				if (true == m_CustomJsonList[i].isInstance)
				{
					++iInstanceCnt;

					iTotalInstanceCnt += m_CustomJsonList[i].iNumInstances;
				}
				if (true == m_CustomJsonList[i].isObject)
					++iObjCnt;
			}
			SEPARATOR;
			ImGui::Text("INSTANCE MODEL COUNT : %d", iInstanceCnt);

			ImGui::Text("TOTAL INSTANCE COUNT : %d", iTotalInstanceCnt);
			SEPARATOR;
			ImGui::Text("INDEPENDENT OBJECT COUNT : %d", iObjCnt);

			m_iObjCnt = iObjCnt;

			_uint iInstanceID = {};
			_uint iObjectID = {};

			if (ImGui::BeginListBox("##custom_json_list"))
			{
				for (_uint i = 0; i < m_CustomJsonList.size(); ++i)
				{
					_bool isSelected = (m_iCustomJsonListIndex == i);

					string strModelName = "ID-%d:";

					strModelName += m_CustomJsonList[i].strModelName;

					_char szModelName[MAX_PATH] = {};

					sprintf_s(szModelName, strModelName.c_str(), i);

					if (ImGui::Selectable(szModelName, isSelected))
						m_iCustomJsonListIndex = i;
				}

				ImGui::EndListBox();
			} SEPARATOR;

			if (ImGui::Button("LIST##custom"))
				m_isCustomJsonInfoList = !m_isCustomJsonInfoList;

			ImGui::End();
		}
		});

#pragma endregion

#pragma region WIDGET : CUSTOM JSON 에서 로드해온 리스트들의 정보

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isCustomJsonInfoList)
		{
			ImGui::Begin("LIST INFO##custom", &m_isCustomJsonInfoList, ImGuiWindowFlags_AlwaysAutoResize);

			if (0 == m_CustomJsonList.size())
			{

			}
			else
			{
				ImGui::Text("MODEL NAME\n%s", m_CustomJsonList[m_iCustomJsonListIndex].strModelName.c_str());
				SEPARATOR;
				if (true == m_CustomJsonList[m_iCustomJsonListIndex].isInstance)
				{
					_uint iInstCnt = m_CustomJsonList[m_iCustomJsonListIndex].iNumInstances;
					ImGui::Text("INSTANCE COUNT : %d", iInstCnt);
					SEPARATOR;
					for (_uint i = 0; i < iInstCnt; ++i)
					{
						FLOAT3_DATA vPos = m_CustomJsonList[m_iCustomJsonListIndex].vInstancePosition[i];
						FLOAT3_DATA vScale = m_CustomJsonList[m_iCustomJsonListIndex].vInstanceScale[i];
						FLOAT4_DATA vQuat = m_CustomJsonList[m_iCustomJsonListIndex].vInstanceQuaternion[i];
						ImGui::Text("POSITION\nX : %.3f\nY : %.3f\nZ : %.3f", vPos.x, vPos.y, vPos.z);
						ImGui::Text("SCALE\nX : %.3f\nY : %.3f\nZ : %.3f", vScale.x, vScale.y, vScale.z);
						ImGui::Text("QUATERNION\nX : %.3f\nY : %.3f\nZ : %.3f\nW : %.3f", vQuat.x, vQuat.y, vQuat.z, vQuat.w);
						SEPARATOR;

					}
				}
				else if (true == m_CustomJsonList[m_iCustomJsonListIndex].isObject)
				{
					ImGui::Text("INDEPENDENT OBJECT");
					SEPARATOR;
					FLOAT3_DATA vPos = m_CustomJsonList[m_iCustomJsonListIndex].vPosition;
					FLOAT3_DATA vScale = m_CustomJsonList[m_iCustomJsonListIndex].vScale;
					FLOAT3_DATA vRot = m_CustomJsonList[m_iCustomJsonListIndex].vRotation;
					ImGui::Text("POSITION\nX : %.3f\nY : %.3f\nZ : %.3f", vPos.x, vPos.y, vPos.z);
					ImGui::Text("SCALE\nX : %.3f\nY : %.3f\nZ : %.3f", vScale.x, vScale.y, vScale.z);
					ImGui::Text("ROTATION\nPitch : %.3f\nYaw : %.3f\nRoll : %.3f", vRot.x, vRot.y, vRot.z);
					SEPARATOR;
				}
			}

			ImGui::End();
		}
		});

#pragma endregion

	return S_OK;
}

HRESULT CLevel_Map::Ready_Json_Edit_Window()
{
#pragma region WIDGET : JSON 을 CUSTOM JSON 으로 만들기

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isJsonWindow)
		{
			ImGui::Begin("JSON LOAD", &m_isJsonWindow, ImGuiWindowFlags_AlwaysAutoResize);

#pragma region JSON 로드 및 VECTOR에 넣기

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

					for (auto& Component : m_Json)
					{
						string strModelName = {};

						_uint iNumInstance = {};

						size_t Start = {};
						size_t End = {};

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

						Start = strModelName.find('\'');
						End = strModelName.rfind('\'');

						if (Start != string::npos && End != string::npos && End > Start)
							strModelName = strModelName.substr(Start + 1, End - Start - 1);

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

								auto& pos = Instance["TransformData"]["Translation"];
								auto& scale = Instance["TransformData"]["Scale3D"];
								auto& quat = Instance["TransformData"]["Rotation"];

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
							auto& Prop = Component["Properties"];

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

							auto& pos = Component["Properties"]["RelativeLocation"];
							auto& scale = Component["Properties"]["RelativeScale3D"];
							auto& rot = Component["Properties"]["RelativeRotation"];

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

			m_iObjCnt = iObjCnt;

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
			ImGui::Begin("LIST INFO##original", &m_isJsonConverted, ImGuiWindowFlags_AlwaysAutoResize);

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
					ImGui::Text("POSITION\nX : %.3f\nY : %.3f\nZ : %.3f", vPos.x, vPos.y, vPos.z);
					ImGui::Text("SCALE\nX : %.3f\nY : %.3f\nZ : %.3f", vScale.x, vScale.y, vScale.z);
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

			ImGui::Text("FOLDER NAME : %s", m_szJsonFolderPath[ENUM_CLASS(m_eMapType)]);

			ImGui::Text("JSON FILE NAME : ");
			SAMELINE;
			ImGui::InputText("##json_save_name", m_szJsonSaveName, IM_ARRAYSIZE(m_szJsonSaveName));

			SAMELINE;
			if (ImGui::Button("SAVE##json"))
			{
				string strPath = m_szJsonCustomPath;

				strPath += m_szJsonFolderPath[ENUM_CLASS(m_eMapType)];

				filesystem::create_directories(strPath);

				strPath += m_szJsonSaveName;
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

HRESULT CLevel_Map::Ready_Light_Window()
{
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isLightSettingWindow)
		{
			ImGui::Begin("LIGHT WINDOW", &m_isLightSettingWindow, ImGuiWindowFlags_AlwaysAutoResize);

			if (true == m_isFindFixLight)
			{
				if (LIGHT_DESC::DIRECTIONAL == m_FixLightDesc.eType)
				{
					ImGui::Text("DIRECTIONAL");
					SEPARATOR;

					ImGui::Text("DIRECTION"); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("X##DIRdirx", &m_FixLightDesc.vDirection.x); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("Y##DIRdiry", &m_FixLightDesc.vDirection.y); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("Z##DIRdirz", &m_FixLightDesc.vDirection.z); SAMELINE;
					SEPARATOR;

					ImGui::Text("DIFFUSE"); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("R##DIRdifx", &m_FixLightDesc.vDiffuse.x); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("G##DIRdify", &m_FixLightDesc.vDiffuse.y); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("B##DIRdifz", &m_FixLightDesc.vDiffuse.z); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("A##DIRdifw", &m_FixLightDesc.vDiffuse.w); SAMELINE;
					SEPARATOR;

					ImGui::Text("AMBIENT"); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("R##DIRambx", &m_FixLightDesc.vAmbient.x); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("G##DIRamby", &m_FixLightDesc.vAmbient.y); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("B##DIRambz", &m_FixLightDesc.vAmbient.z); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("A##DIRambw", &m_FixLightDesc.vAmbient.w); SAMELINE;
					SEPARATOR;

					ImGui::Text("SPECULAR"); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("R##DIRspecx", &m_FixLightDesc.vSpecular.x); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("G##DIRspecy", &m_FixLightDesc.vSpecular.y); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("B##DIRspecz", &m_FixLightDesc.vSpecular.z); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("A##DIRspecw", &m_FixLightDesc.vSpecular.w); SAMELINE;
					SEPARATOR;
				}
				else if (LIGHT_DESC::POINT == m_FixLightDesc.eType)
				{
					ImGui::Text("POINT");
					SEPARATOR;

					ImGui::Text("POSITION"); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("X##POIposx", &m_FixLightDesc.vPosition.x); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("Y##POIposy", &m_FixLightDesc.vPosition.y); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("Z##POIposz", &m_FixLightDesc.vPosition.z); SAMELINE;

					if (true == m_isAddLightPoint)
					{
						m_isAddLightPoint = false;

						m_FixLightDesc.vPosition.x = m_vLightPoint.x;
						m_FixLightDesc.vPosition.y = m_vLightPoint.y;
						m_FixLightDesc.vPosition.z = m_vLightPoint.z;
					}

					SEPARATOR;

					ImGui::Text("RANGE"); SAMELINE; ITEMWIDTH(80.f); ImGui::InputFloat("##POIrange", &m_FixLightDesc.fRange); SAMELINE;
					SEPARATOR;

					ImGui::Text("DIFFUSE"); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("R##POIdifx", &m_FixLightDesc.vDiffuse.x); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("G##POIdify", &m_FixLightDesc.vDiffuse.y); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("B##POIdifz", &m_FixLightDesc.vDiffuse.z); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("A##POIdifw", &m_FixLightDesc.vDiffuse.w); SAMELINE;
					SEPARATOR;

					ImGui::Text("AMBIENT"); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("R##POIambx", &m_FixLightDesc.vAmbient.x); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("G##POIamby", &m_FixLightDesc.vAmbient.y); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("B##POIambz", &m_FixLightDesc.vAmbient.z); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("A##POIambw", &m_FixLightDesc.vAmbient.w); SAMELINE;
					SEPARATOR;

					ImGui::Text("SPECULAR"); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("R##POIspecx", &m_FixLightDesc.vSpecular.x); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("G##POIspecy", &m_FixLightDesc.vSpecular.y); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("B##POIspecz", &m_FixLightDesc.vSpecular.z); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("A##POIspecw", &m_FixLightDesc.vSpecular.w); SAMELINE;
					SEPARATOR;
				}
				if (ImGui::Button("APPLY"))
				{
					m_pGameInstance->Set_LightDesc(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP), m_FixLightDesc);
				} SAMELINE;
				if (ImGui::Button("DONE"))
				{
					m_strFixLightTag.clear();
					m_isFindFixLight = false;
					ZeroMemory(&m_FixLightDesc, sizeof(LIGHT_DESC));
				}
				
			}
			else
			{
				ImGui::Text("LIGHT LIST");
				if (ImGui::BeginListBox("##light_list"))
				{
					for (_uint i = 0; i < m_LightTags.size(); ++i)
					{
						_bool isSelected = (m_iLightTagIndex == i);

						if (ImGui::Selectable(m_LightTags[i].c_str(), isSelected))
							m_iLightTagIndex = i;
					}

					ImGui::EndListBox();
				} SEPARATOR;
				if (0 != m_LightTags.size() && ImGui::Button("TURN ON"))
				{
					m_isFixLight = false;
					m_isAddLight = false;
					m_LightDesc.eType = LIGHT_DESC::END;

					m_pGameInstance->Set_LightEnable(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP), true);

				} SAMELINE;;
				if (0 != m_LightTags.size() && ImGui::Button("TURN OFF"))
				{
					m_isFixLight = false;
					m_isAddLight = false;
					m_LightDesc.eType = LIGHT_DESC::END;

					m_pGameInstance->Set_LightEnable(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP), false);

				} SEPARATOR;
				if (ImGui::Button("ADD LIGHT"))
				{
					m_isAddLight = !m_isAddLight;
					m_isFixLight = false;
					m_LightDesc.eType = LIGHT_DESC::END;
				} SAMELINE;
				if (ImGui::Button("FIX LIGHT"))
				{
					m_isFixLight = !m_isFixLight;
					m_isAddLight = false;
					m_LightDesc.eType = LIGHT_DESC::END;
				} SEPARATOR;

				// Add Light 띄우기
				if (true == m_isAddLight)
				{
					if (ImGui::Button("DIRECTIONAL"))
						m_LightDesc.eType = LIGHT_DESC::DIRECTIONAL;
					if (ImGui::Button("POINT"))
						m_LightDesc.eType = LIGHT_DESC::POINT;

					if (LIGHT_DESC::DIRECTIONAL == m_LightDesc.eType)
					ImGui::Text("CURRENT LIGHT TYPE : DIRECTIONAL");

					if (LIGHT_DESC::POINT == m_LightDesc.eType)
					ImGui::Text("CURRENT LIGHT TYPE : POINT");

					ImGui::Text("LIGHT TAG : ");
					ImGui::InputText("##light_tag", m_szLightTag, IM_ARRAYSIZE(m_szLightTag));
					m_strLightTag = m_szLightTag;

					if (LIGHT_DESC::DIRECTIONAL == m_LightDesc.eType)
					{
						m_LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

						m_LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
						m_LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
						m_LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 1.f);
					}
					if (LIGHT_DESC::POINT == m_LightDesc.eType)
					{
						m_LightDesc.vPosition = _float4(1.f, -1.f, 1.f, 0.f);
						m_LightDesc.fRange = 10.f;

						m_LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
						m_LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
						m_LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 1.f);
					}

					if (LIGHT_DESC::END != m_LightDesc.eType)
					{
						if (ImGui::Button("LIGHT ADD"))
						{
							m_pGameInstance->Add_Light(AnsiToWString(m_strLightTag), ENUM_CLASS(LEVEL::MAP), m_LightDesc);
							m_LightTags.push_back(m_strLightTag);

							ZeroMemory(&m_szLightTag, sizeof(m_szLightTag));
							ZeroMemory(&m_LightDesc, sizeof(LIGHT_DESC));
							m_LightDesc.eType = LIGHT_DESC::END;

							m_isAddLight = !m_isAddLight;
							m_isFixLight = false;
						}
					}
				};
				// Fix Light 띄우기
				if (true == m_isFixLight)
				{
					m_isFindFixLight = false;
					ZeroMemory(&m_FixLightDesc, sizeof(LIGHT_DESC));

					m_strFixLightTag = m_szFixLightTag;

					const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP));
					if (nullptr == pLightDesc)
					{

					}
					else
					{
						m_FixLightDesc = *pLightDesc;
						m_isFindFixLight = true;

						m_isAddLight = false;
						m_isFixLight = false;
					}
				}
			}

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

	filesystem::create_directories(strPath);

	for (const auto& entry : filesystem::recursive_directory_iterator(strPath.c_str()))
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

string CLevel_Map::Find_ModelPath(const string& strModelName)
{
	string strRoot = "../../Client/Bin/Resources/Models/Environment/Prop/";

	for (auto& entry : filesystem::recursive_directory_iterator(strRoot))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".fbx")
		{
			if (entry.path().stem() == strModelName)
				return entry.path().string();
		}
	}

	return "NOTFOUND";
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
