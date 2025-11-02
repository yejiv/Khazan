#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Level_Title.h"
#include "Level_HeinMach.h"
#include "Level_Crevice.h"
#include "Level_Embars.h"
#include "Level_Viper.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
	, m_pClientInstance { CClientInstance::GetInstance() }
{
	Safe_AddRef(m_pClientInstance);
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
	m_pLoader->Update();

	if (true == m_pLoader->isFinished() && 
		GetKeyState(VK_SPACE) & 0x8000)
	{
		if (m_pClientInstance->Get_CurrLevel() != m_eNextLevelID)
		{
			m_pClientInstance->Clear_CameraManager(ENUM_CLASS(m_pClientInstance->Get_CurrLevel()));
#ifdef _DEBUG
			m_pClientInstance->CameraTool_Clear();
#endif
			m_pClientInstance->Set_PrevLevel(m_pClientInstance->Get_CurrLevel());
			m_pClientInstance->Set_CurrLevel(m_eNextLevelID);
		}

		CLevel* pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL::TITLE:
 			pNewLevel = CLevel_Title::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::HEINMACH:
			m_pGameInstance->DeleteOctree();
			m_pGameInstance->CreateOctree({ 260.f, 0.f, 215.f }, 1000.f, 3);
			pNewLevel = CLevel_HeinMach::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::CREVICE:
			pNewLevel = CLevel_Crevice::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::EMBARS:
			pNewLevel = CLevel_Embars::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::VIPER:
			pNewLevel = CLevel_Viper::Create(m_pDevice, m_pContext);
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
	Safe_Release(m_pClientInstance);
}
