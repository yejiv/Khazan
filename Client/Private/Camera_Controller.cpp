#include "Camera_Controller.h"
#include "ClientInstance.h"
#include "GameInstance.h"
#include "Camera_Compre.h"
#include "Transform.h"

CCamera_Controller::CCamera_Controller()
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pClientInstance{ CClientInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pClientInstance);
}


HRESULT CCamera_Controller::Initialize()
{
	m_tCreateCameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	m_tCreateCameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	m_tCreateCameraDesc.fFovy = 60.0f;
	m_tCreateCameraDesc.fNear = 0.1f;
	m_tCreateCameraDesc.fFar = 500.f;
	m_tCreateCameraDesc.fSpeedPerSec = 10.f;
	m_tCreateCameraDesc.fRotationPerSec = 90.0f;
	m_tCreateCameraDesc.fMouseSensor = 0.2f;
	m_tCreateCameraDesc.iCameraType = ENUM_CLASS(CAMERATYPE::FREE);

	FAILED_CHECK(Ready_Camera(TEXT("Layer_Camera")), E_FAIL);

	FAILED_CHECK(Ready_ImGui(), E_FAIL);

	return S_OK;
}

void CCamera_Controller::Update(_float fTimeDelta)
{

	if (m_pGameInstance->Key_Down(DIK_LCONTROL))
	{
		_float3 vPos{};
		m_pGameInstance->isPicked(&vPos);
		int a = 1;
	}

}

HRESULT CCamera_Controller::Ready_ImGui()
{
	m_pGameInstance->AddWidget(TEXT("Camera"), [this]() {
		Ready_ImGui_Create();
		Ready_ImGui_List();
		Ready_ImGui_Active_Camera_Info();
		Ready_ImGui_Active_Camera_Animation();
		Ready_ImGui_Active_Camera_Animation_Item();
		Ready_ImGui_Active_Camera_Event_Item();
		Ready_Guizmo();
		Ready_Level();
		});
	return S_OK;
}

void CCamera_Controller::Ready_ImGui_Create()
{
	ImGui::Begin("Create Camera");
	ImGui::InputText("Name", m_szCreate_CameraName, sizeof(MAX_PATH));
	m_tCreateCameraDesc.strCameraTag = CharToWString(m_szCreate_CameraName);
	ImGui::TextUnformatted("Type");
	ImGui::SameLine();
	if (ImGui::RadioButton("FREE", static_cast<CAMERATYPE>(m_tCreateCameraDesc.iCameraType) == CAMERATYPE::FREE))
		m_tCreateCameraDesc.iCameraType = ENUM_CLASS(CAMERATYPE::FREE);
	ImGui::SameLine();
	if (ImGui::RadioButton("SPRING", static_cast<CAMERATYPE>(m_tCreateCameraDesc.iCameraType) == CAMERATYPE::SPRING))
		m_tCreateCameraDesc.iCameraType = ENUM_CLASS(CAMERATYPE::SPRING);
	ImGui::InputFloat4("Eye", &m_tCreateCameraDesc.vEye.x);
	ImGui::InputFloat4("At", &m_tCreateCameraDesc.vAt.x);
	ImGui::InputFloat("Fovy", &m_tCreateCameraDesc.fFovy);
	ImGui::InputFloat("Near", &m_tCreateCameraDesc.fNear);
	ImGui::InputFloat("Far", &m_tCreateCameraDesc.fFar);
	ImGui::InputFloat("MouseSensor", &m_tCreateCameraDesc.fMouseSensor);
	ImGui::InputFloat("RotationSpeed", &m_tCreateCameraDesc.fRotationPerSec);
	ImGui::InputFloat("Speed", &m_tCreateCameraDesc.fSpeedPerSec);


	if (ImGui::Button(U8STR("생성"), ImVec2(60.f, 30.f)))
	{
		Create_Camera();
	};
	ImGui::InputText("CameraLoadFilePath", m_szLoadFilePath, MAX_PATH);
	if (ImGui::Button("Load_Camera"))
	{
		string filePath = m_szLoadFilePath;
		filePath += ".json";
		ifstream In(filePath);
		if (!In.is_open())
		{
			MSG_BOX(TEXT("UI JSON 파일 불러오기 실패"));
			In.close();
		}
		else
		{
			nlohmann::json jsonData;
			In >> jsonData;

			CCamera_Compre::CAMERA_COMPRE_DESC CameraDesc{};

			CameraDesc.strCameraTag = AnsiToWString(jsonData["Name"]);
			CameraDesc.vEye.x = jsonData["Eye"]["x"];
			CameraDesc.vEye.y = jsonData["Eye"]["y"];
			CameraDesc.vEye.z = jsonData["Eye"]["z"];
			CameraDesc.vEye.w = jsonData["Eye"]["w"];
			CameraDesc.vAt.x = jsonData["At"]["x"];
			CameraDesc.vAt.y = jsonData["At"]["y"];
			CameraDesc.vAt.z = jsonData["At"]["z"];
			CameraDesc.vAt.w = jsonData["At"]["w"];
			CameraDesc.fFovy = jsonData["Fovy"];
			CameraDesc.fNear = jsonData["Near"];
			CameraDesc.fFar = jsonData["Far"];
			CameraDesc.fSpeedPerSec = jsonData["SpeedPerSec"];
			CameraDesc.fRotationPerSec = jsonData["RotationPerSec"];
			CameraDesc.fMouseSensor = jsonData["MouseSensor"];
			CameraDesc.iCameraType = jsonData["CameraType"];

			map<_wstring, vector<CAMERA_KEYFRAME>> Animations;
			for (auto Animation : jsonData["Animation"])
			{
				vector<CAMERA_KEYFRAME> KeyFrames;
				for (auto Ani : Animation["Animations"])
				{
					CAMERA_KEYFRAME KeyFrame{};
					KeyFrame.vTranslation.x = Ani["Translation"]["x"];
					KeyFrame.vTranslation.y = Ani["Translation"]["y"];
					KeyFrame.vTranslation.z = Ani["Translation"]["z"];
					KeyFrame.vLookAt.x = Ani["LookAt"]["x"];
					KeyFrame.vLookAt.y = Ani["LookAt"]["y"];
					KeyFrame.vLookAt.z = Ani["LookAt"]["z"];
					KeyFrame.vLookAt.w = Ani["LookAt"]["w"];
					KeyFrame.fSpeed = Ani["Speed"];
					KeyFrame.fTrackPosition = Ani["TrackPosition"];

					KeyFrames.push_back(KeyFrame);
				}

				Animations.emplace(AnsiToWString(Animation["Name"]), KeyFrames);
			}

			map<_wstring, vector<CAMERA_EVENT_DATA>> Events;
			for (auto Event : jsonData["Event"])
			{
				vector<CAMERA_EVENT_DATA> EventDatas;
				for (auto EveData : Event["Events"])
				{
					CAMERA_EVENT_DATA Event{};
					Event.iEventType = EveData["EventType"];
					Event.strEventKey = AnsiToWString(EveData["EventKey"]);
					Event.isComplete = EveData["isComplete"];
					Event.fTrackPosition = EveData["TrackPosition"];


					EventDatas.push_back(Event);
				}

				Events.emplace(AnsiToWString(Event["Name"]), EventDatas);
			}

			CCamera_Compre* pCamera = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(m_eCurrentLevel), TEXT("Prototype_GameObject_Camera_Compre"), &CameraDesc));
			pCamera->Set_IsActive(false);
			pCamera->Load(Animations, Events);

			m_pClientInstance->Add_Camera(ENUM_CLASS(m_eCurrentLevel), pCamera);

			m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Camera"), pCamera);

		}


	}

	ImGui::End();

}

void CCamera_Controller::Ready_ImGui_List()
{
	vector<CCamera*> Cameras = m_pClientInstance->Get_pCameras(ENUM_CLASS(m_eCurrentLevel));
	if (Cameras.size() > 0)
	{
		ImGui::Begin("List Camera");

		ImGui::TextUnformatted("List");
		ImGui::BeginChild(string("##box_CameraList").c_str(),
			ImVec2(0, 200), true);
		for (size_t i = 0; i < Cameras.size(); i++)
		{
			bool is_selected = (i == m_iListSelectCamera);
			if (ImGui::Selectable(WStringToAnsi(Cameras[i]->Get_CameraTag()).c_str(), is_selected))
			{
				m_iListSelectCamera = i;
			}
		}

		ImGui::EndChild();



		CCamera* pCamera = m_pClientInstance->Get_ActiveCamera();
		if (pCamera != nullptr)
		{
			ImGui::InputText("Name", m_szEdit_CameraName, sizeof(MAX_PATH));
			m_tEditCameraDesc.strCameraTag = CharToWString(m_szEdit_CameraName);
			ImGui::TextUnformatted("Type");
			ImGui::SameLine();
			if (ImGui::RadioButton("FREE", static_cast<CAMERATYPE>(m_tEditCameraDesc.iCameraType) == CAMERATYPE::FREE))
				m_tEditCameraDesc.iCameraType = ENUM_CLASS(CAMERATYPE::FREE);
			ImGui::SameLine();
			if (ImGui::RadioButton("SPRING", static_cast<CAMERATYPE>(m_tEditCameraDesc.iCameraType) == CAMERATYPE::SPRING))
				m_tEditCameraDesc.iCameraType = ENUM_CLASS(CAMERATYPE::SPRING);
			ImGui::InputFloat("Fovy", &m_tEditCameraDesc.fFovy);
			ImGui::InputFloat("Near", &m_tEditCameraDesc.fNear);
			ImGui::InputFloat("Far", &m_tEditCameraDesc.fFar);
			ImGui::InputFloat("MouseSensor", &m_tEditCameraDesc.fMouseSensor);
		}
		if (ImGui::Button("Active", ImVec2(60.f, 30.f)))
		{
			m_pClientInstance->Change_Camera(ENUM_CLASS(m_eCurrentLevel), Cameras[m_iListSelectCamera]->Get_CameraTag());

			CCamera* pCamera = m_pClientInstance->Get_ActiveCamera();

			if (pCamera != nullptr)
			{
				m_tEditCameraDesc = dynamic_cast<CCamera_Compre*>(pCamera)->Get_Desc();
				strcpy_s(m_szEdit_CameraName, WStringToAnsi(m_tEditCameraDesc.strCameraTag).c_str());
			}

		}
		ImGui::SameLine();
		if (ImGui::Button("Edit", ImVec2(60.f, 30.f)))
		{
			pCamera->Set_DefaultData(m_tEditCameraDesc);
		}

		ImGui::InputText("CameraSaveFilePath", m_szSaveFilePath, MAX_PATH);
		if (ImGui::Button("Save_Camera"))
		{
			string filePath = m_szSaveFilePath;
			filePath += ".json";
			nlohmann::ordered_json SaveData;
			m_pClientInstance->Save_Json_Camera(ENUM_CLASS(m_eCurrentLevel), Cameras[m_iListSelectCamera]->Get_CameraTag(), SaveData);
			ofstream Out(filePath, ios::out | ios::trunc);
			if (!Out.is_open())
			{
				MSG_BOX(TEXT("Json 파일 저장 실패"));
				Out.close();
			}
			else
			{
				MSG_BOX(TEXT("Json 파일 저장 성공"));
				Out << SaveData.dump(4);
				Out.close();
			}
		}

		ImGui::End();
	}
}

void CCamera_Controller::Ready_ImGui_Active_Camera_Info()
{
	CCamera* pCamera = m_pClientInstance->Get_ActiveCamera();
	if (pCamera != nullptr)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(TEXT("Com_Transform")));
		_vector vPos = pTransform->Get_State(STATE::POSITION);
		_vector vLook = pTransform->Get_State(STATE::LOOK);

		ImGui::Begin("Active Camera Info");

		_char szPosBuffer[MAX_PATH];
		snprintf(szPosBuffer, sizeof(szPosBuffer), "X : %.2f, Y : %.2f, Z : %.2f", vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2]);
		ImGui::Text(szPosBuffer);
		_char szLookBuffer[MAX_PATH];
		snprintf(szLookBuffer, sizeof(szLookBuffer), "X : %.2f, Y : %.2f, Z : %.2f, W : %.2f", vLook.m128_f32[0], vLook.m128_f32[1], vLook.m128_f32[2], vLook.m128_f32[3]);
		ImGui::Text(szLookBuffer);

		ImGui::End();

	}

}
void CCamera_Controller::Ready_ImGui_Active_Camera_Animation()
{
	CCamera* pCamera = m_pClientInstance->Get_ActiveCamera();
	if (pCamera != nullptr)
	{
		ImGui::Begin("Animation");

		ImGui::InputText("Tag", m_szCreate_AnimationName, sizeof(m_szCreate_AnimationName));

		if (ImGui::Button("Create", ImVec2(60.f, 30.f)))
		{
			pCamera->Create_Animation(AnsiToWString(m_szCreate_AnimationName));
		}

		map<_wstring, vector<CAMERA_KEYFRAME>>* Animations = pCamera->Get_AllAnimations();
		ImGui::TextUnformatted("Animation List");
		ImGui::BeginChild(string("##box_AnimationList").c_str(), ImVec2(0, 200), true);
		for (auto Animation : *Animations)
		{
			bool is_selected = (Animation.first == m_strListSelectAnimation);
			if (ImGui::Selectable(WStringToAnsi(Animation.first).c_str(), is_selected))
			{
				m_strListSelectAnimation = Animation.first;
			}
		}

		ImGui::EndChild();

		if (ImGui::Button("Play", ImVec2(60.f, 30.f)))
		{
			pCamera->Set_Animation(m_strListSelectAnimation);
		}

		ImGui::End();
	}

}

void CCamera_Controller::Ready_ImGui_Active_Camera_Animation_Item()
{
	CCamera* pCamera = m_pClientInstance->Get_ActiveCamera();
	if (pCamera != nullptr)
	{
		vector<CAMERA_KEYFRAME>* pAnimations = pCamera->Get_Animations(m_strListSelectAnimation);

		if (pAnimations != nullptr)
		{
			ImGui::Begin("Animation_Item");

			for (size_t i = 0; i < pAnimations->size(); i++)
			{
				ImGui::PushID((int)i);
				ImGui::BeginGroup();

				ImGui::InputFloat("TrackPosition", &(*pAnimations)[i].fTrackPosition);
				ImGui::InputFloat3("Translation", &(*pAnimations)[i].vTranslation.x);
				ImGui::InputFloat4("LookAt", &(*pAnimations)[i].vLookAt.x);
				ImGui::InputFloat("Speed", &(*pAnimations)[i].fSpeed);

				ImGui::EndGroup();

				ImVec2 rect_min = ImGui::GetItemRectMin();
				ImVec2 rect_max = ImGui::GetItemRectMax();

				// 외곽선만 그리기 (두께와 라운딩은 스타일에 맞춤)
				float thickness = 1.0f;
				float rounding = ImGui::GetStyle().FrameRounding;
				ImGui::GetWindowDrawList()->AddRect(
					rect_min, rect_max,
					ImGui::GetColorU32(ImGuiCol_Border),
					rounding, 0, thickness
				);

				// 아래쪽 패딩만
				ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 필요하면 8.0f 조절
				ImGui::PopID();
			}

			if (ImGui::Button("Add", ImVec2(60.f, 30.f)))
			{
				pCamera->Create_Animation_Item(m_strListSelectAnimation);
			}

			ImGui::End();
		}

	}

}

void CCamera_Controller::Ready_ImGui_Active_Camera_Event_Item()
{
	CCamera* pCamera = m_pClientInstance->Get_ActiveCamera();
	if (pCamera != nullptr)
	{
		vector<CAMERA_EVENT_DATA>* Events = pCamera->Get_Events(m_strListSelectAnimation);
		vector<CAMERA_KEYFRAME>* pAnimations = pCamera->Get_Animations(m_strListSelectAnimation);

		if (pAnimations != nullptr)
		{
			ImGui::Begin("Event_Item");

			if (Events == nullptr)
			{
				if (ImGui::Button("Create", ImVec2(60.f, 30.f)))
				{
					pCamera->Create_Event(m_strListSelectAnimation);
				}
			}

			if (Events != nullptr)
			{
				for (size_t i = 0; i < Events->size(); i++)
				{
					ImGui::PushID((int)i);
					ImGui::BeginGroup();


					_char szEventKey[MAX_PATH];
					strcpy_s(szEventKey, WStringToAnsi((*Events)[i].strEventKey).c_str());
					if (ImGui::InputText("EventKey", szEventKey, sizeof(szEventKey)))
					{
						(*Events)[i].strEventKey = AnsiToWString(szEventKey);
					}
					_int iEventType = static_cast<_int>((*Events)[i].iEventType);
					if (ImGui::InputInt("EventType", &iEventType))
					{
						(*Events)[i].iEventType = static_cast<_uint>(iEventType);
					}
					ImGui::InputFloat("TrackPosition", &(*Events)[i].fTrackPosition);
					ImGui::Checkbox("isComplete", &(*Events)[i].isComplete);


					ImGui::EndGroup();

					ImVec2 rect_min = ImGui::GetItemRectMin();
					ImVec2 rect_max = ImGui::GetItemRectMax();

					// 외곽선만 그리기 (두께와 라운딩은 스타일에 맞춤)
					float thickness = 1.0f;
					float rounding = ImGui::GetStyle().FrameRounding;
					ImGui::GetWindowDrawList()->AddRect(
						rect_min, rect_max,
						ImGui::GetColorU32(ImGuiCol_Border),
						rounding, 0, thickness
					);

					// 아래쪽 패딩만
					ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 필요하면 8.0f 조절
					ImGui::PopID();
				}

				if (ImGui::Button("Add", ImVec2(60.f, 30.f)))
				{
					pCamera->Create_Event_Item(m_strListSelectAnimation);
				}


			}
			ImGui::End();
		}
	}

}

void CCamera_Controller::Ready_Guizmo()
{
	//CGameObject* pPlayer = m_pGameInstance->Get_BackGameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Player"));
	//CTransform* pTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
	//// 1) 카메라 행렬 준비
	//XMMATRIX view = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	//XMMATRIX proj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	//XMMATRIX model = pTransform->Get_WorldMatrix();     // 편집할 오브젝트 월드행렬

	//XMMATRIX viewT = XMMatrixTranspose(view);
	//XMMATRIX projT = XMMatrixTranspose(proj);
	//XMMATRIX modelT = XMMatrixTranspose(model);

	//float viewM[16];  XMStoreFloat4x4((XMFLOAT4X4*)viewM, viewT);
	//float projM[16];  XMStoreFloat4x4((XMFLOAT4X4*)projM, projT);
	//float modelM[16]; XMStoreFloat4x4((XMFLOAT4X4*)modelM, modelT);

	////// 2) 기즈모 파라미터 설정
	//ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE; // ROTATE, SCALE
	//ImGuizmo::MODE      mode = ImGuizmo::LOCAL;     // WORLD/LOCAL
	////bool useSnap = false;
	////float snap[3] = { 1.f, 1.f, 1.f };            // 스냅 그리드 간격(원하면 사용)

	////ImGui::Begin("Scene");
	//////ImVec2 winPos = ImGui::GetWindowPos();
	//////ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
	//////ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
	//////ImVec2 gizmoPos = ImVec2(winPos.x + contentMin.x, winPos.y + contentMin.y);
	//////ImVec2 gizmoSize = ImVec2(contentMax.x - contentMin.x, contentMax.y - contentMin.y);

	////const ImGuiViewport* vp = ImGui::GetMainViewport();
	////ImGuizmo::SetRect(vp->Pos.x, vp->Pos.y, vp->Size.x, vp->Size.y);

	////// 3) 디버그: 그리드/큐브 그려보기 (카메라가 원점 안 보면 안 보일 수 있으니 모델도 같이)
	////float I[16] = { 1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1 };
	////ImGuizmo::DrawGrid(viewM, projM, I, 10.0f);
	////ImGuizmo::DrawCubes(viewM, projM, modelM, 1);
	//////// 3) 조작
	//////ImGuizmo::Manipulate(
	//////	viewM, projM, op, mode,
	//////	modelM,                 // in/out
	//////	nullptr,                // 델타행렬 필요하면 여기에
	//////	useSnap ? snap : nullptr
	//////);

	////ImGui::End();

	//// 4) 결과 되돌리기(다시 row-major로)
	///*XMMATRIX modelT2 = XMLoadFloat4x4((XMFLOAT4X4*)modelM);
	//XMMATRIX model2 = XMMatrixTranspose(modelT2);
	//pTransform->Set_WorldMatrix(model2);*/

	//if (ImGui::Begin("Scene"))
	//{
	//	ImVec2 winPos = ImGui::GetWindowPos();
	//	ImVec2 cmin = ImGui::GetWindowContentRegionMin();
	//	ImVec2 cmax = ImGui::GetWindowContentRegionMax();
	//	ImVec2 pos = ImVec2(winPos.x + cmin.x, winPos.y + cmin.y);
	//	ImVec2 size = ImVec2(cmax.x - cmin.x, cmax.y - cmin.y);

	//	// 창 포그라운드(현재 뷰포트)로
	//	ImGuizmo::SetDrawlist();                // 중요: 인자 없는 버전
	//	ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

	//	ImGuizmo::Manipulate(viewM, projM, op, mode, modelM);
	//}
	//ImGui::End();
}

void CCamera_Controller::Ready_Level()
{
	ImGui::Begin("Select Level");
	vector<const char*> Labels;
	Labels.push_back("STATIC");
	Labels.push_back("LOADING");
	Labels.push_back("HEINGMACH");
	Labels.push_back("CREVICE");
	Labels.push_back("EMBARS");
	Labels.push_back("VIPER");


	for (size_t i = 0; i < ENUM_CLASS(LEVEL::END); i++)
	{
		if (i == 0 || i == 1)
			continue;
		if (ImGui::RadioButton(Labels[i], &m_isSelectLevel[i]))
		{
			m_eCurrentLevel == static_cast<LEVEL>(i);
		}
	}

	ImGui::End();
}

void CCamera_Controller::Ready_Player()
{
}

void CCamera_Controller::CameraTool_Clear()
{
	m_iListSelectCamera = 0;
	m_strListSelectAnimation = TEXT("");
}

HRESULT CCamera_Controller::Ready_Camera(const _wstring& strLayerTag)
{


	return S_OK;
}

void CCamera_Controller::Create_Camera()
{
	CCamera_Compre::CAMERA_COMPRE_DESC Desc{};
	Desc.vEye = m_tCreateCameraDesc.vEye;
	Desc.vAt = m_tCreateCameraDesc.vAt;
	Desc.fFovy = XMConvertToRadians(m_tCreateCameraDesc.fFovy);
	Desc.fNear = m_tCreateCameraDesc.fNear;
	Desc.fFar = m_tCreateCameraDesc.fFar;
	Desc.fSpeedPerSec = m_tCreateCameraDesc.fSpeedPerSec;
	Desc.fRotationPerSec = XMConvertToRadians(m_tCreateCameraDesc.fRotationPerSec);
	Desc.fMouseSensor = m_tCreateCameraDesc.fMouseSensor;
	Desc.iCameraType = m_tCreateCameraDesc.iCameraType;
	Desc.strCameraTag = m_tCreateCameraDesc.strCameraTag;

	CCamera_Compre* pCamera = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(m_eCurrentLevel), TEXT("Prototype_GameObject_Camera_Compre"), &Desc));
	pCamera->Set_IsActive(false);

	m_pClientInstance->Add_Camera(ENUM_CLASS(m_eCurrentLevel), pCamera);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Camera"), pCamera);
}

CCamera_Controller* CCamera_Controller::Create()
{
	CCamera_Controller* pInstance = new CCamera_Controller();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Controller"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Controller::Free()
{
	__super::Free();
	Safe_Release(m_pClientInstance);
	Safe_Release(m_pGameInstance);
}
