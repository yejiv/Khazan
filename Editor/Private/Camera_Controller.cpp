#include "Camera_Controller.h"
#include "GameInstance.h"
#include "Camera_Compre.h"
#include "Transform.h"

CCamera_Controller::CCamera_Controller()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
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
	
}

HRESULT CCamera_Controller::Ready_ImGui()
{
	m_pGameInstance->AddWidget(TEXT("Camera"), [this]() {

		FAILED_CHECK(Ready_ImGui_Create(), E_FAIL);

		FAILED_CHECK(Ready_ImGui_List(), E_FAIL);

		FAILED_CHECK(Ready_ImGui_Active_Camera_Info(), E_FAIL);

		FAILED_CHECK(Ready_ImGui_Active_Camera_Animation(), E_FAIL);

		FAILED_CHECK(Ready_ImGui_Active_Camera_Animation_Item(), E_FAIL);

		FAILED_CHECK(Ready_ImGui_Active_Camera_Event_Item(), E_FAIL);

	});
	return S_OK;
}

HRESULT CCamera_Controller::Ready_ImGui_Create()
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
	

	if (ImGui::Button("Create", ImVec2(60.f, 30.f)))
	{
		Create_Camera();
	};


	ImGui::End();


	return S_OK;
}

HRESULT CCamera_Controller::Ready_ImGui_List()
{
	vector<CCamera*> Cameras = m_pGameInstance->Get_pCameras(ENUM_CLASS(LEVEL::CAMERA));
	if (Cameras.size() > 0)
	{
		ImGui::Begin("List Camera");
		
		ImGui::TextUnformatted("List");
		ImGui::BeginChild(string("##box_CameraList").c_str(),
			ImVec2(0, 200),true);
		for(size_t i = 0; i < Cameras.size(); i++)
		{
			bool is_selected = (i == m_iListSelectCamera);
			if (ImGui::Selectable(WStringToAnsi(Cameras[i]->Get_CameraTag()).c_str(), is_selected))
			{
				m_iListSelectCamera = i;
			}
		}

		ImGui::EndChild();



		CCamera* pCamera = m_pGameInstance->Get_ActiveCamera();
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
			m_pGameInstance->Change_Camera(ENUM_CLASS(LEVEL::CAMERA), m_tCreateCameraDesc.strCameraTag);

			CCamera* pCamera = m_pGameInstance->Get_ActiveCamera();

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

		ImGui::End();
	}


	
	return S_OK;
}

HRESULT CCamera_Controller::Ready_ImGui_Active_Camera_Info()
{
	CCamera* pCamera = m_pGameInstance->Get_ActiveCamera();
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

	return S_OK;

}
HRESULT CCamera_Controller::Ready_ImGui_Active_Camera_Animation()
{
	CCamera* pCamera = m_pGameInstance->Get_ActiveCamera();
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

	return S_OK;
}

HRESULT CCamera_Controller::Ready_ImGui_Active_Camera_Animation_Item()
{
	CCamera* pCamera = m_pGameInstance->Get_ActiveCamera();
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

	return S_OK;
}

HRESULT CCamera_Controller::Ready_ImGui_Active_Camera_Event_Item()
{
	CCamera* pCamera = m_pGameInstance->Get_ActiveCamera();
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

	return S_OK;
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

	CCamera_Compre* pCamera = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::CAMERA), TEXT("Prototype_GameObject_Camera_Compre"), &m_tCreateCameraDesc));
	pCamera->Set_IsActive(false);

	m_pGameInstance->Add_Camera(ENUM_CLASS(LEVEL::CAMERA), pCamera);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::CAMERA), TEXT("Layer_Camera"), pCamera);
}

Editor::CCamera_Controller* CCamera_Controller::Create()
{
	CCamera_Controller* pInstance = new CCamera_Controller();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Controller"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Editor::CCamera_Controller::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
