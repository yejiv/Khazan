#include "Level_Viper.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Level_Loading.h"

#pragma region MAP OBJECT
#include "MapObject_Header.h"
#pragma endregion

#pragma region MONSTER
#include "Viper.h"
#pragma endregion


#include "Player.h"
#include "Khazan_Spear.h"
#include "Khazan_GSword.h"
#include "Camera_Compre.h"
#include "Sequence_Viper_SecondPhase.h"
#include "Sequence_Viper_CutScene.h"

#pragma region ITEM
#include "Interaction_Item.h"
#pragma endregion

CLevel_Viper::CLevel_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_pClientInstance(CClientInstance::GetInstance())
{
	Safe_AddRef(m_pClientInstance);
}

HRESULT CLevel_Viper::Initialize()
{   
    // 플레이어, 카메라, 트리거

    CHECK_FAILED(Ready_Layer_Effect(TEXT("Layer_Effect")), E_FAIL);

    CHECK_FAILED(Ready_Layer_Player(TEXT("Layer_Creature_Player")), E_FAIL);
    CHECK_FAILED(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);
    CHECK_FAILED(Ready_Trigger(TEXT("Layer_Trigger"), TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);
       
    // 우선 맵 오브젝트 서브 레벨 로드
    
    //CHECK_FAILED(Ready_Lights(TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);
    CHECK_FAILED(Ready_Lights(), E_FAIL);

    CHECK_FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"), TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);

    CHECK_FAILED(Ready_Layer_Cloud(TEXT("Layer_Sky"), TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);

    // 맵 오브젝트 서브 레벨 로드
    for (_uint i = 0; i < VIPER_SUBLV; ++i)
    {        
        CHECK_FAILED(Ready_Layer_Monster_SubLV(TEXT("Layer_Viper"), TEXT("Viper"), i, LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);
    }

    CHECK_FAILED(Ready_Sequence(), E_FAIL);

    // 맵 오브젝트 서브 레벨 로드
    for (_uint i = 0; i < VIPER_SUBLV; ++i)
    {
        CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("Viper"), i, LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);
    }
    //CHECK_FAILED(Ready_Layer_MapObject_Interactive(TEXT("Layer_MapObject_Interact"), TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);
    CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Laye0r_MapObject_Inst"), TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);

    //CHECK_FAILED(Ready_Layer_Monster_Viper(TEXT("Layer_Monster")), E_FAIL);
    //CClientInstance::GetInstance()->Fade_Out();
    CHECK_FAILED(Ready_Item(), E_FAIL);
    if (!Wait_All_Futures())
        return E_FAIL;
    
    CHECK_FAILED(Ready_Layer_Decal(), E_FAIL);

    // 바이퍼 레벨 초기 포그, 스카이박스, 클라우드 세팅
    CHECK_FAILED(Ready_Shader_Settings(), E_FAIL);

    m_futures.clear();

    CClientInstance::GetInstance()->Fade_In();

    CClientInstance::GetInstance()->Set_Volume_BGM(0.65f);
    CClientInstance::GetInstance()->BGM_Viper_Entry();

    //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_snow1"), XMVectorSet(0.f, 0.f, 0.f, 1.f));
    m_pClientInstance->Set_PlayerInput(true);
    return S_OK;
}

void CLevel_Viper::Update(_float fTimeDelta)
{
	//if (GetKeyState(VK_RETURN) & 0x8000)
	//{
	//	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::HEINMACH))))
	//		return;
	//}
	//if (GetKeyState(VK_RETURN) & 0x8000)
	//	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
	//		return;
	//}

    if (m_pGameInstance->Key_Down(DIK_F1))
    {
        m_pClientInstance->Camera_Switch_CameraMode(CAMERATYPE::FREE);
    }
    else if (m_pGameInstance->Key_Down(DIK_F2))
    {
        m_pClientInstance->Camera_Switch_CameraMode(CAMERATYPE::PLAYER);
    }

   if (m_pGameInstance->Key_Down(DIK_END, INPUT_TYPE::FORCE))
    {
        CViper* pViper = dynamic_cast<CViper*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Viper")));
        CKhazan_GSword* pKhazan = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Creature_Player")));
        //CSequence_Viper_CutScene* pSequence = CSequence_Viper_CutScene::Create(pViper, pKhazan);
        CSequence_Viper_SecondPhase* pSequence = CSequence_Viper_SecondPhase::Create(pViper, pKhazan);

        SEQ_REQ_PLAY_DESC tPlayDesc{};
        tPlayDesc.tId.iSeq = 100010;
        tPlayDesc.pAsset = L"Viper_CutScene";
        tPlayDesc.fStartTime = 0.f;

        m_pGameInstance->SEQ_AdoptAndPlay(pSequence, tPlayDesc);
    }

    if (m_pGameInstance->Key_Down(DIK_0, INPUT_TYPE::FORCE))
    {
        m_pClientInstance->Camera_Force_AniEnd();
        m_pClientInstance->Camera_Switch_CameraMode(CAMERATYPE::FREE);
    }
	return;
}

HRESULT CLevel_Viper::Render()
{
	SetWindowText(g_hWnd, TEXT("바이퍼 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_UI()
{

	return S_OK;
}
HRESULT CLevel_Viper::Ready_Layer_Player(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag,
	//	ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Khazan_Spear"), TIME_CHANNEL::PLAYER, &Desc)))
	//	return E_FAIL;

    CGameObject::GAMEOBJECT_DESC Desc;
    Desc.iLevelIndex = ENUM_CLASS(LEVEL::VIPER);

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag,
        ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Khazan_GSword"), TIME_CHANNEL::PLAYER, &Desc)))
        return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_Camera(const _wstring& strLayerTag)
{
    CCamera_Compre::CAMERA_COMPRE_DESC	PlayerCameraDesc{};

    PlayerCameraDesc.vEye = _float4(0.51f, 2.08f, -3.94f, 1.f);
    PlayerCameraDesc.vAt = _float4(-0.13f, -0.12f, 0.98f, 1.f);
    PlayerCameraDesc.fFovy = XMConvertToRadians(60.0f);
    PlayerCameraDesc.fNear = 0.1f;
    PlayerCameraDesc.fFar = 6000.f;
    PlayerCameraDesc.fSpeedPerSec = 10.f;
    PlayerCameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    PlayerCameraDesc.fMouseSensor = 0.2f;
    PlayerCameraDesc.iCameraType = ENUM_CLASS(CAMERATYPE::PLAYER);

    CCamera_Compre* pCamera_Player = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Camera_Compre"), &PlayerCameraDesc));
    pCamera_Player->Set_IsActive(true);
    CGameObject* pPlayer = m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Creature_Player"));
    pCamera_Player->Set_ObjMatrix(dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr());

    static_cast<CKhazan_GSword*>(pPlayer)->Set_Camera(pCamera_Player);

    m_pClientInstance->Add_Camera(ENUM_CLASS(LEVEL::VIPER), pCamera_Player);

    m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag, pCamera_Player);

    m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::VIPER), ENUM_CLASS(CAMERATYPE::PLAYER));


    return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_Sky(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_Viper::Ready_Layer_Cloud(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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


HRESULT CLevel_Viper::Ready_Layer_MapObject_SubLV(const _wstring& strLayerTag, const _tchar* pDataFileName, _uint iSubLV, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

	_tchar szDataFilePath[MAX_PATH] = {};

	wsprintf(szDataFilePath, TEXT("%s_LV%d_object.dat"), strDataFilePath.c_str(), iSubLV);

	strDataFilePath = szDataFilePath;

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(strDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("데이터 파일이 없거나 박준영 문제"), E_FAIL);

	// 1. 오브젝트의 총 개수
	_uint iObjectCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

    _uint iDestIndex = { 0 };    
	// 오브젝트 총 개수만큼 순회
	for (_uint i = 0; i < iObjectCnt; ++i)
	{
		CProp_Object::PROP_OBJECT_DESC ObjectDesc = {};

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

		// 5. 객체의 속성 불러오기
		MAPOBJECT_PROPERTIES PropProperties = {};
		CHECK_FALSE(ReadFile(hFile, &PropProperties, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr), false);

		ObjectDesc.Properties = PropProperties;
       
        if (wcscmp(ObjectDesc.szModelName, L"Prototype_Component_Model_WP_TDL_Bridge_Collision_004") == 0)
        {
            CProp_Destructible::PROP_DEST_DESC ObeliskDesc = {};

            ObeliskDesc.eLevel = eCurrentLevel;

            MAPOBJECT_PROPERTIES PropProperties4 = {};

            ObeliskDesc.Properties = PropProperties4;
            ObeliskDesc.WorldMatrix = WorldMatrix;
            ObeliskDesc.WorldMatrix._42 -= 400.f;
            ObeliskDesc.iIndex = iDestIndex;
            CObelisk* pObelisk = dynamic_cast<CObelisk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Obelisk"), &ObeliskDesc));
            m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag, pObelisk);            
            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
                ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Obelisk"), TIME_CHANNEL::WORLD, &ObeliskDesc)))
                return E_FAIL;

            iDestIndex++;

            continue;
        }

		if (iSubLV == 0)
		{

			// 일단 단일 오브젝트로 배치하고 추후에 인스턴스, 인터렉티브, 다이나믹 으로 나누겠습니다.
			lock_guard<mutex> lock(m_Mutex);
			CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc));
			if (!pObject)
				return E_FAIL;
			_bool isAdd = m_pGameInstance->AddStaticObject(pObject, { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43 }, 3.f);
			//Safe_Release(pObject);
			/*CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(CurLevel), LayerTag,
				ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc), E_FAIL);*/
			if (isAdd)
				Safe_Release(pObject);
			else
				return E_FAIL;
		}
		else {
			lock_guard<mutex> lock(m_Mutex);
			CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc));
			if (!pObject)
				return E_FAIL;
			_bool isAdd = m_pGameInstance->AddStaticObject(pObject, { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43 }, 3.f);
			//Safe_Release(pObject);
			/*CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(CurLevel), LayerTag,
				ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc), E_FAIL);*/
			if (isAdd)
				Safe_Release(pObject);
			else
				return E_FAIL;
		}

	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_MapObject_Interactive(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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


		// 일단 단일 오브젝트로 배치하고 추후에 인스턴스, 인터렉티브, 다이나믹 으로 나누겠습니다.
		//m_pGameInstance->Add_FireTask([this, objDesc = ObjectDesc, curLevel = eCurrentLevel, enumType = eType]() mutable {

		//	if (INTERACTIVE_TYPE::CHECKPOINT == enumType)
		//	{
		//	CHECK_FAILED(
		//		m_pGameInstance->Add_GameObject_ToLayer(
		//			ENUM_CLASS(objDesc.eLevel),
		//			TEXT("Layer_MapObject_Interact"),
		//			ENUM_CLASS(curLevel),
		//			TEXT("Prototype_GameObject_Prop_BladeNexus"),
		//			&objDesc // 캡처된 값의 주소 -> 안전
		//		),
		//		E_FAIL);
		//	}

		//	});

		switch (eType)
		{
		case INTERACTIVE_TYPE::CHECKPOINT:
		{
			_int iBladeNexusID = {};
			CHECK_FALSE(ReadFile(hFile, &iBladeNexusID, sizeof(_int), &dwByte, nullptr), E_FAIL);
			ObjectDesc.pOtherDesc = &iBladeNexusID;
			CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_BladeNexus"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
			break;
		}
		case INTERACTIVE_TYPE::CHEST:
		{
			BOX_ITEMS ItemBoxDesc = {};
			CHECK_FALSE(ReadFile(hFile, &ItemBoxDesc, sizeof(BOX_ITEMS), &dwByte, nullptr), E_FAIL);
			ObjectDesc.pOtherDesc = &ItemBoxDesc;
			CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_BigChest"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
			break;
		}
		case INTERACTIVE_TYPE::TOMBSTONE:
		{
			CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_TombStone"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
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

HRESULT CLevel_Viper::Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

	// 동일한 파일명의 _inst.dat 불러오기
	strDataFilePath += TEXT("_inst.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(strDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("데이터 파일이 없거나 박준영 문제"), E_FAIL);

	// 1. 오브젝트의 총 개수
	_uint iObjectCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

	// 오브젝트 총 개수만큼 순회
	for (_uint i = 0; i < iObjectCnt; ++i)
	{
		CProp_Object::PROP_OBJECT_DESC ObjectDesc = {};

		ObjectDesc.eLevel = eCurrentLevel;

		// 2. 프로토 타입 태그 길이 불러오기
		_uint iPrototypeTagLen = {};
		CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 3. 프로토 타입 태그 이름 불러오기
		_tchar szPrototypeTag[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

		// 불러온 태그 카피
		memcpy(ObjectDesc.szModelName, szPrototypeTag, sizeof(ObjectDesc.szModelName));

		// 4. 객체의 속성 불러오기
		MAPOBJECT_PROPERTIES PropProperties = {};
		CHECK_FALSE(ReadFile(hFile, &PropProperties, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr), false);

		ObjectDesc.Properties = PropProperties;

		// 인스턴스 객체 슈웃
		//m_pGameInstance->Add_FireTask([this, objDesc = ObjectDesc, curLevel = eCurrentLevel]() mutable {
		//	CHECK_FAILED(
		//		m_pGameInstance->Add_GameObject_ToLayer(
		//			ENUM_CLASS(objDesc.eLevel),
		//			TEXT("Layer_MapObject_Inst"),
		//			ENUM_CLASS(curLevel),
		//			TEXT("Prototype_GameObject_Prop_Static"),
		//			&objDesc // 캡처된 값의 주소 -> 안전
		//		),
		//		E_FAIL
		//	);
		//	});
		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Static"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Viper::Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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
		/*m_pGameInstance->Add_FireTask([this, szLightTag = szLightTag, eCurrentLevel = ENUM_CLASS(eCurrentLevel), LightDesc = LightDesc]() mutable {
			m_pGameInstance->Add_Light(szLightTag, ENUM_CLASS(eCurrentLevel), LightDesc, true);
			return S_OK;
			});*/

	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Viper::Ready_Lights()
{
    // Test MainLight
    LIGHT_DESC LightDesc = {};
    LightDesc.eType = LIGHT_DESC::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("MainLight"), ENUM_CLASS(LEVEL::VIPER), LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(-30.103f, -27.f, 185.861f, 1.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 4.5f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Viper_CutScene_PointLight"), ENUM_CLASS(LEVEL::VIPER), LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.0f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.fRange = 25.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Player_PointLight_Orange"), ENUM_CLASS(LEVEL::VIPER), LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 10.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Player_PointLight_White"), ENUM_CLASS(LEVEL::VIPER), LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 15.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Player_PointLight_Gray"), ENUM_CLASS(LEVEL::VIPER), LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(-30.103f, -28.f, 185.861f, 1.f);
    LightDesc.vDiffuse = _float4(2.2f, 2.2f, 2.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 0.8f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 2.5f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Viper_Core"), ENUM_CLASS(LEVEL::VIPER), LightDesc, false)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 3.5f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Viper_TwinBlade_R"), ENUM_CLASS(LEVEL::VIPER), LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vDiffuse = _float4(0.9f, 0.85f, 0.7f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 15.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Viper_Thunder"), ENUM_CLASS(LEVEL::VIPER), LightDesc, false)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(-30.2f, -29.8f, 173.7f, 1.f);
    LightDesc.vDiffuse = _float4(0.9f, 0.8f, 0.7f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 100.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Viper_Thunder_Ambient"), ENUM_CLASS(LEVEL::VIPER), LightDesc, false)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Viper::Ready_Trigger(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
	// Json 기본 경로
	_wstring strJsonFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		strJsonFilePath += TEXT("HeinMach/");
		break;
	case KHAZAN_MAP::CREVICE:
		strJsonFilePath += TEXT("Crevice/");
		break;
	case KHAZAN_MAP::EMBARS:
		strJsonFilePath += TEXT("Embars/");
		break;
	case KHAZAN_MAP::VIPER:
		strJsonFilePath += TEXT("Viper/");
		break;
	default:
		break;
	}

	strJsonFilePath += pDataFileName;

	strJsonFilePath += TEXT("_trigger.json");

	ifstream ifs(strJsonFilePath);
	if (!ifs.is_open())
	{
		MSG_BOX(TEXT("Json read failed"));
		return E_FAIL;
	}

	JSON j = {};
	ifs >> j;
	ifs.close();

	JSON_MAP_TRIGGER_DATA TriggerData = j.get<JSON_MAP_TRIGGER_DATA>();

	_uint iNumTrigger = TriggerData.iNumTrigger;

	for (_uint i = 0; i < iNumTrigger; ++i)
	{
		CTrigger::TRIGGER_DESC TriggerDesc = {};

		_float4x4 WorldMatrix = {};
		memcpy(&TriggerDesc.WorldMatrix, &TriggerData.WorldMatrix[i], sizeof(_float4x4));

		TriggerDesc.strTriggerKey = TriggerData.TriggerKey[i];

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Viper_Trigger"), TIME_CHANNEL::MAP, &TriggerDesc), E_FAIL);
	}

	return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_Monster_SubLV(const _wstring& strLayerTag, const _tchar* pDataFileName, _uint iSubLV, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
    _wstring strJsonFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

    switch (eMap)
    {
    case Client::KHAZAN_MAP::HEINMACH:
        break;
    case Client::KHAZAN_MAP::CREVICE:
        break;
    case Client::KHAZAN_MAP::EMBARS:
        break;
    case Client::KHAZAN_MAP::VIPER:
        strJsonFilePath += TEXT("Viper/");
        break;
    }


    strJsonFilePath += pDataFileName;
    _tchar szJsonFilePath[MAX_PATH] = {};

    wsprintf(szJsonFilePath, TEXT("%s_LV%d_spawn.json"), strJsonFilePath.c_str(), iSubLV);

    strJsonFilePath = szJsonFilePath;

    ifstream ifs(strJsonFilePath);
    if (!ifs.is_open())
    {
        // 해당 서브 레벨에 몬스터 정보가 존재하지 않음 ( 몬스터 키값, 월드 행렬 등등 )
        return S_OK;
    }

    JSON j = {};
    ifs >> j;
    ifs.close();

    JSON_MAP_MONSTER_SPAWN_DATA MonsterData = j.get<JSON_MAP_MONSTER_SPAWN_DATA>();

    _uint iNumMonster = MonsterData.iNumMonster;

    for (_uint i = 0; i < iNumMonster; ++i)
    {
        _float4x4 WorldMatrix = {};
        memcpy(&WorldMatrix, &MonsterData.WorldMatrix[i], sizeof(_float4x4));

        if ("Viper" == MonsterData.MonsterKey[i])
        {
            WorldMatrix._41 = -37.938f;
            WorldMatrix._42 = -15.453f;       
            WorldMatrix._43 = 223.393f;

            CMonster::MONSTER_DESC MonsterDesc{};
            MonsterDesc.fAttack = 10.f;
            MonsterDesc.fMaxHP = 500.f;
            MonsterDesc.fMaxStamina = 100.f;
            MonsterDesc.fMoveSpeed = 10.f;
            MonsterDesc.fSpeedPerSec = 3.f;
            MonsterDesc.fRotationPerSec = 180.f;
            MonsterDesc.iLevelIndex = ENUM_CLASS(LEVEL::VIPER);
            MonsterDesc.WorldMatrix = WorldMatrix;
            MonsterDesc.strName = MonsterData.MonsterKey[i];

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag,
                ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Monster_Viper"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
                return E_FAIL;
        }
    }




    return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_Monster_Viper(const _wstring& strLayerTag)
{
     // CMonster::MONSTER_DESC MonsterDesc{};
     // MonsterDesc.fAttack = 10.f;
     // MonsterDesc.fMaxHP = 1000.f;
     // MonsterDesc.fMaxStamina = 250.f;
     // MonsterDesc.fMoveSpeed = 10.f;
     // MonsterDesc.fSpeedPerSec = 3.f;
     // MonsterDesc.fRotationPerSec = 180.f;
     // //XMStoreFloat4x4(&MonsterDesc.WorldMatrix,XMMatrixIdentity());
     // XMStoreFloat4x4(&MonsterDesc.WorldMatrix, XMMatrixIdentity());
     // MonsterDesc.WorldMatrix.m[3][0] = -32.365f;
     // MonsterDesc.WorldMatrix.m[3][1] = -26.5f;
     // MonsterDesc.WorldMatrix.m[3][2] = 182.409f;
     // MonsterDesc.strName = "Viper";
     // MonsterDesc.iLevelIndex = ENUM_CLASS(LEVEL::VIPER);
     // if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag,
     //     ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Monster_Viper"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
     //     return E_FAIL;

     CMonster::MONSTER_DESC MonsterDesc{};
     MonsterDesc.fAttack = 100.f;
     MonsterDesc.fMaxHP = 100.f;
     MonsterDesc.fMaxStamina = 250.f;
     MonsterDesc.fMoveSpeed = 10.f;
     MonsterDesc.fSpeedPerSec = 3.f;
     MonsterDesc.fRotationPerSec = 180.f;
     //XMStoreFloat4x4(&MonsterDesc.WorldMatrix,XMMatrixIdentity());
     XMStoreFloat4x4(&MonsterDesc.WorldMatrix, XMMatrixIdentity());
     MonsterDesc.WorldMatrix.m[3][0] = -32.365f;
     MonsterDesc.WorldMatrix.m[3][1] = -26.5f;
     MonsterDesc.WorldMatrix.m[3][2] = 182.409f;
     
     MonsterDesc.strName = "Halberd";
     MonsterDesc.iLevelIndex = ENUM_CLASS(LEVEL::VIPER);
     if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag,
         ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Monster_Halberd"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
         return E_FAIL;

    return S_OK;
}


HRESULT CLevel_Viper::Ready_Layer_Effect(const _wstring& strLayerTag)
{
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpearWind"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpiralSpear_SpearFX"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Blust"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Blust2"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Blust3"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Blust4"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Blust5"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Blust6"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Stamp"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("BlustSmall"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Spawn"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("BloodHit"), 100);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Open"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Snow"), 4);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Snow_Once"), 20);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("GhostKnight"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("GhostKnight_static"), 4);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("GhostKnight_static_connect"), 4);

    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Rot_Start"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Rot_Loop"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Rot_End"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Grap"), 10);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("scream"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_snow1"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_snow2"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("HandTrail_Up"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("HandTrail_V"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("HandTrailLR"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("HandTrailLR2"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("HandTrailRL"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_Land"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_Land_Big"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpearTrailLR"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpearTrailRL"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SphereTrail_V"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_Twinkle_Small"), 50);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_Twinkle_Big"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_blood_loop"), 5);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_blood2_loop"), 5);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_blood_once"), 5);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_blood2_once"), 5);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_mouth_particle"), 5);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_fire2"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_fire3"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_scream_cutscene"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Body_Particle"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Body_Particle_Blust"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("mist1"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("mist2"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("mist3"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("mist4"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("mist5"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_Footprint"), 5);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Soward_Particle_red"), 1);//이거 객체가 들고있는 게 나을텐데
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_Tornado"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_ChangeSnow"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_CutSceen_Land"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Point_Particle_Blust"), 1); 

    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("FerociousMomentum0"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpiningCharger0"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpiningCharger1"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpiningCharger2"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpiningCharger_Smoke"), 50);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpiningCharger_Trail"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Manifest_Strength_Land"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpiningCharger_Smoke_Red"), 50);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("SpiningCharger_Trail_V"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Giant_Hunt_Land"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Giant_Roar"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("DarkShadow_Land_1"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("DarkShadow_Land_2"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Body_Wind"), 4);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("particle"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("particle2"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Inner_Range_Ground"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Dawn_BloodTrail1"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Dawn_BloodTrail2"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("GS_StrongATK"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Ghost_Dark_Shadow_Land"), 1);

    // [Player Ect] 
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Guard"), 100);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("PerfectGaurd"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Lachryma"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Lachryma_Arm"), 3);

    //임시
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Stamp"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::VIPER), TEXT("Halberd_Weapon_Wind"), 3);
    return S_OK;
}

HRESULT CLevel_Viper::Ready_Shader_Settings()
{
    // 그림자
    SHADOW_DESC ShadowDesc{};
    ShadowDesc.fSplit = 35.f;
    ShadowDesc.vLightDir = _float3(0.1f, -1.f, -0.9f);
    ShadowDesc.fBias = 0.001f;
    ShadowDesc.fIntensity = 0.75f;
    m_pGameInstance->Set_ShadowDesc(ShadowDesc);

    // 초기 Fog
    FOG_CONFIG FogConfig{};
    FogConfig.eType = FOG_CONFIG::EXP;
    FogConfig.fDensity = 0.05f;
    FogConfig.fBias = 0.925f;
    FogConfig.vColor = _float4(0.055f, 0.110f, 0.157f, 1.f);
    FogConfig.Noise.isEnable = false;
    FogConfig.isUseHeight = true;
    FogConfig.fBaseHeight = -145.f;
    FogConfig.isUseSubColor = true;
    FogConfig.fSubColorStartHeight = 1154.f;
    FogConfig.vSubColor = _float4(0.235f, 0.318f, 0.341f, 1.f);
    FogConfig.fLightBleedStrength = 0.f;
    m_pGameInstance->Set_FogConfig(FogConfig);

    // 초기 스카이 박스 세팅
    SKY_DESC SkyDesc{};
    SkyDesc.vNebulaColorR = _float3(0.235f, 0.318f, 0.341f);
    SkyDesc.vNebulaColorG = _float3(0.055f, 0.110f, 0.157f);
    SkyDesc.vNebulaColorB = _float3(0.055f, 0.110f, 0.157f);
    SkyDesc.fStarStrength = 0.f;
    SkyDesc.fMoonSize = 0.8f;
    SkyDesc.vMoonDirection = _float3(-0.21f, 0.19f, 1.f);
    SkyDesc.vMoonColor = _float3(0.631f, 0.631f, 0.631f);
    SkyDesc.fMoonIntensity = 2.5f;
    static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 0))->Start_LerpSky(SkyDesc, 0.f);

    // 초기 클라우드 세팅
    CLOUD_DESC CloudDesc{};
    CloudDesc.vCloudColor = _float3(1.f, 1.f, 1.f);
    CloudDesc.fCloudSpeed = 0.25f;
    CloudDesc.fCloudScale = 3.f;
    CloudDesc.fCloudDensity = 0.2f;
    CloudDesc.fCloudLightIntensity = 1.f;
    CloudDesc.vLightDir = _float3(0.f, 1.f, 0.f);
    CloudDesc.fDynamic = 1.f;
    static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 1))->Start_LerpCloud(CloudDesc, 0.f);

    return S_OK;
}

HRESULT CLevel_Viper::Ready_Item()
{
    CGameObject::GAMEOBJECT_DESC desc{};

    desc.iLevelIndex = ENUM_CLASS(LEVEL::VIPER);

    m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Item"), ENUM_CLASS(LEVEL::VIPER), TEXT("Item"), &desc, 10);

    return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_Decal()
{
    // Decal
    if (FAILED(m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Decal"),
        ENUM_CLASS(LEVEL::VIPER), TEXT("Pool_Decal"), nullptr, 100)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Viper::Ready_Sequence()
{

    CViper* pViper = dynamic_cast<CViper*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Viper")));
    CKhazan_GSword* pKhazan = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Creature_Player")));
    CSequence_Viper_SecondPhase* pSequence = CSequence_Viper_SecondPhase::Create(pViper, pKhazan);
    SEQ_REQ_PLAY_DESC desc{};
    pSequence->Initialize(desc);
    m_pClientInstance->Push_Sequence(TEXT("Viper_SecondPhase"), pSequence);

    return S_OK;
}

_bool CLevel_Viper::Wait_All_Futures()
{
    bool all_ok = true;

    for (auto& f : m_futures)
    {
        if (!f.valid())
            continue;

        try
        {
            const HRESULT hr = f.get(); // 딱 1번만
            if (FAILED(hr))
                all_ok = false;
        }
        catch (...)
        {
            all_ok = false;
        }
    }

    m_futures.clear();
    return all_ok;
}

CLevel_Viper* CLevel_Viper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Viper* pInstance = new CLevel_Viper(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Viper"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Viper::Free()
{
	__super::Free();

	Safe_Release(m_pClientInstance);
}