#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"

#include "Level_Editor.h"
#include "Level_Map.h"
#include "Level_Animation.h"
#include "Level_Effect.h"
#include "Level_UI.h"
#include "Level_Shader.h"
#include "Level_Camera.h"
#include "Level_AI.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_GameObjects()))
		return E_FAIL;

	/* 다음 레벨을 위한 로딩작업을 시작 한다. */
	if (FAILED(Ready_LoadingThread()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		CleanImgui();
		CLevel* pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL::EDITOR:
			pNewLevel = CLevel_Editor::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::MAP:
			pNewLevel = CLevel_Map::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::ANIMATION:
			pNewLevel = CLevel_Animation::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::EFFECT:
			pNewLevel = CLevel_Effect::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::UI:
			pNewLevel = CLevel_UI::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::SHADER:
			pNewLevel = CLevel_Shader::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::CAMERA:
			pNewLevel = CLevel_Camera::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::AI:
			pNewLevel = CLevel_AI::Create(m_pDevice, m_pContext);
			break;
		}

		if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(m_eNextLevelID), pNewLevel)))
			return;
	}
}

HRESULT CLevel_Loading::Render()
{
	/* 생성해놓은 객체들을 렌더한다. */
	m_pLoader->Show_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_GameObjects()
{
	return S_OK;
}

HRESULT CLevel_Loading::Ready_LoadingThread()
{
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::CleanImgui()
{
	m_pGameInstance->CleanMenu(TEXT("Animation"));
	m_pGameInstance->CleanMenu(TEXT("Map"));
	m_pGameInstance->CleanMenu(TEXT("Effect"));
	m_pGameInstance->CleanMenu(TEXT("Model"));
	m_pGameInstance->CleanMenu(TEXT("UI"));
	m_pGameInstance->CleanMenu(TEXT("Shader"));
	m_pGameInstance->CleanMenu(TEXT("Camera"));

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);

}
