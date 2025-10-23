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

	Test_Player_Move(fTimeDelta);
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
	//CHECK_FAILED(Ready_Default_Lights(), E_FAIL);

	//CHECK_FAILED(Ready_Layer_Khazan(TEXT("Layer_Khazan")), E_FAIL);

	CHECK_FAILED(Ready_Layer_Camera(TEXT("Layer_Map_Camera")), E_FAIL);

	CHECK_FAILED(Ready_Layer_Terrain(TEXT("Layer_Map_Terrain")), E_FAIL);
	
	CHECK_FAILED(Ready_Layer_Preview(TEXT("Layer_Preview")), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_Khazan(const _wstring& strLayerTag)
{
	// Prototype_GameObject_Map_TestPlayer

	CMap_TestPlayer::GAMEOBJECT_DESC ObjDesc = {};

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Map_TestPlayer")), E_FAIL);

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

HRESULT CLevel_Map::Ready_Layer_Preview(const _wstring& strLayerTag)
{
	CProp::PROP_DESC Desc = {};

	Desc.fRotationPerSec = XMConvertToRadians(90.f);

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Preview"), &Desc), E_FAIL);

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

void CLevel_Map::Test_Player_Move(_float fTimeDelta)
{
	if (true == m_isFixObjectWindow)
		return;

	if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
	{
		_float3 vPosition = {};

		if (m_pGameInstance->isPicked(&vPosition))
		{
			CTransform* pKhazan = static_cast<CTransform*>(m_pGameInstance->Find_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Khazan"), TEXT("Com_Transform")));
			CHECK_NULLPTR(pKhazan, );

			pKhazan->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
		}
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

						m_FixBaseMatrix = XMMatrixIdentity();

						ZeroMemory(&m_vFixScale, sizeof(_float3));
						ZeroMemory(&m_vFixRotation, sizeof(_float3));
						ZeroMemory(&m_vFixPosition, sizeof(_float3));

						m_vFixScale = m_pFixTransformCom->Get_Scaled();
						XMStoreFloat3(&m_vFixPosition, m_pFixTransformCom->Get_State(STATE::POSITION));

						m_FixBaseMatrix = m_FixWorldMatrix = m_pFixTransformCom->Get_WorldMatrix();

						// ======================================================
						// ======================================================

						m_isFixObjectWindow = true;
						m_eFixType = FIX_OBJECT::FIX;
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

	CHECK_FAILED(Ready_Prototype_List_Window(), E_FAIL);

	CHECK_FAILED(Ready_Prop_Fix_Window(), E_FAIL);

	CHECK_FAILED(Ready_Prop_List_Window(), E_FAIL);

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

			if (ImGui::Button("INFORMATION OR MAIN"))
				m_isInformation = !m_isInformation;

				if (true == m_isInformation)
				{
					ImGui::Text("F1 + LB      : SELECT OBJECT");
					ImGui::Text("F2 + LB      : SELECT INSTANCE");
					ImGui::Text("F3 + LB      : ADD LIGHT POSITION");
					ImGui::Text("F4 + LB      : SELECTED OBJECT MOVE");

					SEPARATOR;

					ImGui::Text("[            : PREVIEW");
					ImGui::Text("]            : CANCEL PREVIEW");

					SEPARATOR;

					ImGui::Text("F5           : MOVE TERRAIN");
					ImGui::Text("F6           : TERRAIN RENDER");

					SEPARATOR;

					_float4 vCamPos = *m_pGameInstance->Get_CamPosition();

					ImGui::Text("CAMERA POS");
					ImGui::Text("X : %.3f", vCamPos.x);
					ImGui::Text("Y : %.3f", vCamPos.y);
					ImGui::Text("Z : %.3f", vCamPos.z);
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
					
					ImGui::Text("MAP DATA SAVE & LOAD");
					if (ImGui::Button("SAVE")) m_isSaveObjectWindow = !m_isSaveObjectWindow;
					if (false == m_isLoaded)
					{
						SAMELINE;
						if (ImGui::Button("LOAD")) m_isLoadObjectWindow = !m_isLoadObjectWindow;
					}
					SEPARATOR;
					
					ImGui::Text("PROP LIST");
					if (ImGui::Button("OBJECT##active"))		m_isObjectWindow = !m_isObjectWindow;
					SEPARATOR;

					ImGui::Text("ADD PROTOTYPES");
					ImGui::Text("FOLDER : "); SAMELINE;
					ImGui::InputText("##folder_name_convert", m_szFolderName, IM_ARRAYSIZE(m_szFolderName));

					_uint iFolderNameLen = strlen(m_szFolderName);

					if (0 != iFolderNameLen)
					{
						if (ImGui::Button("PROTOTYPES ADD"))
						{
							Add_Prototype_ByFolder(m_szFolderName);
							ZeroMemory(m_szFolderName, sizeof(m_szFolderName));
						}

						if (ImGui::Button("FBX FILE CONVERT ( .fbx > .dat )"))
						{
							Fbxs_Convert_To_Dat(m_szFolderName);
							ZeroMemory(m_szFolderName, sizeof(m_szFolderName));
						}
					}
					SEPARATOR;

					ImGui::Text("DON'T USE");

					if (ImGui::Button("CLEAR JSON LIST"))
					{
						m_isMainWindow = { true };
						m_isPrototypeWindow = { false };

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

HRESULT CLevel_Map::Ready_Prototype_List_Window()
{
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPrototypeWindow)
		{
			ImGui::Begin("PROTOTYPE WINDOW", &m_isPrototypeWindow, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("MODEL PROTOTYPES");
			ImGui::Text("SEARCH : "); SAMELINE;
			ImGui::InputText("##search_model_name", m_szSearchPrototypeName, IM_ARRAYSIZE(m_szSearchPrototypeName)); SAMELINE;

			if (ImGui::Button("CLEAR"))
				ZeroMemory(m_szSearchPrototypeName, sizeof(m_szSearchPrototypeName));

			if (ImGui::BeginListBox("##prototype_object_list"))
			{
				string strSearchName = m_szSearchPrototypeName;
				transform(strSearchName.begin(), strSearchName.end(), strSearchName.begin(), ::tolower);		// 검색할 모델을 소문자로 변환

				for (_uint i = 0; i < m_Prototypes_Obj.size(); ++i)
				{
					string strModelName = m_Prototypes_Obj[i];
					transform(strModelName.begin(), strModelName.end(), strModelName.begin(), ::tolower);		// 찾을 모델을 소문자로 변환

					if (true == strSearchName.empty() || strModelName.find(strSearchName) != string::npos)
					{
						_bool isSelected = (m_iIndex_PrtObj == i);

						if (ImGui::Selectable(m_Prototypes_Obj[i].c_str(), isSelected))
						{
							m_iIndex_PrtObj = i;
							CProp_Preview* pProp = static_cast<CProp_Preview*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Preview")));
							if (nullptr == pProp)
							{
								OutputDebugStringA("프리뷰 == nullptr");
							}
							else if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB))
							{
								pProp->Set_MouseMove(true);
								pProp->Set_MouseMove(true);
							}
							else
							{
								pProp->Reset_Preview();

								CModel* pModel = static_cast<CModel*>(m_pGameInstance->Find_Prototype_ForPreview(ENUM_CLASS(LEVEL::MAP), AnsiToWString(m_Prototypes_Obj[m_iIndex_PrtObj])));
								if (nullptr == pModel)
									pProp->Set_PrototypeModel(nullptr);
								else
									pProp->Set_PrototypeModel(pModel);
							}
						}
					}
				}

				ImGui::EndListBox();
			} SEPARATOR;

			ImGui::Text("SCALE SIZE : "); SAMELINE;
			ImGui::InputFloat("##input_scale_add", &m_fAddScale, 0.001f, 0.005f); SAMELINE;
			if (ImGui::Button("RESET")) m_fAddScale = 0.005f;
			SEPARATOR;

			ImGui::Checkbox("CAMERA POS ADD", &m_isCameraPosAdd); SEPARATOR;

			if (false == m_isCameraPosAdd)
			{
				ImGui::Text("ADD POS Y : "); SAMELINE;
				ImGui::InputFloat("##add_pos_y", &m_fAddPositionY, 0.1f, 1.f); SEPARATOR;
			}

			ImGui::Checkbox("SNOW", &m_AddObjectProperties.isSnow); SAMELINE;
			ImGui::Checkbox("COLLIDER", &m_AddObjectProperties.isCollider); SAMELINE;
			ImGui::Checkbox("BLENDED", &m_AddObjectProperties.isBlended); SAMELINE;
			ImGui::Checkbox("INSTANCE", &m_AddObjectProperties.isInstance); SEPARATOR;
			ImGui::Checkbox("SHADOW", &m_AddObjectProperties.isShadow); SAMELINE;
			ImGui::Checkbox("BACKGROUND", &m_AddObjectProperties.isBackGround); SEPARATOR;

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
						if (false == m_isCameraPosAdd)
							vPos.y = m_fAddPositionY;
					}
				}
				else
				{
					XMStoreFloat3(&vPos, XMLoadFloat4(m_pGameInstance->Get_CamPosition()));
					if (false == m_isCameraPosAdd)
						vPos.y = m_fAddPositionY;
				}

				_matrix WorldMatrix = XMMatrixIdentity();

				// 스케일 기존 0.005f, 위치는 마우스 피킹 위치 혹은 카메라 위치
				WorldMatrix.r[0] *= m_fAddScale;
				WorldMatrix.r[1] *= m_fAddScale;
				WorldMatrix.r[2] *= m_fAddScale;
				WorldMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&vPos), 1.f);

				XMStoreFloat4x4(&ObjectDesc.WorldMatrix, WorldMatrix);

				ObjectDesc.Properties = m_AddObjectProperties;

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

	return S_OK;
}

HRESULT CLevel_Map::Ready_Prop_Fix_Window()
{
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

			if (FIX_OBJECT::FIX == m_eFixType)
			{
				ImGui::Text("SCALE FIX"); SAMELINE;
				if (ImGui::Button("-")) { m_vFixScale.x -= 0.001f; m_vFixScale.y -= 0.001f; m_vFixScale.z -= 0.001f; } SAMELINE;
				if (ImGui::Button("+")) { m_vFixScale.x += 0.001f; m_vFixScale.y += 0.001f; m_vFixScale.z += 0.001f; } SEPARATOR;

				ImGui::Text("SCALE X : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##scalex", &m_vFixScale.x, 0.001f, 0.01f);
				ImGui::SliderFloat("##sliderdetailscalex", &m_vFixScale.x, 0.001f, 10.f);
				ImGui::Text("SCALE Y : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##scaley", &m_vFixScale.y, 0.001f, 0.01f);
				ImGui::SliderFloat("##sliderdetailscaley", &m_vFixScale.y, 0.001f, 10.f);
				ImGui::Text("SCALE Z : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##scalez", &m_vFixScale.z, 0.001f, 0.01f);
				ImGui::SliderFloat("##sliderdetailscalez", &m_vFixScale.z, 0.001f, 10.f);

				if (0.001f > m_vFixScale.x) m_vFixScale.x = 0.001f;
				if (0.001f > m_vFixScale.y) m_vFixScale.y = 0.001f;
				if (0.001f > m_vFixScale.z) m_vFixScale.z = 0.001f;

				SEPARATOR;
				SEPARATOR;

				ImGui::Text("ROTATION FIX");
				SEPARATOR;

				ImGui::Text("ROT X : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##rotationx", &m_vFixRotation.x, 1.f, 5.f);
				ImGui::SliderFloat("##sliderrotationx", &m_vFixRotation.x, -180.f, 180.f);
				ImGui::Text("ROT Y : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##rotationy", &m_vFixRotation.y, 1.f, 5.f);
				ImGui::SliderFloat("##sliderrotationy", &m_vFixRotation.y, -180.f, 180.f);
				ImGui::Text("ROT Z : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##rotationz", &m_vFixRotation.z, 1.f, 5.f);
				ImGui::SliderFloat("##sliderrotationz", &m_vFixRotation.z, -180.f, 180.f);

				_float fPitch = XMConvertToRadians(m_vFixRotation.x);
				_float fYaw = XMConvertToRadians(m_vFixRotation.y);
				_float fRoll = XMConvertToRadians(m_vFixRotation.z);

				_matrix DeltaRotMatirx = XMMatrixRotationZ(fRoll) * XMMatrixRotationX(fPitch) * XMMatrixRotationY(fYaw);

				_vector vScale = {};
				_vector vRotation = {};
				_vector vTranslation = {};

				XMMatrixDecompose(&vScale, &vRotation, &vTranslation, m_FixBaseMatrix);

				_matrix BaseRotMatrix = XMMatrixRotationQuaternion(vRotation);

				_matrix AddRotMatrix = DeltaRotMatirx * BaseRotMatrix;

				_matrix NewWorldMatrix = XMMatrixScaling(m_vFixScale.x, m_vFixScale.y, m_vFixScale.z) * AddRotMatrix * XMMatrixTranslation(m_vFixPosition.x, m_vFixPosition.y, m_vFixPosition.z);

				m_FixWorldMatrix = NewWorldMatrix;

				SEPARATOR;
				SEPARATOR;

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

				_float fPosMove = { 0.01f };

				if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, 0.f)) fPosMove *= 10.f;

				ImGui::Text("POS X : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##positionx", &m_vFixPosition.x, fPosMove, fPosMove * 5.f);
				ImGui::Text("POS Y : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##positiony", &m_vFixPosition.y, fPosMove, fPosMove * 5.f);
				ImGui::Text("POS Z : "); SAMELINE; ITEMWIDTH(100.f); ImGui::InputFloat("##positionz", &m_vFixPosition.z, fPosMove, fPosMove * 5.f);

				m_FixWorldMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&m_vFixPosition), 1.f);

				m_pFixTransformCom->Set_State(STATE::RIGHT, m_FixWorldMatrix.r[0]);
				m_pFixTransformCom->Set_State(STATE::UP, m_FixWorldMatrix.r[1]);
				m_pFixTransformCom->Set_State(STATE::LOOK, m_FixWorldMatrix.r[2]);
				m_pFixTransformCom->Set_State(STATE::POSITION, m_FixWorldMatrix.r[3]);

				SEPARATOR;
				SEPARATOR;

			}

#pragma region 속성 설정

			ImGui::Text("SETTING");

			MAPOBJECT_PROPERTIES PropProperties = m_pFixPropObj->Get_Properties();

			ImGui::Checkbox("SNOW", &PropProperties.isSnow);
			SAMELINE;

			ImGui::Checkbox("COLLIDER", &PropProperties.isCollider);
			SAMELINE;

			ImGui::Checkbox("BLENDED", &PropProperties.isBlended);
			SAMELINE;

			ImGui::Checkbox("INSTANCE", &PropProperties.isInstance);
			SEPARATOR;

			ImGui::Checkbox("SHADOW", &PropProperties.isShadow);
			SAMELINE;

			ImGui::Checkbox("BACKGROUND", &PropProperties.isBackGround);
			SEPARATOR;

			m_pFixPropObj->Set_Properties(PropProperties);

			SEPARATOR;
			SEPARATOR;

#pragma endregion

			if (ImGui::Button("DONE") || m_pGameInstance->Key_Down(DIK_RETURN) || m_pGameInstance->Key_Down(DIK_NUMPADENTER))
			{
				m_FixBaseMatrix = XMMatrixIdentity();

				ZeroMemory(&m_vFixScale, sizeof(_float3));
				ZeroMemory(&m_vFixRotation, sizeof(_float3));
				ZeroMemory(&m_vFixPosition, sizeof(_float3));

				m_pFixPropObj = nullptr;
				m_pFixTransformCom = nullptr;
				m_isFixObjectWindow = false;
				m_eFixType = FIX_OBJECT::END;

			} SAMELINE;
			if (ImGui::Button("RESET"))
			{
				m_pFixTransformCom->Set_State(STATE::RIGHT, m_FixBaseMatrix.r[0]);
				m_pFixTransformCom->Set_State(STATE::UP, m_FixBaseMatrix.r[1]);
				m_pFixTransformCom->Set_State(STATE::LOOK, m_FixBaseMatrix.r[2]);
				m_pFixTransformCom->Set_State(STATE::POSITION, m_FixBaseMatrix.r[3]);

				m_FixBaseMatrix = XMMatrixIdentity();

				ZeroMemory(&m_vFixScale, sizeof(_float3));
				ZeroMemory(&m_vFixRotation, sizeof(_float3));
				ZeroMemory(&m_vFixPosition, sizeof(_float3));

				m_pFixPropObj = nullptr;
				m_pFixTransformCom = nullptr;
				m_isFixObjectWindow = false;
				m_eFixType = FIX_OBJECT::END;
			}
			SEPARATOR;
			SEPARATOR;
			if (ImGui::Button("DELETE (DELETE)") || m_pGameInstance->Key_Down(DIK_DELETE))
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
				m_pFixTransformCom = nullptr;
				m_isFixObjectWindow = false;
				m_eFixType = FIX_OBJECT::END;
			}

			ImGui::End();
		}
		});

	return S_OK;
}

HRESULT CLevel_Map::Ready_Prop_List_Window()
{
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isObjectWindow)
		{
			ImGui::Begin("PROP OBJECT WINDOW", &m_isObjectWindow, ImGuiWindowFlags_AlwaysAutoResize);

			if (false == m_isCheckRender)
			{
				ImGui::Text("OBJECT LIST");
				ImGui::Text("SEARCH : "); SAMELINE;
				ImGui::InputText("##search_object_name", m_szSearchObjectName, IM_ARRAYSIZE(m_szSearchObjectName)); SAMELINE;

				if (ImGui::Button("CLEAR"))
					ZeroMemory(m_szSearchObjectName, sizeof(m_szSearchObjectName));

				if (ImGui::BeginListBox("##prop_object_list"))
				{
					if (m_iObjectListIndex >= m_ObjectList.size())
						m_iObjectListIndex = m_ObjectList.size() - 1;

					string strSearchName = m_szSearchObjectName;
					transform(strSearchName.begin(), strSearchName.end(), strSearchName.begin(), ::tolower);		// 검색할 모델을 소문자로 변환

					for (_uint i = 0; i < m_ObjectList.size(); ++i)
					{
						_wstring strModelName = m_ObjectList[i]->Get_ModelName();
						transform(strModelName.begin(), strModelName.end(), strModelName.begin(), ::tolower);		// 찾을 모델을 소문자로 변환

						if (true == strSearchName.empty() || strModelName.find(AnsiToWString(strSearchName)) != string::npos)
						{
							_bool isSelected = (m_iObjectListIndex == i);

							string strModelName = WStringToAnsi(m_ObjectList[i]->Get_ModelName()) + "##id_%d";

							_char szModelName[MAX_PATH] = {};

							sprintf_s(szModelName, strModelName.c_str(), i);

							if (ImGui::Selectable(szModelName, isSelected))
								m_iObjectListIndex = i;
						}
					}

					ImGui::EndListBox();
				} SEPARATOR;

				if (0 != m_ObjectList.size())
				{
					ImGui::Text("OBJECT NUM : %d", m_ObjectList.size());
					SEPARATOR;
				}
			}
			if (ImGui::Button("ALL SNOW ON"))
			{
				MAPOBJECT_PROPERTIES PropProperties = {};

				for (auto& pObj : m_ObjectList)
				{
					PropProperties = pObj->Get_Properties();
					PropProperties.isSnow = true;
					pObj->Set_Properties(PropProperties);
				}
			} SAMELINE;
			if (ImGui::Button("ALL SNOW OFF"))
			{
				MAPOBJECT_PROPERTIES PropProperties = {};

				for (auto& pObj : m_ObjectList)
				{
					PropProperties = pObj->Get_Properties();
					PropProperties.isSnow = false;
					pObj->Set_Properties(PropProperties);
				}
			} SEPARATOR;

			if (ImGui::Button("CHECK RENDER ON"))
			{
				m_isCheckRender = true;

				for (auto& pProp : m_ObjectList)
				{
					pProp->Set_CheckRender(true);
					pProp->Set_RenderProperties(&m_RenderProperties);
				}
			}
			SAMELINE;
			if (ImGui::Button("CHECK RENDER OFF"))
			{
				m_isCheckRender = false;

				for (auto& pProp : m_ObjectList)
					pProp->Set_CheckRender(false);

				m_RenderProperties.isSnow = false;
				m_RenderProperties.isCollider = false;
				m_RenderProperties.isBlended = false;
				m_RenderProperties.isInstance = false;
				m_RenderProperties.isShadow = false;
				m_RenderProperties.isBackGround = false;
			}
			SEPARATOR;

			if (true == m_isCheckRender)
			{
				ImGui::Text("PLEASE CHECK RENDER");

				ImGui::Checkbox("SNOW", &m_RenderProperties.isSnow);
				SAMELINE;

				ImGui::Checkbox("COLLIDER", &m_RenderProperties.isCollider);
				SAMELINE;

				ImGui::Checkbox("BLENDED", &m_RenderProperties.isBlended);
				SAMELINE;

				ImGui::Checkbox("INSTANCE", &m_RenderProperties.isInstance);
				SEPARATOR;

				ImGui::Checkbox("SHADOW", &m_RenderProperties.isShadow);
				SAMELINE;

				ImGui::Checkbox("BACKGROUND", &m_RenderProperties.isBackGround);
				SEPARATOR;

				SEPARATOR;
			}
			else
			{
				if (0 != m_ObjectList.size() && m_iObjectListIndex < m_ObjectList.size())
				{
					CProp* pObjProp = m_ObjectList[m_iObjectListIndex];

					_wstring strModelName = m_ObjectList[m_iObjectListIndex]->Get_ModelName();

					_char szModelName[MAX_PATH] = {};
					memcpy(szModelName, WStringToAnsi(strModelName).c_str(), sizeof(szModelName));

					ImGui::Text("MODEL NAME : ");
					ImGui::InputText("##model_name_by_list", szModelName, IM_ARRAYSIZE(szModelName));
					SEPARATOR;

#pragma region 속성 설정

					if (false == m_isCheckRender)
					{
						MAPOBJECT_PROPERTIES PropProperties = m_ObjectList[m_iObjectListIndex]->Get_Properties();

						ImGui::Checkbox("SNOW", &PropProperties.isSnow);
						SAMELINE;

						ImGui::Checkbox("COLLIDER", &PropProperties.isCollider);
						SAMELINE;

						ImGui::Checkbox("BLENDED", &PropProperties.isBlended);
						SAMELINE;

						ImGui::Checkbox("INSTANCE", &PropProperties.isInstance);
						SEPARATOR;

						ImGui::Checkbox("SHADOW", &PropProperties.isShadow);
						SAMELINE;

						ImGui::Checkbox("BACKGROUND", &PropProperties.isBackGround);
						SEPARATOR;

						m_ObjectList[m_iObjectListIndex]->Set_Properties(PropProperties);

						SEPARATOR;
					}

#pragma endregion

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
						if (nullptr != m_ObjectList[m_iObjectListIndex] && false == m_isFixObjectWindow)
						{
							m_pFixPropObj = m_ObjectList[m_iObjectListIndex];
							m_pFixTransformCom = static_cast<CTransform*>(m_ObjectList[m_iObjectListIndex]->Get_Component(TEXT("Com_Transform")));
							CHECK_NULLPTR_MSG(m_pFixTransformCom, TEXT("Fix Transform == nullptr"), );

							m_FixBaseMatrix = XMMatrixIdentity();

							ZeroMemory(&m_vFixScale, sizeof(_float3));
							ZeroMemory(&m_vFixRotation, sizeof(_float3));
							ZeroMemory(&m_vFixPosition, sizeof(_float3));

							m_vFixScale = m_pFixTransformCom->Get_Scaled();
							XMStoreFloat3(&m_vFixPosition, m_pFixTransformCom->Get_State(STATE::POSITION));

							m_FixBaseMatrix = m_FixWorldMatrix = m_pFixTransformCom->Get_WorldMatrix();

							// ======================================================
							// ======================================================

							m_isFixObjectWindow = true;
							m_eFixType = FIX_OBJECT::FIX;
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
			}
			ImGui::End();
		}
		});

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
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRdirx", &m_FixLightDesc.vDirection.x);
					ImGui::Text("Axis Y : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRdiry", &m_FixLightDesc.vDirection.y);
					ImGui::Text("Axis Z : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRdirz", &m_FixLightDesc.vDirection.z);
					SEPARATOR;

					ImGui::Text("DIFFUSE");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRdifx", &m_FixLightDesc.vDiffuse.x, 0.01f, 0.05f);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRdify", &m_FixLightDesc.vDiffuse.y, 0.01f, 0.05f);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRdifz", &m_FixLightDesc.vDiffuse.z, 0.01f, 0.05f);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRdifw", &m_FixLightDesc.vDiffuse.w, 0.01f, 0.05f);
					SEPARATOR;

					ImGui::Text("AMBIENT");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRambx", &m_FixLightDesc.vAmbient.x, 0.01f, 0.05f);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRamby", &m_FixLightDesc.vAmbient.y, 0.01f, 0.05f);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRambz", &m_FixLightDesc.vAmbient.z, 0.01f, 0.05f);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRambw", &m_FixLightDesc.vAmbient.w, 0.01f, 0.05f);
					SEPARATOR;

					ImGui::Text("SPECULAR");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRspecx", &m_FixLightDesc.vSpecular.x, 0.01f, 0.05f);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRspecy", &m_FixLightDesc.vSpecular.y, 0.01f, 0.05f);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRspecz", &m_FixLightDesc.vSpecular.z, 0.01f, 0.05f);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##DIRspecw", &m_FixLightDesc.vSpecular.w, 0.01f, 0.05f);
					SEPARATOR;
				}
				else if (LIGHT_DESC::POINT == m_FixLightDesc.eType)
				{
					ImGui::Text("POINT");
					SEPARATOR;

					ImGui::Text("POSITION");
					ImGui::Text("X : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIposx", &m_FixLightDesc.vPosition.x, 0.1f, 0.5f);
					ImGui::Text("Y : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIposy", &m_FixLightDesc.vPosition.y, 0.1f, 0.5f);
					ImGui::Text("Z : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIposz", &m_FixLightDesc.vPosition.z, 0.1f, 0.5f);

					if (true == m_isAddLightPoint)
					{
						m_isAddLightPoint = false;

						m_FixLightDesc.vPosition.x = m_vLightPoint.x;
						m_FixLightDesc.vPosition.y = m_vLightPoint.y;
						m_FixLightDesc.vPosition.z = m_vLightPoint.z;
					}

					SEPARATOR;

					ImGui::Text("RANGE"); SAMELINE; ITEMWIDTH(160.f); ImGui::InputFloat("##POIrange", &m_FixLightDesc.fRange, 0.1f, 0.5f);
					SEPARATOR;

					ImGui::Text("DIFFUSE");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIdifx", &m_FixLightDesc.vDiffuse.x, 0.01f, 0.05f);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIdify", &m_FixLightDesc.vDiffuse.y, 0.01f, 0.05f);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIdifz", &m_FixLightDesc.vDiffuse.z, 0.01f, 0.05f);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIdifw", &m_FixLightDesc.vDiffuse.w, 0.01f, 0.05f);
					SEPARATOR;

					ImGui::Text("AMBIENT");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIambx", &m_FixLightDesc.vAmbient.x, 0.01f, 0.05f);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIamby", &m_FixLightDesc.vAmbient.y, 0.01f, 0.05f);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIambz", &m_FixLightDesc.vAmbient.z, 0.01f, 0.05f);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIambw", &m_FixLightDesc.vAmbient.w, 0.01f, 0.05f);
					SEPARATOR;

					ImGui::Text("SPECULAR");
					ImGui::Text("R : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIspecx", &m_FixLightDesc.vSpecular.x, 0.01f, 0.05f);
					ImGui::Text("G : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIspecy", &m_FixLightDesc.vSpecular.y, 0.01f, 0.05f);
					ImGui::Text("B : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIspecz", &m_FixLightDesc.vSpecular.z, 0.01f, 0.05f);
					ImGui::Text("A : "); SAMELINE;
					ITEMWIDTH(160.f); ImGui::InputFloat("##POIspecw", &m_FixLightDesc.vSpecular.w, 0.01f, 0.05f);
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
							_bool isCheckSameTag = { false };

							for (auto& pLightTag : m_LightTags)
							{
								if (pLightTag == m_strLightTag)
									isCheckSameTag = true;
							}

							if (true == isCheckSameTag)
							{
								OutputDebugStringA("조명 태그 중복");
							}
							else
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

#pragma region 클라나 다른곳에서 사용할 바이너리 저장

				if (false == Prototype_Save_Binary())
				{
					_int a = 10;
				}
				if (false == Instance_Prototype_Save_Binary())
				{
					_int a = 10;
				}
				if (false == Object_Save_Binary())
				{
					_int a = 10;
				}
				if (false == Instance_Object_Save_Binary())
				{
					_int a = 10;
				}

#pragma endregion


#pragma region 조명 일괄 저장

				if (false == Lights_Save_Binary())
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
				
#pragma region 프로토타입 일괄 불러오기

				if (false == Prototypes_Load_Binary())
				{
#ifdef _DEBUG
					OutputDebugStringA("프로토타입 정보 바이너리 불러오기 실패");
#endif // _DEBUG
				}
				else
				{
					// 프로토타입 윈도우 띄우기
					m_isPrototypeWindow = true;
				}

#pragma endregion

#pragma region 오브젝트 일괄 불러오기

				if (false == Objects_Load_Binary())
				{
#ifdef _DEBUG
					OutputDebugStringA("단일 오브젝트 정보 바이너리 불러오기 실패");
#endif // _DEBUG
				}
				else
				{
					// 오브젝트 리스트 윈도우 띄우기
					m_isObjectWindow = true;
				}

#pragma endregion

#pragma region 조명 일괄 불러오기

				if (false == Lights_Load_Binary())
				{
#ifdef _DEBUG
					OutputDebugStringA("조명 정보 바이너리 불러오기 실패");
#endif // _DEBUG
				}
				else
				{
					// 조명 윈도우 띄우기
					//m_isLightSettingWindow = true;
				}

#pragma endregion

				m_isLoadObjectWindow = false;
				m_isLoaded = true;
			}

			ImGui::End();
		}
		});

#pragma endregion

	return S_OK;
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

	if (strcmp("ALL", pFolderName))
	{
		strRootPath += pFolderName;

		strRootPath += '/';
	}

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

		JSON_MAP_PROTOTYPE_DATA PrototypeJson = {};

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

				PrototypeJson.FileName.push_back(WStringToAnsi(strModelName));

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

		PrototypeJson.iNumPrototypes = iPrototypeCnt;

		for (auto& pPrototype : Prototypes)
		{
			// 2. 어떤 타입인지 저장 ( Object, Instance, Dynamic, Interactive ) , enum class MAPOBJECT_TYPE은 unsigned short 사용으로 조금 메모리 절약
			_ushort sMapObjType = static_cast<_ushort>(pPrototype.second.eType);
			WriteFile(hPrototypeFile, &sMapObjType, sizeof(_ushort), &dwByte, nullptr);

			// 프로토 타입 태그 길이
			_uint iPrototypeTagLen = pPrototype.first.size();
			// 모델 경로 길이
			_uint iModelPathLen = pPrototype.second.strModelPath.size();

			PrototypeJson.PrototypeTag.push_back(WStringToAnsi(pPrototype.first));

			PrototypeJson.FilePath.push_back(pPrototype.second.strModelPath);

			// 3. 프로토 타입 태그 길이 저장
			WriteFile(hPrototypeFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr);
			// 4. 프로토 타입 태그 이름 저장
			WriteFile(hPrototypeFile, pPrototype.first.c_str(), sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr);

			// 5. 모델 경로 길이 저장
			WriteFile(hPrototypeFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr);
			// 6. 모델 경로 이름 저장
			WriteFile(hPrototypeFile, pPrototype.second.strModelPath.c_str(), sizeof(_char) * iModelPathLen, &dwByte, nullptr);
		}

		JSON j = PrototypeJson;

		_wstring strJsonFilePath = AnsiToWString(m_strMapInfoFilePath);

		strJsonFilePath += TEXT("_prototypes.json");

		ofstream ofs(strJsonFilePath);

		if (!ofs.is_open())
		{
			OutputDebugStringA("프로토타입 Json 파일입출력 실패");
		}

		ofs << j.dump(4);
		ofs.close();

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

			// 5. 객체당 속성 저장
			MAPOBJECT_PROPERTIES PropDesc = pProp->Get_Properties();
			WriteFile(hObjectFile, &PropDesc, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr);
		}
		// 단일 오브젝트 이외의 것들 추가 예정
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hObjectFile);

	return true;
}

#pragma region 실질적인 사용하는 바이너리 파일
_bool CLevel_Map::Prototype_Save_Binary()
{
	// 프로토 타입 저장할때는 인스턴스용 모델인지, 아니면 일반 모델인지 구분해서 저장을 해야한다.
	// Object로 사용한 Model만 프로토타입 등록
	_wstring strPrototypeInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strPrototypeInfoPath += TEXT("_prototype.dat");

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
			// 인스턴스 모델 프로토타입 세이브는 다른 함수에서
			if (true == pProp->Get_Properties().isInstance)
				continue;

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
			// 프로토 타입 태그 길이
			_uint iPrototypeTagLen = pPrototype.first.size();
			// 모델 경로 길이
			_uint iModelPathLen = pPrototype.second.strModelPath.size();

			// 2. 프로토 타입 태그 길이 저장
			WriteFile(hPrototypeFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr);
			// 3. 프로토 타입 태그 이름 저장
			WriteFile(hPrototypeFile, pPrototype.first.c_str(), sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr);

			// 4. 모델 경로 길이 저장
			WriteFile(hPrototypeFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr);
			// 5. 모델 경로 이름 저장
			WriteFile(hPrototypeFile, pPrototype.second.strModelPath.c_str(), sizeof(_char) * iModelPathLen, &dwByte, nullptr);
		}

		// 검사용 map clear;
		Prototypes.clear();
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hPrototypeFile);

	return true;
}
#pragma endregion

_bool CLevel_Map::Instance_Prototype_Save_Binary()
{
	// 프로토 타입 저장할때는 인스턴스용 모델인지, 아니면 일반 모델인지 구분해서 저장을 해야한다.
	// Object로 사용한 Model만 프로토타입 등록
	_wstring strPrototypeInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strPrototypeInfoPath += TEXT("_prototype_inst.dat");

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

		map<const _wstring, SAVE_PROTOTYPE_INSTANCE> Prototypes;

		_uint iInstanceCnt = {};

		// 단일 오브젝트 순회하면서 모델 이름 알아오기 ( Prototype 태그로 사용할 것 )
		for (auto& pProp : m_ObjectList)
		{
			// 인스턴스 아닌 모델은 다른 함수에서
			if (false == pProp->Get_Properties().isInstance)
				continue;

			// 기본 양식 지키기 ( Prototype_Component_Model_모델파일명 )
			_wstring strPrototypeTag = TEXT("Prototype_Component_Model_");

			// 기존 모델 명
			wstring strModelName = pProp->Get_ModelName();

			strPrototypeTag += strModelName;

			// 인스턴스 모델은 태그 뒤에 _Inst 붙이기
			strPrototypeTag += TEXT("_Inst");

			auto iter = Prototypes.find(strPrototypeTag);

			if (iter == Prototypes.end())
			{
				string strModelPath = Find_ModelPath(WStringToAnsi(strModelName).c_str(), ".dat");

				CHECK_EQUAL_MSG("NOTFOUND", strModelPath, TEXT("모델 경로 못찾음"), false);

				replace(strModelPath.begin(), strModelPath.end(), '\\', '/');

				SAVE_PROTOTYPE_INSTANCE Save_Proto_Inst = {};
				Save_Proto_Inst.strModelPath = strModelPath;

				_matrix WorldMatrix = static_cast<CTransform*>(pProp->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();

				VTXINSTANCE_MESH InstanceData = {};

				XMStoreFloat4(&InstanceData.vRight, WorldMatrix.r[0]);
				XMStoreFloat4(&InstanceData.vUp, WorldMatrix.r[1]);
				XMStoreFloat4(&InstanceData.vLook, WorldMatrix.r[2]);
				XMStoreFloat4(&InstanceData.vTranslation, WorldMatrix.r[3]);

				InstanceData.iID = iInstanceCnt++;

				Save_Proto_Inst.InstanceData.push_back(InstanceData);

				Prototypes.emplace(strPrototypeTag, Save_Proto_Inst);

				// 중복 아닐때만 Count 증가
				++iPrototypeCnt;
			}
			else
			{
				_matrix WorldMatrix = static_cast<CTransform*>(pProp->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();

				VTXINSTANCE_MESH InstanceData = {};

				XMStoreFloat4(&InstanceData.vRight, WorldMatrix.r[0]);
				XMStoreFloat4(&InstanceData.vUp, WorldMatrix.r[1]);
				XMStoreFloat4(&InstanceData.vLook, WorldMatrix.r[2]);
				XMStoreFloat4(&InstanceData.vTranslation, WorldMatrix.r[3]);

				InstanceData.iID = iInstanceCnt++;

				iter->second.InstanceData.push_back(InstanceData);
			}
		}

		// 1. 프로토 타입의 총 개수 저장 ( 이만큼 루프 돌릴거 )
		WriteFile(hPrototypeFile, &iPrototypeCnt, sizeof(_uint), &dwByte, nullptr);

		for (auto& pPrototype : Prototypes)
		{
			// 프로토 타입 태그 길이
			_uint iPrototypeTagLen = pPrototype.first.size();
			// 모델 경로 길이
			_uint iModelPathLen = pPrototype.second.strModelPath.size();

			// 2. 프로토 타입 태그 길이 저장
			WriteFile(hPrototypeFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr);
			// 3. 프로토 타입 태그 이름 저장
			WriteFile(hPrototypeFile, pPrototype.first.c_str(), sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr);

			// 4. 모델 경로 길이 저장
			WriteFile(hPrototypeFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr);
			// 5. 모델 경로 이름 저장
			WriteFile(hPrototypeFile, pPrototype.second.strModelPath.c_str(), sizeof(_char) * iModelPathLen, &dwByte, nullptr);

			// 6. 행렬 총 개수 저장
			_uint iNumInstances = static_cast<_uint>(pPrototype.second.InstanceData.size());
			WriteFile(hPrototypeFile, &iNumInstances, sizeof(_uint), &dwByte, nullptr);

			// 7. 인스턴싱 개수만큼 루프
			for (_uint i = 0; i < iNumInstances; ++i)
			{
				WriteFile(hPrototypeFile, &pPrototype.second.InstanceData[i].vRight, sizeof(_float4), &dwByte, nullptr);
				WriteFile(hPrototypeFile, &pPrototype.second.InstanceData[i].vUp, sizeof(_float4), &dwByte, nullptr);
				WriteFile(hPrototypeFile, &pPrototype.second.InstanceData[i].vLook, sizeof(_float4), &dwByte, nullptr);
				WriteFile(hPrototypeFile, &pPrototype.second.InstanceData[i].vTranslation, sizeof(_float4), &dwByte, nullptr);
				WriteFile(hPrototypeFile, &pPrototype.second.InstanceData[i].iID, sizeof(_uint), &dwByte, nullptr);
			}
		}

		// 검사용 map clear;
		Prototypes.clear();
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hPrototypeFile);

	return true;
}

_bool CLevel_Map::Object_Save_Binary()
{
	_wstring strObjectInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strObjectInfoPath += TEXT("_object.dat");

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
		{
			// 인스턴스 속성이 아니면 카운트 증가 X
			if (false == pProp->Get_Properties().isInstance)
				++iObjectCnt;
		}

		// 1. 오브젝트의 총 개수 저장
		WriteFile(hObjectFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);

		// 단일 오브젝트 순회하면서 모델 이름 알아오기 ( Prototype 태그로 사용할 것 )
		for (auto& pProp : m_ObjectList)
		{
			// 인스턴스 속성이면 단일 오브젝트니까 다음 순회
			if (true == pProp->Get_Properties().isInstance)
				continue;

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

			// 5. 객체당 속성 저장
			MAPOBJECT_PROPERTIES PropDesc = pProp->Get_Properties();
			WriteFile(hObjectFile, &PropDesc, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr);
		}
		// 단일 오브젝트 이외의 것들 추가 예정
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hObjectFile);

	return true;
}

_bool CLevel_Map::Instance_Object_Save_Binary()
{
	_wstring strObjectInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strObjectInfoPath += TEXT("_inst.dat");

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
		// 중복 모델 체크
		map<_wstring, CProp*> InstObj;

		// 오브젝트 총 개수 카운트
		_uint iObjectCnt = {};

		for (auto& pProp : m_ObjectList)
		{
			// 인스턴스 속성일때만
			if (true == pProp->Get_Properties().isInstance)
			{
				auto iter = InstObj.find(pProp->Get_ModelName());

				if (iter == InstObj.end())
				{
					InstObj.emplace(pProp->Get_ModelName(), pProp);

					++iObjectCnt;
				}
			}
		}

		// 1. 인스턴스에 사용할 오브젝트의 총 개수 저장
		WriteFile(hObjectFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);

		// 단일 오브젝트 순회하면서 모델 이름 알아오기 ( Prototype 태그로 사용할 것 )
		for (auto& pInstProp : InstObj)
		{
			// 기본 양식 지키기 ( Prototype_Component_Model_모델파일명 ) ( Layer 추가에 사용할 것, 모델명 던져주기 )
			_wstring strPrototypeTag = TEXT("Prototype_Component_Model_");
			strPrototypeTag += pInstProp.second->Get_ModelName();

			// 인스턴스는 프로토타입 세이브와 동일하게 _Inst 로 저장
			strPrototypeTag += TEXT("_Inst");

			// 모델 이름 길이
			_uint iPrototypeLen = strPrototypeTag.size();

			// 2. 프로토 타입 태그 길이 저장
			WriteFile(hObjectFile, &iPrototypeLen, sizeof(_uint), &dwByte, nullptr);
			// 3. 프로토 타입 태그 이름 저장
			WriteFile(hObjectFile, strPrototypeTag.c_str(), sizeof(_tchar) * iPrototypeLen, &dwByte, nullptr);

			// 4. 객체당 속성 저장
			MAPOBJECT_PROPERTIES PropDesc = pInstProp.second->Get_Properties();
			WriteFile(hObjectFile, &PropDesc, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr);
		}
		// 단일 오브젝트 이외의 것들 추가 예정
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hObjectFile);

	return true;
}

_bool CLevel_Map::Lights_Save_Binary()
{
	_wstring strLightInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strLightInfoPath += TEXT("_lights.dat");

	DWORD dwByte = {};

	// 프로토타입 핸들 개방
	HANDLE hLightFile = CreateFile(strLightInfoPath.c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hLightFile)
	{
		CloseHandle(hLightFile);
		return false;
	}
	else
	{
		_uint iLightCnt = {};

		for (auto& pLightTag : m_LightTags)
		{
			if (true == m_pGameInstance->Is_LightEnable(AnsiToWString(pLightTag), ENUM_CLASS(LEVEL::MAP)))
				++iLightCnt;
		}

		// 1. 프로토 타입의 총 개수 저장 ( 이만큼 루프 돌릴거 )
		WriteFile(hLightFile, &iLightCnt, sizeof(_uint), &dwByte, nullptr);

		for (auto& pLightTag : m_LightTags)
		{
			if (false == m_pGameInstance->Is_LightEnable(AnsiToWString(pLightTag), ENUM_CLASS(LEVEL::MAP)))
				continue;

			const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(AnsiToWString(pLightTag), ENUM_CLASS(LEVEL::MAP));
			CHECK_NULLPTR(pLightDesc, false);

			_uint iLightTagLen = static_cast<_uint>(pLightTag.size());

			// 2. 조명 태그 길이 저장
			WriteFile(hLightFile, &iLightTagLen, sizeof(_uint), &dwByte, nullptr);

			// 3. 조명 태그 저장 ( _wstring으로 넣을수있게 바로 변환 )
			WriteFile(hLightFile, AnsiToWString(pLightTag).c_str(), sizeof(_tchar) * iLightTagLen, &dwByte, nullptr);

			// 4. 조명 구조체 저장
			WriteFile(hLightFile, pLightDesc, sizeof(LIGHT_DESC), &dwByte, nullptr);
		}
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hLightFile);

	return true;
}

_bool CLevel_Map::Prototypes_Load_Binary()
{
	_wstring pDataFilePath = AnsiToWString(m_strMapInfoFilePath);

	pDataFilePath += TEXT("_prototypes.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL(INVALID_HANDLE_VALUE, hFile, false);

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

			// Prototype_Component_Model_ 자르기 시작 ( 에디터에서 보기 편하게 태그 제거 )
			_wstring strFullPrototypeTag = szPrototypeTag;
			_wstring strPreFix = { TEXT("Prototype_Component_Model_") };

			size_t Pos = strFullPrototypeTag.find(strPreFix);

			if (Pos != wstring::npos)
			{
				strFullPrototypeTag = strFullPrototypeTag.substr(Pos + strPreFix.length());
			}

			wcscpy_s(szPrototypeTag, strFullPrototypeTag.c_str());
			// Prototype_Component_Model_ 자르기 끝

			if (true == m_pGameInstance->Already_Registered_Prototype(ENUM_CLASS(LEVEL::MAP), szPrototypeTag))
				continue;

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
		else
		{
			CloseHandle(hFile);
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

		// Prototype_Component_Model_ 자르기 시작 ( 에디터에서 보기 편하게 태그 제거 )
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

		// 5. 객체의 속성 불러오기
		MAPOBJECT_PROPERTIES PropProperties = {};
		CHECK_FALSE(ReadFile(hFile, &PropProperties, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr), false);

		ObjectDesc.Properties = PropProperties;

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj"),
			ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc), false);

		CProp* pProp = static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj")));
		CHECK_NULLPTR_MSG(pProp, TEXT("[OBJECT LOAD] 오브젝트 찾기 실패"), false);

		m_ObjectList.push_back(pProp);
	}

	CloseHandle(hFile);

	return true;
}

_bool CLevel_Map::Lights_Load_Binary()
{
	_wstring strLightInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strLightInfoPath += TEXT("_lights.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(strLightInfoPath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL(INVALID_HANDLE_VALUE, hFile, false);

	// 1. 오브젝트의 총 개수
	_uint iLightCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iLightCnt, sizeof(_uint), &dwByte, nullptr), false);

	// 오브젝트 총 개수만큼 순회
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

		m_pGameInstance->Add_Light(szLightTag, ENUM_CLASS(LEVEL::MAP), LightDesc, true);

		_wstring strLightTag = szLightTag;
		m_LightTags.push_back(WStringToAnsi(strLightTag));

		m_iLightTagIndex = m_LightTags.size() - 1;

		ZeroMemory(&m_szLightTag, sizeof(m_szLightTag));
		m_strLightTag.clear();
		ZeroMemory(&m_LightDesc, sizeof(LIGHT_DESC));
		m_LightDesc.eType = LIGHT_DESC::END;

		m_isAddLight = !m_isAddLight;
		m_isFixLight = false;

		
	}

	CloseHandle(hFile);

	return true;
}

void CLevel_Map::MapEditor_Close_Windows()
{
	m_isPrototypeWindow = false;

	m_isObjectWindow = false;

	m_isFixObjectWindow = false;

	m_isLightSettingWindow = false;

	m_isSaveObjectWindow = false;

	m_isLoadObjectWindow = false;
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
	MapEditor_Close_Windows();

	__super::Free();
}
