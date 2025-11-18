#include "Debug_Manager.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CDebug_Manager::CDebug_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pClientInstance { CClientInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pClientInstance);
}

HRESULT CDebug_Manager::Initialize()
{
	Ready_FPS();

	Ready_CameraDebug();

	Ready_JoltDebug();

	return S_OK;
}

void CDebug_Manager::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
}

void CDebug_Manager::Ready_FPS()
{
#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Debug"), [&]() {
		ImGui::Begin("FPS");

		++m_iRenderCount;

		if (m_fTimeAcc >= 1.f)
		{
			snprintf(m_szFPS, sizeof(m_szFPS), "FPS:%d", m_iRenderCount);			
			m_fTimeAcc = 0.f;
			m_iRenderCount = 0;
		}
		ImGui::Text(m_szFPS);

		ImGui::End();
		});
#endif
}

void CDebug_Manager::Ready_CameraDebug()
{
#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Debug"), [&]() {
		ImGui::Begin("Camera");

		_float3 vCameraPos = m_pClientInstance->Get_ActiveCameraPos();

		_char szPosBuffer[MAX_PATH];
		snprintf(szPosBuffer, sizeof(szPosBuffer), "POS : X : %.2f, Y : %.2f, Z : %.2f", vCameraPos.x, vCameraPos.y, vCameraPos.z);
		ImGui::Text(szPosBuffer);

		_float4 vCameraLook = m_pClientInstance->Get_ActiveCameraLook();

		_char szLookBuffer[MAX_PATH];
		snprintf(szLookBuffer, sizeof(szLookBuffer), "LOOK : X : %.2f, Y : %.2f, Z : %.2f, W : %.2f", vCameraLook.x, vCameraLook.y, vCameraLook.z, vCameraLook.w);
		ImGui::Text(szLookBuffer);

		ImGui::Text("Frustum");

		const _float4* vPoint = m_pGameInstance->Get_Frustum_Point();

		for (size_t i = 0; i < 8; i++)
		{
			_char szPointBuffer[MAX_PATH];
			snprintf(szPointBuffer, sizeof(szPointBuffer), "POINT %.1f : X : %.2f, Y : %.2f, Z : %.2f, W : %.2f", i, vPoint[i].x, vPoint[i].y, vPoint[i].z, vPoint[i].w);
			ImGui::Text(szPointBuffer);
		}

		ImGui::Spacing();

		const _float4* vWorldPoint = m_pGameInstance->Get_Frustum_WorldPoints();

		for (size_t i = 0; i < 8; i++)
		{
			_char szPointBuffer[MAX_PATH];
			snprintf(szPointBuffer, sizeof(szPointBuffer), "WorldPOINT %.1f : X : %.2f, Y : %.2f, Z : %.2f, W : %.2f", i, vWorldPoint[i].x, vWorldPoint[i].y, vWorldPoint[i].z, vWorldPoint[i].w);
			ImGui::Text(szPointBuffer);
		}

		ImGui::Spacing();

		const _float4* vWorldPlane = m_pGameInstance->Get_Frustum_WorldPlanes();

		for (size_t i = 0; i < 6; i++)
		{
			_char szPlaneBuffer[MAX_PATH];
			snprintf(szPlaneBuffer, sizeof(szPlaneBuffer), "WorldPlane %.1f : X : %.2f, Y : %.2f, Z : %.2f, W : %.2f", i, vWorldPlane[i].x, vWorldPlane[i].y, vWorldPlane[i].z, vWorldPlane[i].w);
			ImGui::Text(szPlaneBuffer);
		}
		ImGui::End();
		});
#endif
}

void CDebug_Manager::Ready_JoltDebug()
{
#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Debug"), [&]() {
		ImGui::Begin("Jolt");

		ImGui::Text("Collision Render");
		vector<const char*> Labels;
		Labels.push_back("MAP_STATIC");
		Labels.push_back("MAP_DYNAMIC");
		Labels.push_back("MAP_INTERACT");
		Labels.push_back("PLAYER");
        Labels.push_back("PLAYERATTACK");
		Labels.push_back("MONSTER");
		Labels.push_back("MONSTERATTACK");
		Labels.push_back("ITEM");
		Labels.push_back("EFFECT");
		Labels.push_back("SKILL");
		Labels.push_back("CAMERA");
        Labels.push_back("MAP_STATIC_TRIGGER");
        Labels.push_back("MAP_MOVE_FLATFORM");

		for (size_t i = 0; i < Labels.size(); i++)
		{
			if (ImGui::Checkbox(Labels[i], &m_isCollisionRender[i]))
			{
				if (m_isCollisionRender[i])
				{
					m_pGameInstance->Set_DrawFilter(i);
				}
				else
				{
					m_pGameInstance->Remove_DrawFilter(i);
				}
			}
		}
		
		ImGui::End();
		});
#endif
}

CDebug_Manager* CDebug_Manager::Create()
{
	CDebug_Manager* pInstance = new CDebug_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CDebug_Manager"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDebug_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pClientInstance);
}
