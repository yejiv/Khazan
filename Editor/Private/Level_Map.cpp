#include "Level_Map.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "MapEditor_Header.h"

#include <iostream>

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

	//CHECK_FAILED(Ready_Preview_Model_RanderTargets(), E_FAIL);

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

HRESULT CLevel_Map::Ready_Preview_Model_RanderTargets()
{
	/* For.Dif_RT_MapEditor */
	//CHECK_FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Dif_RT_MapEditor"), 200, 200, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 0.f)), E_FAIL);

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

						_float4x4 WorldMatrix = {};
						XMStoreFloat4x4(&WorldMatrix, m_pFixTransformCom->Get_WorldMatrix());

						_vector vRight = XMVector3Normalize(XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&WorldMatrix._11)));
						_vector vUp = XMVector3Normalize(XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&WorldMatrix._21)));
						_vector vLook = XMVector3Normalize(XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&WorldMatrix._31)));

						_matrix RotationMatrix = {};
						RotationMatrix.r[0] = XMVectorSetW(vRight, 0.f);
						RotationMatrix.r[1] = XMVectorSetW(vUp, 0.f);
						RotationMatrix.r[2] = XMVectorSetW(vLook, 0.f);
						RotationMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

						_float4x4 RotMatrix = {};
						XMStoreFloat4x4(&RotMatrix, RotationMatrix);

						_float fPitch, fYaw, fRoll;

						// Pitch(X)
						fPitch = atan2f(-RotMatrix._32, RotMatrix._33);

						// Yaw(Y)
						fYaw = asinf(RotMatrix._31);

						// Roll(Z)
						fRoll = atan2f(-RotMatrix._21, RotMatrix._11);

						// Degree 변환 및 보정
						m_vFixRotation.x = XMConvertToDegrees(fPitch);
						m_vFixRotation.y = XMConvertToDegrees(fYaw);
						m_vFixRotation.z = XMConvertToDegrees(fRoll);

						auto Clamp180 = [](float deg)
							{
								while (deg > 180.f) deg -= 360.f;
								while (deg < -180.f) deg += 360.f;
								return deg;
							};

						m_vFixRotation.x = Clamp180(m_vFixRotation.x);
						m_vFixRotation.y = Clamp180(m_vFixRotation.y);
						m_vFixRotation.z = Clamp180(m_vFixRotation.z);

						// ======================================================
						// ======================================================

						m_isFixObjectWindow = true;
						m_eFixType = FIX_OBJECT::FIX_ALL;
						return;
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

	CHECK_FAILED(Ready_Prop_Edit_Window(), E_FAIL);

	CHECK_FAILED(Ready_Light_Window(), E_FAIL);

	CHECK_FAILED(Ready_Object_SaveLoad_Window(), E_FAIL);

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
					
					ImGui::Text("OBJECT SAVE & LOAD");
					if (ImGui::Button("SAVE")) m_isSaveObjectWindow = !m_isSaveObjectWindow;
					SAMELINE;
					if (ImGui::Button("LOAD")) m_isLoadObjectWindow = !m_isLoadObjectWindow;
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

					ImGui::Text("ADD PROTOTYPES");
					ImGui::Text("FOLDER : "); SAMELINE;
					ImGui::InputText("##folder_name_convert", m_szFolderName, IM_ARRAYSIZE(m_szFolderName));

					if (ImGui::Button("PROTOTYPES ADD")) Add_Prototype_ByFolder(m_szFolderName);

					if (ImGui::Button("FBX FILE CONVERT ( .fbx > .dat )")) Fbxs_Convert_To_Dat(m_szFolderName);

					SEPARATOR;

					ImGui::Text("DON'T USE");

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

						m_iObjCnt = {};						// 단일 객체 갯수 확인용

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
			if (0 != m_Prototypes_Inst.size() && false == m_isLightSettingWindow && false == m_isFixObjectWindow &&
				(ImGui::Button("ADD (T)") || m_pGameInstance->Key_Down(DIK_T)))
			{
				CModel_Instance* pModelInst = static_cast<CModel_Instance*>(m_pGameInstance->Find_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_InstObj"), TEXT("Com_Model"), m_iIndex_PrtInst));
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
					{
						TempData.InstanceMatrix.r[3] = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
						TempData.InstanceMatrix.r[3].m128_f32[1] = 0.f;
					}
				}
				else
				{
					TempData.InstanceMatrix.r[3] = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
					TempData.InstanceMatrix.r[3].m128_f32[1] = 0.f;
				}

				pModelInst->Add_Instance(TempData);
			} SEPARATOR;

			ImGui::Text("MODEL_PROTOTYPES_OBJECT");
			ImGui::Text("SEARCH : "); SAMELINE;
			ImGui::InputText("##search_model_name", m_szSearchModelName, IM_ARRAYSIZE(m_szSearchModelName)); SAMELINE;

			if (ImGui::Button("CLEAR"))
				ZeroMemory(m_szSearchModelName, sizeof(m_szSearchModelName));

			if (ImGui::BeginListBox("##prototype_object_list"))
			{
				string strSearchName = m_szSearchModelName;
				transform(strSearchName.begin(), strSearchName.end(), strSearchName.begin(), ::tolower);		// 검색할 모델을 소문자로 변환

				for (_uint i = 0; i < m_Prototypes_Obj.size(); ++i)
				{
					string strModelName = m_Prototypes_Obj[i];
					transform(strModelName.begin(), strModelName.end(), strModelName.begin(), ::tolower);		// 찾을 모델을 소문자로 변환

					if (true == strSearchName.empty() || strModelName.find(strSearchName) != string::npos)
					{
						_bool isSelected = (m_iIndex_PrtObj == i);

						if (ImGui::Selectable(m_Prototypes_Obj[i].c_str(), isSelected))
							m_iIndex_PrtObj = i;
					}
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

				_float3 vPos = {};

				if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB))
				{
					_float3 vPickedPos = {};

					if (m_pGameInstance->isPicked(&vPickedPos))
						vPos = vPickedPos;
					else
					{
						XMStoreFloat3(&vPos, XMLoadFloat4(m_pGameInstance->Get_CamPosition()));
						vPos.y = 0.f;
					}
				}
				else
				{
					XMStoreFloat3(&vPos, XMLoadFloat4(m_pGameInstance->Get_CamPosition()));
					vPos.y = 0.f;
				}

				_matrix WorldMatrix = XMMatrixIdentity();

				// 스케일 0.01, 위치는 마우스 피킹 위치 혹은 카메라 위치
				WorldMatrix.r[0] *= 0.01f;
				WorldMatrix.r[1] *= 0.01f;
				WorldMatrix.r[2] *= 0.01f;
				WorldMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&vPos), 1.f);

				XMStoreFloat4x4(&ObjectDesc.WorldMatrix, WorldMatrix);

#pragma region ICE 있는지 검사

				_wstring strSearchName = TEXT("ice");
				transform(strSearchName.begin(), strSearchName.end(), strSearchName.begin(), ::tolower);

				_wstring strModelName = ObjectDesc.szModelName;
				transform(strModelName.begin(), strModelName.end(), strModelName.begin(), ::tolower);		// 찾을 모델을 소문자로 변환

				if (strModelName.find(strSearchName) != string::npos)
				{
					ObjectDesc.isBlended = true;
				}
				else
				{
					ObjectDesc.isBlended = false;
				}

#pragma endregion

				CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj"),
					ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc), );

				CProp* pObject_Prop = static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj")));
				CHECK_NULLPTR_MSG(pObject_Prop, TEXT("엥"), );

				m_ObjectList.push_back(pObject_Prop);

				m_iObjectListIndex = m_ObjectList.size() - 1;

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

				ImGui::Text("MODEL NAME : "); SAMELINE;
				ImGui::InputText("##copy_batch_modelname", szModelName, IM_ARRAYSIZE(szModelName)); SEPARATOR;
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

				ImGui::Text("X : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##scalex", &m_vFixScale.x, 0.001f, 0.01f);
				ImGui::SliderFloat("##sliderdetailscalex", &m_vFixScale.x, 0.001f, 10.f);
				ImGui::Text("Y : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##scaley", &m_vFixScale.y, 0.001f, 0.01f);
				ImGui::SliderFloat("##sliderdetailscaley", &m_vFixScale.y, 0.001f, 10.f);
				ImGui::Text("Z : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##scalez", &m_vFixScale.z, 0.001f, 0.01f);
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
				ImGui::SliderFloat("##sliderrotationx", &m_vFixRotation.x, -180.f, 180.f);
				ImGui::Text("Y : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##rotationy", &m_vFixRotation.y, 1.f, 5.f);
				ImGui::SliderFloat("##sliderrotationy", &m_vFixRotation.y, -180.f, 180.f);
				ImGui::Text("Z : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##rotationz", &m_vFixRotation.z, 1.f, 5.f);
				ImGui::SliderFloat("##sliderrotationz", &m_vFixRotation.z, -180.f, 180.f);
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

				ImGui::Text("X : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##positionx", &m_vFixPosition.x, 0.1f, 0.5f);
				ImGui::Text("Y : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##positiony", &m_vFixPosition.y, 0.1f, 0.5f);
				ImGui::Text("Z : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##positionz", &m_vFixPosition.z, 0.1f, 0.5f);
				SEPARATOR;

				m_pFixTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vFixPosition), 1.f));
			}

			if (ImGui::Button("DONE") || m_pGameInstance->Key_Down(DIK_RETURN) || m_pGameInstance->Key_Down(DIK_NUMPADENTER))
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

					for (_uint i = 0; i < m_ObjectList.size(); )
					{
						if (m_pFixPropObj == m_ObjectList[i])
						{
							swap(m_ObjectList[i], m_ObjectList.back());
							m_ObjectList.pop_back();
							break;
						}
						else
							++i;
					}

					m_pFixPropObj = nullptr;
				}
				
				m_pFixPropObj = nullptr;
				ZeroMemory(m_szModelName, sizeof(m_szModelName));
				m_pFixTransformCom = nullptr;
				m_isFixObjectWindow = false;
				m_eFixType = FIX_OBJECT::END;
			}

			ImGui::End();
		}
		});

#pragma endregion

#pragma region WIDGET : 단일 오브젝트 리스트

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPropWindow[ENUM_CLASS(PROP_SPECIES::OBJECT)])
		{
			ImGui::Begin("PROP OBJECT WINDOW", &m_isPropWindow[ENUM_CLASS(PROP_SPECIES::OBJECT)], ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::BeginListBox("##prop_object_list"))
			{
				if (m_iObjectListIndex >= m_ObjectList.size())
					m_iObjectListIndex = m_ObjectList.size() - 1;

				for (_uint i = 0; i < m_ObjectList.size(); ++i)
				{
					_bool isSelected = (m_iObjectListIndex == i);

					string strModelName = WStringToAnsi(m_ObjectList[i]->Get_ModelName()) + "##id_%d";

					_char szModelName[MAX_PATH] = {};

					sprintf_s(szModelName, strModelName.c_str(), i);

					if (ImGui::Selectable(szModelName, isSelected))
						m_iObjectListIndex = i;
				}

				ImGui::EndListBox();
			} SEPARATOR;

			if (0 != m_ObjectList.size())
			{
				ImGui::Text("%d", m_ObjectList.size());
				SEPARATOR;
			}

			if (ImGui::Button("SNOW ON"))
			{
				for (auto& pObj : m_ObjectList)
					pObj->Set_ShaderPass(4);
			} SAMELINE;
			if (ImGui::Button("SNOW OFF"))
			{
				for (auto& pObj : m_ObjectList)
					pObj->Set_ShaderPass(2);
			} SEPARATOR;

			if (0 != m_ObjectList.size() && m_iObjectListIndex < m_ObjectList.size())
			{
				_wstring strModelName = m_ObjectList[m_iObjectListIndex]->Get_ModelName();

				ImGui::Text("MODEL NAME : %s", WStringToAnsi(strModelName).c_str());
				SEPARATOR;

				CTransform* pTransform = static_cast<CTransform*>(m_ObjectList[m_iObjectListIndex]->Get_Component(TEXT("Com_Transform")));
				CHECK_NULLPTR(pTransform, );

				_float3 vPosition = {};

				XMStoreFloat3(&vPosition, pTransform->Get_State(STATE::POSITION));

				ImGui::Text("POSITION");
				ImGui::Text("X : %.3f", vPosition.x);
				ImGui::Text("Y : %.3f", vPosition.y);
				ImGui::Text("Z : %.3f", vPosition.z);
				SEPARATOR;
			}
			if (0 != m_ObjectList.size())
			{
				if (ImGui::Button("FIX"))
				{
					if (nullptr != m_ObjectList[m_iObjectListIndex])
					{
						m_pFixPropObj = m_ObjectList[m_iObjectListIndex];
						m_pFixTransformCom = static_cast<CTransform*>(m_ObjectList[m_iObjectListIndex]->Get_Component(TEXT("Com_Transform")));
						CHECK_NULLPTR_MSG(m_pFixTransformCom, TEXT("Fix Transform == nullptr"), );

						m_vFixScale = m_pFixTransformCom->Get_Scaled();
						XMStoreFloat3(&m_vFixPosition, m_pFixTransformCom->Get_State(STATE::POSITION));

						_vector vScale = {};
						_vector vQuaternion = {};
						_vector vTranslation = {};

						XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, m_pFixTransformCom->Get_WorldMatrix());

						_matrix RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

						_float3 vDegree = {};

						vDegree.x = atan2f(RotationMatrix.r[2].m128_f32[1], RotationMatrix.r[2].m128_f32[2]);
						vDegree.y = asinf(RotationMatrix.r[2].m128_f32[0]);
						vDegree.z = atan2f(RotationMatrix.r[1].m128_f32[0], RotationMatrix.r[0].m128_f32[0]);

						vDegree.x = XMConvertToDegrees(vDegree.x);
						vDegree.y = XMConvertToDegrees(vDegree.y);
						vDegree.z = XMConvertToDegrees(vDegree.z);

						m_vFixRotation = vDegree;

						m_isFixObjectWindow = true;
						
						m_eFixType = FIX_OBJECT::FIX_ALL;
					}
				}
				SAMELINE;
				if (ImGui::Button("DELETE"))
				{
					m_isFixObjectWindow = false;

					if (nullptr != m_ObjectList[m_iObjectListIndex])
					{
						m_ObjectList[m_iObjectListIndex]->Set_IsDead(true);

						for (_uint i = 0; i < m_ObjectList.size(); )
						{
							if (m_ObjectList[m_iObjectListIndex] == m_ObjectList[i])
							{
								swap(m_ObjectList[m_iObjectListIndex], m_ObjectList.back());
								m_ObjectList.pop_back();
								break;
							}
							else
								++i;
						}

						if (m_iObjectListIndex >= m_ObjectList.size())
							m_iObjectListIndex = m_ObjectList.size() - 1;

						m_pFixPropObj = nullptr;
					}

					m_pFixPropObj = nullptr;
					m_pFixTransformCom = nullptr;
					m_eFixType = FIX_OBJECT::END;
				}
			}
			ImGui::End();
		}
		});

#pragma endregion

#pragma region WIDGET : 정적 인스턴싱 오브젝트 리스트

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

					if (ImGui::Button("NORMALIZE"))
						XMStoreFloat4(&m_FixLightDesc.vDirection, XMVector3Normalize(XMLoadFloat4(&m_FixLightDesc.vDirection)));

					ImGui::Text("Axis X : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRdirx", &m_FixLightDesc.vDirection.x);
					ImGui::Text("Axis Y : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRdiry", &m_FixLightDesc.vDirection.y);
					ImGui::Text("Axis Z : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRdirz", &m_FixLightDesc.vDirection.z);
					SEPARATOR;

					ImGui::Text("DIFFUSE");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRdifx", &m_FixLightDesc.vDiffuse.x);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRdify", &m_FixLightDesc.vDiffuse.y);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRdifz", &m_FixLightDesc.vDiffuse.z);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRdifw", &m_FixLightDesc.vDiffuse.w);
					SEPARATOR;

					ImGui::Text("AMBIENT");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRambx", &m_FixLightDesc.vAmbient.x);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRamby", &m_FixLightDesc.vAmbient.y);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRambz", &m_FixLightDesc.vAmbient.z);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRambw", &m_FixLightDesc.vAmbient.w);
					SEPARATOR;

					ImGui::Text("SPECULAR");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRspecx", &m_FixLightDesc.vSpecular.x);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRspecy", &m_FixLightDesc.vSpecular.y);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRspecz", &m_FixLightDesc.vSpecular.z);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##DIRspecw", &m_FixLightDesc.vSpecular.w);
					SEPARATOR;
				}
				else if (LIGHT_DESC::POINT == m_FixLightDesc.eType)
				{
					ImGui::Text("POINT");
					SEPARATOR;

					ImGui::Text("POSITION");
					ImGui::Text("X : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIposx", &m_FixLightDesc.vPosition.x, 1.f, 5.f);
					ImGui::Text("Y : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIposy", &m_FixLightDesc.vPosition.y, 1.f, 5.f);
					ImGui::Text("Z : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIposz", &m_FixLightDesc.vPosition.z, 1.f, 5.f);

					if (true == m_isAddLightPoint)
					{
						m_isAddLightPoint = false;

						m_FixLightDesc.vPosition.x = m_vLightPoint.x;
						m_FixLightDesc.vPosition.y = m_vLightPoint.y;
						m_FixLightDesc.vPosition.z = m_vLightPoint.z;
					}

					SEPARATOR;

					ImGui::Text("RANGE"); SAMELINE; ITEMWIDTH(80.f); ImGui::InputFloat("##POIrange", &m_FixLightDesc.fRange);
					SEPARATOR;

					ImGui::Text("DIFFUSE");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIdifx", &m_FixLightDesc.vDiffuse.x);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIdify", &m_FixLightDesc.vDiffuse.y);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIdifz", &m_FixLightDesc.vDiffuse.z);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIdifw", &m_FixLightDesc.vDiffuse.w);
					SEPARATOR;

					ImGui::Text("AMBIENT");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIambx", &m_FixLightDesc.vAmbient.x);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIamby", &m_FixLightDesc.vAmbient.y);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIambz", &m_FixLightDesc.vAmbient.z);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIambw", &m_FixLightDesc.vAmbient.w);
					SEPARATOR;

					ImGui::Text("SPECULAR");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIspecx", &m_FixLightDesc.vSpecular.x);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIspecy", &m_FixLightDesc.vSpecular.y);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIspecz", &m_FixLightDesc.vSpecular.z);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(80.f); ImGui::InputFloat("##POIspecw", &m_FixLightDesc.vSpecular.w);
					SEPARATOR;
				}

				m_pGameInstance->Set_LightDesc(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP), m_FixLightDesc);

				if (ImGui::Button("DONE") || m_pGameInstance->Key_Down(DIK_RETURN) || m_pGameInstance->Key_Down(DIK_NUMPADENTER))
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
					SAMELINE;
					if (ImGui::Button("POINT"))
						m_LightDesc.eType = LIGHT_DESC::POINT;
					SEPARATOR;
					if (LIGHT_DESC::DIRECTIONAL == m_LightDesc.eType)
					ImGui::Text("CURRENT LIGHT TYPE : DIRECTIONAL");

					if (LIGHT_DESC::POINT == m_LightDesc.eType)
					ImGui::Text("CURRENT LIGHT TYPE : POINT");

					ImGui::Text("LIGHT TAG : "); SAMELINE;
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
						if (0 != m_strLightTag.size() && ImGui::Button("LIGHT ADD"))
						{
							m_pGameInstance->Add_Light(AnsiToWString(m_strLightTag), ENUM_CLASS(LEVEL::MAP), m_LightDesc);
							m_LightTags.push_back(m_strLightTag);

							m_iLightTagIndex = m_LightTags.size() - 1;

							ZeroMemory(&m_szLightTag, sizeof(m_szLightTag));
							m_strLightTag.clear();
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

HRESULT CLevel_Map::Ready_Object_SaveLoad_Window()
{
#pragma region WIDGET : OBJECT SAVE 윈도우

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isSaveObjectWindow)
		{
			ImGui::Begin("OBJECT SAVE WINDOW", &m_isSaveObjectWindow, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("MAP INFO SAVE FILE PATH : "); ITEMWIDTH(350.f);
			ImGui::InputText("##map_info_save_path", m_szMapInfoFilePath, IM_ARRAYSIZE(m_szMapInfoFilePath));
			ImGui::Text("MAP INFO FILE NAME : "); ITEMWIDTH(350.f);
			ImGui::InputText("##map_info_save_name", m_szMapInfoFileName, IM_ARRAYSIZE(m_szMapInfoFileName));

			SEPARATOR;

			if (ImGui::Button("SAVE"))
			{
				// m_strMapInfoFilePath : 뒤에 _prototypes.dat, _objs.dat, insts.dat 이런식으로 ㄱㄱ
				m_strMapInfoFilePath = m_szMapInfoFilePath;
				m_strMapInfoFilePath += m_szMapInfoFileName;

#pragma region 프로토타입 일괄 저장

				if (false == Prototypes_Save_Binary())
				{
#ifdef _DEBUG
					OutputDebugStringA("프로토 타입 정보 바이너리화 실패");
#endif // _DEBUG
					return;
				}

#pragma endregion

#pragma region 오브젝트 일괄 저장

				if (false == Objects_Save_Binary())
				{
#ifdef _DEBUG
					OutputDebugStringA("단일 오브젝트 정보 바이너리화 실패");
#endif // _DEBUG
					return;
				}

#pragma endregion

				m_isSaveObjectWindow = false;
			}

			ImGui::End();
		}
		});

#pragma endregion

#pragma region WIDGET : OBJECT LOAD 윈도우

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isLoadObjectWindow)
		{
			ImGui::Begin("OBJECT LOAD WINDOW", &m_isLoadObjectWindow, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("MAP INFO LOAD FILE PATH : "); ITEMWIDTH(350.f);
			ImGui::InputText("##map_info_load_path", m_szMapInfoFilePath, IM_ARRAYSIZE(m_szMapInfoFilePath));
			ImGui::Text("MAP INFO FILE NAME : "); ITEMWIDTH(350.f);
			ImGui::InputText("##map_info_load_name", m_szMapInfoFileName, IM_ARRAYSIZE(m_szMapInfoFileName));

			SEPARATOR;

			if (ImGui::Button("LOAD"))
			{
				// m_strMapInfoFilePath : 뒤에 _prototypes.dat, _objs.dat, insts.dat 이런식으로 ㄱㄱ
				m_strMapInfoFilePath = m_szMapInfoFilePath;
				m_strMapInfoFilePath += m_szMapInfoFileName;
				
#pragma region 프로토타입 데이터 통해 일괄 등록

				if (false == Prototypes_Load_Binary())
				{
#ifdef _DEBUG
					OutputDebugStringA("단일 오브젝트 정보 바이너리 불러오기 실패");
#endif // _DEBUG
					return;
				}

				// 프로토타입 윈도우 띄우기
				m_isPrototypeWindow = true;

#pragma endregion

#pragma region 오브젝트 일괄 저장

				if (false == Objects_Load_Binary())
				{
#ifdef _DEBUG
					OutputDebugStringA("단일 오브젝트 정보 바이너리 불러오기 실패");
#endif // _DEBUG
					return;
				}

#pragma endregion

				m_isLoadObjectWindow = false;

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

void CLevel_Map::Fbxs_Convert_To_Dat(const _char* pFolderName)
{
	vector<string> FBXPaths;
	string strRootPath = "../../Client/Bin/Resources/Map/Prop/";

	strRootPath += pFolderName;

	try
	{
		for (const auto& entry : filesystem::recursive_directory_iterator(strRootPath))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".fbx")
			{
				FBXPaths.push_back(entry.path().string());
			}
		}
	}
	catch (const std::exception& e)
	{
		cerr << "filesystem error: " << e.what() << endl;
	}

	_matrix PreTransformMatrix = XMMatrixIdentity();

	// 스케일 변환 ( 1 / 100 )
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	for (auto& pPath : FBXPaths)
	{
		replace(pPath.begin(), pPath.end(), '\\', '/');

		CEditor_Model* pModel = CEditor_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, pPath.c_str(), PreTransformMatrix);
		if (nullptr == pModel)
		{
			string strFailName = "[MISSING]" + pPath;
			OutputDebugStringA(strFailName.c_str());
			continue;
		}

		m_strDataSavePath = m_szDataSavePath;

		_char szFileName[MAX_PATH] = {};

		_splitpath_s(pPath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		m_strDataSavePath += szFileName;
		m_strDataSavePath += ".fbx";

		pModel->ExportModel_NoMsg(m_strDataSavePath);

		Safe_Release(pModel);
	}
}

void CLevel_Map::Add_Prototype_ByFolder(const _char* pFolderName, _bool isAnim)
{
	vector<string> FBXPaths;
	string strRootPath = "../../Client/Bin/Data/Map/Prop/";

	if (true == isAnim)
		strRootPath += "Anim/";
	else
		strRootPath += "NonAnim/";

	strRootPath += pFolderName;

	strRootPath += '/';

	try
	{
		for (const auto& entry : filesystem::recursive_directory_iterator(strRootPath))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".dat")
			{
				FBXPaths.push_back(entry.path().string());
			}
		}
	}
	catch (const std::exception& e)
	{
		cerr << "filesystem error: " << e.what() << endl;
	}

	_matrix PreTransformMatrix = XMMatrixIdentity();

	// 스케일 변환 ( 1 / 100 )
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	for (auto& pPath : FBXPaths)
	{
		_char szModelName[MAX_PATH] = {};

		_splitpath_s(pPath.c_str(), nullptr, 0, nullptr, 0, szModelName, MAX_PATH, nullptr, 0);

		string strModelName = szModelName;

		if (true == m_pGameInstance->Already_Registered_Prototype(ENUM_CLASS(LEVEL::MAP), AnsiToWString(strModelName)))
			continue;

		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), AnsiToWString(strModelName),
			CModel::Create(m_pDevice, m_pContext, pPath.c_str()))))
		{
			string failed = { "\n프로토타입 등록되어있는듯" };

			OutputDebugStringA(failed.c_str());

			continue;
		}

		m_Prototypes_Obj.push_back(strModelName);
	}

	m_isPrototypeWindow = true;
}

string CLevel_Map::Find_ModelPath(const string& strModelName, const string& strFileExtern)
{
	string strRoot = {};

	if (".fbx" != strFileExtern && ".dat" != strFileExtern)
		return "NOTFOUND";

	if (".fbx" == strFileExtern)
		strRoot = "../../Client/Bin/Resources/Map/Prop/";
	else if (".dat" == strFileExtern)
		strRoot = "../../Client/Bin/Data/Map/";

	for (auto& entry : filesystem::recursive_directory_iterator(strRoot))
	{
		if (entry.is_regular_file() && entry.path().extension() == strFileExtern.c_str())
		{
			if (entry.path().stem() == strModelName)
				return entry.path().string();
		}
	}

	return "NOTFOUND";
}

_bool CLevel_Map::Prototypes_Save_Binary()
{
	// 프로토 타입 저장할때는 인스턴스용 모델인지, 아니면 일반 모델인지 구분해서 저장을 해야한다.
	// Object로 사용한 Model만 프로토타입 등록
	_wstring strPrototypeInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strPrototypeInfoPath += TEXT("_prototypes.dat");

	DWORD dwByte = {};
	
	// 폴더가 존재하지 않으면 생성
	if (false == filesystem::exists(m_szMapInfoFilePath))
	{
		if (false == filesystem::create_directories(m_szMapInfoFilePath))
		{
#ifdef _DEBUG
			OutputDebugStringA("폴더 생성 실패");
#endif // _DEBUg
			return false;
		}
	}

	// 프로토타입 핸들 개방
	HANDLE hPrototypeFile = CreateFile(strPrototypeInfoPath.c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hPrototypeFile)
	{
		CloseHandle(hPrototypeFile);
		return false;
	}
	else
	{
		// 프로토 타입 개수 카운트
		_uint iPrototypeCnt = {};

		map<const _wstring, SAVE_PROTOTYPE> Prototypes;

		// 단일 오브젝트 순회하면서 모델 이름 알아오기 ( Prototype 태그로 사용할 것 )
		for (auto& pProp : m_ObjectList)
		{
			// 기본 양식 지키기 ( Prototype_Component_Model_모델파일명 )
			_wstring strPrototypeTag = TEXT("Prototype_Component_Model_");

			// 기존 모델 명
			wstring strModelName = pProp->Get_ModelName();

			strPrototypeTag += strModelName;

			auto iter = Prototypes.find(strPrototypeTag);

			if (iter == Prototypes.end())
			{
				string strModelPath = Find_ModelPath(WStringToAnsi(strModelName).c_str(), ".dat");

				CHECK_EQUAL_MSG("NOTFOUND", strModelPath, TEXT("모델 경로 못찾음"), false);

				replace(strModelPath.begin(), strModelPath.end(), '\\', '/');

				SAVE_PROTOTYPE Save_Proto = {};
				Save_Proto.eType = MAPOBJECT_TYPE::OBJECT;
				Save_Proto.strModelPath = strModelPath;

				Prototypes.emplace(strPrototypeTag, Save_Proto);

				// 중복 아닐때만 Count 증가
				++iPrototypeCnt;
			}
		}
		
		// 1. 프로토 타입의 총 개수 저장 ( 이만큼 루프 돌릴거 )
		WriteFile(hPrototypeFile, &iPrototypeCnt, sizeof(_uint), &dwByte, nullptr);

		for (auto& pPrototype : Prototypes)
		{
			// 2. 어떤 타입인지 저장 ( Object, Instance, Dynamic, Interactive ) , enum class MAPOBJECT_TYPE은 unsigned short 사용으로 조금 메모리 절약
			_ushort sMapObjType = static_cast<_ushort>(pPrototype.second.eType);
			WriteFile(hPrototypeFile, &sMapObjType, sizeof(_ushort), &dwByte, nullptr);

			// 프로토 타입 태그 길이
			_uint iPrototypeTagLen = pPrototype.first.size();
			// 모델 경로 길이
			_uint iModelPathLen = pPrototype.second.strModelPath.size();

			// 3. 프로토 타입 태그 길이 저장
			WriteFile(hPrototypeFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr);
			// 4. 프로토 타입 태그 이름 저장
			WriteFile(hPrototypeFile, pPrototype.first.c_str(), sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr);

			// 5. 모델 경로 길이 저장
			WriteFile(hPrototypeFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr);
			// 6. 모델 경로 이름 저장
			WriteFile(hPrototypeFile, pPrototype.second.strModelPath.c_str(), sizeof(_char) * iModelPathLen, &dwByte, nullptr);
		}

		// 검사용 map clear;
		Prototypes.clear();
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hPrototypeFile);

	return true;
}

_bool CLevel_Map::Objects_Save_Binary()
{
	_wstring strObjectInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strObjectInfoPath += TEXT("_objects.dat");

	DWORD dwByte = {};

	// 프로토타입 핸들 개방
	HANDLE hObjectFile = CreateFile(strObjectInfoPath.c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hObjectFile)
	{
		CloseHandle(hObjectFile);
		return false;
	}
	else
	{
		// 오브젝트 총 개수 카운트
		_uint iObjectCnt = {};

		for (auto& pProp : m_ObjectList)
			++iObjectCnt;

		// 1. 오브젝트의 총 개수 저장
		WriteFile(hObjectFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);

		// 단일 오브젝트 순회하면서 모델 이름 알아오기 ( Prototype 태그로 사용할 것 )
		for (auto& pProp : m_ObjectList)
		{
			// 기본 양식 지키기 ( Prototype_Component_Model_모델파일명 ) ( Layer 추가에 사용할 것, 모델명 던져주기 )
			_wstring strPrototypeTag = TEXT("Prototype_Component_Model_");
			strPrototypeTag += pProp->Get_ModelName();

			// 모델 이름 길이
			_uint iPrototypeLen = strPrototypeTag.size();

			// 2. 프로토 타입 태그 길이 저장
			WriteFile(hObjectFile, &iPrototypeLen, sizeof(_uint), &dwByte, nullptr);
			// 3. 프로토 타입 태그 이름 저장
			WriteFile(hObjectFile, strPrototypeTag.c_str(), sizeof(_tchar) * iPrototypeLen, &dwByte, nullptr);

			// 객체당 월드행렬 빼오기
			CTransform* pTransform = static_cast<CTransform*>(pProp->Get_Component(TEXT("Com_Transform")));
			CHECK_NULLPTR_MSG(pTransform, TEXT("nullptr == pTransform"), false);

			_float4x4 WorldMatrix = {};

			XMStoreFloat4x4(&WorldMatrix, pTransform->Get_WorldMatrix());

			// 4. 객체당 월드행렬 저장
			WriteFile(hObjectFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
		}
		// 단일 오브젝트 이외의 것들 추가 예정
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hObjectFile);

	return true;
}

_bool CLevel_Map::Prototypes_Load_Binary()
{
	_wstring pDataFilePath = AnsiToWString(m_strMapInfoFilePath);

	pDataFilePath += TEXT("_prototypes.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("[DAT ERROR] 바이너리 파일 오픈 문제"), false);

	// 1. 프로토 타입의 총 개수
	_uint iPrototypeCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iPrototypeCnt, sizeof(_uint), &dwByte, nullptr), false);

	// 프로토 타입의 총 개수만큼 순회
	for (_uint i = 0; i < iPrototypeCnt; ++i)
	{
		// 2. MapObject 타입 가져오기 ( _ushort형으로 저장해서 형변환 후 사용 )
		_ushort sMapObjectType = {};
		CHECK_FALSE(ReadFile(hFile, &sMapObjectType, sizeof(_ushort), &dwByte, nullptr), false);

		MAPOBJECT_TYPE eMapObjType = static_cast<MAPOBJECT_TYPE>(sMapObjectType);

		// MapObject 타입에 따른 조건문
		if (MAPOBJECT_TYPE::OBJECT == eMapObjType ||
			MAPOBJECT_TYPE::INTERACTIVE == eMapObjType ||
			MAPOBJECT_TYPE::DYNAMIC == eMapObjType)
		{
			// CModel 을 열어야 하는 경우 ( Instance X )

			// 3. 프로토 타입 태그 길이 저장
			_uint iPrototypeTagLen = {};
			CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), false);

			// 4. 프로토 타입 태그 이름 저장
			_tchar szPrototypeTag[MAX_PATH] = {};
			CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), false);

			// 5. 모델 경로 길이 저장
			_uint iModelPathLen = {};
			CHECK_FALSE(ReadFile(hFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr), false);

			// 6. 모델 경로 이름 저장
			_char szModelPath[MAX_PATH] = {};
			CHECK_FALSE(ReadFile(hFile, &szModelPath, sizeof(_char) * iModelPathLen, &dwByte, nullptr), false);

			if (true == m_pGameInstance->Already_Registered_Prototype(ENUM_CLASS(LEVEL::MAP), szPrototypeTag))
				continue;

			// Prototype_Component_Model_ 자르기 시작
			_wstring strFullPrototypeTag = szPrototypeTag;
			_wstring strPreFix = { TEXT("Prototype_Component_Model_") };

			size_t Pos = strFullPrototypeTag.find(strPreFix);

			if (Pos != wstring::npos)
			{
				strFullPrototypeTag = strFullPrototypeTag.substr(Pos + strPreFix.length());
			}

			wcscpy_s(szPrototypeTag, strFullPrototypeTag.c_str());
			// Prototype_Component_Model_ 자르기 끝

			if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), szPrototypeTag,
				CModel::Create(m_pDevice, m_pContext, szModelPath))))
			{
				CloseHandle(hFile);
				MSG_BOX(TEXT("[DAT ERROR] 맵 오브젝트 프로토타입 등록 실패 ( CModel )"));
				return false;
			}

			_wstring strPrototypeTag = szPrototypeTag;

			m_Prototypes_Obj.push_back(WStringToAnsi(strPrototypeTag));
		}
		else if (MAPOBJECT_TYPE::STATIC_INST == eMapObjType || MAPOBJECT_TYPE::ANIMATED_INST == eMapObjType)
		{
			// CModel_Instance 를 열어야 하는 경우 ( Instance O )
			// 
			// 3. 프로토 타입 태그 길이 저장
			_uint iPrototypeTagLen = {};
			CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), false);

			// 4. 프로토 타입 태그 이름 저장
			_tchar szPrototypeTag[MAX_PATH] = {};
			CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), false);

			// 5. 모델 경로 길이 저장
			_uint iModelPathLen = {};
			CHECK_FALSE(ReadFile(hFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr), false);

			// 6. 모델 경로 이름 저장
			_char szModelPath[MAX_PATH] = {};
			CHECK_FALSE(ReadFile(hFile, &szModelPath, sizeof(_char) * iModelPathLen, &dwByte, nullptr), false);

			// 추후에 인스턴스 추가해야하는 코드 부분 ( vector<MESH_INSTANCE_DATA> )
			CModelMesh_Instance::MODELMESH_INSTANCE_DESC InstanceDesc = {};

			// 7. 인스턴스 개수
			_uint iNumInstance = {};
			CHECK_FALSE(ReadFile(hFile, &iNumInstance, sizeof(_uint), &dwByte, nullptr), false);

			for (_uint j = 0; j < InstanceDesc.iNumInstance; ++j)
			{
				// 8. 인스턴스 개수 만큼 순회하면서 벡터에 Push_Back ( MapEditor에서 사용한 InstanceID는 빼고 파일 입출력해도 괜찮을 거 같음 )
				_matrix InstanceMatrix = {};
				CHECK_FALSE(ReadFile(hFile, &InstanceMatrix, sizeof(_matrix), &dwByte, nullptr), false);

				MESH_INSTANCE_DATA InstanceData = {};
				InstanceData.InstanceMatrix = InstanceMatrix;
				InstanceData.InstanceID = j;

				InstanceDesc.InstanceData.push_back(InstanceData);
			}

			if (true == m_pGameInstance->Already_Registered_Prototype(ENUM_CLASS(LEVEL::MAP), szPrototypeTag))
				continue;

			if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), szPrototypeTag,
				CModel_Instance::Create(m_pDevice, m_pContext, szModelPath, &InstanceDesc))))
			{
				CloseHandle(hFile);
				MSG_BOX(TEXT("[DAT ERROR] 맵 오브젝트 프로토타입 등록 실패 ( CModel_Instance )"));
				return false;
			}

			return false; // 일단 지금 되면 안됨
		}
		else
		{
			CloseHandle(hFile);
			MSG_BOX(TEXT("[DAT ERROR] DAT 파일 읽는중 TYPE 문제 ( 박준영 문제 )"));
			return false;
		}
	}

	CloseHandle(hFile);

	return true;
}

_bool CLevel_Map::Objects_Load_Binary()
{
	_wstring strObjectInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strObjectInfoPath += TEXT("_objects.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(strObjectInfoPath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL(INVALID_HANDLE_VALUE, hFile, false);

	// 1. 오브젝트의 총 개수
	_uint iObjectCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), false);

	// 오브젝트 총 개수만큼 순회
	for (_uint i = 0; i < iObjectCnt; ++i)
	{
		CProp_Object::PROP_OBJECT_DESC ObjectDesc = {};

		// 맵 에디터용 MapObjectID
		ObjectDesc.iMapObjectID = m_iMapObjectCnt++;

		// 2. 프로토 타입 태그 길이 불러오기
		_uint iPrototypeTagLen = {};
		CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), false);

		// 3. 프로토 타입 태그 이름 불러오기
		_tchar szPrototypeTag[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), false);

		// 불러온 태그 카피
		memcpy(ObjectDesc.szModelName, szPrototypeTag, sizeof(ObjectDesc.szModelName));

		// Prototype_Component_Model_ 자르기 시작
		_wstring strFullPrototypeTag = ObjectDesc.szModelName;
		_wstring strPreFix = { TEXT("Prototype_Component_Model_") };

		size_t Pos = strFullPrototypeTag.find(strPreFix);

		if (Pos != wstring::npos)
		{
			strFullPrototypeTag = strFullPrototypeTag.substr(Pos + strPreFix.length());
		}

		wcscpy_s(ObjectDesc.szModelName, strFullPrototypeTag.c_str());
		// Prototype_Component_Model_ 자르기 끝

		// 현재 이 레벨을 넘겨줌
		ObjectDesc.eLevel = LEVEL::MAP;

		// 4. 객체당 월드 행렬 때오기
		_float4x4 WorldMatrix = {};
		CHECK_FALSE(ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr), false);

		ObjectDesc.WorldMatrix = WorldMatrix;

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj"),
			ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc), false);

		CProp* pProp = static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj")));
		CHECK_NULLPTR_MSG(pProp, TEXT("[OBJECT LOAD] 오브젝트 찾기 실패"), false);

		m_ObjectList.push_back(pProp);
	}

	CloseHandle(hFile);

	return true;
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
