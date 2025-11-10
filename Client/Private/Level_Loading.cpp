#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Level_Title.h"
#include "Level_HeinMach.h"
#include "Level_Crevice.h"
#include "Level_Embars.h"
#include "Level_Viper.h"
#include "Level_Test.h"

#pragma region MAP OBJECT HEADER
#include "MapObject_Header.h"
#pragma endregion

#include "Camera_Free.h"
#include "UI_Loading.h"
#include "ClientInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
	, m_pClientInstance { CClientInstance::GetInstance() }
{
	Safe_AddRef(m_pClientInstance);
}

void CLevel_Loading::Complete()
{
	m_eLoadingState = LOADING_STATE::NEXTLEVEL;
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;
    m_pGameInstance->DeleteOctree();
	m_pGameInstance->Destroy_Jolt();
	m_pGameInstance->Initialize_Jolt(ENUM_CLASS(COLLISION_LAYER::END));
	Ready_ObjectLayer();
	
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_GameObjects()))
		return E_FAIL;

	/* 다음 레벨을 위한 로딩작업을 시작 한다. */
	if (FAILED(Ready_LoadingThread()))
		return E_FAIL;
	CClientInstance::GetInstance()->Fade_In();
	m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
	static_cast<CUI_Loading*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Loading")))->On_Panel();
	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	m_pLoader->Update();

	if (true == m_pLoader->isFinished() && m_eLoadingState == LOADING_STATE::END)
	{
		m_eLoadingState = LOADING_STATE::UI_ON;
	}
	else if (m_eLoadingState == LOADING_STATE::UI_ON)
	{
		static_cast<CUI_Loading*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Loading")))->Finsh_UI();
		m_eLoadingState = LOADING_STATE::INPUT;
	}
	else if(m_eLoadingState == LOADING_STATE::INPUT && m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::UI))
	{
		CClientInstance::GetInstance()->Fade_Out([this]() {this->Complete(); });
	}
	else if (m_eLoadingState == LOADING_STATE::NEXTLEVEL)
	{
		m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);
		m_eLoadingState = LOADING_STATE::END;

		static_cast<CUI_Loading*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Loading")))->Off_Panel();
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
			m_pGameInstance->CreateOctree({ 260.f, 0.f, 215.f }, 1500.f, 3);
			pNewLevel = CLevel_HeinMach::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::TEST:
			m_pGameInstance->CreateOctree({ 0.f, 0.f, 0.f }, 200.f, 3);
			pNewLevel = CLevel_Test::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::CREVICE:
			m_pGameInstance->CreateOctree({ 0.f, 0.f, 20.f }, 200.f, 3);
			pNewLevel = CLevel_Crevice::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::EMBARS:
			pNewLevel = CLevel_Embars::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::VIPER:
			m_pGameInstance->CreateOctree({ 0.f, 0.f, 150.f }, 300.f, 3);
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
	if (LEVEL::TITLE != m_eNextLevelID)
	{
		CHECK_FAILED(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);
		CHECK_FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"), TEXT("Loading"), LEVEL::LOADING), E_FAIL);
		CHECK_FAILED(Ready_Layer_Cloud(TEXT("Layer_Sky"), TEXT("Loading"), LEVEL::LOADING), E_FAIL);
	}

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC CameraDesc = {};

	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(30.f);

	CameraDesc.fMouseSensor = 0.2f;

	CameraDesc.fFovy = XMConvertToRadians(45.f);

	CameraDesc.vEye = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vAt = _float4(-0.6f, 0.5f, 1.f, 1.f);

	CameraDesc.fFar = 10000.f;
	CameraDesc.fNear = 0.1f;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LOADING), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"), TIME_CHANNEL::WORLD, &CameraDesc), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_Sky(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
	_wstring strDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		strDataFilePath += TEXT("HeinMach/");
		break;
	case KHAZAN_MAP::CREVICE:
		strDataFilePath += TEXT("Crevice/");
		break;
	case KHAZAN_MAP::EMBARS:
		strDataFilePath += TEXT("Embars/");
		break;
	case KHAZAN_MAP::VIPER:
		strDataFilePath += TEXT("Viper/");
		break;
	default:
		break;
	}

	strDataFilePath += pDataFileName;

	strDataFilePath += TEXT("_sky.dat");

	CSkySphere::SKY_SPHERE_DESC SkySphereDesc = {};

	SkySphereDesc.eLevel = eCurrentLevel;

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(strDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		// 파일 없으면 생성
		SkySphereDesc.SkyDesc.vNebulaColorR = { 0.1f, 0.1f, 0.1f };
		SkySphereDesc.SkyDesc.vNebulaColorG = { 0.1f, 0.1f, 0.1f };
		SkySphereDesc.SkyDesc.vNebulaColorB = { 0.1f, 0.1f, 0.1f };
		SkySphereDesc.SkyDesc.fStarStrength = { 1.5f };
		SkySphereDesc.SkyDesc.fMoonSize = { 0.45f };
		SkySphereDesc.SkyDesc.vMoonDirection = { -0.8f, 0.55f, 1.f };
		SkySphereDesc.SkyDesc.vMoonColor = { 0.8f, 0.2f, 0.2f };
		SkySphereDesc.SkyDesc.fMoonIntensity = { 1.f };

		SkySphereDesc.fRotationPerSec = XMConvertToRadians(0.f);

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkySphere"), TIME_CHANNEL::WORLD, &SkySphereDesc), E_FAIL);
	}
	else
	{
		CHECK_FAILED(ReadFile(hFile, &SkySphereDesc.SkyDesc, sizeof(SKY_DESC), &dwByte, nullptr), E_FAIL);

		SkySphereDesc.fRotationPerSec = XMConvertToRadians(0.f);

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkySphere"), TIME_CHANNEL::WORLD, &SkySphereDesc), E_FAIL);

		CloseHandle(hFile);
	}

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_Cloud(const _wstring & strLayerTag, const _tchar * pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
	_wstring strDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		strDataFilePath += TEXT("HeinMach/");
		break;
	case KHAZAN_MAP::CREVICE:
		strDataFilePath += TEXT("Crevice/");
		break;
	case KHAZAN_MAP::EMBARS:
		strDataFilePath += TEXT("Embars/");
		break;
	case KHAZAN_MAP::VIPER:
		strDataFilePath += TEXT("Viper/");
		break;
	default:
		break;
	}

	strDataFilePath += pDataFileName;

	strDataFilePath += TEXT("_cloud.dat");

	CCloudSphere::CLOUD_SPHERE_DESC CloudSphereDesc = {};

	CloudSphereDesc.eLevel = eCurrentLevel;

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(strDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloudSphereDesc.fRotationPerSec = XMConvertToRadians(0.f);

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_CloudSphere"), TIME_CHANNEL::WORLD, &CloudSphereDesc), E_FAIL);
	}
	else
	{
		CHECK_FAILED(ReadFile(hFile, &CloudSphereDesc.CloudDesc, sizeof(CLOUD_DESC), &dwByte, nullptr), E_FAIL);

		CloudSphereDesc.fRotationPerSec = XMConvertToRadians(0.f);

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_CloudSphere"), TIME_CHANNEL::WORLD, &CloudSphereDesc), E_FAIL);

		CloseHandle(hFile);
	}

	return S_OK;
}

HRESULT CLevel_Loading::Ready_LoadingThread()
{
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_ObjectLayer()
{
    // BP
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::CAMERA), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(JOLT_BP_LAYER::TRIGGER));

    // MONSTER
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(JOLT_BP_LAYER::TRIGGER));

    // MONSTER ATTACK
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK), ENUM_CLASS(JOLT_BP_LAYER::TRIGGER));

    // PLAYER ATTACK
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK), ENUM_CLASS(JOLT_BP_LAYER::TRIGGER));

    // MAP_STATIC
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC), ENUM_CLASS(JOLT_BP_LAYER::TRIGGER));

    // MAP_INTERACT
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

    // CAMERA
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::CAMERA), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MONSTER));    
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK), ENUM_CLASS(COLLISION_LAYER::MONSTER));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK), ENUM_CLASS(COLLISION_LAYER::PLAYER));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::CAMERA), ENUM_CLASS(COLLISION_LAYER::MONSTER));

    m_pGameInstance->Set_ObjectLayerFilter(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC), true);
    m_pGameInstance->Set_PhysicsSystem();

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
