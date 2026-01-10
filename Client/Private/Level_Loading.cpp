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
#include "Level_Training.h"

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
    m_pGameInstance->Set_NextLevelID(ENUM_CLASS(eNextLevelID));
    m_pGameInstance->DeleteOctree();
	m_pGameInstance->Destroy_Jolt();
	m_pGameInstance->Initialize_Jolt(ENUM_CLASS(COLLISION_LAYER::END));
    m_pGameInstance->Decal_Clear();
    m_pGameInstance->MapDecal_Clear();
    m_pGameInstance->Decal_OnOff(false);
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
    if (!m_isPlayerSound)
    {
        m_isPlayerSound = true;
        m_pGameInstance->PlaySoundLoop(TEXT("bgm_Loading.wav"), 0.5f);
    }

	m_pLoader->Update();

	if (true == m_pLoader->isFinished() && m_eLoadingState == LOADING_STATE::END)
	{
        m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), { EventObject::OnEvent() });
		m_eLoadingState = LOADING_STATE::UI_ON;
	}
	else if (m_eLoadingState == LOADING_STATE::UI_ON)
	{
		static_cast<CUI_Loading*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Loading")))->Finsh_UI();
		m_eLoadingState = LOADING_STATE::INPUT;
	}
	else if(m_eLoadingState == LOADING_STATE::INPUT && m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::UI))
	{
        m_pGameInstance->StopByKey(TEXT("UI_Title_Seq_Logo_Action.wav"));
        m_pGameInstance->PlaySoundOnce(TEXT("UI_Title_Seq_Logo_Action.wav"));
		CClientInstance::GetInstance()->Fade_Out([this]() {this->Complete(); });
	}
	else if (m_eLoadingState == LOADING_STATE::NEXTLEVEL)
	{
		m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);
		m_eLoadingState = LOADING_STATE::END;

		static_cast<CUI_Loading*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Loading")))->Off_Panel();
		if (m_pClientInstance->Get_CurrLevel() != m_eNextLevelID)
		{
            Ready_CleanImgUI();
			m_pClientInstance->Clear_CameraManager(ENUM_CLASS(m_pClientInstance->Get_CurrLevel()));
            m_pClientInstance->Clear_CurrentKey_BGM();
#ifdef _DEBUG
			m_pClientInstance->CameraTool_Clear();
#endif
			m_pClientInstance->Set_PrevLevel(m_pClientInstance->Get_CurrLevel());
			m_pClientInstance->Set_CurrLevel(m_eNextLevelID);
		}

        m_pGameInstance->StopAll();

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
            m_pGameInstance->CreateOctree({ 0.f, -30.f, 20.f }, 300.f, 3);
			pNewLevel = CLevel_Embars::Create(m_pDevice, m_pContext);
			break;
        case LEVEL::VIPER:
            m_pGameInstance->CreateOctree({ 0.f, 0.f, 150.f }, 300.f, 3);
            pNewLevel = CLevel_Viper::Create(m_pDevice, m_pContext);
            break;
        case LEVEL::TRAINING:
            m_pGameInstance->CreateOctree({ 0.f, 0.f, 0.f }, 50.f, 3);
            pNewLevel = CLevel_Training::Create(m_pDevice, m_pContext);
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
        CHECK_FAILED(Ready_Lights(TEXT("Loading"), LEVEL::LOADING), E_FAIL);
        CHECK_FAILED(Ready_Layer_MapObject_Interactive(TEXT("Layer_MapObject_Interact"), TEXT("Loading"), LEVEL::LOADING), E_FAIL);
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

HRESULT CLevel_Loading::Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
    // Dat 기본 경로
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

    strDataFilePath += TEXT("_lights.dat");

    DWORD dwByte = {};

    HANDLE hFile = CreateFile(strDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return E_FAIL;
    }
    CHECK_EQUAL(INVALID_HANDLE_VALUE, hFile, E_FAIL);

    // 1. 조명의 총 개수
    _uint iLightCnt = {};
    CHECK_FALSE(ReadFile(hFile, &iLightCnt, sizeof(_uint), &dwByte, nullptr), false);

    // 조명 총 개수만큼 순회
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

        // 조명 등록
        m_pGameInstance->Add_Light(szLightTag, ENUM_CLASS(eCurrentLevel), LightDesc, true);

    }

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_MapObject_Interactive(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

    // 동일한 파일명의 _objects.dat 불러오기
    strDataFilePath += TEXT("_interactive.dat");

    DWORD dwByte = {};

    HANDLE hFile = CreateFile(strDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return E_FAIL;
    }
    CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("데이터 파일이 없거나 박준영 문제"), E_FAIL);

    // 1. 오브젝트의 총 개수
    _uint iObjectCnt = {};
    CHECK_FALSE(ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

    // 오브젝트 총 개수만큼 순회
    for (_uint i = 0; i < iObjectCnt; ++i)
    {
        CProp_Interactive::PROP_INTERACTIVE_DESC ObjectDesc = {};

        ObjectDesc.eLevel = eCurrentLevel;

        // 2. 프로토 타입 태그 길이 불러오기
        _uint iPrototypeTagLen = {};
        CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

        // 3. 프로토 타입 태그 이름 불러오기
        _tchar szPrototypeTag[MAX_PATH] = {};
        CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

        // 불러온 태그 카피
        memcpy(ObjectDesc.szModelName, szPrototypeTag, sizeof(ObjectDesc.szModelName));

        // 4. 객체당 월드 행렬 때오기
        _float4x4 WorldMatrix = {};
        CHECK_FALSE(ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr), E_FAIL);

        ObjectDesc.WorldMatrix = WorldMatrix;

        // 5. 상호 작용 타입 불러오기
        INTERACTIVE_TYPE eType = {};
        CHECK_FALSE(ReadFile(hFile, &eType, sizeof(INTERACTIVE_TYPE), &dwByte, nullptr), E_FAIL);
        CHECK_EQUAL_MSG(INTERACTIVE_TYPE::END, eType, TEXT("맵 에디터에서 상호 작용 타입 미지정"), false);


        switch (eType)
        {
        case INTERACTIVE_TYPE::CHECKPOINT:
        {
            _int iBladeNexusID = {};
            CHECK_FALSE(ReadFile(hFile, &iBladeNexusID, sizeof(_int), &dwByte, nullptr), E_FAIL);
            ObjectDesc.pOtherDesc = &iBladeNexusID;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LOADING), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Loading_BladeNexus"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        default:
            MSG_BOX(TEXT("잉 있으면 안되는디"));
            break;
        }
    }

    CloseHandle(hFile);

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
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::CAMERA), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC_TRIGGER), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MAP_MOVE_PLATFORM), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MAP_DEST), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MAP_CHUNK), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::CONTROLLER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::INTERACT_OBJECT), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::CLOTH), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::CLOTHBODY), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::PET), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MONSTER_SEARCH), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
#ifdef  _DEBUG
    m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::TEST), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
#endif //  _DEBUG


    // PLAYER
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

    // PLAYER SEARCH
  //  m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
   // m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH), ENUM_CLASS(JOLT_BP_LAYER::TRIGGER));

    // MONSTER SEARCH
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER_SEARCH), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

    // MAP_STATIC
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC), ENUM_CLASS(JOLT_BP_LAYER::TRIGGER));

    // MAP_INTERACT
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

    // CAMERA
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::CAMERA), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

    // MAP_STATIC_TRIGGER
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC_TRIGGER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC_TRIGGER), ENUM_CLASS(JOLT_BP_LAYER::TRIGGER));

    //MAP_MOVE_PLATFORM
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_MOVE_PLATFORM), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

    //MAP_DEST
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_DEST), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

    //MAP_CHUNK
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MAP_CHUNK), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));

    //CONTROLLER
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::CONTROLLER), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::CONTROLLER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));    

    //INTERACT_OBJECT
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::INTERACT_OBJECT), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::INTERACT_OBJECT), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));

    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::CLOTH), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::CLOTHBODY), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PET), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PET), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));

#ifdef  _DEBUG
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::TEST), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::TEST), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
    m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::TEST), ENUM_CLASS(JOLT_BP_LAYER::TRIGGER));
#endif //  _DEBUG


	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MONSTER));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC_TRIGGER));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MAP_MOVE_PLATFORM));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MAP_DEST));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::INTERACT_OBJECT));

    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK), ENUM_CLASS(COLLISION_LAYER::MONSTER));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK), ENUM_CLASS(COLLISION_LAYER::MAP_DEST));

    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH), ENUM_CLASS(COLLISION_LAYER::MONSTER));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER_SEARCH), ENUM_CLASS(COLLISION_LAYER::PLAYER));

    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK), ENUM_CLASS(COLLISION_LAYER::PLAYER));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK), ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK), ENUM_CLASS(COLLISION_LAYER::MAP_DEST));
    
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC_TRIGGER));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(COLLISION_LAYER::MAP_DEST));

	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::CAMERA), ENUM_CLASS(COLLISION_LAYER::MONSTER));

    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC), ENUM_CLASS(COLLISION_LAYER::MAP_CHUNK));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC), ENUM_CLASS(COLLISION_LAYER::INTERACT_OBJECT));

    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::CONTROLLER), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::CONTROLLER), ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::CONTROLLER), ENUM_CLASS(COLLISION_LAYER::PLAYER));

    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::CLOTH), ENUM_CLASS(COLLISION_LAYER::CLOTHBODY));

    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PET), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PET), ENUM_CLASS(COLLISION_LAYER::MAP_STATIC_TRIGGER));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PET), ENUM_CLASS(COLLISION_LAYER::MAP_DEST));  
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PET), ENUM_CLASS(COLLISION_LAYER::MAP_MOVE_PLATFORM));
    m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PET), ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK));

    // 레이캐스트 용
    m_pGameInstance->Set_ObjectLayerFilter(ENUM_CLASS(COLLISION_LAYER::MAP_STATIC), true);
    m_pGameInstance->Set_PhysicsSystem();

    return S_OK;
}

HRESULT CLevel_Loading::Ready_CleanImgUI()
{
#ifdef _DEBUG
        m_pGameInstance->CleanMenu(TEXT("Client"));
#endif
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
