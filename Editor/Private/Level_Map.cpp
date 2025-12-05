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

	Build_ModelPathCache();

#ifdef _DEBUG
	m_pGameInstance->Set_EnableSSAO(false);
	m_pGameInstance->Set_EnableShadow(false);
	m_pGameInstance->Set_EnableFog(false);
	m_pGameInstance->Set_EnableOutline(false);
	m_pGameInstance->Set_EnableToonShade(false);
    m_pGameInstance->Set_EnableMotionBlur(false);
    m_pGameInstance->Set_EnableRadialBlur(false);
    m_pGameInstance->Set_EnableLUT(false);
#endif
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
	Update_MultiFix(fTimeDelta);

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

	CHECK_FAILED(Ready_Layer_Sky(TEXT("Layer_Sky")), E_FAIL);
    
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

HRESULT CLevel_Map::Ready_Layer_Sky(const _wstring& strLayerTag)
{
	CSkySphere::SKY_SPHERE_DESC Desc = {};

	Desc.eLevel = LEVEL::MAP;

	Desc.fRotationPerSec = XMConvertToRadians(1.f);

	Desc.SkyDesc = {};

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_SkySphere"), TIME_CHANNEL::WORLD, &Desc), E_FAIL);

	CCloudSphere::CLOUD_SPHERE_DESC CloudDesc = {};

	CloudDesc.eLevel = LEVEL::MAP;

	CloudDesc.fRotationPerSec = XMConvertToRadians(1.f);

	CloudDesc.CloudDesc = {};

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), strLayerTag,
		ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_CloudSphere"), TIME_CHANNEL::WORLD, &CloudDesc), E_FAIL);

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
    for (_uint i = 0; i < m_DecalList.size(); )
    {
        if (nullptr == m_DecalList[i])
        {
            swap(m_DecalList[i], m_DecalList.back());
            m_DecalList.pop_back();
        }
        else
            ++i;
    }
    for (_uint i = 0; i < m_InteractiveList.size(); )
    {
        if (nullptr == m_InteractiveList[i])
        {
            swap(m_InteractiveList[i], m_InteractiveList.back());
            m_InteractiveList.pop_back();
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

#ifdef _DEBUG
						m_pGameInstance->Set_GizmoObject(m_pFixPropObj);
#endif // _DEBUG

						return;
					}
				}
			}
		}
	}
}

void CLevel_Map::Select_Multi_Fix_Object(_float fTimeDelta)
{
	if (true == m_isFixObjectWindow || false == m_isMultiFixWindow)
		return;

	if (m_pGameInstance->Key_Pressing(DIK_F8, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
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
						// 이미 리스트에 존재하는지 확인
						auto iter = std::find(m_MultiFixList.begin(), m_MultiFixList.end(), pObject);
						if (iter == m_MultiFixList.end())
						{
							m_MultiFixList.push_back(pObject);

							if (m_MultiFixList.size() == 1)
								m_pParentFixObject = pObject; // 첫 번째는 부모로 설정

							_wstring msg = TEXT("[MultiFix] Selected: ");
							msg += pObject->Get_ModelName();
							OutputDebugString(msg.c_str());
						}
						else
						{
							OutputDebugString(TEXT("[MultiFix] Already in list.\n"));
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

void CLevel_Map::Update_MultiFix(_float fTimeDelta)
{
	if (false == m_isMultiFix)
		return;

	CTransform* pParentTransform = static_cast<CTransform*>(m_pParentFixObject->Get_Component(TEXT("Com_Transform")));
	CHECK_NULLPTR_MSG(pParentTransform, TEXT("부모 멀티 트랜스폼 널 피 티 알"), );

	_float4x4 matParent = *pParentTransform->Get_WorldMatrixPtr();
	_float4x4 matBefore = m_matParentBefore;
	
	_bool isChanged = { false };

	for (_uint i = 0; i < 4; ++i)
	{
		for (_uint j = 0; j < 4; ++j)
		{
			if (matParent.m[i][j] != matBefore.m[i][j])
			{
				isChanged = true;
			}
		}
	}

	// 행렬이 달라졌다면
	if (isChanged)
	{
		for (auto& info : m_MultiFixRelatives)
		{
			_matrix matLocal = XMLoadFloat4x4(&info.RelativeMatrix);
			_matrix matWorld = matLocal * XMLoadFloat4x4(&matParent);
			info.pTransform->Set_WorldMatrix(matWorld);
		}

		m_matParentBefore = matParent;
	}
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

    CHECK_FAILED(Ready_MultiFix_Window(), E_FAIL);

    CHECK_FAILED(Ready_Map_Decal_Window(), E_FAIL);

    CHECK_FAILED(Ready_OnOff_Window(), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Map::Ready_Main_Window()
{
#ifdef _DEBUG
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
                    ImGui::Text("Y : %.3f", m_vPickedPos.y); SAMELINE;
                    if (ImGui::Button("COPY"))
                        m_fAddPositionY = m_vPickedPos.y;
					ImGui::Text("Z : %.3f", m_vPickedPos.z);

					SEPARATOR;

					ImGui::Text("O TO P DISTANCE : %.3f", m_fDistance);
				}

				SEPARATOR;
				ImGui::Text("RENDER OPTION");
				if (ImGui::Button("ON/OFF##renderoption"))
					m_isRenderOption = !m_isRenderOption;

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
                    if (ImGui::Button("MOTIONBLUR ON"))
                        m_pGameInstance->Set_EnableMotionBlur(true);
                    SAMELINE;
                    if (ImGui::Button("MOTIONBLUR OFF"))
                        m_pGameInstance->Set_EnableMotionBlur(false);
                    SEPARATOR;
                    if (ImGui::Button("RADIALBLUR ON"))
                        m_pGameInstance->Set_EnableRadialBlur(true);
                    SAMELINE;
                    if (ImGui::Button("RADIALBLUR OFF"))
                        m_pGameInstance->Set_EnableRadialBlur(false);
                    SEPARATOR;
                    if (ImGui::Button("LUT ON"))
                        m_pGameInstance->Set_EnableLUT(true);
                    SAMELINE;
                    if (ImGui::Button("LUT OFF"))
                        m_pGameInstance->Set_EnableLUT(false);
                    SEPARATOR;
				}

                if (ImGui::Button("EMBARS FOG 1 ON"))
                {
                    // 1층 Fog
                    FOG_TRANSITION_DESC FogDesc{};
                    FogDesc.fDensity = 0.05f;
                    FogDesc.fBias = 0.8f;
                    FogDesc.vColor = _float4(0.f, 0.176f, 0.341f, 1.f);
                    FogDesc.isUseHeight = false;
                    FogDesc.isUseNoise = false;
                    m_pGameInstance->Start_FogTransition(1.f, FogDesc);
                }

                if (ImGui::Button("EMBARS FOG B1 ON"))
                {
                    // 지하 포그
                    FOG_TRANSITION_DESC Desc{};
                    Desc.fDensity = 0.05f;
                    Desc.fBias = 0.8f;
                    Desc.vColor = _float4(0.f, 0.058f, 0.117f, 1.f);
                    Desc.isUseHeight = false;
                    Desc.isUseNoise = false;
                    m_pGameInstance->Start_FogTransition(1.f, Desc);
                }

                SEPARATOR;

				ImGui::Text("SPHERE");
				if (ImGui::Button("SKY ON/OFF"))
					m_isSkySphereWindow = !m_isSkySphereWindow;
				SAMELINE;
				if (ImGui::Button("CLOUD ON/OFF"))
					m_isCloudSphereWindow = !m_isCloudSphereWindow;

				SEPARATOR;

				ImGui::Text("CUBE COLLIDER RENDER");
				if (ImGui::Button("CUBE WIRE FRAME") || m_pGameInstance->Key_Down(DIK_TAB))
				{
					for (auto& pProp : m_ObjectList)
					{
						if (pProp->Get_Properties().isCollider && pProp->Get_Properties().isBackGround)
						{
							m_iRenderFrame = 1;
							pProp->Set_ShaderPass(m_iRenderFrame);
						}
					}
				} SAMELINE;
				if (ImGui::Button("CUBE SOLID FRAME") || m_pGameInstance->Key_Down(DIK_CAPSLOCK))
				{
					for (auto& pProp : m_ObjectList)
					{
						if (pProp->Get_Properties().isCollider && pProp->Get_Properties().isBackGround)
							m_iRenderFrame = 2;
							pProp->Set_ShaderPass(m_iRenderFrame);
					}
				}

				SEPARATOR;

                ImGui::Text("SHORTCUT KEY STATUS : "); SAMELINE;
                if (false == m_isActiveShortCutKey)
                {
                    if (ImGui::Button("OFF")) m_isActiveShortCutKey = true;
                }
                if (true == m_isActiveShortCutKey)
                {
                    if (ImGui::Button("ON")) m_isActiveShortCutKey = false;
                }

                SEPARATOR;

                ImGui::Text("ON OFF WINDOW : "); SAMELINE;
                if (ImGui::Button("ON/OFF")) m_isOnOffWindow = !m_isOnOffWindow;

                SEPARATOR;

                ImGui::Text("MULTI FIX WINDOW : "); SAMELINE;
				if (ImGui::Button("ON/OFF")) m_isMultiFixWindow = !m_isMultiFixWindow;

                SEPARATOR;

                ImGui::Text("LIGHT : "); SAMELINE;
				if (ImGui::Button("LIGHT EDIT")) m_isLightSettingWindow = !m_isLightSettingWindow;

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
				SAMELINE;
				if (ImGui::Button("LOAD")) m_isLoadObjectWindow = !m_isLoadObjectWindow;
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

				ImGui::End();
		}
		});
#endif // _DEBUG

	return S_OK;
}

HRESULT CLevel_Map::Ready_Prototype_List_Window()
{
#ifdef _DEBUG
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
				(ImGui::Button("ADD OBJECT (Y)") || (true == m_isActiveShortCutKey && m_pGameInstance->Key_Down(DIK_Y))))
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

				if (pObject_Prop->Get_Properties().isCollider && pObject_Prop->Get_Properties().isBackGround)
					pObject_Prop->Set_ShaderPass(m_iRenderFrame);

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

#ifdef _DEBUG
					m_pGameInstance->Set_GizmoObject(m_pFixPropObj);
#endif // _DEBUG

					m_iSubLevel = m_pFixPropObj->Get_SubLevel();

					m_isFixObjectWindow = true;
					m_eFixType = FIX_OBJECT::FIX;
				}

			} SEPARATOR;

			ImGui::End();
		}
		});
#endif // _DEBUG

	return S_OK;
}

HRESULT CLevel_Map::Ready_Interactive_Prototype_List_Window()
{
	// 이짝에 추가될 상호작용들 로더에도 넣고 여짝에도 넣고 ( 태그 뒷부분만 )
	m_Prototypes_Inter.push_back("BladeNexus");
	m_Prototypes_Inter.push_back("BigChest");
	m_Prototypes_Inter.push_back("TombStone");
    m_Prototypes_Inter.push_back("Trigger");
    m_Prototypes_Inter.push_back("Monster");
    m_Prototypes_Inter.push_back("SmallElevator");
    m_Prototypes_Inter.push_back("Lever");
    m_Prototypes_Inter.push_back("Lever_Gear");
    m_Prototypes_Inter.push_back("Statue");
    m_Prototypes_Inter.push_back("VerticalGate");
    m_Prototypes_Inter.push_back("IronGate");
    m_Prototypes_Inter.push_back("Ladder");
    m_Prototypes_Inter.push_back("GearGate");
    m_Prototypes_Inter.push_back("UnLockGear");
    m_Prototypes_Inter.push_back("LargeElevator");
    m_Prototypes_Inter.push_back("GiantGate");
    m_Prototypes_Inter.push_back("NPC_Daphrona");
    m_Prototypes_Inter.push_back("NPC_Duimuk");
    m_Prototypes_Inter.push_back("NPC_Danjin");
    m_Prototypes_Inter.push_back("DanjinJar");
    m_Prototypes_Inter.push_back("DestinyStone");
    m_Prototypes_Inter.push_back("DestructibleProp");

#ifdef _DEBUG
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

#pragma region 상호 작용 오브젝트 레이어 생성
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
				else if ("BigChest" == m_Prototypes_Inter[m_iIndex_PrtInter])
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
				else if ("TombStone" == m_Prototypes_Inter[m_iIndex_PrtInter]) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
				{
					CTombStone::TOMBSTONE_DESC TombStoneDesc = {};

					TombStoneDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
					TombStoneDesc.eLevel = LEVEL::MAP;
					memcpy(TombStoneDesc.szModelName, strModelTag.c_str(), sizeof(TombStoneDesc.szModelName));		// 프로토타입 태그명

					XMStoreFloat4x4(&TombStoneDesc.WorldMatrix, WorldMatrix);										// 행렬

					TombStoneDesc.eInteractiveType = INTERACTIVE_TYPE::TOMBSTONE;										// 상호 작용 오브젝트 타입

					CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
						ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_TombStone"), TIME_CHANNEL::WORLD, &TombStoneDesc), );
				}
                else if ("Trigger" == m_Prototypes_Inter[m_iIndex_PrtInter]) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
                {
                    CTrigger::TRIGGER_DESC TriggerDesc = {};

                    TriggerDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    TriggerDesc.eLevel = LEVEL::MAP;
                    memcpy(TriggerDesc.szModelName, strModelTag.c_str(), sizeof(TriggerDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&TriggerDesc.WorldMatrix, WorldMatrix);										// 행렬

                    TriggerDesc.eInteractiveType = INTERACTIVE_TYPE::TRIGGER;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Trigger"), TIME_CHANNEL::WORLD, &TriggerDesc), );
                }
                else if ("Monster" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CMap_Spawn::SPAWN_DESC SpawnDesc = {};

                    SpawnDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    SpawnDesc.eLevel = LEVEL::MAP;
                    memcpy(SpawnDesc.szModelName, strModelTag.c_str(), sizeof(SpawnDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&SpawnDesc.WorldMatrix, WorldMatrix);										// 행렬

                    SpawnDesc.eInteractiveType = INTERACTIVE_TYPE::SPAWN;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Spawn"), TIME_CHANNEL::WORLD, &SpawnDesc), );
                }
                else if ("SmallElevator" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CElevatorS::SMALL_ELEVATOR_DESC SmallElevatorDesc = {};

                    SmallElevatorDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    SmallElevatorDesc.eLevel = LEVEL::MAP;
                    memcpy(SmallElevatorDesc.szModelName, strModelTag.c_str(), sizeof(SmallElevatorDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&SmallElevatorDesc.WorldMatrix, WorldMatrix);										// 행렬

                    SmallElevatorDesc.eInteractiveType = INTERACTIVE_TYPE::ELEVATOR;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_SmallElevator"), TIME_CHANNEL::WORLD, &SmallElevatorDesc), );
                }
                else if ("Lever" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CLever::LEVER_DESC LeverDesc = {};

                    LeverDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    LeverDesc.eLevel = LEVEL::MAP;
                    memcpy(LeverDesc.szModelName, strModelTag.c_str(), sizeof(LeverDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&LeverDesc.WorldMatrix, WorldMatrix);										// 행렬

                    LeverDesc.eInteractiveType = INTERACTIVE_TYPE::LEVER;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Lever"), TIME_CHANNEL::WORLD, &LeverDesc), );
                }
                else if ("Lever_Gear" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CLever_Gear::LEVER_GEAR_DESC LeverGearDesc = {};

                    LeverGearDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    LeverGearDesc.eLevel = LEVEL::MAP;
                    memcpy(LeverGearDesc.szModelName, strModelTag.c_str(), sizeof(LeverGearDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&LeverGearDesc.WorldMatrix, WorldMatrix);										// 행렬

                    LeverGearDesc.eInteractiveType = INTERACTIVE_TYPE::GEAR1;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Lever_Gear"), TIME_CHANNEL::WORLD, &LeverGearDesc), );
                }
                else if ("GearGate" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CGearGate::GEARGATE_DESC GearGateDesc = {};

                    GearGateDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    GearGateDesc.eLevel = LEVEL::MAP;
                    memcpy(GearGateDesc.szModelName, strModelTag.c_str(), sizeof(GearGateDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&GearGateDesc.WorldMatrix, WorldMatrix);										// 행렬

                    GearGateDesc.GearEventID.iLeftEventID = 0;
                    GearGateDesc.GearEventID.iRightEventID = 1;

                    GearGateDesc.eInteractiveType = INTERACTIVE_TYPE::GEAR2;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_GearGate"), TIME_CHANNEL::WORLD, &GearGateDesc), );
                }
                else if ("Statue" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CStatue::STATUE_DESC StatueDesc = {};

                    StatueDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    StatueDesc.eLevel = LEVEL::MAP;
                    memcpy(StatueDesc.szModelName, strModelTag.c_str(), sizeof(StatueDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&StatueDesc.WorldMatrix, WorldMatrix);										// 행렬

                    StatueDesc.eInteractiveType = INTERACTIVE_TYPE::STATUE;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Statue"), TIME_CHANNEL::WORLD, &StatueDesc), );
                }
                else if ("VerticalGate" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CVerticalGate::VERTICAL_GATE_DESC VerticalGateDesc = {};

                    VerticalGateDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    VerticalGateDesc.eLevel = LEVEL::MAP;
                    memcpy(VerticalGateDesc.szModelName, strModelTag.c_str(), sizeof(VerticalGateDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&VerticalGateDesc.WorldMatrix, WorldMatrix);										// 행렬

                    VerticalGateDesc.eInteractiveType = INTERACTIVE_TYPE::VERTICALGATE;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_VerticalGate"), TIME_CHANNEL::WORLD, &VerticalGateDesc), );
                }
                else if ("IronGate" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CIronGate::IRONGATE_DESC IronGateDesc = {};

                    IronGateDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    IronGateDesc.eLevel = LEVEL::MAP;
                    memcpy(IronGateDesc.szModelName, strModelTag.c_str(), sizeof(IronGateDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&IronGateDesc.WorldMatrix, WorldMatrix);										// 행렬

                    IronGateDesc.eInteractiveType = INTERACTIVE_TYPE::IRONGATE;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_IronGate"), TIME_CHANNEL::WORLD, &IronGateDesc), );
                }
                else if ("Ladder" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CLadder::LADDER_DESC LadderDesc = {};

                    LadderDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    LadderDesc.eLevel = LEVEL::MAP;
                    memcpy(LadderDesc.szModelName, TEXT("Ladder"), sizeof(LadderDesc.szModelName));		// 프로토타입 태그명

                    LadderDesc.fOffSetHeight = 3.2f;

                    LadderDesc.iSegmentCount = 1;

                    XMStoreFloat4x4(&LadderDesc.WorldMatrix, WorldMatrix);										// 행렬

                    LadderDesc.eInteractiveType = INTERACTIVE_TYPE::LADDER;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Ladder"), TIME_CHANNEL::WORLD, &LadderDesc), );
                }
                else if ("UnLockGear" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CUnLockGear::UNLOCK_GEAR_DESC UnLockGearDesc = {};

                    UnLockGearDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    UnLockGearDesc.eLevel = LEVEL::MAP;
                    memcpy(UnLockGearDesc.szModelName, strModelTag.c_str(), sizeof(UnLockGearDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&UnLockGearDesc.WorldMatrix, WorldMatrix);										// 행렬

                    UnLockGearDesc.eInteractiveType = INTERACTIVE_TYPE::UNLOCKGEAR;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_UnLockGear"), TIME_CHANNEL::WORLD, &UnLockGearDesc), );
                }
                else if ("LargeElevator" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CElevatorL::LARGE_ELEVATOR_DESC LargeElevatorDesc = {};

                    LargeElevatorDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    LargeElevatorDesc.eLevel = LEVEL::MAP;
                    memcpy(LargeElevatorDesc.szModelName, strModelTag.c_str(), sizeof(LargeElevatorDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&LargeElevatorDesc.WorldMatrix, WorldMatrix);										// 행렬

                    LargeElevatorDesc.eInteractiveType = INTERACTIVE_TYPE::LARGEELEVATOR;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_LargeElevator"), TIME_CHANNEL::WORLD, &LargeElevatorDesc), );
                }
                else if ("GiantGate" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CGiantGate::GIANTGATE_DESC GiantGateDesc = {};

                    GiantGateDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    GiantGateDesc.eLevel = LEVEL::MAP;
                    memcpy(GiantGateDesc.szModelName, strModelTag.c_str(), sizeof(GiantGateDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&GiantGateDesc.WorldMatrix, WorldMatrix);										// 행렬

                    GiantGateDesc.eInteractiveType = INTERACTIVE_TYPE::GIANTGATE;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_GiantGate"), TIME_CHANNEL::WORLD, &GiantGateDesc), );
                }
                else if ("NPC_Daphrona" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CNPC_Daphrona::DAPHRONA_DESC DaphronaDesc = {};

                    DaphronaDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    DaphronaDesc.eLevel = LEVEL::MAP;
                    memcpy(DaphronaDesc.szModelName, strModelTag.c_str(), sizeof(DaphronaDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&DaphronaDesc.WorldMatrix, WorldMatrix);										// 행렬

                    DaphronaDesc.eInteractiveType = INTERACTIVE_TYPE::DAPHRONA;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_NPC_Daphrona"), TIME_CHANNEL::WORLD, &DaphronaDesc), );
                }
                else if ("NPC_Duimuk" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CNPC_Duimuk::DUIMUK_DESC DuimukDesc = {};

                    DuimukDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    DuimukDesc.eLevel = LEVEL::MAP;
                    memcpy(DuimukDesc.szModelName, strModelTag.c_str(), sizeof(DuimukDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&DuimukDesc.WorldMatrix, WorldMatrix);										// 행렬

                    DuimukDesc.eInteractiveType = INTERACTIVE_TYPE::DUIMUK;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_NPC_Duimuk"), TIME_CHANNEL::WORLD, &DuimukDesc), );
                }
                else if ("NPC_Danjin" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CNPC_Danjin::DANJIN_DESC DanjinDesc = {};

                    DanjinDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    DanjinDesc.eLevel = LEVEL::MAP;
                    memcpy(DanjinDesc.szModelName, strModelTag.c_str(), sizeof(DanjinDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&DanjinDesc.WorldMatrix, WorldMatrix);										// 행렬

                    DanjinDesc.eInteractiveType = INTERACTIVE_TYPE::DANJIN;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_NPC_Danjin"), TIME_CHANNEL::WORLD, &DanjinDesc), );
                }
                else if ("DanjinJar" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CDanjinJar::DANJINJAR_DESC DanjinJarDesc = {};

                    DanjinJarDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    DanjinJarDesc.eLevel = LEVEL::MAP;
                    memcpy(DanjinJarDesc.szModelName, TEXT("DanjinJar"), sizeof(DanjinJarDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&DanjinJarDesc.WorldMatrix, WorldMatrix);										// 행렬

                    DanjinJarDesc.eInteractiveType = INTERACTIVE_TYPE::DANJINJAR;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_DanjinJar"), TIME_CHANNEL::WORLD, &DanjinJarDesc), );
                }
                else if ("DestinyStone" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CDestinyStone::DESTINYSTONE_DESC DestinyStoneDesc = {};

                    DestinyStoneDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    DestinyStoneDesc.eLevel = LEVEL::MAP;
                    memcpy(DestinyStoneDesc.szModelName, strModelTag.c_str(), sizeof(DestinyStoneDesc.szModelName));		// 프로토타입 태그명

                    XMStoreFloat4x4(&DestinyStoneDesc.WorldMatrix, WorldMatrix);										// 행렬

                    DestinyStoneDesc.eInteractiveType = INTERACTIVE_TYPE::DESTINYSTONE;										// 상호 작용 오브젝트 타입

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_DestinyStone"), TIME_CHANNEL::WORLD, &DestinyStoneDesc), );
                }
                else if ("DestructibleProp" == m_Prototypes_Inter[m_iIndex_PrtInter])
                {
                    CDestructible_Prop::DESTRUCTIBLE_PROP_DESC DestructiblePropDesc = {};

                    DestructiblePropDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                    DestructiblePropDesc.eLevel = LEVEL::MAP;
                    memcpy(DestructiblePropDesc.szModelName, TEXT("DestructibleProp"), sizeof(DestructiblePropDesc.szModelName));		// 프로토타입 태그명

                    WorldMatrix.r[0] *= 0.005f;
                    WorldMatrix.r[1] *= 0.005f;
                    WorldMatrix.r[2] *= 0.005f;

                    XMStoreFloat4x4(&DestructiblePropDesc.WorldMatrix, WorldMatrix);										// 행렬

                    DestructiblePropDesc.eInteractiveType = INTERACTIVE_TYPE::DESTRUCTIBLE;										// 상호 작용 오브젝트 타입

                    DestructiblePropDesc.eModelType = CDestructible_Prop::MODEL_TYPE::FENCE;

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Destructible"), TIME_CHANNEL::WORLD, &DestructiblePropDesc), );
                }
#pragma endregion

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

#ifdef _DEBUG
					m_pGameInstance->Set_GizmoObject(m_pFixPropObj);
#endif // _DEBUG

					m_isFixInteractObjectWindow = true;
					m_eFixType = FIX_OBJECT::FIX;
				}
			}

			ImGui::End();
		}
		});
#endif // _DEBUG

	return S_OK;
}

HRESULT CLevel_Map::Ready_Prop_Fix_Window()
{
#ifdef _DEBUG
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
#ifdef _DEBUG
				m_pGameInstance->Clear_GizmoObject();
#endif // _DEBUG

				m_FixBaseMatrix = XMMatrixIdentity();

				ZeroMemory(&m_vFixScale, sizeof(_float3));
				ZeroMemory(&m_vFixRotation, sizeof(_float3));
				ZeroMemory(&m_vFixPosition, sizeof(_float3));

				m_pFixPropObj = nullptr;
				m_pFixTransformCom = nullptr;
				m_isFixObjectWindow = false;
				m_eFixType = FIX_OBJECT::END;

			} SAMELINE;
			if (ImGui::Button("RESET (R)") || (true == m_isActiveShortCutKey && m_pGameInstance->Key_Down(DIK_R)) || true == isReset)
			{
#ifdef _DEBUG
				m_pGameInstance->Clear_GizmoObject();
#endif // _DEBUG

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
			if (ImGui::Button("DELETE (ESC)") || (true == m_isActiveShortCutKey && m_pGameInstance->Key_Down(DIK_ESCAPE)))
			{
#ifdef _DEBUG
				m_pGameInstance->Clear_GizmoObject();
#endif // _DEBUG

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
#endif // _DEBUG

	return S_OK;
}

HRESULT CLevel_Map::Ready_Interactive_Prop_Fix_Window()
{
#ifdef _DEBUG
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

			if (INTERACTIVE_TYPE::CHECKPOINT== m_pFixPropObj->Get_InteractiveType())
			{
				ImGui::Text("== BLADE NEXUS INFORMATION ==");
				ImGui::Text("BEFORE");

				ImGui::Text("BLADE NEXUS ID : %d", m_iBN_ID);
				SEPARATOR;
				ImGui::Text("FIX ID");
				ImGui::Text("FIX ID : "); SAMELINE;
				ImGui::InputInt("##bn_id_fix", &m_iFix_BN_ID);

				m_pFixPropObj->Set_BladeNexus_ID(m_iFix_BN_ID);
			}
			if (INTERACTIVE_TYPE::CHEST == m_pFixPropObj->Get_InteractiveType())
			{
				ImGui::Text("== CHEST INFORMATION ==");
				ImGui::Text("BEFORE");

				ImGui::Text("FIRST ITEM : %d", m_ItemBox.iItem_0);
				ImGui::Text("SECOND ITEM : %d", m_ItemBox.iItem_1);
				ImGui::Text("THIRD ITEM : %d", m_ItemBox.iItem_2);
				SEPARATOR;
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
            if (INTERACTIVE_TYPE::TRIGGER == m_pFixPropObj->Get_InteractiveType())
            {
                ImGui::Text("== TRIGGER INFORMATION ==");
                ImGui::Text("BEFORE");

                ImGui::Text("TRIGGER KEY : %s", m_strTriggerKey.c_str());
                SEPARATOR;
                ImGui::Text("FIX TRIGGER KEY");
                ImGui::InputText("##fix_trigger_key", m_szFixTriggerKey, IM_ARRAYSIZE(m_szFixTriggerKey));
            }
            if (INTERACTIVE_TYPE::SPAWN == m_pFixPropObj->Get_InteractiveType())
            {
                ImGui::Text("== MONSTER INFORMATION ==");
                ImGui::Text("BEFORE");

                ImGui::Text("MONSTER KEY : %s", m_strMonsterKey.c_str());
                ImGui::Text("MONSTER SUB LEVEL : %d", m_iMonsterSubLevel);
                SEPARATOR;
                ImGui::Text("FIX MONSTER KEY");
                ImGui::InputText("##fix_monster_key", m_szFixMonsterKey, IM_ARRAYSIZE(m_szFixMonsterKey));
                ImGui::Text("FIX MONSTER SUB LEVEL");
                ImGui::InputInt("##fix_monster_sub_level", &m_iFixMonsterSubLevel);
            }
            if (INTERACTIVE_TYPE::ELEVATOR == m_pFixPropObj->Get_InteractiveType())
            {
                CElevatorS* pElevator = static_cast<CElevatorS*>(m_pFixPropObj);

                ImGui::Text("== ELEVATOR INFORMATION ==");

                _float4 vElevatorUpPos = pElevator->Get_Elevator_UpPos();
                _float4 vElevatorDownPos = pElevator->Get_Elevator_DownPos();

                ImGui::Text("UP POSITION");
                ImGui::Text("X : %.4f | Y : %.4f | Z : %.4f", vElevatorUpPos.x, vElevatorUpPos.y, vElevatorUpPos.z);

                ImGui::Text("DOWN POSITION");
                ImGui::Text("X : %.4f | Y : %.4f | Z : %.4f", vElevatorDownPos.x, vElevatorDownPos.y, vElevatorDownPos.z);

                if (ImGui::Button("UP SETTING"))
                {
                    _float4 vSetPos = {};
                    XMStoreFloat4(&vSetPos, m_pFixTransformCom->Get_State(STATE::POSITION));

                    pElevator->Set_Elevator_UpPos(vSetPos);
                } SAMELINE;
                if (ImGui::Button("DOWN SETTING"))
                {
                    _float4 vSetPos = {};
                    XMStoreFloat4(&vSetPos, m_pFixTransformCom->Get_State(STATE::POSITION));

                    pElevator->Set_Elevator_DownPos(vSetPos);
                } SEPARATOR;
            }
            if (INTERACTIVE_TYPE::LEVER == m_pFixPropObj->Get_InteractiveType() ||
                INTERACTIVE_TYPE::GEAR1 == m_pFixPropObj->Get_InteractiveType())
            {
                CProp_Interactive* pLeorGe = static_cast<CProp_Interactive*>(m_pFixPropObj);

                ImGui::Text("== LEVER OR GEAR INFORMATION ==");
                ImGui::Text("BEFORE EVENT ID : %d", m_iInteractEventID);
                SEPARATOR;
                ImGui::Text("FIX EVENT ID : "); SAMELINE;
                ImGui::InputInt("##fix_event_id", &m_iFixEventID);

                pLeorGe->Set_EventID(m_iFixEventID);
            }
            if (INTERACTIVE_TYPE::GEAR2 == m_pFixPropObj->Get_InteractiveType())
            {
                CGearGate* pGearGate = static_cast<CGearGate*>(m_pFixPropObj);

                ImGui::Text("== GEAR GATE INFORMATION ==");
                ImGui::Text("BEFORE LEFT GEAR EVENT ID : %d", m_iLeftGearEventID);
                ImGui::Text("BEFORE RIGHT GEAR EVENT ID : %d", m_iRightGearEventID);
                SEPARATOR;
                ImGui::Text("FIX LEFT EVENT ID : "); SAMELINE;
                ImGui::InputInt("##fix_event_id_left", &m_iFixLeftGearEventID);
                ImGui::Text("FIX RIGHT EVENT ID : "); SAMELINE;
                ImGui::InputInt("##fix_event_id_right", &m_iFixRightGearEventID);
                SEPARATOR;

                pGearGate->Set_DoorGear_EventID(m_iFixLeftGearEventID, m_iFixRightGearEventID);
            }
            if (INTERACTIVE_TYPE::STATUE == m_pFixPropObj->Get_InteractiveType())
            {
                CStatue* pStatue = static_cast<CStatue*>(m_pFixPropObj);

                ImGui::Text("== STATUE INFORMATION ==");
                ImGui::Text("BEFORE EVENT ID : %d", m_iInteractEventID);
                ImGui::Text("BEFORE UNLOCK ROTATION : %d", m_iUnLockRotation);
                SEPARATOR;
                ImGui::Text("FIX EVENT ID : "); SAMELINE;
                ImGui::InputInt("##fix_event_id", &m_iFixEventID);
                ImGui::Text("FIX UNLOCK ROTATION : "); SAMELINE;
                ImGui::InputInt("##fix_rotation_statue", &m_iFixUnLockRotation);

                pStatue->Set_EventID(m_iFixEventID);
                pStatue->Set_StatueUnLockRotation(m_iFixUnLockRotation);
                SEPARATOR;
            }
            if (INTERACTIVE_TYPE::VERTICALGATE == m_pFixPropObj->Get_InteractiveType())
            {
                CVerticalGate* pVerticalGate = static_cast<CVerticalGate*>(m_pFixPropObj);

                ImGui::Text("== STATUE INFORMATION ==");
                ImGui::Text("BEFORE EVENT ID : %d", m_iInteractEventID);
                SEPARATOR;
                ImGui::Text("FIX EVENT ID : "); SAMELINE;
                ImGui::InputInt("##fix_event_id", &m_iFixEventID);

                pVerticalGate->Set_EventID(m_iFixEventID);
                SEPARATOR;
            }
            if (INTERACTIVE_TYPE::IRONGATE == m_pFixPropObj->Get_InteractiveType())
            {
                CIronGate* pIronGate = static_cast<CIronGate*>(m_pFixPropObj);

                ImGui::Text("== IRONGATE ==");
                SEPARATOR;
            }
            if (INTERACTIVE_TYPE::LADDER == m_pFixPropObj->Get_InteractiveType())
            {
                CLadder* pLadder = static_cast<CLadder*>(m_pFixPropObj);

                ImGui::Text("== LADDER INFORMATION ==");
                ImGui::Text("TOP LADDER : "); SAMELINE;
                ImGui::InputFloat("##fix_top_ladder_height", &m_fLadderTopHeight, 0.4f, 0.8f);
                //ImGui::SliderFloat("##fix_top_ladder_height", &m_fLadderTopHeight, m_fLadderTopHeightOffset - 30.f, m_fLadderTopHeightOffset + 30.f);
                ImGui::Text("MIDDLE LADDER : "); SAMELINE;
                ImGui::InputFloat("##fix_mid_ladder_height", &m_fLadderMiddleHeight, 0.4f, 0.8f);
                //ImGui::SliderFloat("##fix_mid_ladder_height", &m_fLadderMiddleHeight, m_fLadderMiddleHeightOffset - 30.f, m_fLadderMiddleHeightOffset + 30.f);

                pLadder->Set_TopPosition_Y(m_fLadderTopHeight);
                pLadder->Set_MiddlePosition_Y(m_fLadderMiddleHeight);

                SEPARATOR;
            }
            if (INTERACTIVE_TYPE::UNLOCKGEAR == m_pFixPropObj->Get_InteractiveType())
            {
                CUnLockGear* pUnLockGear = static_cast<CUnLockGear*>(m_pFixPropObj);

                ImGui::Text("== UNLOCK GEAR INFORMATION ==");
                ImGui::Text("BEFORE EVENT ID : %d", m_iInteractEventID);
                SEPARATOR;
                ImGui::Text("FIX EVENT ID : "); SAMELINE;
                ImGui::InputInt("##fix_event_id", &m_iFixEventID);

                pUnLockGear->Set_EventID(m_iFixEventID);
                SEPARATOR;
            }
            if (INTERACTIVE_TYPE::LARGEELEVATOR == m_pFixPropObj->Get_InteractiveType())
            {
                CElevatorL* pElevator = static_cast<CElevatorL*>(m_pFixPropObj);

                ImGui::Text("== LARGE ELEVATOR INFORMATION ==");

                _float4 vElevatorUpPos = pElevator->Get_Elevator_UpPos();
                _float4 vElevatorMidPos = pElevator->Get_Elevator_MidPos();
                _float4 vElevatorDownPos = pElevator->Get_Elevator_DownPos();

                ImGui::Text("UP POSITION");
                ImGui::Text("X : %.4f | Y : %.4f | Z : %.4f", vElevatorUpPos.x, vElevatorUpPos.y, vElevatorUpPos.z);

                ImGui::Text("MID POSITION");
                ImGui::Text("X : %.4f | Y : %.4f | Z : %.4f", vElevatorMidPos.x, vElevatorMidPos.y, vElevatorMidPos.z);

                ImGui::Text("DOWN POSITION");
                ImGui::Text("X : %.4f | Y : %.4f | Z : %.4f", vElevatorDownPos.x, vElevatorDownPos.y, vElevatorDownPos.z);

                if (ImGui::Button("UP SETTING"))
                {
                    _float4 vSetPos = {};
                    XMStoreFloat4(&vSetPos, m_pFixTransformCom->Get_State(STATE::POSITION));

                    pElevator->Set_Elevator_UpPos(vSetPos);
                } SAMELINE;
                if (ImGui::Button("MID SETTING"))
                {
                    _float4 vSetPos = {};
                    XMStoreFloat4(&vSetPos, m_pFixTransformCom->Get_State(STATE::POSITION));

                    pElevator->Set_Elevator_MidPos(vSetPos);
                } SAMELINE;
                if (ImGui::Button("DOWN SETTING"))
                {
                    _float4 vSetPos = {};
                    XMStoreFloat4(&vSetPos, m_pFixTransformCom->Get_State(STATE::POSITION));

                    pElevator->Set_Elevator_DownPos(vSetPos);
                } SEPARATOR;
            }
            if (INTERACTIVE_TYPE::GIANTGATE == m_pFixPropObj->Get_InteractiveType())
            {
                CIronGate* pIronGate = static_cast<CIronGate*>(m_pFixPropObj);

                ImGui::Text("== GIANT GATE ==");
                SEPARATOR;
            }
            if (INTERACTIVE_TYPE::DAPHRONA == m_pFixPropObj->Get_InteractiveType())
            {
                CNPC_Daphrona* pDaphrona = static_cast<CNPC_Daphrona*>(m_pFixPropObj);

                ImGui::Text("== DAPHRONA ==");
                SEPARATOR;
            }
            if (INTERACTIVE_TYPE::DUIMUK == m_pFixPropObj->Get_InteractiveType())
            {
                CNPC_Duimuk* pDuimuk = static_cast<CNPC_Duimuk*>(m_pFixPropObj);

                ImGui::Text("== DUIMUK ==");
                SEPARATOR;
            }
            if (INTERACTIVE_TYPE::DANJIN == m_pFixPropObj->Get_InteractiveType())
            {
                CNPC_Danjin* pDanjin = static_cast<CNPC_Danjin*>(m_pFixPropObj);

                ImGui::Text("== DANJIN ==");
                SEPARATOR;
            }
            if (INTERACTIVE_TYPE::DANJINJAR == m_pFixPropObj->Get_InteractiveType())
            {
                CDanjinJar* pDanjinJar = static_cast<CDanjinJar*>(m_pFixPropObj);

                ImGui::Text("== DANJIN JAR ==");
                if (ImGui::Button("MODEL TYPE A"))
                    pDanjinJar->Set_DanjinJar_ModelType(CDanjinJar::DANJINJAR_TYPE::A);
                SAMELINE;
                if (ImGui::Button("MODEL TYPE B"))
                    pDanjinJar->Set_DanjinJar_ModelType(CDanjinJar::DANJINJAR_TYPE::B);
                SAMELINE;
                if (ImGui::Button("MODEL TYPE C"))
                    pDanjinJar->Set_DanjinJar_ModelType(CDanjinJar::DANJINJAR_TYPE::C);
                SAMELINE;
                if (ImGui::Button("MODEL TYPE D"))
                    pDanjinJar->Set_DanjinJar_ModelType(CDanjinJar::DANJINJAR_TYPE::D);
                SEPARATOR;
                if (ImGui::Button("MODEL TYPE E"))
                    pDanjinJar->Set_DanjinJar_ModelType(CDanjinJar::DANJINJAR_TYPE::E);
                SAMELINE;
                if (ImGui::Button("MODEL TYPE F"))
                    pDanjinJar->Set_DanjinJar_ModelType(CDanjinJar::DANJINJAR_TYPE::F);
                SAMELINE;
                if (ImGui::Button("MODEL TYPE G"))
                    pDanjinJar->Set_DanjinJar_ModelType(CDanjinJar::DANJINJAR_TYPE::G);
                SAMELINE;
                if (ImGui::Button("MODEL TYPE H"))
                    pDanjinJar->Set_DanjinJar_ModelType(CDanjinJar::DANJINJAR_TYPE::H);
                SEPARATOR;

                ImGui::Text("SETTING STEP");
                if (ImGui::Button("STEP 1")) pDanjinJar->StepPositionSetting(0); SAMELINE;
                if (ImGui::Button("STEP 2")) pDanjinJar->StepPositionSetting(1); SAMELINE;
                if (ImGui::Button("STEP 3")) pDanjinJar->StepPositionSetting(2); SAMELINE;
                if (ImGui::Button("STEP 4")) pDanjinJar->StepPositionSetting(3);
                if (ImGui::Button("STEP 5")) pDanjinJar->StepPositionSetting(4); SAMELINE;
                if (ImGui::Button("STEP 6")) pDanjinJar->StepPositionSetting(5); SAMELINE;
                if (ImGui::Button("STEP 7")) pDanjinJar->StepPositionSetting(6); SAMELINE;
                if (ImGui::Button("STEP 8")) pDanjinJar->StepPositionSetting(7);
                if (ImGui::Button("STEP 9")) pDanjinJar->StepPositionSetting(8); SAMELINE;
                if (ImGui::Button("STEP 10")) pDanjinJar->StepPositionSetting(9); SAMELINE;
                if (ImGui::Button("STEP 11")) pDanjinJar->StepPositionSetting(10); SAMELINE;
                if (ImGui::Button("STEP 12")) pDanjinJar->StepPositionSetting(11);
                if (ImGui::Button("STEP 13")) pDanjinJar->StepPositionSetting(12); SAMELINE;
                if (ImGui::Button("STEP 14")) pDanjinJar->StepPositionSetting(13); SAMELINE;
                if (ImGui::Button("STEP 15")) pDanjinJar->StepPositionSetting(14); SAMELINE;
                if (ImGui::Button("STEP 16")) pDanjinJar->StepPositionSetting(15);
                SEPARATOR;
                SEPARATOR;

                ImGui::Text("MOVE STEP");
                if (ImGui::Button("STEP 1##move")) pDanjinJar->MoveStepPosition(0); SAMELINE;
                if (ImGui::Button("STEP 2##move")) pDanjinJar->MoveStepPosition(1); SAMELINE;
                if (ImGui::Button("STEP 3##move")) pDanjinJar->MoveStepPosition(2); SAMELINE;
                if (ImGui::Button("STEP 4##move")) pDanjinJar->MoveStepPosition(3);
                if (ImGui::Button("STEP 5##move")) pDanjinJar->MoveStepPosition(4); SAMELINE;
                if (ImGui::Button("STEP 6##move")) pDanjinJar->MoveStepPosition(5); SAMELINE;
                if (ImGui::Button("STEP 7##move")) pDanjinJar->MoveStepPosition(6); SAMELINE;
                if (ImGui::Button("STEP 8##move")) pDanjinJar->MoveStepPosition(7);
                if (ImGui::Button("STEP 9##move")) pDanjinJar->MoveStepPosition(8); SAMELINE;
                if (ImGui::Button("STEP 10##move")) pDanjinJar->MoveStepPosition(9); SAMELINE;
                if (ImGui::Button("STEP 11##move")) pDanjinJar->MoveStepPosition(10); SAMELINE;
                if (ImGui::Button("STEP 12##move")) pDanjinJar->MoveStepPosition(11);
                if (ImGui::Button("STEP 13##move")) pDanjinJar->MoveStepPosition(12); SAMELINE;
                if (ImGui::Button("STEP 14##move")) pDanjinJar->MoveStepPosition(13); SAMELINE;
                if (ImGui::Button("STEP 15##move")) pDanjinJar->MoveStepPosition(14); SAMELINE;
                if (ImGui::Button("STEP 16##move")) pDanjinJar->MoveStepPosition(15); SEPARATOR;
            }
            if (INTERACTIVE_TYPE::DESTINYSTONE == m_pFixPropObj->Get_InteractiveType())
            {
                CDestinyStone* pDestinyStone = static_cast<CDestinyStone*>(m_pFixPropObj);

                ImGui::Text("== DESTINY STONE ==");
                SEPARATOR;
            }
            if (INTERACTIVE_TYPE::DESTRUCTIBLE == m_pFixPropObj->Get_InteractiveType())
            {
                CDestructible_Prop* pDestructibleProp = static_cast<CDestructible_Prop*>(m_pFixPropObj);

                string strCurrentModelType = pDestructibleProp->Get_Destructible_ModelType_ByString();

                ImGui::Text("== DESTRUCTIBLE PROP ==");

                ImGui::Text("CURRENT MODEL TYPE : %s", strCurrentModelType.c_str());

                if (ImGui::Button("MODEL TYPE FENCE"))
                    pDestructibleProp->Set_Destructible_ModelType(CDestructible_Prop::MODEL_TYPE::FENCE);
                SAMELINE;
                if (ImGui::Button("MODEL TYPE POT"))
                    pDestructibleProp->Set_Destructible_ModelType(CDestructible_Prop::MODEL_TYPE::POT);
                SAMELINE;
                if (ImGui::Button("MODEL TYPE BARREL"))
                    pDestructibleProp->Set_Destructible_ModelType(CDestructible_Prop::MODEL_TYPE::BARREL);
                SEPARATOR;

                SEPARATOR;
            }

#pragma endregion

			if (ImGui::Button("DONE ( ENTER or MOUSE RB )") || m_pGameInstance->Key_Down(DIK_RETURN) || m_pGameInstance->Key_Down(DIK_NUMPADENTER) || m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
			{
#ifdef _DEBUG
				m_pGameInstance->Clear_GizmoObject();
#endif // _DEBUG

                if (INTERACTIVE_TYPE::TRIGGER == m_pFixPropObj->Get_InteractiveType())
                {
                    m_strFixTriggerKey = m_szFixTriggerKey;

                    static_cast<CTrigger*>(m_pFixPropObj)->Set_TriggerKey(m_strFixTriggerKey);

                    ZeroMemory(m_szFixTriggerKey, sizeof(m_szFixTriggerKey));
                }
                if (INTERACTIVE_TYPE::SPAWN == m_pFixPropObj->Get_InteractiveType())
                {
                    m_strFixMonsterKey = m_szFixMonsterKey;

                    static_cast<CMap_Spawn*>(m_pFixPropObj)->Set_MonsterKey(m_strFixMonsterKey);
                    static_cast<CMap_Spawn*>(m_pFixPropObj)->Set_SubLevel(m_iFixMonsterSubLevel);

                    ZeroMemory(m_szFixMonsterKey, sizeof(m_szFixMonsterKey));
                }

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
			if (ImGui::Button("RESET (R)") || (true == m_isActiveShortCutKey && m_pGameInstance->Key_Down(DIK_R)) || true == isReset)
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
			if (ImGui::Button("DELETE (ESC)") || (true == m_isActiveShortCutKey && m_pGameInstance->Key_Down(DIK_ESCAPE)))
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
#endif // _DEBUG

	return S_OK;
}

HRESULT CLevel_Map::Ready_Prop_List_Window()
{

#ifdef _DEBUG
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

#ifdef _DEBUG

							m_pGameInstance->Set_GizmoObject(m_pFixPropObj);

#endif // _DEBUG

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
#endif // _DEBUG

	return S_OK;
}

HRESULT CLevel_Map::Ready_Interactive_Prop_List_Window()
{
#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isInteractiveWindow)
		{
			ImGui::Begin("PROP INTERACTIVE WINDOW", &m_isInteractiveWindow, ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::Text("INTERACT OBJECT LIST");
            ImGui::Text("SEARCH : "); SAMELINE;
            ImGui::InputText("##search_interact_object_name", m_szSearchInteractObjectName, IM_ARRAYSIZE(m_szSearchInteractObjectName)); SAMELINE;

            if (ImGui::Button("CLEAR"))
                ZeroMemory(m_szSearchInteractObjectName, sizeof(m_szSearchInteractObjectName));

			ITEMWIDTH(300.f);
			if (ImGui::BeginListBox("##prop_interactive_list"))
			{
				if (m_iInteractiveListIndex >= m_InteractiveList.size())
					m_iInteractiveListIndex = m_InteractiveList.size() - 1;

                string strSearchName = m_szSearchInteractObjectName;
                transform(strSearchName.begin(), strSearchName.end(), strSearchName.begin(), ::tolower);		// 검색할 모델을 소문자로 변환

				for (_uint i = 0; i < m_InteractiveList.size(); ++i)
				{
                    _wstring strSearchModelName = {};
                    _wstring strDPModelName = {};
                    if (INTERACTIVE_TYPE::SPAWN == m_InteractiveList[i]->Get_InteractiveType())
                        strSearchModelName = AnsiToWString(static_cast<CMap_Spawn*>(m_InteractiveList[i])->Get_MonsterKey());
                    else if (INTERACTIVE_TYPE::TRIGGER == m_InteractiveList[i]->Get_InteractiveType())
                        strSearchModelName = AnsiToWString(static_cast<CTrigger*>(m_InteractiveList[i])->Get_TriggerKey());
                    else
                        strSearchModelName = m_InteractiveList[i]->Get_ModelName();

                    strDPModelName = strSearchModelName;
                    transform(strSearchModelName.begin(), strSearchModelName.end(), strSearchModelName.begin(), ::tolower);		// 찾을 모델을 소문자로 변환

                    if (true == strSearchName.empty() || strSearchModelName.find(AnsiToWString(strSearchName)) != string::npos)
                    {
                        _bool isSelected = (m_iInteractiveListIndex == i);

                        string strModelName = WStringToAnsi(strDPModelName) + "##id_%d";

                        _char szModelName[MAX_PATH] = {};

                        sprintf_s(szModelName, strModelName.c_str(), i);

                        if (ImGui::Selectable(szModelName, isSelected))
                        {
                            m_iInteractiveListIndex = i;
                            if (INTERACTIVE_TYPE::SPAWN == m_InteractiveList[m_iInteractiveListIndex]->Get_InteractiveType())
                            {
                                static_cast<CMap_Spawn*>(m_InteractiveList[m_iInteractiveListIndex])->Set_Picked();
                            }
                        }
                        else
                        {
                            if (INTERACTIVE_TYPE::SPAWN == m_InteractiveList[m_iInteractiveListIndex]->Get_InteractiveType())
                            {
                                static_cast<CMap_Spawn*>(m_InteractiveList[i])->Set_Red();
                            }
                        }
                    }
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
                CTransform* pTransform = static_cast<CTransform*>(m_InteractiveList[m_iInteractiveListIndex]->Get_Component(TEXT("Com_Transform")));
                if (nullptr != pTransform)
                {
                    ImGui::Text("POSITION");
                    ImGui::Text("X : %.4f", pTransform->Get_WorldMatrixPtr()->_41);
                    ImGui::Text("Y : %.4f", pTransform->Get_WorldMatrixPtr()->_42);
                    ImGui::Text("Z : %.4f", pTransform->Get_WorldMatrixPtr()->_43);
                    SEPARATOR;
                }

				_wstring strModelName = m_InteractiveList[m_iInteractiveListIndex]->Get_ModelName();
				string strTempModelName = WStringToAnsi(strModelName);

				ImGui::Text("MODEL NAME : %s", strTempModelName.c_str());
				SEPARATOR;

                if (INTERACTIVE_TYPE::TRIGGER == m_InteractiveList[m_iInteractiveListIndex]->Get_InteractiveType())
                {
                    ImGui::Text("TRIGGER KEY : %s", static_cast<CTrigger*>(m_InteractiveList[m_iInteractiveListIndex])->Get_TriggerKey().c_str());
                    SEPARATOR;
                }
                if (INTERACTIVE_TYPE::SPAWN == m_InteractiveList[m_iInteractiveListIndex]->Get_InteractiveType())
                {
                    ImGui::Text("MONSTER KEY : %s", static_cast<CMap_Spawn*>(m_InteractiveList[m_iInteractiveListIndex])->Get_MonsterKey().c_str());
                    ImGui::Text("MONSTER SUB LEVEL : %d", static_cast<CMap_Spawn*>(m_InteractiveList[m_iInteractiveListIndex])->Get_SubLevel());
                    SEPARATOR;
                }
                if (INTERACTIVE_TYPE::DANJINJAR == m_InteractiveList[m_iInteractiveListIndex]->Get_InteractiveType())
                {
                    ImGui::Text("DANJIN JAR TYPE : %s", static_cast<CDanjinJar*>(m_InteractiveList[m_iInteractiveListIndex])->Get_DanjinJar_ModelType_ByString().c_str());
                    SEPARATOR;
                }
                if (INTERACTIVE_TYPE::DESTRUCTIBLE == m_InteractiveList[m_iInteractiveListIndex]->Get_InteractiveType())
                {
                    ImGui::Text("DESTRUCTIBLE MODEL TYPE : %s", static_cast<CDestructible_Prop*>(m_InteractiveList[m_iInteractiveListIndex])->Get_Destructible_ModelType_ByString().c_str());
                    SEPARATOR;
                }
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

						if (INTERACTIVE_TYPE::CHECKPOINT == m_pFixPropObj->Get_InteractiveType())
						{
							m_iFix_BN_ID = m_iBN_ID = m_pFixPropObj->Get_BladeNexus_ID();
						}

						if (INTERACTIVE_TYPE::CHEST == m_pFixPropObj->Get_InteractiveType())
						{
							m_FixItemBox = m_ItemBox = m_pFixPropObj->Get_ItemBox();
						}

                        if (INTERACTIVE_TYPE::TRIGGER == m_pFixPropObj->Get_InteractiveType())
                        {
                            CTrigger* pTrigger = static_cast<CTrigger*>(m_pFixPropObj);
                            m_strFixTriggerKey = m_strTriggerKey = pTrigger->Get_TriggerKey();
                            memcpy(m_szFixTriggerKey, m_strFixTriggerKey.c_str(), MAX_PATH);
                        }

                        if (INTERACTIVE_TYPE::SPAWN == m_pFixPropObj->Get_InteractiveType())
                        {
                            CMap_Spawn* pSpawn = static_cast<CMap_Spawn*>(m_pFixPropObj);
                            m_strFixMonsterKey = m_strMonsterKey = pSpawn->Get_MonsterKey();
                            m_iFixMonsterSubLevel = m_iMonsterSubLevel = pSpawn->Get_SubLevel();
                            memcpy(m_szFixMonsterKey, m_strFixMonsterKey.c_str(), MAX_PATH);
                        }

                        if (INTERACTIVE_TYPE::ELEVATOR == m_pFixPropObj->Get_InteractiveType())
                        {
                            CElevatorS* pElevator = static_cast<CElevatorS*>(m_pFixPropObj);

                            m_vElevatorUpPos = pElevator->Get_Elevator_UpPos();
                            m_vElevatorDownPos = pElevator->Get_Elevator_DownPos();
                        }

                        if (INTERACTIVE_TYPE::LEVER == m_pFixPropObj->Get_InteractiveType() ||
                            INTERACTIVE_TYPE::GEAR1 == m_pFixPropObj->Get_InteractiveType())
                        {
                            CProp_Interactive* pLever = static_cast<CProp_Interactive*>(m_pFixPropObj);

                            m_iFixEventID = m_iInteractEventID = pLever->Get_EventID();
                        }

                        if (INTERACTIVE_TYPE::GEAR2 == m_pFixPropObj->Get_InteractiveType())
                        {
                            CGearGate* pGearGate = static_cast<CGearGate*>(m_pFixPropObj);

                            CGearGate::DOOR_GEAR_EVENTID EventIDs = { pGearGate->Get_DoorGear_EventID() };

                            m_iFixLeftGearEventID = m_iLeftGearEventID = EventIDs.iLeftEventID;
                            m_iFixRightGearEventID = m_iRightGearEventID = EventIDs.iRightEventID;
                        }

                        if (INTERACTIVE_TYPE::STATUE == m_pFixPropObj->Get_InteractiveType())
                        {
                            CStatue* pStatue = static_cast<CStatue*>(m_pFixPropObj);

                            m_iFixEventID = m_iInteractEventID = pStatue->Get_EventID();
                            m_iFixUnLockRotation = m_iUnLockRotation = pStatue->Get_StatueUnLockRotation();
                        }

                        if (INTERACTIVE_TYPE::VERTICALGATE == m_pFixPropObj->Get_InteractiveType())
                        {
                            CVerticalGate* pStatue = static_cast<CVerticalGate*>(m_pFixPropObj);

                            m_iFixEventID = m_iInteractEventID = pStatue->Get_EventID();
                        }

                        if (INTERACTIVE_TYPE::IRONGATE == m_pFixPropObj->Get_InteractiveType())
                        {
                            CIronGate* pStatue = static_cast<CIronGate*>(m_pFixPropObj);
                        }

                        if (INTERACTIVE_TYPE::LADDER == m_pFixPropObj->Get_InteractiveType())
                        {
                            CLadder* pLadder = static_cast<CLadder*>(m_pFixPropObj);

                            m_fLadderTopHeightOffset = m_fLadderTopHeight = pLadder->Get_TopPosition_Y();

                            m_fLadderMiddleHeightOffset = m_fLadderMiddleHeight = pLadder->Get_MiddlePosition_Y();
                        }

                        if (INTERACTIVE_TYPE::UNLOCKGEAR == m_pFixPropObj->Get_InteractiveType())
                        {
                            CUnLockGear* pUnLockGear = static_cast<CUnLockGear*>(m_pFixPropObj);

                            m_iFixEventID = m_iInteractEventID = pUnLockGear->Get_EventID();
                        }

                        if (INTERACTIVE_TYPE::LARGEELEVATOR == m_pFixPropObj->Get_InteractiveType())
                        {
                            CElevatorL* pElevator = static_cast<CElevatorL*>(m_pFixPropObj);

                            m_vElevatorUpPos = pElevator->Get_Elevator_UpPos();
                            m_vElevatorMidPos = pElevator->Get_Elevator_MidPos();
                            m_vElevatorDownPos = pElevator->Get_Elevator_DownPos();
                        }

                        if (INTERACTIVE_TYPE::GIANTGATE == m_pFixPropObj->Get_InteractiveType())
                        {
                            CGiantGate* pGiantGate = static_cast<CGiantGate*>(m_pFixPropObj);
                        }

                        if (INTERACTIVE_TYPE::DAPHRONA == m_pFixPropObj->Get_InteractiveType() ||
                            INTERACTIVE_TYPE::DUIMUK == m_pFixPropObj->Get_InteractiveType() ||
                            INTERACTIVE_TYPE::DANJIN == m_pFixPropObj->Get_InteractiveType())
                        {
                            // NPC 일단 빈칸
                        }

                        if (INTERACTIVE_TYPE::DANJINJAR == m_pFixPropObj->Get_InteractiveType())
                        {
                            //  항아리 일단 빈칸
                        }

                        if (INTERACTIVE_TYPE::DESTINYSTONE == m_pFixPropObj->Get_InteractiveType())
                        {
                            //  귀석 일단 빈칸
                        }

                        if (INTERACTIVE_TYPE::DESTRUCTIBLE == m_pFixPropObj->Get_InteractiveType())
                        {
                            // 파괴 가능 오브젝트 일단 빈칸
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

			if (ImGui::Button("EXPORT ONLY INTERACTIVE"))
			{
				m_strMapInfoFilePath = m_szMapInfoFilePath;
				m_strMapInfoFilePath += m_szMapInfoFileName;

				if (false == Interactive_Object_Save_Binary())
				{
					_int a = 10;
				}
			}

            if (ImGui::Button("EXPORT ONLY TRIGGER"))
            {
                m_strMapInfoFilePath = m_szMapInfoFilePath;
                m_strMapInfoFilePath += m_szMapInfoFileName;

                if (false == Trigger_Save_Json())
                {
                    _int a = 10;
                }
            }

            if (ImGui::Button("EXPORT ONLY MONSTER"))
            {
                m_strMapInfoFilePath = m_szMapInfoFilePath;
                m_strMapInfoFilePath += m_szMapInfoFileName;

                if (false == Monster_Save_Json())
                {
                    _int a = 10;
                }
            }

			ImGui::End();
		}
		});
#endif // _DEBUG

	return S_OK;
}

HRESULT CLevel_Map::Ready_Light_Window()
{
#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isLightSettingWindow)
		{
			ImGui::Begin("LIGHT WINDOW", &m_isLightSettingWindow, ImGuiWindowFlags_AlwaysAutoResize);

            if (true == m_isFixBatchLight)
            {
                ImGui::Text("DIFFUSE");
                ImGui::ColorPicker4("##batch_point_dif_light", reinterpret_cast<_float*>(&m_BatchOnLightDesc.vDiffuse.x));
                SEPARATOR;

                ImGui::Text("AMBIENT R : "); SAMELINE;
                ImGui::InputFloat("##batch_fix_pt_amb_light_r", &m_BatchOnLightDesc.vAmbient.x);
                ImGui::Text("AMBIENT G : "); SAMELINE;
                ImGui::InputFloat("##batch_fix_pt_amb_light_g", &m_BatchOnLightDesc.vAmbient.y);
                ImGui::Text("AMBIENT B : "); SAMELINE;
                ImGui::InputFloat("##batch_fix_pt_amb_light_b", &m_BatchOnLightDesc.vAmbient.z);
                ImGui::Text("AMBIENT A : "); SAMELINE;
                ImGui::InputFloat("##batch_fix_pt_amb_light_a", &m_BatchOnLightDesc.vAmbient.w);
                //ImGui::ColorPicker4("##batch_point_amb_light", reinterpret_cast<_float*>(&m_BatchOnLightDesc.vAmbient.x));
                SEPARATOR;

                ImGui::Text("SPECULAR R : "); SAMELINE;
                ImGui::InputFloat("##batch_fix_pt_spe_light_r", &m_BatchOnLightDesc.vSpecular.x);
                ImGui::Text("SPECULAR G : "); SAMELINE;
                ImGui::InputFloat("##batch_fix_pt_spe_light_g", &m_BatchOnLightDesc.vSpecular.y);
                ImGui::Text("SPECULAR B : "); SAMELINE;
                ImGui::InputFloat("##batch_fix_pt_spe_light_b", &m_BatchOnLightDesc.vSpecular.z);
                ImGui::Text("SPECULAR A : "); SAMELINE;
                ImGui::InputFloat("##batch_fix_pt_spe_light_a", &m_BatchOnLightDesc.vSpecular.w);
                //ImGui::ColorPicker4("##batch_point_spec_light", reinterpret_cast<_float*>(&m_BatchOnLightDesc.vSpecular.x));
                SEPARATOR;

                if (ImGui::Button("BATCH DESC"))
                {
                    _uint iSize = m_LightTags.size();

                    for (_uint i = 0; i < iSize; ++i)
                    {
                        wstring strLightTag = AnsiToWString(m_LightTags[i]);

                        _bool isAble = m_pGameInstance->Is_LightEnable(strLightTag, ENUM_CLASS(LEVEL::MAP));
                        if (false == isAble)
                            continue;

                        LIGHT_DESC LightDesc = *m_pGameInstance->Get_LightDesc(strLightTag, ENUM_CLASS(LEVEL::MAP));
                        if (LIGHT_DESC::TYPE::POINT != LightDesc.eType)
                            continue;

                        m_BatchOnLightDesc.vPosition = LightDesc.vPosition;
                        m_BatchOnLightDesc.fRange = LightDesc.fRange;
                        m_BatchOnLightDesc.eType = LightDesc.eType;

                        if (true == isAble)
                            m_pGameInstance->Set_LightDesc(strLightTag, ENUM_CLASS(LEVEL::MAP), m_BatchOnLightDesc);
                    }

                    m_isFixBatchLight = false;
                }
            }
			else if (true == m_isFindFixLight)
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
                    ImGui::ColorPicker4("##direct_dif_light", reinterpret_cast<_float*>(&m_FixLightDesc.vDiffuse.x));
					SEPARATOR;

                    ImGui::Text("AMBIENT R : "); SAMELINE;
                    ImGui::InputFloat("##fix_dir_amb_light_r", &m_FixLightDesc.vAmbient.x);
                    ImGui::Text("AMBIENT G : "); SAMELINE;
                    ImGui::InputFloat("##fix_dir_amb_light_g", &m_FixLightDesc.vAmbient.y);
                    ImGui::Text("AMBIENT B : "); SAMELINE;
                    ImGui::InputFloat("##fix_dir_amb_light_b", &m_FixLightDesc.vAmbient.z);
                    ImGui::Text("AMBIENT A : "); SAMELINE;
                    ImGui::InputFloat("##fix_dir_amb_light_a", &m_FixLightDesc.vAmbient.w);
                    //ImGui::ColorPicker4("##direct_amb_light", reinterpret_cast<_float*>(&m_FixLightDesc.vAmbient.x));
                    SEPARATOR;

                    ImGui::Text("SPECULAR R : "); SAMELINE;
                    ImGui::InputFloat("##fix_dir_spe_light_r", &m_FixLightDesc.vSpecular.x);
                    ImGui::Text("SPECULAR G : "); SAMELINE;
                    ImGui::InputFloat("##fix_dir_spe_light_g", &m_FixLightDesc.vSpecular.y);
                    ImGui::Text("SPECULAR B : "); SAMELINE;
                    ImGui::InputFloat("##fix_dir_spe_light_b", &m_FixLightDesc.vSpecular.z);
                    ImGui::Text("SPECULAR A : "); SAMELINE;
                    ImGui::InputFloat("##fix_dir_spe_light_a", &m_FixLightDesc.vSpecular.w);
                    //ImGui::ColorPicker4("##direct_spec_light", reinterpret_cast<_float*>(&m_FixLightDesc.vSpecular.x));

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
                    ImGui::ColorPicker4("##point_dif_light", reinterpret_cast<_float*>(&m_FixLightDesc.vDiffuse.x));
                    SEPARATOR;

                    ImGui::Text("AMBIENT R : "); SAMELINE;
                    ImGui::InputFloat("##fix_pt_amb_light_r", &m_FixLightDesc.vAmbient.x);
                    ImGui::Text("AMBIENT G : "); SAMELINE;
                    ImGui::InputFloat("##fix_pt_amb_light_g", &m_FixLightDesc.vAmbient.y);
                    ImGui::Text("AMBIENT B : "); SAMELINE;
                    ImGui::InputFloat("##fix_pt_amb_light_b", &m_FixLightDesc.vAmbient.z);
                    ImGui::Text("AMBIENT A : "); SAMELINE;
                    ImGui::InputFloat("##fix_pt_amb_light_a", &m_FixLightDesc.vAmbient.w);
                    //ImGui::ColorPicker4("##point_amb_light", reinterpret_cast<_float*>(&m_FixLightDesc.vAmbient.x));
                    SEPARATOR;

                    ImGui::Text("SPECULAR R : "); SAMELINE;
                    ImGui::InputFloat("##fix_pt_spe_light_r", &m_FixLightDesc.vSpecular.x);
                    ImGui::Text("SPECULAR G : "); SAMELINE;
                    ImGui::InputFloat("##fix_pt_spe_light_g", &m_FixLightDesc.vSpecular.y);
                    ImGui::Text("SPECULAR B : "); SAMELINE;
                    ImGui::InputFloat("##fix_pt_spe_light_b", &m_FixLightDesc.vSpecular.z);
                    ImGui::Text("SPECULAR A : "); SAMELINE;
                    ImGui::InputFloat("##fix_pt_spe_light_a", &m_FixLightDesc.vSpecular.w);
                    //ImGui::ColorPicker4("##point_spec_light", reinterpret_cast<_float*>(&m_FixLightDesc.vSpecular.x));
                    SEPARATOR;
				}

				m_pGameInstance->Set_LightDesc(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP), m_FixLightDesc);

                if (ImGui::Button("DONE") || m_pGameInstance->Key_Down(DIK_RETURN) || m_pGameInstance->Key_Down(DIK_NUMPADENTER) || m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
                {
                    m_strFixLightTag.clear();
                    m_isFindFixLight = false;
                    ZeroMemory(&m_FixLightDesc, sizeof(LIGHT_DESC));
                }

                if (ImGui::Button("RESET") || m_pGameInstance->Key_Down(DIK_R))
                {
                    m_pGameInstance->Set_LightDesc(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP), m_OriginalLightDesc);

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
                if (0 != m_LightTags.size())
                {
                    _bool isEnable = m_pGameInstance->Is_LightEnable(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP));
                    if (true == isEnable)
                        ImGui::Text("ENABLE");
                    else
                        ImGui::Text("DISABLE");
                    SEPARATOR;
                }
				if (0 != m_LightTags.size() && ImGui::Button("TURN ON"))
				{
					m_isFixLight = false;
					m_isAddLight = false;
					m_LightDesc.eType = LIGHT_DESC::END;

					m_pGameInstance->Set_LightEnable(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP), true);

				} SAMELINE;
                if (0 != m_LightTags.size() && ImGui::Button("TURN OFF"))
                {
                    m_isFixLight = false;
                    m_isAddLight = false;
                    m_LightDesc.eType = LIGHT_DESC::END;

                    m_pGameInstance->Set_LightEnable(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP), false);

                } SAMELINE;
                if (0 != m_LightTags.size() && ImGui::Button("REVERSE ON/OFF"))
                {
                    _uint iSize = m_LightTags.size();

                    for (_uint i = 0; i < iSize; ++i)
                    {
                        wstring strLightTag = AnsiToWString(m_LightTags[i]);

                        _bool isAble = m_pGameInstance->Is_LightEnable(strLightTag, ENUM_CLASS(LEVEL::MAP));

                        if (true == isAble)
                            m_pGameInstance->Set_LightEnable(strLightTag, ENUM_CLASS(LEVEL::MAP), false);
                        else
                            m_pGameInstance->Set_LightEnable(strLightTag, ENUM_CLASS(LEVEL::MAP), true);
                    }

                } SEPARATOR;
				if (ImGui::Button("ADD LIGHT"))
				{
					m_isAddLight = !m_isAddLight;
					m_isFixLight = false;
					m_LightDesc.eType = LIGHT_DESC::END;
				} SAMELINE;
				if (ImGui::Button("FIX LIGHT"))
				{
                    m_OriginalLightDesc = *m_pGameInstance->Get_LightDesc(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP));

					m_isFixLight = !m_isFixLight;
					m_isAddLight = false;
					m_LightDesc.eType = LIGHT_DESC::END;
				} SAMELINE;
                if (ImGui::Button("MOVE TO LIGHT"))
                {
                    // LIGHT_DESC FindLightDesc = *m_pGameInstance->Get_LightDesc(AnsiToWString(m_LightTags[m_iLightTagIndex]), ENUM_CLASS(LEVEL::MAP));
                    // 
                    // if (LIGHT_DESC::TYPE::POINT == FindLightDesc.eType)
                    // {
                    //     static_cast<CCamera_Map*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Map_Camera")))->Set_MapCameraPosition(FindLightDesc.vPosition);
                    // }
                    
                }
                if (ImGui::Button("FIX BATCH POINT LIGHT"))
                {
                    m_isFixBatchLight = !m_isFixBatchLight;
                }

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

                                CMap_Light::MAP_LIGHT_DESC MapLightDesc = {};

                                MapLightDesc.pLightDesc = m_pGameInstance->Get_LightDesc(AnsiToWString(m_strLightTag), ENUM_CLASS(LEVEL::MAP));

                                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_PointLight"), ENUM_CLASS(LEVEL::MAP),
                                    TEXT("Prototype_GameObject_Prop_Light"), TIME_CHANNEL::WORLD, &MapLightDesc), false);

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
#endif // _DEBUG

#pragma endregion

	return S_OK;
}

HRESULT CLevel_Map::Ready_Object_SaveLoad_Window()
{
#pragma region WIDGET : OBJECT SAVE 윈도우

#ifdef _DEBUG
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
                    Monster_Save_Json_ByLevel(i);
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


#pragma region 데칼 일괄 저장

                if (false == Decals_Save_Binary())
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
#endif // _DEBUG

#pragma endregion

#pragma region WIDGET : OBJECT LOAD 윈도우

#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (true == m_isLoadObjectWindow)
		{
			ImGui::Begin("OBJECT LOAD WINDOW", &m_isLoadObjectWindow, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("MAP INFO LOAD FILE PATH : "); ITEMWIDTH(350.f);
			ImGui::InputText("##map_info_load_path", m_szMapInfoFilePath, IM_ARRAYSIZE(m_szMapInfoFilePath));
			ImGui::Text("MAP INFO FILE NAME : "); ITEMWIDTH(350.f);
			ImGui::InputText("##map_info_load_name", m_szMapInfoFileName, IM_ARRAYSIZE(m_szMapInfoFileName));

			SEPARATOR;

			if (false == m_isLoaded)
			{
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

#pragma region 데칼 일괄 불러오기

                if (false == Decals_Load_Binary())
                {
#ifdef _DEBUG
                    OutputDebugStringA("데칼 정보 바이너리 불러오기 실패");
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
			}

            if (ImGui::Button("TRIGGER LOAD"))
            {
                m_strMapInfoFilePath = m_szMapInfoFilePath;
                m_strMapInfoFilePath += m_szMapInfoFileName;

                Trigger_objects_Load_Json();
            }

            if (ImGui::Button("MONSTER LOAD"))
            {
                m_strMapInfoFilePath = m_szMapInfoFilePath;
                m_strMapInfoFilePath += m_szMapInfoFileName;

                Monster_objects_Load_Json();
            }

            if (ImGui::Button("LIGHT LOAD"))
            {
                m_strMapInfoFilePath = m_szMapInfoFilePath;
                m_strMapInfoFilePath += m_szMapInfoFileName;

                Lights_Load_Binary();
            }

			ImGui::End();
		}
		});
#endif // _DEBUG

#pragma endregion

	return S_OK;
}

HRESULT CLevel_Map::Ready_SkySphere_Window()
{
	m_pSkySphere = static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Sky"), 0));
	CHECK_NULLPTR(m_pSkySphere, E_FAIL);

	m_pCloudSphere = static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_Sky"), 1));
	CHECK_NULLPTR(m_pCloudSphere, E_FAIL);

	m_FixSkyDesc = m_pSkySphere->Get_SkyDesc();

	m_FixCloudDesc = m_pCloudSphere->Get_CloudDesc();

#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isSkySphereWindow)
		{
			ImGui::Begin("SKY SPHERE WINDOW", &m_isSkySphereWindow, ImGuiWindowFlags_AlwaysAutoResize);

			_float fMoonIntensity = m_FixSkyDesc.fMoonIntensity;
			_float fMoonSize = m_FixSkyDesc.fMoonSize;
			_float fStarStrength = m_FixSkyDesc.fStarStrength;
			_float3 vMoonColor = m_FixSkyDesc.vMoonColor;
			_float3 vMoonDirection = m_FixSkyDesc.vMoonDirection;
			_float3 vNebulaColorR = m_FixSkyDesc.vNebulaColorR;
			_float3 vNebulaColorG = m_FixSkyDesc.vNebulaColorG;
			_float3 vNebulaColorB = m_FixSkyDesc.vNebulaColorB;

			ImGui::Text("SKY COLOR");
			ImGui::Text("COLOR PALHETT");
			ImGui::ColorPicker3("##r_edit", reinterpret_cast<_float*>(&m_FixSkyDesc.vNebulaColorR));
			SEPARATOR;

			ImGui::Text("MOON SIZE"); SAMELINE;
			ImGui::InputFloat("##fix_moon_size", &m_FixSkyDesc.fMoonSize, 0.01f, 0.1f);
			SEPARATOR;

			ImGui::Text("MOON COLOR");
			ImGui::Text("COLOR PALHETT");
			ImGui::ColorPicker3("##moon_color_edit", reinterpret_cast<_float*>(&m_FixSkyDesc.vMoonColor));
			SEPARATOR;

			ImGui::Text("MOON INTENSITY"); SAMELINE;
			ImGui::InputFloat("##fix_moon_inten", &m_FixSkyDesc.fMoonIntensity, 0.01f, 0.1f);
			SEPARATOR;

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

	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isCloudSphereWindow)
		{
			ImGui::Begin("CLOUD SPHERE WINDOW", &m_isCloudSphereWindow, ImGuiWindowFlags_AlwaysAutoResize);

			_float fCloudDensity = m_FixCloudDesc.fCloudDensity;	//
			_float fCloudLightIntensity = m_FixCloudDesc.fCloudLightIntensity;		//
			_float fCloudScale = m_FixCloudDesc.fCloudScale;		//
			_float fCloudSpeed = m_FixCloudDesc.fCloudSpeed;		//
			_float3 vCloudColor = m_FixCloudDesc.vCloudColor;		//
			_float3 vLightDir = m_FixCloudDesc.vLightDir;			//
			_float isDynamic = m_FixCloudDesc.fDynamic;
			 
			ImGui::Text("CLOUD COLOR");
            ImGui::Text("COLOR PALHETT");
            ImGui::ColorPicker3("##r_edit", reinterpret_cast<_float*>(&m_FixCloudDesc.vCloudColor.x));
			SEPARATOR;

			ImGui::Text("CLOUD SCALE"); SAMELINE;
			ImGui::InputFloat("##fix_cloud_scale", &m_FixCloudDesc.fCloudScale, 0.01f, 0.1f);
			SEPARATOR;

			ImGui::Text("CLOUD SPEED"); SAMELINE;
			ImGui::InputFloat("##fix_cloud_speed", &m_FixCloudDesc.fCloudSpeed, 0.01f, 0.1f);
			SEPARATOR;

			ImGui::Text("CLOUD DENSITY"); SAMELINE;
			ImGui::InputFloat("##fix_cloud_density", &m_FixCloudDesc.fCloudDensity, 0.01f, 0.1f);
			SEPARATOR;

			ImGui::Text("CLOUD INTENSITY"); SAMELINE;
			ImGui::InputFloat("##fix_cloud_inten", &m_FixCloudDesc.fCloudLightIntensity, 0.01f, 0.1f);
			SEPARATOR;

			ImGui::Text("LIGHT DIRECTION");
			ImGui::Text("X"); SAMELINE;
			ImGui::InputFloat("##fix_cloudlight_dir_X", &m_FixCloudDesc.vLightDir.x, 0.01f, 0.1f);
			ImGui::Text("Y"); SAMELINE;
			ImGui::InputFloat("##fix_cloudlight_dir_Y", &m_FixCloudDesc.vLightDir.y, 0.01f, 0.1f);
			ImGui::Text("Z"); SAMELINE;
			ImGui::InputFloat("##fix_cloudlight_dir_Z", &m_FixCloudDesc.vLightDir.z, 0.01f, 0.1f);
			SEPARATOR;

			ImGui::Text("CLOUD DYNAMIC"); SAMELINE;
			if (ImGui::Button("ON"))
				m_FixCloudDesc.fDynamic = 1.f;
			SAMELINE;
			if (ImGui::Button("OFF"))
				m_FixCloudDesc.fDynamic = 0.f;

			m_pCloudSphere->Set_CloudDesc(m_FixCloudDesc);

			SEPARATOR;
			ImGui::Text("PATH : %s", m_szMapInfoFilePath);
			ImGui::Text("CLOUD SPHERE SAVE FILE : "); SAMELINE;
			ImGui::InputText("##sky_file_name", m_szMapInfoFileName, IM_ARRAYSIZE(m_szMapInfoFileName));

			SEPARATOR;
			if (ImGui::Button("SAVE"))
			{
				string strPath = m_szMapInfoFilePath;
				strPath += m_szMapInfoFileName;
				strPath += "_cloud.dat";

				DWORD dwByte = {};

				HANDLE hFile = CreateFile(AnsiToWString(strPath).c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (INVALID_HANDLE_VALUE == hFile)
				{
					_int a = 10;
				}
				else
				{
					WriteFile(hFile, &m_FixCloudDesc, sizeof(CLOUD_DESC), &dwByte, nullptr);
				}

				CloseHandle(hFile);

			} SAMELINE;
			if (ImGui::Button("LOAD"))
			{
				string strPath = m_szMapInfoFilePath;
				strPath += m_szMapInfoFileName;
				strPath += "_cloud.dat";

				DWORD dwByte = {};

				HANDLE hFile = CreateFile(AnsiToWString(strPath).c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (INVALID_HANDLE_VALUE == hFile)
				{
					_int a = 10;
				}
				else
				{
					ReadFile(hFile, &m_FixCloudDesc, sizeof(CLOUD_DESC), &dwByte, nullptr);
				}

				CloseHandle(hFile);
			}

			ImGui::End();
		}
		});
#endif // _DEBUG

	return S_OK;
}

HRESULT CLevel_Map::Ready_MultiFix_Window()
{
#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
		if (m_isMultiFixWindow && nullptr != m_pParentFixObject)
		{
			ImGui::Begin("MULTI FIX WINDOW", &m_isMultiFixWindow, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("MULTI FIX LIST ( 0 == PARENT )");

			if (ImGui::BeginListBox("##multifix_list"))
			{
				_wstring strFixModelName = {};
				string strFixModel = {};

				for (_uint i = 0; i < m_MultiFixList.size(); ++i)
				{
					_bool isSelected = (m_iMultiFixIndex == i);

					strFixModelName = m_MultiFixList[i]->Get_ModelName();
					strFixModel = WStringToAnsi(strFixModelName);

					if (ImGui::Selectable(strFixModel.c_str(), isSelected))
						m_iMultiFixIndex = i;
				}

				ImGui::EndListBox();
			} SEPARATOR;

			ImGui::Text("LIST COUNT : %d", m_MultiFixList.size());
			SEPARATOR;

			if (ImGui::Button("FIX"))
			{
				if (m_MultiFixList.size() > 1)
				{
					m_isMultiFix = true;

					m_pGameInstance->Set_GizmoObject(m_pParentFixObject);

					CTransform* pParentTransform = static_cast<CTransform*>(m_pParentFixObject->Get_Component(TEXT("Com_Transform")));
					XMMATRIX matParent = pParentTransform->Get_WorldMatrix();
					XMMATRIX invParent = XMMatrixInverse(nullptr, matParent);

					XMStoreFloat4x4(&m_matOriginalParentMatrix, matParent);

					m_MultiFixRelatives.clear();

					for (size_t i = 1; i < m_MultiFixList.size(); ++i)
					{
						CTransform* pChildTransform = static_cast<CTransform*>(m_MultiFixList[i]->Get_Component(TEXT("Com_Transform")));
						if (pChildTransform == nullptr)
							continue;

						XMMATRIX matChild = pChildTransform->Get_WorldMatrix();
						XMMATRIX matRelative = matChild * invParent;

						FIX_RELATIVE_DESC tDesc{};
						tDesc.pTransform = pChildTransform;
						XMStoreFloat4x4(&tDesc.RelativeMatrix, matRelative);

						m_MultiFixRelatives.push_back(tDesc);
					}

					// 부모 행렬 저장 (변화 감지용)
					XMStoreFloat4x4(&m_matParentBefore, matParent);

					OutputDebugString(TEXT("[MultiFix] Fixed relative transforms.\n"));
				}
			}
            if (ImGui::Button("RESET"))
            {
                static_cast<CTransform*>(m_pParentFixObject->Get_Component(TEXT("Com_Transform")))->Set_WorldMatrix(XMLoadFloat4x4(&m_matOriginalParentMatrix));
            } SAMELINE;
            if (ImGui::Button("COPY"))
            {
                if (m_MultiFixList.empty())
                    return;

                vector<CProp*> CopyList;
                CopyList.reserve(m_MultiFixList.size());

                // 1. 현재 MultiFix 리스트의 오브젝트들을 그대로 복제
                for (CProp* pProp : m_MultiFixList)
                {
                    CHECK_NULLPTR(pProp, );

                    CTransform* pTransform = static_cast<CTransform*>(
                        pProp->Get_Component(TEXT("Com_Transform")));
                    CHECK_NULLPTR(pTransform, );

                    CProp_Object::PROP_OBJECT_DESC ObjectDesc = {};

                    ObjectDesc.iMapObjectID = m_iMapObjectCnt++;
                    ObjectDesc.eLevel = LEVEL::MAP;

                    memcpy(ObjectDesc.szModelName, pProp->Get_ModelName(), sizeof(ObjectDesc.szModelName));

                    ObjectDesc.WorldMatrix = *pTransform->Get_WorldMatrixPtr();

                    ObjectDesc.Properties = pProp->Get_Properties();
                    ObjectDesc.iSubLevel = pProp->Get_SubLevel();

                    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(
                        ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj"),
                        ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"),
                        TIME_CHANNEL::WORLD, &ObjectDesc), );

                    CProp* pObject_Prop =
                        static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(
                            ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj")));
                    CHECK_NULLPTR_MSG(pObject_Prop, TEXT("엥"), );

                    m_ObjectList.push_back(pObject_Prop);
                    CopyList.push_back(pObject_Prop);
                }

                // 2. 기존 MultiFix 리스트를 새 복제 리스트로 교체
                m_MultiFixList.swap(CopyList);

                // 3. 새 그룹의 첫 번째를 부모로 지정
                if (!m_MultiFixList.empty())
                {
                    m_pParentFixObject = m_MultiFixList.front();
                    m_iMultiFixIndex = 0;
                }
                else
                {
                    m_pParentFixObject = nullptr;
                    m_iMultiFixIndex = 0;
                    m_MultiFixRelatives.clear();
                    m_isMultiFix = false;
                    m_pGameInstance->Clear_GizmoObject();
                    return;
                }

                // 4. 새 부모 기준으로 다시 FIX 상태 세팅 (자동 FIX)
                CTransform* pParentTransform =
                    static_cast<CTransform*>(m_pParentFixObject->Get_Component(TEXT("Com_Transform")));
                CHECK_NULLPTR(pParentTransform, );

                XMMATRIX matParent = pParentTransform->Get_WorldMatrix();
                XMMATRIX invParent = XMMatrixInverse(nullptr, matParent);

                XMStoreFloat4x4(&m_matOriginalParentMatrix, matParent);
                XMStoreFloat4x4(&m_matParentBefore, matParent);

                m_MultiFixRelatives.clear();

                for (size_t i = 1; i < m_MultiFixList.size(); ++i)
                {
                    CTransform* pChildTransform =
                        static_cast<CTransform*>(m_MultiFixList[i]->Get_Component(TEXT("Com_Transform")));
                    if (pChildTransform == nullptr)
                        continue;

                    XMMATRIX matChild = pChildTransform->Get_WorldMatrix();
                    XMMATRIX matRelative = matChild * invParent;

                    FIX_RELATIVE_DESC tDesc{};
                    tDesc.pTransform = pChildTransform;
                    XMStoreFloat4x4(&tDesc.RelativeMatrix, matRelative);

                    m_MultiFixRelatives.push_back(tDesc);
                }

                m_isMultiFix = true;
                m_pGameInstance->Set_GizmoObject(m_pParentFixObject);
            }
			if (ImGui::Button("DONE"))
			{
				m_pGameInstance->Clear_GizmoObject();

				m_isMultiFix = false;
				m_MultiFixRelatives.clear();
				m_MultiFixList.clear();
				m_iMultiFixIndex = 0;
				m_pParentFixObject = nullptr;
			}

			ImGui::End();
		}
		});

#endif // _DEBUG

	return S_OK;
}

HRESULT CLevel_Map::Ready_Map_Decal_Window()
{
    m_pDecalTexture[ENUM_CLASS(DECALTYPE::LINEAR)] = m_pGameInstance->Get_DecalTexture(DECALTYPE::LINEAR);
    m_pDecalTexture[ENUM_CLASS(DECALTYPE::CURVE)] = m_pGameInstance->Get_DecalTexture(DECALTYPE::CURVE);
    m_pDecalTexture[ENUM_CLASS(DECALTYPE::CIRCLE)] = m_pGameInstance->Get_DecalTexture(DECALTYPE::CIRCLE);

#ifdef _DEBUG
    m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
        if (m_isDecalWindow)
        {
            ImGui::Begin("DECAL WINDOW", &m_isDecalWindow, ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::Text("LINEAR LIST");

            for (_uint i = 0; i < m_pDecalTexture[ENUM_CLASS(DECALTYPE::LINEAR)]->Get_NumTextures(); ++i)
            {
                ImTextureID pTexture = (ImTextureID)m_pDecalTexture[ENUM_CLASS(DECALTYPE::LINEAR)]->Get_Texture(i);
                ImVec2 vSize = ImVec2(75.f, 75.f);

                if (ImGui::ImageButton(pTexture, vSize))
                {
                    m_iDecalTextureIndex = i;
                    m_eDecalType = DECALTYPE::LINEAR;

                    m_iTextureIndex = i;
                }
                if (0 == i || 0 != i % 4)
                    SAMELINE;
            }
            SEPARATOR;

            ImGui::Text("CIRCLE LIST");
            for (_uint i = 0; i < m_pDecalTexture[ENUM_CLASS(DECALTYPE::CIRCLE)]->Get_NumTextures(); ++i)
            {
                ImTextureID pTexture = (ImTextureID)m_pDecalTexture[ENUM_CLASS(DECALTYPE::CIRCLE)]->Get_Texture(i);
                ImVec2 vSize = ImVec2(75.f, 75.f);

                if (ImGui::ImageButton(pTexture, vSize))
                {
                    m_iDecalTextureIndex = i;
                    m_eDecalType = DECALTYPE::CIRCLE;

                    m_iTextureIndex = i;
                }
                if (0 == i || 0 != i % 4)
                    SAMELINE;
            }
            SEPARATOR;

            ImGui::Text("CURVE LIST");
            for (_uint i = 0; i < m_pDecalTexture[ENUM_CLASS(DECALTYPE::CURVE)]->Get_NumTextures(); ++i)
            {
                ImTextureID pTexture = (ImTextureID)m_pDecalTexture[ENUM_CLASS(DECALTYPE::CURVE)]->Get_Texture(i);
                ImVec2 vSize = ImVec2(75.f, 75.f);

                if (ImGui::ImageButton(pTexture, vSize))
                {
                    m_iDecalTextureIndex = i;
                    m_eDecalType = DECALTYPE::CURVE;

                    m_iTextureIndex = i;
                }
                if (0 == i || 0 != i % 4)
                    SAMELINE;
            }

            SEPARATOR;

            m_DecalDesc.eType = m_eDecalType;

            ImGui::ColorPicker3("##decal_color_picker", reinterpret_cast<_float*>(&m_DecalDesc.vColor));
            m_DecalDesc.vPosition = m_vPickedPos;
            m_DecalDesc.vScale = _float3(1.f, 1.f, 1.f);

            ImGui::Text("SELECT MASK COLOR"); SAMELINE;
            if (ImGui::Button("RED")) m_fDecalThreshold = 0.f; SAMELINE;
            if (ImGui::Button("GREEN")) m_fDecalThreshold = 0.5f; SAMELINE;
            if (ImGui::Button("BLUE")) m_fDecalThreshold = 1.f;

            SEPARATOR;
            if (ImGui::Button("ADD"))
            {
                if (0 == m_DecalList.size()) m_isDecalListWindow = true;

                CDecal* pDecal = static_cast<CDecal*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Decal")));
                CHECK_NULLPTR(pDecal, false);

                pDecal->Set_EnableDecoration(true);

                pDecal->Set_Desc(m_DecalDesc);

                pDecal->Set_Threshold(m_fDecalThreshold);

                pDecal->Set_TextureIndex(m_iTextureIndex);

                m_DecalList.push_back(pDecal);
                m_pGameInstance->Batch_Decal(pDecal);

                m_iDecalListIndex = m_DecalList.size() - 1;

                m_pFixDecal = pDecal;

                m_isDecalFixWindow = true;

                m_pGameInstance->Set_GizmoObject(m_pFixDecal);
            }

            ImGui::End();
        }
        });

    m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
        if (m_isDecalListWindow)
        {
            ImGui::Begin("DECAL LIST WINDOW", &m_isDecalListWindow, ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::Text("DECAL LIST");

            if (false == m_DecalList.empty())
            {
                if (ImGui::BeginListBox("##decal_list"))
                {
                    for (_uint i = 0; i < m_DecalList.size(); ++i)
                    {
                        _char szDecalName[MAX_PATH] = {};

                        sprintf_s(szDecalName, "%d_DECAL", i);

                        _bool isSelected = (m_iDecalListIndex == i);

                        if (ImGui::Selectable(szDecalName, isSelected))
                            m_iDecalListIndex = i;
                    }

                    ImGui::EndListBox();
                } SEPARATOR;
            }
            if (ImGui::Checkbox("WIREFRAME", &m_isDecalWireFrame))
            {
                for (auto& pDecal : m_DecalList)
                    pDecal->Set_WireFrame(m_isDecalWireFrame);
            }
            if (ImGui::Button("FIX"))
            {
                if (0 < m_DecalList.size())
                {
                    m_pFixDecal = m_DecalList[m_iDecalListIndex];

                    m_pGameInstance->Set_GizmoObject(m_pFixDecal);

                    m_FixDecalDesc = m_DecalDesc = m_pFixDecal->Get_Desc();

                    m_fDecalThreshold = m_pFixDecal->Get_Threshold();

                    m_DecalWorldMatrix = *static_cast<CTransform*>(m_pFixDecal->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr();

                    m_isDecalFixWindow = true;
                }
            } SAMELINE;
            if (ImGui::Button("DELETE"))
            {
                if (0 < m_DecalList.size())
                {
                    if (nullptr != m_DecalList[m_iDecalListIndex])
                    {
                        m_DecalList[m_iDecalListIndex]->Set_IsDead(true);
                        m_DecalList[m_iDecalListIndex]->Set_IsPool(true);

                        for (_uint i = 0; i < m_DecalList.size(); )
                        {
                            if (m_DecalList[m_iDecalListIndex] == m_DecalList[i])
                            {
                                swap(m_DecalList[m_iDecalListIndex], m_DecalList.back());
                                m_DecalList.pop_back();
                                break;
                            }
                            else
                                ++i;
                        }

                        if (m_iDecalListIndex >= m_DecalList.size())
                            m_iDecalListIndex = m_DecalList.size() - 1;

                        m_pFixDecal = nullptr;
                    }

                    m_isDecalFixWindow = false;
                }
            }

            ImGui::End();
        }
        });

    m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
            if (m_isDecalFixWindow)
            {
                ImGui::Begin("DECAL FIX WINDOW", &m_isDecalFixWindow, ImGuiWindowFlags_AlwaysAutoResize);

                ImGui::Text("FIX DECAL COLOR");
                ImGui::ColorPicker3("fix_decal_color", reinterpret_cast<_float*>(&m_FixDecalDesc.vColor));

                m_pFixDecal->Set_DecalColor(m_FixDecalDesc.vColor);
                SEPARATOR;

                ImGui::Text("SELECT MASK COLOR"); SAMELINE;
                if (ImGui::Button("RED")) m_fDecalThreshold = 0.f; SAMELINE;
                if (ImGui::Button("GREEN")) m_fDecalThreshold = 0.5f; SAMELINE;
                if (ImGui::Button("BLUE")) m_fDecalThreshold = 1.f;
                SEPARATOR;

                m_pFixDecal->Set_Threshold(m_fDecalThreshold);

                if (ImGui::Button("DONE ( ENTER or MOUSE RB )") || m_pGameInstance->Key_Down(DIK_RETURN) || m_pGameInstance->Key_Down(DIK_NUMPADENTER) || m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
                {
                    m_pGameInstance->Clear_GizmoObject();

                    ZeroMemory(&m_DecalDesc, sizeof(DECAL_DESC));

                    m_isDecalFixWindow = false;
                } SAMELINE;
                if (ImGui::Button("RESET (R)") || (true == m_isActiveShortCutKey && m_pGameInstance->Key_Down(DIK_R)))
                {
                    m_pGameInstance->Clear_GizmoObject();

                    m_pFixDecal->Set_Desc(m_DecalDesc);
                    static_cast<CTransform*>(m_pFixDecal->Get_Component(TEXT("Com_Transform")))->Set_WorldMatrix_4x4(m_DecalWorldMatrix);

                    m_pFixDecal = nullptr;
                    m_isDecalFixWindow = false;
                }
                if (ImGui::Button("DELETE"))
                {
                    m_pGameInstance->Clear_GizmoObject();

                    if (0 < m_DecalList.size())
                    {
                        if (nullptr != m_pFixDecal)
                        {
                            m_pFixDecal->Set_IsDead(true);
                            m_pFixDecal->Set_IsPool(true);

                            for (_uint i = 0; i < m_DecalList.size(); )
                            {
                                if (m_pFixDecal == m_DecalList[i])
                                {
                                    swap(m_pFixDecal, m_DecalList.back());
                                    m_DecalList.pop_back();
                                    break;
                                }
                                else
                                    ++i;
                            }

                            if (m_iDecalListIndex >= m_DecalList.size())
                                m_iDecalListIndex = m_DecalList.size() - 1;

                            m_pFixDecal = nullptr;
                        }
                    }

                    m_isDecalFixWindow = false;
                }

                ImGui::End();
            }
            });
#endif // _DEBUG

    return S_OK;
}

HRESULT CLevel_Map::Ready_OnOff_Window()
{
    m_pGameInstance->AddWidget(TEXT("Map"), [this]() {
        if (m_isOnOffWindow)
        {
            ImGui::Begin("ON/OFF WINDOW", &m_isOnOffWindow, ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::Text("EDITOR WINDOWS");
            ImGui::Separator();

            ImGui::Checkbox("PROTOTYPE WINDOW", &m_isPrototypeWindow);
            ImGui::Checkbox("PROTOTYPE INTERACTIVE WINDOW", &m_isPrototypeInteractiveWindow);
            ImGui::Checkbox("OBJECT WINDOW", &m_isObjectWindow);
            ImGui::Checkbox("INTERACTIVE WINDOW", &m_isInteractiveWindow);
            ImGui::Checkbox("LIGHT SETTING WINDOW", &m_isLightSettingWindow);
            ImGui::Checkbox("SAVE OBJECT WINDOW", &m_isSaveObjectWindow);
            ImGui::Checkbox("LOAD OBJECT WINDOW", &m_isLoadObjectWindow);
            ImGui::Checkbox("SKYSPHERE WINDOW", &m_isSkySphereWindow);
            ImGui::Checkbox("CLOUDSPHERE WINDOW", &m_isCloudSphereWindow);
            ImGui::Checkbox("MULTI FIX WINDOW", &m_isMultiFixWindow);
            ImGui::Checkbox("DECAL ADD WINDOW", &m_isDecalWindow);
            ImGui::Checkbox("DECAL LIST WINDOW", &m_isDecalListWindow);

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
	if (".fbx" != strFileExtern && ".dat" != strFileExtern)
		return "NOTFOUND";

	string strModelFilePath = strModelName + strFileExtern;

	auto iter = m_ModelPathCache.find(strModelFilePath);
	if (iter != m_ModelPathCache.end())
		return iter->second;

	return "NOTFOUND";
	/*
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
	*/
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

_bool CLevel_Map::Monster_Save_Json()
{
    _wstring strMonsterFilePath = AnsiToWString(m_strMapInfoFilePath);

    strMonsterFilePath += TEXT("_spawn.json");

    _uint iMonsterCnt = { 0 };

    JSON_MAP_MONSTER_SPAWN_DATA MonsterJson = {};

    // 트리거 야매 ( 가져오깅
    for (auto& pProp : m_InteractiveList)
    {
        if (INTERACTIVE_TYPE::SPAWN == pProp->Get_InteractiveType())
        {
            CTransform* pTransform = static_cast<CTransform*>(pProp->Get_Component(TEXT("Com_Transform")));

            pTransform->Scale(_float3(1.f, 1.f, 1.f));
            _float4x4 WorldMatrix = *pTransform->Get_WorldMatrixPtr();
            pTransform->Scale(_float3(0.005f, 0.005f, 0.005f));

            string MonsterKey = static_cast<CMap_Spawn*>(pProp)->Get_MonsterKey();
            _int iSubLevel = static_cast<CMap_Spawn*>(pProp)->Get_SubLevel();

            FLOAT4X4_DATA matWorldData = {};
            memcpy(&matWorldData, &WorldMatrix, sizeof(_float4x4));

            MonsterJson.WorldMatrix.push_back(matWorldData);
            MonsterJson.MonsterKey.push_back(MonsterKey);
            MonsterJson.SubLevel.push_back(iSubLevel);

            ++iMonsterCnt;
        }
    }

    MonsterJson.iNumMonster = iMonsterCnt;

    JSON j = MonsterJson;

    ofstream ofs(strMonsterFilePath);

    if (!ofs.is_open())
    {
        OutputDebugStringA("몬스터 스폰 Json 파일입출력 실패");
    }

    ofs << j.dump(4);
    ofs.close();

#pragma endregion

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
            // 트리거 타입일 경우 컨티뉴
            if (INTERACTIVE_TYPE::TRIGGER == pProp->Get_InteractiveType())
                continue;
            // 몬스터 타입일 경우 컨티뉴
            if (INTERACTIVE_TYPE::SPAWN == pProp->Get_InteractiveType())
                continue;

			// 현재 등록되어있는 상호 작용 객체 카운트 증가
			++iObjectCnt;
		}

		// 1. 오브젝트의 총 개수 저장
		WriteFile(hObjectFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr);

		// 단일 오브젝트 순회하면서 모델 이름 알아오기 ( Prototype 태그로 사용할 것 ) ( 상호작용은 Prototype_Component_Model_귀검, 상자, 이런식으로 간단하게 갈것 )
		for (auto& pProp : m_InteractiveList)
		{
            // 트리거 타입 일 경우 패스
            if (INTERACTIVE_TYPE::TRIGGER == pProp->Get_InteractiveType())
                continue;
            // 몬스터 타입 일 경우 패스
            if (INTERACTIVE_TYPE::SPAWN == pProp->Get_InteractiveType())
                continue;

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

			// 추가적으로 넣어줘야 할 게 있는 경우 여기 채우기
			if (INTERACTIVE_TYPE::CHECKPOINT == eType)
			{
				_int iBladeNexusID = {};
				iBladeNexusID = pProp->Get_BladeNexus_ID();
				WriteFile(hObjectFile, &iBladeNexusID, sizeof(_int), &dwByte, nullptr);
			}
			if (INTERACTIVE_TYPE::CHEST == eType)
			{
				// 6. 아이템 3개 ID 넘기기 ( 구조체 Editor, Client 동일하게 )
				CMapObject::ITEMBOX_DESC ItemBoxDesc = {};
				ItemBoxDesc = pProp->Get_ItemBox();
				WriteFile(hObjectFile, &ItemBoxDesc, sizeof(CMapObject::ITEMBOX_DESC), &dwByte, nullptr);
			}
            if (INTERACTIVE_TYPE::TOMBSTONE == eType)
            {
                _int o_ing = 0;
            }
            if (INTERACTIVE_TYPE::ELEVATOR == eType)
            {
                CElevatorS::ELEVATOR_POS ElevatorPos = {};

                ElevatorPos.vUp = static_cast<CElevatorS*>(pProp)->Get_Elevator_UpPos();
                ElevatorPos.vDown = static_cast<CElevatorS*>(pProp)->Get_Elevator_DownPos();

                WriteFile(hObjectFile, &ElevatorPos, sizeof(CElevatorS::ELEVATOR_POS), &dwByte, nullptr);
            }
            if (INTERACTIVE_TYPE::LEVER == eType ||
                INTERACTIVE_TYPE::GEAR1 == eType)
            {
                _int iEventID = static_cast<CProp_Interactive*>(pProp)->Get_EventID();

                WriteFile(hObjectFile, &iEventID, sizeof(_int), &dwByte, nullptr);
            }
            if (INTERACTIVE_TYPE::GEAR2 == eType)
            {
                CGearGate::DOOR_GEAR_EVENTID EventIDs = { static_cast<CGearGate*>(pProp)->Get_DoorGear_EventID() };

                WriteFile(hObjectFile, &EventIDs, sizeof(CGearGate::DOOR_GEAR_EVENTID), &dwByte, nullptr);
            }
            if (INTERACTIVE_TYPE::STATUE == eType)
            {
                _int iEventID = static_cast<CProp_Interactive*>(pProp)->Get_EventID();

                WriteFile(hObjectFile, &iEventID, sizeof(_int), &dwByte, nullptr);

                CStatue::STATUE_ROTATION StatueRotation = {};
                StatueRotation.iUnLockRotation = static_cast<CStatue*>(pProp)->Get_StatueUnLockRotation();

                WriteFile(hObjectFile, &StatueRotation, sizeof(CStatue::STATUE_ROTATION), &dwByte, nullptr);
            }
            if (INTERACTIVE_TYPE::VERTICALGATE == eType)
            {
                _int iEventID = static_cast<CProp_Interactive*>(pProp)->Get_EventID();

                WriteFile(hObjectFile, &iEventID, sizeof(_int), &dwByte, nullptr);
            }
            if (INTERACTIVE_TYPE::IRONGATE == eType)
            {
                // 철문 일단 공백
            }
            if (INTERACTIVE_TYPE::LADDER == eType)
            {
                CLadder* pLadder = static_cast<CLadder*>(pProp);

                _float fLocalTopHeight = pLadder->Get_TopPosition_Y();

                WriteFile(hObjectFile, &fLocalTopHeight, sizeof(_float), &dwByte, nullptr);

                fLocalTopHeight -= 2.8f;

                _int iSegmentCount = static_cast<_int>(fLocalTopHeight / 0.4f);

                WriteFile(hObjectFile, &iSegmentCount, sizeof(_int), &dwByte, nullptr);
            }
            if (INTERACTIVE_TYPE::UNLOCKGEAR == eType)
            {
                _int iEventID = static_cast<CProp_Interactive*>(pProp)->Get_EventID();

                WriteFile(hObjectFile, &iEventID, sizeof(_int), &dwByte, nullptr);
            }
            if (INTERACTIVE_TYPE::LARGEELEVATOR == eType)
            {
                CElevatorL::LARGE_ELEVATOR_POS ElevatorPos = {};

                ElevatorPos.vUp = static_cast<CElevatorL*>(pProp)->Get_Elevator_UpPos();
                ElevatorPos.vMid = static_cast<CElevatorL*>(pProp)->Get_Elevator_MidPos();
                ElevatorPos.vDown = static_cast<CElevatorL*>(pProp)->Get_Elevator_DownPos();

                WriteFile(hObjectFile, &ElevatorPos, sizeof(CElevatorL::LARGE_ELEVATOR_POS), &dwByte, nullptr);
            }
            if (INTERACTIVE_TYPE::GIANTGATE == eType)
            {
                // 큰 문 일단 공백
            }
            if (INTERACTIVE_TYPE::DAPHRONA == eType ||
                INTERACTIVE_TYPE::DUIMUK == eType ||
                INTERACTIVE_TYPE::DANJIN == eType)
            {
                // NPC 일단 공백
            }
            if (INTERACTIVE_TYPE::DANJINJAR == eType)
            {
                CDanjinJar* pDanjinJar = static_cast<CDanjinJar*>(pProp);

                CDanjinJar::DANJINJAR_TYPE eJarType = { pDanjinJar->Get_DanjinJar_ModelType() };

                WriteFile(hObjectFile, &eJarType, sizeof(CDanjinJar::DANJINJAR_TYPE), &dwByte, nullptr);

                CDanjinJar::DANJINJAR_STEP StepPosition = { pDanjinJar->Get_StepPosition() };

                WriteFile(hObjectFile, &StepPosition, sizeof(CDanjinJar::DANJINJAR_STEP), &dwByte, nullptr);
            }
            if (INTERACTIVE_TYPE::DESTINYSTONE == eType)
            {
                // 귀석 일단 공백
            }
            if (INTERACTIVE_TYPE::DESTRUCTIBLE == eType)
            {
                CDestructible_Prop* pDestructibleProp = static_cast<CDestructible_Prop*>(pProp);

                CDestructible_Prop::MODEL_TYPE eModelType = { pDestructibleProp->Get_Destructible_ModelType() };

                WriteFile(hObjectFile, &eModelType, sizeof(CDestructible_Prop::MODEL_TYPE), &dwByte, nullptr);
            }
		}
	}

	// 프로토타입 핸들 닫기
	CloseHandle(hObjectFile);

	return true;
}

_bool CLevel_Map::Trigger_Save_Json()
{
	_wstring strTriggerFilePath = AnsiToWString(m_strMapInfoFilePath);

	strTriggerFilePath += TEXT("_trigger.json");

	_uint iTriggerCnt = { 0 };

	JSON_MAP_TRIGGER_DATA TriggerJson = {};

	// 트리거 야매 ( 가져오깅
	for (auto& pProp : m_InteractiveList)
	{
		if (INTERACTIVE_TYPE::TRIGGER == pProp->Get_InteractiveType())
		{
			_float4x4 WorldMatrix = *static_cast<CTransform*>(pProp->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr();
			string TriggerKey = static_cast<CTrigger*>(pProp)->Get_TriggerKey();

			FLOAT4X4_DATA matWorldData = {};
			memcpy(&matWorldData, &WorldMatrix, sizeof(_float4x4));

			TriggerJson.WorldMatrix.push_back(matWorldData);
			TriggerJson.TriggerKey.push_back(TriggerKey);

			++iTriggerCnt;
		}
	}

	TriggerJson.iNumTrigger = iTriggerCnt;

	JSON j = TriggerJson;

	ofstream ofs(strTriggerFilePath);

	if (!ofs.is_open())
	{
		OutputDebugStringA("프로토타입 Json 파일입출력 실패");
	}

	ofs << j.dump(4);
	ofs.close();

#pragma endregion

	return true;
}

_bool CLevel_Map::Monster_Save_Json_ByLevel(_uint iLevel)
{
    _wstring strMonsterFilePath = AnsiToWString(m_strMapInfoFilePath);

    _tchar szMonsterLevelInfoPath[MAX_PATH] = {};

    wsprintf(szMonsterLevelInfoPath, TEXT("%s_LV%d_spawn.json"), strMonsterFilePath.c_str(), iLevel);

    strMonsterFilePath = szMonsterLevelInfoPath;

    _int iCheck = {};

    for (auto& pProp : m_InteractiveList)
    {
        if (INTERACTIVE_TYPE::SPAWN == pProp->Get_InteractiveType() && iLevel == pProp->Get_SubLevel())
            ++iCheck;
    }
    CHECK_EQUAL(0, iCheck, true);

    _uint iMonsterCnt = { 0 };

    JSON_MAP_MONSTER_SPAWN_DATA MonsterJson = {};

    // 몬스터 스폰 야매 ( 가져오깅
    for (auto& pProp : m_InteractiveList)
    {
        if (INTERACTIVE_TYPE::SPAWN == pProp->Get_InteractiveType() && iLevel == pProp->Get_SubLevel())
        {
            CTransform* pTransform = static_cast<CTransform*>(pProp->Get_Component(TEXT("Com_Transform")));

            pTransform->Scale(_float3(1.f, 1.f, 1.f));
            _float4x4 WorldMatrix = *pTransform->Get_WorldMatrixPtr();
            pTransform->Scale(_float3(0.005f, 0.005f, 0.005f));

            string MonsterKey = static_cast<CMap_Spawn*>(pProp)->Get_MonsterKey();
            _int iSubLevel = static_cast<CMap_Spawn*>(pProp)->Get_SubLevel();

            FLOAT4X4_DATA matWorldData = {};
            memcpy(&matWorldData, &WorldMatrix, sizeof(_float4x4));

            MonsterJson.WorldMatrix.push_back(matWorldData);
            MonsterJson.MonsterKey.push_back(MonsterKey);
            MonsterJson.SubLevel.push_back(iSubLevel);

            ++iMonsterCnt;
        }
    }

    MonsterJson.iNumMonster = iMonsterCnt;

    JSON j = MonsterJson;

    ofstream ofs(strMonsterFilePath);

    if (!ofs.is_open())
    {
        OutputDebugStringA("몬스터 스폰 Json 파일입출력 실패");
    }

    ofs << j.dump(4);
    ofs.close();

#pragma endregion

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

_bool CLevel_Map::Decals_Save_Binary()
{
    _wstring strDecalInfoPath = AnsiToWString(m_strMapInfoFilePath);

    strDecalInfoPath += TEXT("_decals.dat");

    DWORD dwByte = {};

    // 프로토타입 핸들 개방
    HANDLE hFile = CreateFile(strDecalInfoPath.c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        return false;
    }
    else
    {
        _uint iDecalCnt = {};

        for (auto& pDecal : m_DecalList)
        {
            ++iDecalCnt;
        }

        // 1. 데칼의 총 개수 저장 ( 이만큼 루프 돌릴거 )
        WriteFile(hFile, &iDecalCnt, sizeof(_uint), &dwByte, nullptr);

        for (auto& pDecal : m_DecalList)
        {
            DECAL_DESC DecalDesc = pDecal->Get_Desc();

            // 2. 데칼의 구조체 저장
            WriteFile(hFile, &DecalDesc, sizeof(DECAL_DESC), &dwByte, nullptr);

            _float fThreshold = pDecal->Get_Threshold();

            // 3. 데칼의 쓰레스 홀드 저장 ( 마스크 )
            WriteFile(hFile, &fThreshold, sizeof(_float), &dwByte, nullptr);

            _uint iTextureIndex = pDecal->Get_TextureIndex();

            // 4. 데칼의 텍스쳐 인덱스 저장
            WriteFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);

            _float4x4 WorldMatrix = *static_cast<CTransform*>(pDecal->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr();
            // 5. 데칼의 월드 행렬 저장
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
        }
    }

    // 프로토타입 핸들 닫기
    CloseHandle(hFile);

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

				CHECK_FALSE(ReadFile(hObjectFile, &BladeNexusDesc.iBladeNexus_ID, sizeof(_int), &dwByte, nullptr), false);

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
            else if (INTERACTIVE_TYPE::TOMBSTONE == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CTombStone::TOMBSTONE_DESC TombStoneDesc = {};

                TombStoneDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                TombStoneDesc.eLevel = LEVEL::MAP;
                memcpy(TombStoneDesc.szModelName, TEXT("Prototype_Component_Model_TombStone"), sizeof(TombStoneDesc.szModelName));		// 프로토타입 태그명

                TombStoneDesc.WorldMatrix = WorldMatrix;									// 행렬

                TombStoneDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_TombStone"), TIME_CHANNEL::WORLD, &TombStoneDesc), false);
            }
            else if (INTERACTIVE_TYPE::ELEVATOR == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CElevatorS::SMALL_ELEVATOR_DESC ElevatorDesc = {};

                ElevatorDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                ElevatorDesc.eLevel = LEVEL::MAP;
                memcpy(ElevatorDesc.szModelName, TEXT("Prototype_Component_Model_SmallElevator"), sizeof(ElevatorDesc.szModelName));		// 프로토타입 태그명

                ElevatorDesc.WorldMatrix = WorldMatrix;									// 행렬

                ElevatorDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                // 엘리베이터인 경우 위 위치, 아래 위치 가져오기
                CHECK_FALSE(ReadFile(hObjectFile, &ElevatorDesc.ElevatorPos, sizeof(CElevatorS::ELEVATOR_POS), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_SmallElevator"), TIME_CHANNEL::WORLD, &ElevatorDesc), false);
            }
            else if (INTERACTIVE_TYPE::LEVER == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CLever::LEVER_DESC LeverDesc = {};

                LeverDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                LeverDesc.eLevel = LEVEL::MAP;
                memcpy(LeverDesc.szModelName, TEXT("Prototype_Component_Model_Lever"), sizeof(LeverDesc.szModelName));		// 프로토타입 태그명

                LeverDesc.WorldMatrix = WorldMatrix;									// 행렬

                LeverDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                // 레버나 기어인 경우 이벤트 아이디 가져오기
                CHECK_FALSE(ReadFile(hObjectFile, &LeverDesc.iEventID, sizeof(_int), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Lever"), TIME_CHANNEL::WORLD, &LeverDesc), false);
            }
            else if (INTERACTIVE_TYPE::GEAR1 == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CLever_Gear::LEVER_GEAR_DESC LeverGearDesc = {};

                LeverGearDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                LeverGearDesc.eLevel = LEVEL::MAP;
                memcpy(LeverGearDesc.szModelName, TEXT("Prototype_Component_Model_Lever_Gear"), sizeof(LeverGearDesc.szModelName));		// 프로토타입 태그명

                LeverGearDesc.WorldMatrix = WorldMatrix;									// 행렬

                LeverGearDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                // 레버나 기어인 경우 이벤트 아이디 가져오기
                CHECK_FALSE(ReadFile(hObjectFile, &LeverGearDesc.iEventID, sizeof(_int), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Lever_Gear"), TIME_CHANNEL::WORLD, &LeverGearDesc), false);
            }
            else if (INTERACTIVE_TYPE::GEAR2 == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CGearGate::GEARGATE_DESC GearGateDesc = {};

                GearGateDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                GearGateDesc.eLevel = LEVEL::MAP;
                memcpy(GearGateDesc.szModelName, TEXT("Prototype_Component_Model_GearGate"), sizeof(GearGateDesc.szModelName));		// 프로토타입 태그명

                GearGateDesc.WorldMatrix = WorldMatrix;									// 행렬

                GearGateDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                // 레버나 기어인 경우 이벤트 아이디 가져오기
                CHECK_FALSE(ReadFile(hObjectFile, &GearGateDesc.GearEventID, sizeof(CGearGate::DOOR_GEAR_EVENTID), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_GearGate"), TIME_CHANNEL::WORLD, &GearGateDesc), false);
            }
            else if (INTERACTIVE_TYPE::STATUE == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CStatue::STATUE_DESC StatueDesc = {};

                StatueDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                StatueDesc.eLevel = LEVEL::MAP;
                memcpy(StatueDesc.szModelName, TEXT("Prototype_Component_Model_Statue"), sizeof(StatueDesc.szModelName));		// 프로토타입 태그명

                StatueDesc.WorldMatrix = WorldMatrix;									// 행렬

                StatueDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                // 레버나 기어인 경우 이벤트 아이디 가져오기
                CHECK_FALSE(ReadFile(hObjectFile, &StatueDesc.iEventID, sizeof(_int), &dwByte, nullptr), false);

                CHECK_FALSE(ReadFile(hObjectFile, &StatueDesc.StatueRotation, sizeof(CStatue::STATUE_ROTATION), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Statue"), TIME_CHANNEL::WORLD, &StatueDesc), false);
            }
            else if (INTERACTIVE_TYPE::VERTICALGATE == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CVerticalGate::VERTICAL_GATE_DESC VerticalGateDesc = {};

                VerticalGateDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                VerticalGateDesc.eLevel = LEVEL::MAP;
                memcpy(VerticalGateDesc.szModelName, TEXT("Prototype_Component_Model_VerticalGate"), sizeof(VerticalGateDesc.szModelName));		// 프로토타입 태그명

                VerticalGateDesc.WorldMatrix = WorldMatrix;									// 행렬

                VerticalGateDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                // 레버나 기어인 경우 이벤트 아이디 가져오기
                CHECK_FALSE(ReadFile(hObjectFile, &VerticalGateDesc.iEventID, sizeof(_int), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_VerticalGate"), TIME_CHANNEL::WORLD, &VerticalGateDesc), false);
            }
            else if (INTERACTIVE_TYPE::IRONGATE == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CIronGate::IRONGATE_DESC IronGateDesc = {};

                IronGateDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                IronGateDesc.eLevel = LEVEL::MAP;
                memcpy(IronGateDesc.szModelName, TEXT("Prototype_Component_Model_IronGate"), sizeof(IronGateDesc.szModelName));		// 프로토타입 태그명

                IronGateDesc.WorldMatrix = WorldMatrix;									// 행렬

                IronGateDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_IronGate"), TIME_CHANNEL::WORLD, &IronGateDesc), false);
            }
            else if (INTERACTIVE_TYPE::LADDER == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CLadder::LADDER_DESC LadderDesc = {};

                LadderDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                LadderDesc.eLevel = LEVEL::MAP;
                memcpy(LadderDesc.szModelName, TEXT("Ladder"), sizeof(LadderDesc.szModelName));		// 프로토타입 태그명

                LadderDesc.WorldMatrix = WorldMatrix;									// 행렬

                LadderDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CHECK_FALSE(ReadFile(hObjectFile, &LadderDesc.fOffSetHeight, sizeof(_float), &dwByte, nullptr), false);

                CHECK_FALSE(ReadFile(hObjectFile, &LadderDesc.iSegmentCount, sizeof(_int), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Ladder"), TIME_CHANNEL::WORLD, &LadderDesc), false);
            }
            else if (INTERACTIVE_TYPE::UNLOCKGEAR == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CUnLockGear::UNLOCK_GEAR_DESC UnLockGearDesc = {};

                UnLockGearDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                UnLockGearDesc.eLevel = LEVEL::MAP;
                memcpy(UnLockGearDesc.szModelName, TEXT("Prototype_Component_Model_UnLockGear"), sizeof(UnLockGearDesc.szModelName));		// 프로토타입 태그명

                UnLockGearDesc.WorldMatrix = WorldMatrix;									// 행렬

                UnLockGearDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CHECK_FALSE(ReadFile(hObjectFile, &UnLockGearDesc.iEventID, sizeof(_int), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_UnLockGear"), TIME_CHANNEL::WORLD, &UnLockGearDesc), false);
            }
            else if (INTERACTIVE_TYPE::LARGEELEVATOR == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CElevatorL::LARGE_ELEVATOR_DESC ElevatorDesc = {};

                ElevatorDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                ElevatorDesc.eLevel = LEVEL::MAP;
                memcpy(ElevatorDesc.szModelName, TEXT("Prototype_Component_Model_LargeElevator"), sizeof(ElevatorDesc.szModelName));		// 프로토타입 태그명

                ElevatorDesc.WorldMatrix = WorldMatrix;									// 행렬

                ElevatorDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                // 엘리베이터인 경우 위 위치, 아래 위치 가져오기
                CHECK_FALSE(ReadFile(hObjectFile, &ElevatorDesc.ElevatorPos, sizeof(CElevatorL::LARGE_ELEVATOR_POS), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_LargeElevator"), TIME_CHANNEL::WORLD, &ElevatorDesc), false);
            }
            else if (INTERACTIVE_TYPE::GIANTGATE == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CGiantGate::GIANTGATE_DESC GiantGateDesc = {};

                GiantGateDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                GiantGateDesc.eLevel = LEVEL::MAP;
                memcpy(GiantGateDesc.szModelName, TEXT("Prototype_Component_Model_GiantGate"), sizeof(GiantGateDesc.szModelName));		// 프로토타입 태그명

                GiantGateDesc.WorldMatrix = WorldMatrix;									// 행렬

                GiantGateDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_GiantGate"), TIME_CHANNEL::WORLD, &GiantGateDesc), false);
            }
            else if (INTERACTIVE_TYPE::DAPHRONA == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CNPC_Daphrona::DAPHRONA_DESC DaphronaDesc = {};

                DaphronaDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                DaphronaDesc.eLevel = LEVEL::MAP;
                memcpy(DaphronaDesc.szModelName, TEXT("Prototype_Component_Model_NPC_Daphrona"), sizeof(DaphronaDesc.szModelName));		// 프로토타입 태그명

                DaphronaDesc.WorldMatrix = WorldMatrix;									// 행렬

                DaphronaDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_NPC_Daphrona"), TIME_CHANNEL::WORLD, &DaphronaDesc), false);
            }
            else if (INTERACTIVE_TYPE::DUIMUK == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CNPC_Duimuk::DUIMUK_DESC DuimukDesc = {};

                DuimukDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                DuimukDesc.eLevel = LEVEL::MAP;
                memcpy(DuimukDesc.szModelName, TEXT("Prototype_Component_Model_NPC_Duimuk"), sizeof(DuimukDesc.szModelName));		// 프로토타입 태그명

                DuimukDesc.WorldMatrix = WorldMatrix;									// 행렬

                DuimukDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_NPC_Duimuk"), TIME_CHANNEL::WORLD, &DuimukDesc), false);
            }
            else if (INTERACTIVE_TYPE::DANJIN == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CNPC_Danjin::DANJIN_DESC DanjinDesc = {};

                DanjinDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                DanjinDesc.eLevel = LEVEL::MAP;
                memcpy(DanjinDesc.szModelName, TEXT("Prototype_Component_Model_NPC_Danjin"), sizeof(DanjinDesc.szModelName));		// 프로토타입 태그명

                DanjinDesc.WorldMatrix = WorldMatrix;									// 행렬

                DanjinDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_NPC_Danjin"), TIME_CHANNEL::WORLD, &DanjinDesc), false);
            }
            else if (INTERACTIVE_TYPE::DANJINJAR == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CDanjinJar::DANJINJAR_DESC DanjinJarDesc = {};

                DanjinJarDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                DanjinJarDesc.eLevel = LEVEL::MAP;
                memcpy(DanjinJarDesc.szModelName, TEXT("DanjinJar"), sizeof(DanjinJarDesc.szModelName));		// 프로토타입 태그명

                DanjinJarDesc.WorldMatrix = WorldMatrix;									// 행렬

                DanjinJarDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CDanjinJar::DANJINJAR_TYPE eJarType = {};

                CHECK_FALSE(ReadFile(hObjectFile, &DanjinJarDesc.eJarType, sizeof(CDanjinJar::DANJINJAR_TYPE), &dwByte, nullptr), false);

                CDanjinJar::DANJINJAR_STEP StepPosition = {};

                CHECK_FALSE(ReadFile(hObjectFile, &DanjinJarDesc.StepPosition, sizeof(CDanjinJar::DANJINJAR_STEP), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_DanjinJar"), TIME_CHANNEL::WORLD, &DanjinJarDesc), false);
            }
            else if (INTERACTIVE_TYPE::DESTINYSTONE == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CDestinyStone::DESTINYSTONE_DESC DestinyStoneDesc = {};

                DestinyStoneDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                DestinyStoneDesc.eLevel = LEVEL::MAP;
                memcpy(DestinyStoneDesc.szModelName, TEXT("Prototype_Component_Model_DestinyStone"), sizeof(DestinyStoneDesc.szModelName));		// 프로토타입 태그명

                DestinyStoneDesc.WorldMatrix = WorldMatrix;									// 행렬

                DestinyStoneDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_DestinyStone"), TIME_CHANNEL::WORLD, &DestinyStoneDesc), false);
            }
            else if (INTERACTIVE_TYPE::DESTRUCTIBLE == eType) // 상호작용 계속 추가 예정 ( 이 함수 위쪽도 )
            {
                CDestructible_Prop::DESTRUCTIBLE_PROP_DESC DestructiblePropDesc = {};

                DestructiblePropDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
                DestructiblePropDesc.eLevel = LEVEL::MAP;
                memcpy(DestructiblePropDesc.szModelName, TEXT("DestructibleProp"), sizeof(DestructiblePropDesc.szModelName));		// 프로토타입 태그명

                DestructiblePropDesc.WorldMatrix = WorldMatrix;									// 행렬

                DestructiblePropDesc.eInteractiveType = eType;										// 상호 작용 오브젝트 타입

                CDestructible_Prop::MODEL_TYPE eModelType = {};

                CHECK_FALSE(ReadFile(hObjectFile, &DestructiblePropDesc.eModelType, sizeof(CDestructible_Prop::MODEL_TYPE), &dwByte, nullptr), false);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
                    ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Destructible"), TIME_CHANNEL::WORLD, &DestructiblePropDesc), false);
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

_bool CLevel_Map::Trigger_objects_Load_Json()
{
	_wstring strTriggerInfoPath = AnsiToWString(m_strMapInfoFilePath);

	strTriggerInfoPath += TEXT("_trigger.json");

	ifstream ifs(strTriggerInfoPath);

	if (!ifs.is_open())
	{
		OutputDebugStringA("트리거 제이슨 없거나 문제잇음 일단 true 반환");
		return true;
	}

	JSON j = {};
	ifs >> j;
	ifs.close();

	JSON_MAP_TRIGGER_DATA TriggerData = j.get<JSON_MAP_TRIGGER_DATA>();

	_uint iNumTrigger = TriggerData.iNumTrigger;

	for (_uint i = 0; i < iNumTrigger; ++i)
	{
		CTrigger::TRIGGER_DESC TriggerDesc = {};

		TriggerDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
		TriggerDesc.eLevel = LEVEL::MAP;
		memcpy(TriggerDesc.szModelName, TEXT("Prototype_Component_Model_Trigger"), sizeof(TriggerDesc.szModelName));		// 프로토타입 태그명
		TriggerDesc.strTriggerKey = TriggerData.TriggerKey[i];

		memcpy(&TriggerDesc.WorldMatrix, &TriggerData.WorldMatrix[i], sizeof(_float4x4));										// 행렬

		TriggerDesc.eInteractiveType = INTERACTIVE_TYPE::TRIGGER;										// 상호 작용 오브젝트 타입

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
			ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Trigger"), TIME_CHANNEL::WORLD, &TriggerDesc), false);

		CProp* pInteractive_Prop = static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive")));
		CHECK_NULLPTR_MSG(pInteractive_Prop, TEXT("엥"), false);

		m_InteractiveList.push_back(pInteractive_Prop);
	}

	return true;
}

_bool CLevel_Map::Monster_objects_Load_Json()
{
    _wstring strMonsterInfoPath = AnsiToWString(m_strMapInfoFilePath);

    strMonsterInfoPath += TEXT("_spawn.json");

    ifstream ifs(strMonsterInfoPath);

    if (!ifs.is_open())
    {
        OutputDebugStringA("몬스터 제이슨 없거나 문제잇음 일단 true 반환");
        return true;
    }

    JSON j = {};
    ifs >> j;
    ifs.close();

    JSON_MAP_MONSTER_SPAWN_DATA MonsterData = j.get<JSON_MAP_MONSTER_SPAWN_DATA>();

    _uint iNumMonster = MonsterData.iNumMonster;

    for (_uint i = 0; i < iNumMonster; ++i)
    {
        CMap_Spawn::SPAWN_DESC SpawnDesc = {};

        SpawnDesc.iMapObjectID = m_iMapObjectCnt++;					// 사실상 의미 X
        SpawnDesc.eLevel = LEVEL::MAP;
        memcpy(SpawnDesc.szModelName, TEXT("Prototype_Component_Model_Monster"), sizeof(SpawnDesc.szModelName));		// 프로토타입 태그명
        SpawnDesc.strMonsterKey = MonsterData.MonsterKey[i];
        SpawnDesc.iSubLevel = MonsterData.SubLevel[i];

        memcpy(&SpawnDesc.WorldMatrix, &MonsterData.WorldMatrix[i], sizeof(_float4x4));										// 행렬

        SpawnDesc.eInteractiveType = INTERACTIVE_TYPE::SPAWN;										// 상호 작용 오브젝트 타입

        CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive"),
            ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Spawn"), TIME_CHANNEL::WORLD, &SpawnDesc), false);

        CProp* pInteractive_Prop = static_cast<CProp*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_MapObj_Interactive")));
        CHECK_NULLPTR_MSG(pInteractive_Prop, TEXT("엥"), false);

        m_InteractiveList.push_back(pInteractive_Prop);
    }

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

        _wstring strLightTag = szLightTag;

        _bool isRegistered = { false };

        for (_uint i = 0; i < m_LightTags.size(); ++i)
        {
            if (AnsiToWString(m_LightTags[i]) == strLightTag)
            {
                isRegistered = true;
                break;
            }
        }

        if (true == isRegistered)
            continue;

		m_pGameInstance->Add_Light(szLightTag, ENUM_CLASS(LEVEL::MAP), LightDesc, true);

        CMap_Light::MAP_LIGHT_DESC MapLightDesc = {};

        MapLightDesc.pLightDesc = m_pGameInstance->Get_LightDesc(szLightTag, ENUM_CLASS(LEVEL::MAP));

        CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::MAP), TEXT("Layer_PointLight"), ENUM_CLASS(LEVEL::MAP),
            TEXT("Prototype_GameObject_Prop_Light"), TIME_CHANNEL::WORLD, &MapLightDesc), false);

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

_bool CLevel_Map::Decals_Load_Binary()
{
    _wstring strDecalInfoPath = AnsiToWString(m_strMapInfoFilePath);

    strDecalInfoPath += TEXT("_decals.dat");

    DWORD dwByte = {};

    // 프로토타입 핸들 개방
    HANDLE hFile = CreateFile(strDecalInfoPath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        return false;
    }
    else
    {
        _uint iDecalCnt = {};
        // 1. 데칼의 총 개수 불러오기
        CHECK_FALSE(ReadFile(hFile, &iDecalCnt, sizeof(_uint), &dwByte, nullptr), false);

        for (_uint i = 0; i < iDecalCnt; ++i)
        {
            CDecal* pDecal = static_cast<CDecal*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Decal")));
            CHECK_NULLPTR(pDecal, false);

            DECAL_DESC DecalDesc = {};
            // 2. 데칼의 구조체 불러오기
            CHECK_FALSE(ReadFile(hFile, &DecalDesc, sizeof(DECAL_DESC), &dwByte, nullptr), false);
            pDecal->Set_Desc(DecalDesc);

            _float fThreshold = {};
            // 3. 데칼의 쓰레스 홀드 불러오기 ( 마스크 )
            CHECK_FALSE(ReadFile(hFile, &fThreshold, sizeof(_float), &dwByte, nullptr), false);
            pDecal->Set_Threshold(fThreshold);

            _uint iTextureIndex = {};
            // 4. 데칼의 텍스쳐 인덱스 불러오기
            CHECK_FALSE(ReadFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr), false);
            pDecal->Set_TextureIndex(iTextureIndex);

            _float4x4 WorldMatrix = {};
            // 5. 데칼의 월드 행렬 불러오기
            CHECK_FALSE(ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr), false);
            pDecal->Set_WorldMatrix(WorldMatrix);

            pDecal->Set_EnableDecoration(true);

            m_DecalList.push_back(pDecal);
            m_pGameInstance->Batch_Decal(pDecal);
        }
    }

    // 프로토타입 핸들 닫기
    CloseHandle(hFile);

    return true;
}

void CLevel_Map::Build_ModelPathCache()
{
	m_ModelPathCache.clear();

	vector<pair<string, string>> Roots = {
		{"../../Client/Bin/Resources/Map/Prop/", ".fbx"},
		{"../../Client/Bin/Data/Map/", ".dat"}
	};

	for (auto& [root, ext] : Roots)
	{
		for (auto& entry : filesystem::recursive_directory_iterator(root))
		{
			if (entry.is_regular_file() && entry.path().extension() == ext)
			{
				string name = entry.path().stem().string();
				string path = entry.path().string();

				// .fbx / .dat 둘 다 있을 수 있으므로 구분
				string key = name + ext;
				m_ModelPathCache[key] = path;
			}
		}
	}

#ifdef _DEBUG
	OutputDebugStringA(("Model cache built: " + std::to_string(m_ModelPathCache.size()) + " entries\n").c_str());
#endif
}

void CLevel_Map::MapEditor_Close_Windows()
{
    m_isMainWindow = true;
    m_isPrototypeInteractiveWindow = false;
    m_isInteractiveWindow = false;
    m_isFixInteractObjectWindow = false;
    m_isSkySphereWindow = false;
    m_isCloudSphereWindow = false;
    m_isMultiFixWindow = false;
    m_isInformation = false;

    // 2. 오브젝트 / 인터랙티브 리스트 초기화
    m_ObjectList.clear();
    m_InteractiveList.clear();
    m_iObjectListIndex = {};
    m_iInteractiveListIndex = {};
    m_iInstObjectCnt = {};
    m_iMapObjectCnt = {};
    m_iBN_ID = {};
    m_iFix_BN_ID = {};
    m_iSubLevel = {};
    m_iAddSubLevel = {};
    m_iRenderFrame = {};
    m_iRenderSubLevel = {};
    m_iMaxSubLevel = {};
    m_iNumInstance = 5;
    m_fInstanceRange = 3.f;

    m_pFixPropObj = nullptr;
    m_pFixTransformCom = nullptr;

    // 3. 오브젝트 수정용 데이터 초기화
    m_vFixScale = {};
    m_vFixRotation = {};
    m_vFixPosition = {};
    m_FixBaseMatrix = XMMatrixIdentity();
    m_FixWorldMatrix = {};
    m_fAddScale = 0.005f;
    m_isCameraPosAdd = false;
    m_fAddPositionY = 0.f;
    m_AddObjectProperties = {};
    m_RenderProperties = {};
    m_isRandomRotation = false;
    m_isCheckRender = false;

    // 4. 멀티픽스 관련 초기화
    m_isMultiFix = false;
    m_MultiFixList.clear();
    m_MultiFixRelatives.clear();
    m_pParentFixObject = nullptr;
    m_iMultiFixIndex = {};
    m_matParentBefore = {};
    m_matOriginalParentMatrix = {};

    // 5. 트리거 관련 초기화
    m_strTriggerKey.clear();
    m_strMonsterKey.clear();
    ZeroMemory(m_szFixTriggerKey, sizeof(m_szFixTriggerKey));
    ZeroMemory(m_szFixMonsterKey, sizeof(m_szFixMonsterKey));
    m_strFixTriggerKey.clear();
    m_strFixMonsterKey.clear();
    m_iMonsterSubLevel = {};
    m_iFixMonsterSubLevel = {};

    // 6. 맵 저장/로드 관련 초기화
    ZeroMemory(m_szMapInfoFilePath, sizeof(m_szMapInfoFilePath));
    strcpy_s(m_szMapInfoFilePath, "../../Client/Bin/Data/Map/MapData/");
    m_strMapInfoFilePath.clear();

    ZeroMemory(m_szMapInfoFileName, sizeof(m_szMapInfoFileName));
    m_strMapInfoFileName.clear();
    m_isLoaded = false;

    // 7. 라이트 관련 초기화
    m_LightDesc = {};
    m_FixLightDesc = {};
    ZeroMemory(m_szLightTag, sizeof(m_szLightTag));
    ZeroMemory(m_szFixLightTag, sizeof(m_szFixLightTag));
    m_strLightTag.clear();
    m_strFixLightTag.clear();
    m_LightTags.clear();
    m_iLightTagIndex = {};
    m_isAddLight = false;
    m_isFixLight = false;
    m_isFindFixLight = false;
    m_isAddLightPoint = false;
    m_vLightPoint = {};

    // 8. FBX 변환 관련
    ZeroMemory(m_szFolderName, sizeof(m_szFolderName));
    m_iPropPrototype = 0;
    ZeroMemory(m_szPropFolder, sizeof(m_szPropFolder));
    m_isAnim = false;
    ZeroMemory(m_szDataSavePath, sizeof(m_szDataSavePath));
    strcpy_s(m_szDataSavePath, "../../Client/Bin/Data/Map/");
    m_strDataSavePath.clear();

    // 9. 프로토타입 리스트 초기화
    m_Prototypes_Obj.clear();
    m_Prototypes_Inter.clear();
    m_iIndex_PrtObj = {};
    m_iIndex_PrtInter = {};
    ZeroMemory(m_szSearchPrototypeName, sizeof(m_szSearchPrototypeName));
    ZeroMemory(m_szSearchObjectName, sizeof(m_szSearchObjectName));
    ZeroMemory(m_szSearchInteractObjectName, sizeof(m_szSearchInteractObjectName));

    // 10. 픽킹 및 거리 관련 변수 초기화
    m_vPickedPos = {};
    m_vDistancePos[0] = {};
    m_vDistancePos[1] = {};
    m_fDistance = 0.f;
    m_isRenderOption = false;
    m_isObjectListView = true;
    m_isActiveShortCutKey = false;

    // 11. Sky / Cloud Sphere 초기화
    m_FixSkyDesc = {};
    m_FixCloudDesc = {};
    m_pSkySphere = nullptr;
    m_pCloudSphere = nullptr;

    // 12. 캐시 정리
    m_ModelPathCache.clear();
    m_ModelPathCache.rehash(0);
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

    m_ModelPathCache.clear();
    m_ModelPathCache.rehash(0);

    m_DecalList.clear();
}
