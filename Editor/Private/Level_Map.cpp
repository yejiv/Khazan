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
	Select_Multi_Fix_Object(fTimeDelta);
	Select_Add_LightPoint(fTimeDelta);
	Measure_Distance(fTimeDelta);

	return;
}

HRESULT CLevel_Map::Render()
{
	SetWindowText(g_hWnd, TEXT("맵 툴"));

	return S_OK;
}

HRESULT CLevel_Map::Ready_Defaults()
{
	CHECK_FAILED(Ready_Layer_Khazan(TEXT("Layer_Khazan")), E_FAIL);

	CHECK_FAILED(Ready_Layer_Camera(TEXT("Layer_Map_Camera")), E_FAIL);

	CHECK_FAILED(Ready_Layer_Terrain(TEXT("Layer_Map_Terrain")), E_FAIL);
	
	CHECK_FAILED(Ready_Layer_Preview(TEXT("Layer_Preview")), E_FAIL);

	CHECK_FAILED(Ready_Layer_SkySphere(TEXT("Layer_Sky")), E_FAIL);

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

	MapDesc.fFar = 10000.f;
	MapDesc.fNear = 0.1f;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Camera_Map"), TIME_CHANNEL::WORLD, &MapDesc), E_FAIL);

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
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Preview"), TIME_CHANNEL::WORLD, &Desc), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Layer_SkySphere(const _wstring& strLayerTag)
{
	CSkySphere::SKY_SPHERE_DESC Desc = {};

	Desc.eLevel = LEVEL::MAP;

	Desc.fRotationPerSec = XMConvertToRadians(1.f);

	Desc.SkyDesc = {};

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_SkySphere"), TIME_CHANNEL::WORLD, &Desc), E_FAIL);

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
	if (true == m_isFixObjectWindow || true == m_isFixInteractObjectWindow)
		return;

	if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
	{
		_float3 vPosition = {};

		if (m_pGameInstance->isPicked(&vPosition))
		{
			CTransform* pKhazan = static_cast<CTransform*>(m_pGameInstance->Find_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Khazan"), TEXT("Com_Transform")));
			CHECK_NULLPTR(pKhazan, );

			m_vPickedPos = vPosition;

			pKhazan->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
		}
	}
}

void CLevel_Map::Select_Fix_Object(_float fTimeDelta)
{
	if (true == m_isFixObjectWindow || true == m_isFixInteractObjectWindow)
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

						m_iSubLevel = m_pFixPropObj->Get_SubLevel();

						m_isFixObjectWindow = true;
						m_eFixType = FIX_OBJECT::FIX;

						m_pGameInstance->Set_GizmoObject(m_pFixPropObj);

						return;
					}
				}
			}
		}
	}
}

void CLevel_Map::Select_Multi_Fix_Object(_float fTimeDelta)
{
	return;

	if (true == m_isFixObjectWindow)
		return;

	if (m_pGameInstance->Key_Pressing(DIK_F2, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
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
						if (0 == m_MultiFixObjList.size())
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

							m_iSubLevel = m_pFixPropObj->Get_SubLevel();

							m_isFixObjectWindow = true;
							m_eFixType = FIX_OBJECT::FIX;

							m_pGameInstance->Set_GizmoObject(m_pFixPropObj);
							m_MultiFixObjList.push_back(m_pFixPropObj);
						}
						else
						{
							auto pFound = find(m_MultiFixObjList.begin(), m_MultiFixObjList.end(), pObject);

							if (pFound == m_MultiFixObjList.end())
							{
								m_MultiFixObjList.push_back(pObject);
							}
						}

						return;
					}
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

void CLevel_Map::Measure_Distance(_float fTimeDelta)
{
	_float3 vPosition = {};

	if (m_pGameInstance->Key_Pressing(DIK_O, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
	{
		if (m_pGameInstance->isPicked(&vPosition))
		{
			m_vDistancePos[0] = vPosition;
		}
	}

	if (m_pGameInstance->Key_Pressing(DIK_P, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
	{
		if (m_pGameInstance->isPicked(&vPosition))
		{
			m_vDistancePos[1] = vPosition;
		}
	}

	m_fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vDistancePos[0]) - XMLoadFloat3(&m_vDistancePos[1])));
}

HRESULT CLevel_Map::Ready_DefaultImGui_For_MapTool()
{
	CHECK_FAILED(Ready_Main_Window(), E_FAIL);

	CHECK_FAILED(Ready_Prototype_List_Window(), E_FAIL);

	CHECK_FAILED(Ready_Interactive_Prototype_List_Window(), E_FAIL);

	CHECK_FAILED(Ready_Prop_Fix_Window(), E_FAIL);

	CHECK_FAILED(Ready_Interactive_Prop_Fix_Window(), E_FAIL);

	CHECK_FAILED(Ready_Prop_List_Window(), E_FAIL);
	
	CHECK_FAILED(Ready_Interactive_Prop_List_Window(), E_FAIL);

	CHECK_FAILED(Ready_Light_Window(), E_FAIL);

	CHECK_FAILED(Ready_Object_SaveLoad_Window(), E_FAIL);

	CHECK_FAILED(Ready_SkySphere_Window(), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Main_Window()
{
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isMainWindow)
		{
			ImGui::Begin("MAIN WINDOW", &m_isMainWindow, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("INFORMAION");
			if (ImGui::Button("ON/OFF##information"))
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
					ImGui::Text("F7           : TERRAIN WIREFRAME");

					SEPARATOR;

					_float4 vCamPos = *m_pGameInstance->Get_CamPosition();

					ImGui::Text("CAMERA POS");
					ImGui::Text("X : %.3f", vCamPos.x);
					ImGui::Text("Y : %.3f", vCamPos.y);
					ImGui::Text("Z : %.3f", vCamPos.z);

					SEPARATOR;

					ImGui::Text("PICKED POS");
					ImGui::Text("X : %.3f", m_vPickedPos.x);
					ImGui::Text("Y : %.3f", m_vPickedPos.y);
					ImGui::Text("Z : %.3f", m_vPickedPos.z);

					SEPARATOR;

					ImGui::Text("O TO P DISTANCE : %.3f", m_fDistance);
				}

				SEPARATOR;
				ImGui::Text("RENDER OPTION");
				if (ImGui::Button("ON/OFF##renderoption"))
					m_isRenderOption = !m_isRenderOption;

				SEPARATOR;
				ImGui::Text("SKY SPHERE");
				if (ImGui::Button("ON/OFF##sky"))
					m_isSkySphereWindow = !m_isSkySphereWindow;

				SEPARATOR;

				if (true == m_isRenderOption)
				{
					if (ImGui::Button("SSAO ON"))
						m_pGameInstance->Set_EnableSSAO(true);
					SAMELINE;
					if (ImGui::Button("SSAO OFF"))
						m_pGameInstance->Set_EnableSSAO(false);
					SEPARATOR;
					if (ImGui::Button("SHADOW ON"))
						m_pGameInstance->Set_EnableShadow(true);
					SAMELINE;
					if (ImGui::Button("SHADOW OFF"))
						m_pGameInstance->Set_EnableShadow(false);
					SEPARATOR;
					if (ImGui::Button("FOG ON"))
						m_pGameInstance->Set_EnableFog(true);
					SAMELINE;
					if (ImGui::Button("FOG OFF"))
						m_pGameInstance->Set_EnableFog(false);
					SEPARATOR;
					if (ImGui::Button("OUTLINE ON"))
						m_pGameInstance->Set_EnableOutline(true);
					SAMELINE;
					if (ImGui::Button("OUTLINE OFF"))
						m_pGameInstance->Set_EnableOutline(false);
					SEPARATOR;
					if (ImGui::Button("TOONSHADE ON"))
						m_pGameInstance->Set_EnableToonShade(true);
					SAMELINE;
					if (ImGui::Button("TOONSHADE OFF"))
						m_pGameInstance->Set_EnableToonShade(false);
					SEPARATOR;
				}

				ImGui::Text("LIGHT");
				if (ImGui::Button("LIGHT EDIT"))
				{
					m_isLightSettingWindow = !m_isLightSettingWindow;
				}
				SEPARATOR;

				ImGui::Text("MAP DATA SAVE & LOAD");
				if (ImGui::Button("SAVE"))
				{
					_int iMaxSubLevel = {};

					for (auto& pProp : m_ObjectList)
					{
						if (iMaxSubLevel < pProp->Get_SubLevel())
							iMaxSubLevel = pProp->Get_SubLevel();
					}

					m_iMaxSubLevel = iMaxSubLevel;

					m_isSaveObjectWindow = !m_isSaveObjectWindow;
				}
				if (false == m_isLoaded)
				{
					SAMELINE;
					if (ImGui::Button("LOAD")) m_isLoadObjectWindow = !m_isLoadObjectWindow;
				}
				SEPARATOR;
				
				ImGui::Text("PROP LIST");
				if (ImGui::Button("OBJECT##active"))		m_isObjectWindow = !m_isObjectWindow;
				SAMELINE;
				if (ImGui::Button("INTERACTIVE##active"))	m_isInteractiveWindow = !m_isInteractiveWindow;
				SEPARATOR;

				ImGui::Text("ADD PROTOTYPES");
				ImGui::Text("FOLDER : "); SAMELINE;
				ImGui::InputText("##folder_name_addprototype_or_convert", m_szFolderName, IM_ARRAYSIZE(m_szFolderName));

				_uint iFolderNameLen = strlen(m_szFolderName);

				if (0 != iFolderNameLen)
				{
					SEPARATOR;
					ImGui::Text("PROTOTYPE DATA FOLDER : %s", m_szPropFolder);
					if (ImGui::Button("PROP##prop_folder")) m_iPropPrototype = 0;
					SAMELINE;
					if (ImGui::Button("INTERACTIVE PROP##interactive_prop_folder")) m_iPropPrototype = 1;
					//SAMELINE;

					if (0 == m_iPropPrototype)
					{
						ImGui::Text("IS ANIMATION DATA : "); SAMELINE;
						if (true == m_isAnim)
							ImGui::Text("ANIM");
						else
							ImGui::Text("NON ANIM");
						if (ImGui::Button("ANIM##if_anim")) m_isAnim = true;
						SAMELINE;
						if (ImGui::Button("NONANIM##if_nonanim")) m_isAnim = false;
					}

					switch (m_iPropPrototype)
					{
					case 0:
						memcpy(m_szPropFolder, "Prop/", MAX_PATH);
						break;
					case 1:
						memcpy(m_szPropFolder, "InteractiveProp/", MAX_PATH);
						break;
					case 2:

						break;
					case 3:

						break;
					case 4:

						break;
					}

					SEPARATOR;
					if (ImGui::Button("PROTOTYPES ADD"))
					{
						Add_Prototype_ByFolder(m_szFolderName, m_isAnim);
						ZeroMemory(m_szFolderName, sizeof(m_szFolderName));
					}

					if (ImGui::Button("FBX FILE CONVERT ( .fbx > .dat )"))
					{
						Fbxs_Convert_To_Dat(m_szFolderName);
						ZeroMemory(m_szFolderName, sizeof(m_szFolderName));
					}
				}

				/*
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
				*/

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

			ITEMWIDTH(300.f);
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

			ImGui::Text("PROTOTYPE COUNT : %d", m_Prototypes_Obj.size());
			SEPARATOR;

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

			if (ImGui::Checkbox("COLLIDER", &m_AddObjectProperties.isCollider))
			{
				if (true == m_AddObjectProperties.isCollider)
					m_AddObjectProperties.isInstance = false;
			} SAMELINE;

			ImGui::Checkbox("ICE", &m_AddObjectProperties.isIce); SAMELINE;

			if (ImGui::Checkbox("INSTANCE", &m_AddObjectProperties.isInstance))
			{
				if (true == m_AddObjectProperties.isInstance)
					m_AddObjectProperties.isCollider = false;
			} SEPARATOR;

			ImGui::Checkbox("SHADOW", &m_AddObjectProperties.isShadow); SAMELINE;

			ImGui::Checkbox("BACKGROUND", &m_AddObjectProperties.isBackGround); SAMELINE;

			ImGui::Checkbox("PLANT", &m_AddObjectProperties.isPlant); SEPARATOR;

			if (true == m_AddObjectProperties.isInstance)
			{
				ImGui::Checkbox("RANDOM ROTATION ?", &m_isRandomRotation);
				//ImGui::Text("RANGE : "); SAMELINE;
				//ImGui::InputFloat("##instancing_range", &m_fInstanceRange);
				//ImGui::Text("INSTANCE NUM : "); SAMELINE;
				//ImGui::InputInt("##instancing_count", &m_iNumInstance);

				// 인스턴스일때, 반지름, 그 안에 생길 인스턴싱 모델의 개수 넘기고 랜덤하게 생기게
			}
			else if (false == m_AddObjectProperties.isInstance)
				m_isRandomRotation = false;

			ImGui::Text("PUT SUB LEVEL : ");
			ImGui::InputInt("##input_sub_level", &m_iAddSubLevel); SEPARATOR;

			// 단일 오브젝트 Layer 추가
			if (false == m_isLightSettingWindow && false == m_isFixObjectWindow && false == m_isFixInteractObjectWindow &&
				(ImGui::Button("ADD OBJECT (Y)") || m_pGameInstance->Key_Down(DIK_Y)))
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
					{
						vPos = vPickedPos;
					}
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

				if (true == m_isRandomRotation)
				{
					_float fRandomRadian_Y = XMConvertToRadians(m_pGameInstance->Rand(0.f, 360.f));

					WorldMatrix = XMMatrixRotationY(fRandomRadian_Y);
				}

				// 스케일 기존 0.005f, 위치는 마우스 피킹 위치 혹은 카메라 위치
				WorldMatrix.r[0] *= m_fAddScale;
				WorldMatrix.r[1] *= m_fAddScale;
				WorldMatrix.r[2] *= m_fAddScale;
				WorldMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&vPos), 1.f);

				XMStoreFloat4x4(&ObjectDesc.WorldMatrix, WorldMatrix);

				ObjectDesc.Properties = m_AddObjectProperties;

				ObjectDesc.iSubLevel = m_iAddSubLevel;

				CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj"),
					ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"), TIME_CHANNEL::WORLD, &ObjectDesc), );

				CProp* pObject_Prop = static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj")));
				CHECK_NULLPTR_MSG(pObject_Prop, TEXT("엥"), );

				m_ObjectList.push_back(pObject_Prop);

				m_iObjectListIndex = m_ObjectList.size() - 1;

				if (nullptr != m_ObjectList[m_iObjectListIndex] && false == m_isFixObjectWindow && false == m_isFixInteractObjectWindow)
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

					m_pGameInstance->Set_GizmoObject(m_pFixPropObj);

					m_iSubLevel = m_pFixPropObj->Get_SubLevel();

					m_isFixObjectWindow = true;
					m_eFixType = FIX_OBJECT::FIX;
				}

			} SEPARATOR;

			ImGui::End();
		}
		});

	return S_OK;
}

HRESULT CLevel_Map::Ready_Interactive_Prototype_List_Window()
{
	// 이짝에 추가될 상호작용들 로더에도 넣고 여짝에도 넣고 ( 태그 뒷부분만 )
	m_Prototypes_Inter.push_back("BladeNexus");
	m_Prototypes_Inter.push_back("BigChest");

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isPrototypeWindow)
		{
			ImGui::Begin("INTERACTIVE PROTOTYPE WINDOW", &m_isPrototypeWindow, ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::BeginListBox("##prototype_interactive_list"))
			{
				for (_uint i = 0; i < m_Prototypes_Inter.size(); ++i)
				{
					_bool isSelected = (m_iIndex_PrtInter == i);

					if (ImGui::Selectable(m_Prototypes_Inter[i].c_str(), isSelected)) m_iIndex_PrtInter = i;
				}

				ImGui::EndListBox();
			}
			SEPARATOR;

			ImGui::Checkbox("CAMERA POS ADD", &m_isCameraPosAdd); SEPARATOR;

			if (false == m_isCameraPosAdd)
			{
				ImGui::Text("ADD POS Y : "); SAMELINE;
				ImGui::InputFloat("##add_pos_y", &m_fAddPositionY, 0.1f, 1.f); SEPARATOR;
			}

			if (false == m_isLightSettingWindow && false == m_isFixObjectWindow && false == m_isFixInteractObjectWindow &&
				(ImGui::Button("ADD INTERACTIVE")))
			{
				_wstring strModelTag = TEXT("Prototype_Component_Model_");

				strModelTag += AnsiToWString(m_Prototypes_Inter[m_iIndex_PrtInter]);

				_float3 vPos = {};

				if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB))
				{
					_float3 vPickedPos = {};

					if (m_pGameInstance->isPicked(&vPickedPos))
					{
						vPos = vPickedPos;
					}
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

				WorldMatrix.r[0] *= 1.f;
				WorldMatrix.r[1] *= 1.f;
				WorldMatrix.r[2] *= 1.f;
				WorldMatrix.r[3] = XMVectorSetW(XMLoadFloat3(&vPos), 1.f);

				if ("BladeNexus" == m_Prototypes_Inter[m_iIndex_PrtInter])
				{
					CBladeNexus::BLADENEXUS_DESC BladeNexusDesc = {};

					BladeNexusDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
					BladeNexusDesc.eLevel = LEVEL::MAP;									
					memcpy(BladeNexusDesc.szModelName, strModelTag.c_str(), sizeof(BladeNexusDesc.szModelName));		// 프로토타입 태그명

					XMStoreFloat4x4(&BladeNexusDesc.WorldMatrix, WorldMatrix);											// 행렬

					BladeNexusDesc.eInteractiveType = INTERACTIVE_TYPE::CHECKPOINT;										// 상호 작용 오브젝트 타입

					CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
						ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_BladeNexus"), TIME_CHANNEL::WORLD, &BladeNexusDesc), );
				}
				else if ("BigChest" == m_Prototypes_Inter[m_iIndex_PrtInter]) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
				{
					CBigChest::BIGCHEST_DESC BigChestDesc = {};

					BigChestDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
					BigChestDesc.eLevel = LEVEL::MAP;
					memcpy(BigChestDesc.szModelName, strModelTag.c_str(), sizeof(BigChestDesc.szModelName));		// 프로토타입 태그명

					XMStoreFloat4x4(&BigChestDesc.WorldMatrix, WorldMatrix);										// 행렬

					BigChestDesc.eInteractiveType = INTERACTIVE_TYPE::CHEST;										// 상호 작용 오브젝트 타입

					CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
						ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_BigChest"), TIME_CHANNEL::WORLD, &BigChestDesc), );
				}

				CProp* pInteractive_Prop = static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive")));
				CHECK_NULLPTR_MSG(pInteractive_Prop, TEXT("엥"), );

				m_InteractiveList.push_back(pInteractive_Prop);

				m_iInteractiveListIndex = m_InteractiveList.size() - 1;

				if (nullptr != m_InteractiveList[m_iInteractiveListIndex] && false == m_isFixObjectWindow && false == m_isFixInteractObjectWindow)
				{
					m_pFixPropObj = m_InteractiveList[m_iInteractiveListIndex];
					m_pFixTransformCom = static_cast<CTransform*>(m_InteractiveList[m_iInteractiveListIndex]->Get_Component(TEXT("Com_Transform")));
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

					m_pGameInstance->Set_GizmoObject(m_pFixPropObj);

					m_isFixInteractObjectWindow = true;
					m_eFixType = FIX_OBJECT::FIX;
				}
			}

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

			_bool isReset = { false };

			if (nullptr != m_pFixPropObj)
			{
				_char szModelName[MAX_PATH] = {};
				WideCharToMultiByte(CP_ACP, 0, m_pFixPropObj->Get_ModelName(), -1, szModelName, MAX_PATH, nullptr, nullptr);

				ImGui::Text("MODEL NAME : "); SAMELINE;
				ImGui::InputText("##copy_batch_modelname", szModelName, IM_ARRAYSIZE(szModelName)); SAMELINE;
				
				if (ImGui::Button("COPY"))
				{
					memcpy(&m_szSearchPrototypeName, &szModelName, MAX_PATH);
					isReset = true;

				} SEPARATOR;

				_float3 vFixObjPos = {};
				XMStoreFloat3(&vFixObjPos, m_pFixTransformCom->Get_State(STATE::POSITION));

				_float3 vFixObjScale = {};
				vFixObjScale = m_pFixTransformCom->Get_Scaled();

				ImGui::Text("== OBJECT SCALE ==");
				ImGui::Text("SCALE X : %0.4f", vFixObjScale.x);
				ImGui::Text("SCALE Y : %0.4f", vFixObjScale.y);
				ImGui::Text("SCALE Z : %0.4f", vFixObjScale.z);

				ImGui::Text("\n== OBJECT POSITION ==");
				ImGui::Text("POSITION X : %0.2f", vFixObjPos.x);
				ImGui::Text("POSITION Y : %0.2f", vFixObjPos.y);
				ImGui::Text("POSITION Z : %0.2f", vFixObjPos.z);

				SEPARATOR;
			}

#pragma region 속성 설정

			if (m_pGameInstance->Key_Pressing(DIK_F4, 0.000001f) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
			{
				_float3 vPickPos = {};

				if (m_pGameInstance->isPicked(&vPickPos))
				{
					m_pFixTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPickPos), 1.f));
				}
			}

			ImGui::Text("SETTING");

			MAPOBJECT_PROPERTIES PropProperties = m_pFixPropObj->Get_Properties();

			ImGui::Checkbox("SNOW", &PropProperties.isSnow);
			SAMELINE;

			if (ImGui::Checkbox("COLLIDER", &PropProperties.isCollider))
			{
				if (true == PropProperties.isCollider)
					PropProperties.isInstance = false;
			}
			SAMELINE;

			ImGui::Checkbox("ICE", &PropProperties.isIce);
			SAMELINE;

			if (ImGui::Checkbox("INSTANCE", &PropProperties.isInstance))
			{
				if (true == PropProperties.isInstance)
					PropProperties.isCollider = false;
			}
			SEPARATOR;

			ImGui::Checkbox("SHADOW", &PropProperties.isShadow);
			SAMELINE;

			ImGui::Checkbox("BACKGROUND", &PropProperties.isBackGround);
			SAMELINE;

			ImGui::Checkbox("PLANT", &PropProperties.isPlant);
			SEPARATOR;

			m_pFixPropObj->Set_Properties(PropProperties);

			ImGui::Text("SET LEVEL : "); SAMELINE;
			ImGui::InputInt("##set_level_fix", &m_iSubLevel);

			m_pFixPropObj->Set_SubLevel(m_iSubLevel);

			SEPARATOR;
			SEPARATOR;

#pragma endregion

			if (ImGui::Button("DONE ( ENTER or MOUSE RB )") || m_pGameInstance->Key_Down(DIK_RETURN) || m_pGameInstance->Key_Down(DIK_NUMPADENTER) || m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
			{
				m_pGameInstance->Clear_GizmoObject();

				m_FixBaseMatrix = XMMatrixIdentity();

				ZeroMemory(&m_vFixScale, sizeof(_float3));
				ZeroMemory(&m_vFixRotation, sizeof(_float3));
				ZeroMemory(&m_vFixPosition, sizeof(_float3));

				m_pFixPropObj = nullptr;
				m_pFixTransformCom = nullptr;
				m_isFixObjectWindow = false;
				m_eFixType = FIX_OBJECT::END;

			} SAMELINE;
			if (ImGui::Button("RESET (R)") || m_pGameInstance->Key_Down(DIK_R) || true == isReset)
			{
				m_pGameInstance->Clear_GizmoObject();

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
			if (ImGui::Button("DELETE (ESC)") || m_pGameInstance->Key_Down(DIK_ESCAPE))
			{
				m_pGameInstance->Clear_GizmoObject();

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

HRESULT CLevel_Map::Ready_Interactive_Prop_Fix_Window()
{
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isFixInteractObjectWindow)
		{
			ImGui::Begin("OBJECT FIX WINDOW", &m_isFixInteractObjectWindow, ImGuiWindowFlags_AlwaysAutoResize);

			_bool isReset = { false };

			if (nullptr != m_pFixPropObj)
			{
				_char szModelName[MAX_PATH] = {};
				WideCharToMultiByte(CP_ACP, 0, m_pFixPropObj->Get_ModelName(), -1, szModelName, MAX_PATH, nullptr, nullptr);

				ImGui::Text("MODEL NAME : "); SAMELINE;
				ImGui::InputText("##copy_batch_modelname", szModelName, IM_ARRAYSIZE(szModelName)); SEPARATOR;

				_float3 vFixObjPos = {};
				XMStoreFloat3(&vFixObjPos, m_pFixTransformCom->Get_State(STATE::POSITION));

				_float3 vFixObjScale = {};
				vFixObjScale = m_pFixTransformCom->Get_Scaled();

				ImGui::Text("== OBJECT SCALE ==");
				ImGui::Text("SCALE X : %0.4f", vFixObjScale.x);
				ImGui::Text("SCALE Y : %0.4f", vFixObjScale.y);
				ImGui::Text("SCALE Z : %0.4f", vFixObjScale.z);

				ImGui::Text("\n== OBJECT POSITION ==");
				ImGui::Text("POSITION X : %0.2f", vFixObjPos.x);
				ImGui::Text("POSITION Y : %0.2f", vFixObjPos.y);
				ImGui::Text("POSITION Z : %0.2f", vFixObjPos.z);

				SEPARATOR;
			}

#pragma region 속성 설정

			if (INTERACTIVE_TYPE::CHEST == m_pFixPropObj->Get_InteractiveType())
			{
				ImGui::Text("== CHEST INFOMATION ==");
				ImGui::Text("BEFORE");

				ImGui::Text("FIRST ITEM : %d", m_ItemBox.iItem_0);
				ImGui::Text("SECOND ITEM : %d", m_ItemBox.iItem_1);
				ImGui::Text("THIRD ITEM : %d", m_ItemBox.iItem_2);

				ImGui::Text("FIX ITEM");
				ImGui::Text("FIX FIRST ITEM : "); SAMELINE;
				ImGui::InputInt("##item_list_fix_0", &m_FixItemBox.iItem_0);
				ImGui::Text("FIX SECOND ITEM : "); SAMELINE;
				ImGui::InputInt("##item_list_fix_1", &m_FixItemBox.iItem_1);
				ImGui::Text("FIX THIRD ITEM : "); SAMELINE;
				ImGui::InputInt("##item_list_fix_2", &m_FixItemBox.iItem_2);

				m_pFixPropObj->Set_ItemBox(m_FixItemBox);

				SEPARATOR;
			}

#pragma endregion

			if (ImGui::Button("DONE ( ENTER or MOUSE RB )") || m_pGameInstance->Key_Down(DIK_RETURN) || m_pGameInstance->Key_Down(DIK_NUMPADENTER) || m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
			{
				m_pGameInstance->Clear_GizmoObject();

				m_FixBaseMatrix = XMMatrixIdentity();

				ZeroMemory(&m_vFixScale, sizeof(_float3));
				ZeroMemory(&m_vFixRotation, sizeof(_float3));
				ZeroMemory(&m_vFixPosition, sizeof(_float3));

				m_pFixPropObj = nullptr;
				m_pFixTransformCom = nullptr;
				m_isFixInteractObjectWindow = false;
				ZeroMemory(&m_FixItemBox, sizeof(CMapObject::ITEMBOX_DESC));

				m_eFixType = FIX_OBJECT::END;

			} SAMELINE;
			if (ImGui::Button("RESET (R)") || m_pGameInstance->Key_Down(DIK_R) || true == isReset)
			{
				m_pGameInstance->Clear_GizmoObject();

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
				m_isFixInteractObjectWindow = false;
				ZeroMemory(&m_FixItemBox, sizeof(CMapObject::ITEMBOX_DESC));

				m_eFixType = FIX_OBJECT::END;
			}
			SEPARATOR;
			SEPARATOR;
			if (ImGui::Button("DELETE (ESC)") || m_pGameInstance->Key_Down(DIK_ESCAPE))
			{
				m_pGameInstance->Clear_GizmoObject();

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
				m_isFixInteractObjectWindow = false;
				ZeroMemory(&m_FixItemBox, sizeof(CMapObject::ITEMBOX_DESC));

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
				SAMELINE;
				if (ImGui::Button("LIST VIEW"))
					m_isObjectListView = !m_isObjectListView;

				if (true == m_isObjectListView)
				{
					ITEMWIDTH(300.f);
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
				} SEPARATOR;

				if (0 != m_ObjectList.size())
				{
					ImGui::Text("OBJECT NUM : %d", m_ObjectList.size());
					SEPARATOR;
				}
			}

			if (ImGui::Button("CHECK RENDER ON"))
			{
				m_isCheckRender = true;

				for (auto& pProp : m_ObjectList)
				{
					pProp->Set_CheckRender(true);
					pProp->Set_RenderProperties(&m_RenderProperties);
					pProp->Set_RenderSubLevel(&m_iRenderSubLevel);
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
				m_RenderProperties.isIce = false;
				m_RenderProperties.isInstance = false;
				m_RenderProperties.isShadow = false;
				m_RenderProperties.isBackGround = false;
				m_RenderProperties.isPlant = false;
			}
			SEPARATOR;

			if (true == m_isCheckRender)
			{
				ImGui::Text("PLEASE CHECK RENDER");

				ImGui::Checkbox("SNOW", &m_RenderProperties.isSnow);
				SAMELINE;

				ImGui::Checkbox("COLLIDER", &m_RenderProperties.isCollider);
				SAMELINE;

				ImGui::Checkbox("ICE", &m_RenderProperties.isIce);
				SAMELINE;

				ImGui::Checkbox("INSTANCE", &m_RenderProperties.isInstance);
				SEPARATOR;

				ImGui::Checkbox("SHADOW", &m_RenderProperties.isShadow);
				SAMELINE;

				ImGui::Checkbox("BACKGROUND", &m_RenderProperties.isBackGround);
				SAMELINE;

				ImGui::Checkbox("PLANT", &m_RenderProperties.isPlant);
				SEPARATOR;

				ImGui::Text("( 0 UNDER == ALL ) RENDER SUB LEVEL : "); SAMELINE;
				ImGui::InputInt("##render_sub_level", &m_iRenderSubLevel);

				SEPARATOR;
			}
			else if (true == m_isObjectListView)
			{
				if (0 != m_ObjectList.size() && m_iObjectListIndex < m_ObjectList.size())
				{
					_wstring strModelName = m_ObjectList[m_iObjectListIndex]->Get_ModelName();
					string strTempModelName = WStringToAnsi(strModelName);

					ImGui::Text("MODEL NAME : %s", strTempModelName.c_str());
					SEPARATOR;

#pragma region 속성 설정

					if (false == m_isCheckRender)
					{
						MAPOBJECT_PROPERTIES PropProperties = m_ObjectList[m_iObjectListIndex]->Get_Properties();

						ImGui::Checkbox("SNOW", &PropProperties.isSnow);
						SAMELINE;

						if (ImGui::Checkbox("COLLIDER", &PropProperties.isCollider))
						{
							if (true == PropProperties.isCollider)
								PropProperties.isInstance = false;
						}
						SAMELINE;

						ImGui::Checkbox("ICE", &PropProperties.isIce);
						SAMELINE;

						if (ImGui::Checkbox("INSTANCE", &PropProperties.isInstance))
						{
							if (true == PropProperties.isInstance)
								PropProperties.isCollider = false;
						}
						SEPARATOR;

						ImGui::Checkbox("SHADOW", &PropProperties.isShadow);
						SAMELINE;

						ImGui::Checkbox("BACKGROUND", &PropProperties.isBackGround);
						SAMELINE;

						ImGui::Checkbox("PLANT", &PropProperties.isPlant);
						SEPARATOR;

						m_ObjectList[m_iObjectListIndex]->Set_Properties(PropProperties);

						SEPARATOR;
					}

#pragma endregion
				}
				if (0 != m_ObjectList.size())
				{
					if (ImGui::Button("FIX"))
					{
						if (nullptr != m_ObjectList[m_iObjectListIndex] && false == m_isFixObjectWindow && false == m_isFixInteractObjectWindow)
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

							m_pGameInstance->Set_GizmoObject(m_pFixPropObj);

							m_iSubLevel = m_pFixPropObj->Get_SubLevel();

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

HRESULT CLevel_Map::Ready_Interactive_Prop_List_Window()
{
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isInteractiveWindow)
		{
			ImGui::Begin("PROP INTERACTIVE WINDOW", &m_isInteractiveWindow, ImGuiWindowFlags_AlwaysAutoResize);

			ITEMWIDTH(300.f);
			if (ImGui::BeginListBox("##prop_interactive_list"))
			{
				if (m_iInteractiveListIndex >= m_InteractiveList.size())
					m_iInteractiveListIndex = m_InteractiveList.size() - 1;

				string strSearchName = m_szSearchObjectName;
				transform(strSearchName.begin(), strSearchName.end(), strSearchName.begin(), ::tolower);		// 검색할 모델을 소문자로 변환

				for (_uint i = 0; i < m_InteractiveList.size(); ++i)
				{
					_bool isSelected = (m_iInteractiveListIndex == i);

					string strModelName = WStringToAnsi(m_InteractiveList[i]->Get_ModelName()) + "##id_%d";

					_char szModelName[MAX_PATH] = {};

					sprintf_s(szModelName, strModelName.c_str(), i);

					if (ImGui::Selectable(szModelName, isSelected))
						m_iInteractiveListIndex = i;
				}

				ImGui::EndListBox();
			} SEPARATOR;

			if (0 != m_InteractiveList.size())
			{
				ImGui::Text("INTERACTIVE OBJECT NUM : %d", m_InteractiveList.size());
				SEPARATOR;
			}
			if (0 != m_InteractiveList.size() && m_iInteractiveListIndex < m_InteractiveList.size())
			{
				_wstring strModelName = m_InteractiveList[m_iInteractiveListIndex]->Get_ModelName();
				string strTempModelName = WStringToAnsi(strModelName);

				ImGui::Text("MODEL NAME : %s", strTempModelName.c_str());
				SEPARATOR;

			}
			if (0 != m_InteractiveList.size())
			{
				if (ImGui::Button("FIX"))
				{
					if (nullptr != m_InteractiveList[m_iInteractiveListIndex] && false == m_isFixObjectWindow && false == m_isFixInteractObjectWindow)
					{
						m_pFixPropObj = m_InteractiveList[m_iInteractiveListIndex];
						m_pFixTransformCom = static_cast<CTransform*>(m_InteractiveList[m_iInteractiveListIndex]->Get_Component(TEXT("Com_Transform")));
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

						m_pGameInstance->Set_GizmoObject(m_pFixPropObj);

						if (INTERACTIVE_TYPE::CHEST == m_pFixPropObj->Get_InteractiveType())
						{
							m_FixItemBox = m_ItemBox = m_pFixPropObj->Get_ItemBox();
						}

						m_isFixInteractObjectWindow = true;
						m_eFixType = FIX_OBJECT::FIX;
					}
				}
				SAMELINE;
				if (ImGui::Button("DELETE"))
				{
					m_isFixInteractObjectWindow = false;

					if (nullptr != m_InteractiveList[m_iInteractiveListIndex])
					{
						m_InteractiveList[m_iInteractiveListIndex]->Set_IsDead(true);

						for (_uint i = 0; i < m_InteractiveList.size(); )
						{
							if (m_InteractiveList[m_iInteractiveListIndex] == m_InteractiveList[i])
							{
								swap(m_InteractiveList[m_iInteractiveListIndex], m_InteractiveList.back());
								m_InteractiveList.pop_back();
								break;
							}
							else
								++i;
						}

						if (m_iInteractiveListIndex >= m_InteractiveList.size())
							m_iInteractiveListIndex = m_InteractiveList.size() - 1;

						m_pFixPropObj = nullptr;
					}

					m_pFixPropObj = nullptr;
					m_pFixTransformCom = nullptr;
					m_eFixType = FIX_OBJECT::END;
				}
			}

			if (ImGui::Button("EXPORT"))
			{
				m_strMapInfoFilePath = m_szMapInfoFilePath;
				m_strMapInfoFilePath += m_szMapInfoFileName;

				if (false == Interactive_Object_Save_Binary())
				{
					_int a = 10;
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

				if (ImGui::Button("DONE") || m_pGameInstance->Key_Down(DIK_RETURN) || m_pGameInstance->Key_Down(DIK_NUMPADENTER) || m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
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

			ImGui::Text("TOTAL LEVEL : "); SAMELINE;
			ImGui::InputInt("##total_level_parts", &m_iMaxSubLevel);

			if (ImGui::Button("SAVE_SUB_LEVELS"))
			{
				m_strMapInfoFilePath = m_szMapInfoFilePath;
				m_strMapInfoFilePath += m_szMapInfoFileName;

				for (_int i = 0; i <= m_iMaxSubLevel; ++i)
				{
					Object_Save_Binary_ByLevel(i);
				}
			}
			SAMELINE;
			if (ImGui::Button("ALL_SAVE"))
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
			SAMELINE;
			if (ImGui::Button("SAVE LIGHT"))
			{
				m_strMapInfoFilePath = m_szMapInfoFilePath;
				m_strMapInfoFilePath += m_szMapInfoFileName;

				if (false == Lights_Save_Binary())
				{
#ifdef _DEBUG
					OutputDebugStringA("단일 오브젝트 정보 바이너리화 실패");
#endif // _DEBUG
					return;
				}
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

				_bool isLoadComplete = { true };
				
#pragma region 프로토타입 일괄 불러오기

				if (false == Prototypes_Load_Binary())
				{
#ifdef _DEBUG
OutputDebugStringA("프로토타입 정보 바이너리 불러오기 실패");
#endif // _DEBUG

					isLoadComplete = false;

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

					isLoadComplete = false;

				}
				else
				{
					// 오브젝트 리스트 윈도우 띄우기
					m_isObjectWindow = true;
				}

#pragma endregion

#pragma region 상호 작용 오브젝트 일괄 불러오기

				if (false == Interactive_Objects_Load_Binary())
				{
#ifdef _DEBUG
					OutputDebugStringA("단일 오브젝트 정보 바이너리 불러오기 실패");
#endif // _DEBUG

					isLoadComplete = false;

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

					isLoadComplete = false;

				}
				else
				{
					// 조명 윈도우 띄우기
					//m_isLightSettingWindow = true;
				}

#pragma endregion

				if (true == isLoadComplete)
				{
					m_isLoadObjectWindow = false;
					m_isLoaded = true;
				}
			}

			ImGui::End();
		}
		});

#pragma endregion

	return S_OK;
}

HRESULT CLevel_Map::Ready_SkySphere_Window()
{
	m_pSkySphere = static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Sky")));
	CHECK_NULLPTR(m_pSkySphere, E_FAIL);

	m_FixSkyDesc = m_pSkySphere->Get_SkyDesc();

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isSkySphereWindow)
		{
			ImGui::Begin("SKY SPHERE WINDOW", &m_isSkySphereWindow, ImGuiWindowFlags_AlwaysAutoResize);

			_float fMoonIntensity = m_FixSkyDesc.fMoonIntensity;
			_float fMoonSize = m_FixSkyDesc.fMoonSize;
			_float fStarStrength = m_FixSkyDesc.fStarStrength;
			_float3 vMoonColor = m_FixSkyDesc.vMoonColor;
			_float3 vMoonDirection = m_FixSkyDesc.vMoonDirection;
			_float3 vNebulaColor = m_FixSkyDesc.vNebulaColor;

			ImGui::Text("SKY COLOR");
			ImGui::Text("R"); SAMELINE;
			ImGui::InputFloat("##fix_sky_color_R", &m_FixSkyDesc.vNebulaColor.x, 0.01f, 0.1f);
			ImGui::Text("G"); SAMELINE;
			ImGui::InputFloat("##fix_sky_color_G", &m_FixSkyDesc.vNebulaColor.y, 0.01f, 0.1f);
			ImGui::Text("B"); SAMELINE;
			ImGui::InputFloat("##fix_sky_color_B", &m_FixSkyDesc.vNebulaColor.z, 0.01f, 0.1f);
			SEPARATOR;

			ImGui::Text("MOON SIZE"); SAMELINE;
			ImGui::InputFloat("##fix_moon_size", &m_FixSkyDesc.fMoonSize, 0.01f, 0.1f);

			ImGui::Text("MOON COLOR");
			ImGui::Text("R"); SAMELINE;
			ImGui::InputFloat("##fix_moon_color_R", &m_FixSkyDesc.vMoonColor.x, 0.01f, 0.1f);
			ImGui::Text("G"); SAMELINE;
			ImGui::InputFloat("##fix_moon_color_G", &m_FixSkyDesc.vMoonColor.y, 0.01f, 0.1f);
			ImGui::Text("B"); SAMELINE;
			ImGui::InputFloat("##fix_moon_color_B", &m_FixSkyDesc.vMoonColor.z, 0.01f, 0.1f);
			SEPARATOR;

			ImGui::Text("MOON INTENSITY"); SAMELINE;
			ImGui::InputFloat("##fix_moon_inten", &m_FixSkyDesc.fMoonIntensity, 0.01f, 0.1f);

			ImGui::Text("MOON POSITION");
			ImGui::Text("X"); SAMELINE;
			ImGui::InputFloat("##fix_moon_dir_X", &m_FixSkyDesc.vMoonDirection.x, 0.01f, 0.1f);
			ImGui::Text("Y"); SAMELINE;
			ImGui::InputFloat("##fix_moon_dir_Y", &m_FixSkyDesc.vMoonDirection.y, 0.01f, 0.1f);
			ImGui::Text("Z"); SAMELINE;
			ImGui::InputFloat("##fix_moon_dir_Z", &m_FixSkyDesc.vMoonDirection.z, 0.01f, 0.1f);
			SEPARATOR;

			ImGui::Text("STAR STRENGTH"); SAMELINE;
			ImGui::InputFloat("##fix_moon_str", &m_FixSkyDesc.fStarStrength, 0.01f, 0.1f);

			m_pSkySphere->Set_SkyDesc(m_FixSkyDesc);

			SEPARATOR;
			ImGui::Text("PATH : %s", m_szMapInfoFilePath);
			ImGui::Text("SKY SPHERE SAVE FILE : "); SAMELINE;
			ImGui::InputText("##sky_file_name", m_szMapInfoFileName, IM_ARRAYSIZE(m_szMapInfoFileName));

			SEPARATOR;
			if (ImGui::Button("SAVE"))
			{
				string strPath = m_szMapInfoFilePath;
				strPath += m_szMapInfoFileName;
				strPath += "_sky.dat";

				DWORD dwByte = {};

				HANDLE hFile = CreateFile(AnsiToWString(strPath).c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (INVALID_HANDLE_VALUE == hFile)
				{
					_int a = 10;
				}
				else
				{
					WriteFile(hFile, &m_FixSkyDesc, sizeof(SKY_DESC), &dwByte, nullptr);
				}

				CloseHandle(hFile);

			} SAMELINE;
			if (ImGui::Button("LOAD"))
			{
				string strPath = m_szMapInfoFilePath;
				strPath += m_szMapInfoFileName;
				strPath += "_sky.dat";

				DWORD dwByte = {};

				HANDLE hFile = CreateFile(AnsiToWString(strPath).c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (INVALID_HANDLE_VALUE == hFile)
				{
					_int a = 10;
				}
				else
				{
					ReadFile(hFile, &m_FixSkyDesc, sizeof(SKY_DESC), &dwByte, nullptr);
				}

				CloseHandle(hFile);
			}

			ImGui::End();
		}
		});

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
	string strRootPath = "../../Client/Bin/Data/Map/";
	strRootPath += m_szPropFolder;

	if (0 == m_iPropPrototype)
	{
		if (true == isAnim)
			strRootPath += "Anim/";
		else
			strRootPath += "NonAnim/";
	}

	strRootPath += pFolderName;

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

			// 6. 객체의 SaveLevel 저장
			_int iSaveLevel = pProp->Get_SubLevel();
			WriteFile(hObjectFile, &iSaveLevel, sizeof(_int), &dwByte, nullptr);
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
	_wstring strObjectInfoPath_Hot = AnsiToWString(m_strMapInfoFilePath);

	strObjectInfoPath += TEXT("_object.dat");
	strObjectInfoPath_Hot += TEXT("_object_hot.dat");

	DWORD dwByte = {};

	// 프로토타입 핸들 개방
	HANDLE hObjectFile = CreateFile(strObjectInfoPath.c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hObjectFile)
	{
		return false;
	}

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

_bool CLevel_Map::Interactive_Object_Save_Binary()
{
	_wstring strObjectInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strObjectInfoPath += TEXT("_interactive.dat");

	DWORD dwByte = {};

	// 프로토타입 핸들 개방
	HANDLE hObjectFile = CreateFile(strObjectInfoPath.c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hObjectFile)
	{
		return false;
	}
	else
	{
		// 오브젝트 총 개수 카운트
		_uint iObjectCnt = {};

		for (auto& pProp : m_InteractiveList)
		{
			// 현재 등록되어있는 상호 작용 객체 카운트 증가
			++iObjectCnt;
		}

		// 1. 오브젝트의 총 개수 저장
		WriteFile(hObjectFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);

		// 단일 오브젝트 순회하면서 모델 이름 알아오기 ( Prototype 태그로 사용할 것 ) ( 상호작용은 Prototype_Component_Model_귀검, 상자, 이런식으로 간단하게 갈것 )
		for (auto& pProp : m_InteractiveList)
		{
			// 상호작용 애들은 애초에 Prototype_Component_Model_귀검, 상자, 이런식임 )
			_wstring strPrototypeTag = pProp->Get_ModelName();

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

			// 5. 객체의 상호작용 오브젝트 저장 ( 클라랑 에디터랑 맞추기 ) ( MapObject::MAPOBJECT_DESC::INTERACTIVE_TYPE )
			INTERACTIVE_TYPE eType = pProp->Get_InteractiveType();
			CHECK_EQUAL(INTERACTIVE_TYPE::END, eType, false);
			WriteFile(hObjectFile, &eType, sizeof(INTERACTIVE_TYPE), &dwByte, nullptr);

			// ( 추가적으로 체스트 인 경우 )
			if (INTERACTIVE_TYPE::CHEST == eType)
			{
				// 6. 아이템 3개 ID 넘기기 ( 구조체 Editor, Client 동일하게 )
				CMapObject::ITEMBOX_DESC ItemBoxDesc = {};
				ItemBoxDesc = pProp->Get_ItemBox();
				WriteFile(hObjectFile, &ItemBoxDesc, sizeof(CMapObject::ITEMBOX_DESC), &dwByte, nullptr);
			}
		}
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hObjectFile);

	return true;
}

_bool CLevel_Map::Object_Save_Binary_ByLevel(_uint iLevel)
{
	_wstring strObjectInfoPath = AnsiToWString(m_strMapInfoFilePath);

	_tchar szObjectLevelInfoPath[MAX_PATH] = {};

	wsprintf(szObjectLevelInfoPath, TEXT("%s_LV%d_object.dat"), strObjectInfoPath.c_str(), iLevel);

	strObjectInfoPath = szObjectLevelInfoPath;

	DWORD dwByte = {};

	// 프로토타입 핸들 개방
	HANDLE hObjectFile = CreateFile(szObjectLevelInfoPath, GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hObjectFile)
	{
		return false;
	}

	// 오브젝트 총 개수 카운트
	_uint iObjectCnt = {};

	for (auto& pProp : m_ObjectList)
	{
		// 인스턴스 속성이 아니면 카운트 증가 X
		if (true == pProp->Get_Properties().isInstance)
			continue;

		if (iLevel != pProp->Get_SubLevel())
			continue;

		++iObjectCnt;
	}

	// 1. 오브젝트의 총 개수 저장 ( 지정한 특정 레벨의 )
	WriteFile(hObjectFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);

	// 단일 오브젝트 순회하면서 모델 이름 알아오기 ( Prototype 태그로 사용할 것 )
	for (auto& pProp : m_ObjectList)
	{
		// 인스턴스 속성이면 단일 오브젝트니까 다음 순회
		if (true == pProp->Get_Properties().isInstance)
			continue;

		// 저장된 Level 값이랑 일치하지 않으면 다음 순회
		if (iLevel != pProp->Get_SubLevel())
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

		// 6. 객체의 소 레벨 불러오기
		_int iSaveLevel = {};
		CHECK_FALSE(ReadFile(hFile, &iSaveLevel, sizeof(_int), &dwByte, nullptr), false);

		ObjectDesc.iSubLevel = iSaveLevel;

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj"),
			ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"), TIME_CHANNEL::WORLD, &ObjectDesc), false);

		CProp* pProp = static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj")));
		CHECK_NULLPTR_MSG(pProp, TEXT("[OBJECT LOAD] 오브젝트 찾기 실패"), false);

		m_ObjectList.push_back(pProp);
	}

	CloseHandle(hFile);

	return true;
}

_bool CLevel_Map::Interactive_Objects_Load_Binary()
{
	_wstring strObjectInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strObjectInfoPath += TEXT("_interactive.dat");

	DWORD dwByte = {};

	// 프로토타입 핸들 개방
	HANDLE hObjectFile = CreateFile(strObjectInfoPath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hObjectFile)
	{
		CloseHandle(hObjectFile);
		return false;
	}
	else
	{
		// 오브젝트 총 개수 카운트
		_uint iObjectCnt = {};
		// 1. 오브젝트의 총 개수 불러오기
		CHECK_FALSE(ReadFile(hObjectFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), false);

		// 단일 오브젝트 순회하면서 모델 이름 알아오기 ( Prototype 태그로 사용할 것 ) ( 상호작용은 Prototype_Component_Model_귀검, 상자, 이런식으로 간단하게 갈것 )
		for (_uint i = 0; i < iObjectCnt; ++i)
		{
			// 상호작용 애들은 애초에 Prototype_Component_Model_귀검, 상자, 이런식임 )
			_tchar szPrototypeTag[MAX_PATH] = {};

			// 모델 이름 길이
			_uint iPrototypeLen = {};

			// 2. 프로토 타입 태그 길이 저장
			CHECK_FALSE(ReadFile(hObjectFile, &iPrototypeLen, sizeof(_uint), &dwByte, nullptr), false);
			// 3. 프로토 타입 태그 이름 저장
			CHECK_FALSE(ReadFile(hObjectFile, szPrototypeTag, sizeof(_tchar) * iPrototypeLen, &dwByte, nullptr), false);

			_float4x4 WorldMatrix = {};

			// 4. 객체당 월드행렬 저장
			CHECK_FALSE(ReadFile(hObjectFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr), false);

			// 5. 객체의 상호작용 오브젝트 저장 ( 클라랑 에디터랑 맞추기 ) ( MapObject::MAPOBJECT_DESC::INTERACTIVE_TYPE )
			INTERACTIVE_TYPE eType = { INTERACTIVE_TYPE::END };
			CHECK_FALSE(ReadFile(hObjectFile, &eType, sizeof(INTERACTIVE_TYPE), &dwByte, nullptr), false);
			CHECK_EQUAL(INTERACTIVE_TYPE::END, eType, false);

			if (INTERACTIVE_TYPE::CHECKPOINT == eType)
			{
				CBladeNexus::BLADENEXUS_DESC BladeNexusDesc = {};

				BladeNexusDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
				BladeNexusDesc.eLevel = LEVEL::MAP;
				memcpy(BladeNexusDesc.szModelName, TEXT("Prototype_Component_Model_BladeNexus"), sizeof(BladeNexusDesc.szModelName));		// 프로토타입 태그명

				BladeNexusDesc.WorldMatrix = WorldMatrix;										// 행렬

				BladeNexusDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

				CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
					ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_BladeNexus"), TIME_CHANNEL::WORLD, &BladeNexusDesc), false);
			}
			else if (INTERACTIVE_TYPE::CHEST == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
			{
				CBigChest::BIGCHEST_DESC BigChestDesc = {};

				BigChestDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
				BigChestDesc.eLevel = LEVEL::MAP;
				memcpy(BigChestDesc.szModelName, TEXT("Prototype_Component_Model_BigChest"), sizeof(BigChestDesc.szModelName));		// 프로토타입 태그명

				BigChestDesc.WorldMatrix = WorldMatrix;									// 행렬

				BigChestDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

				// 상자 타입인 경우 아이템 박스 구조체도 슥슥 쇽쇽
				CHECK_FALSE(ReadFile(hObjectFile, &BigChestDesc.ItemBox, sizeof(CMapObject::ITEMBOX_DESC), &dwByte, nullptr), false);

				CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
					ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_BigChest"), TIME_CHANNEL::WORLD, &BigChestDesc), false);
			}

			CProp* pInteractive_Prop = static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive")));
			CHECK_NULLPTR_MSG(pInteractive_Prop, TEXT("엥"), false);

			m_InteractiveList.push_back(pInteractive_Prop);
		}

		m_iInteractiveListIndex = m_InteractiveList.size() - 1;
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hObjectFile);

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
