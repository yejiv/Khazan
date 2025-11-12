#include "Level_HeinMach.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Event_Defines.h"
#include "Player.h"
#include "Camera_Compre.h"
#include "Dummy.h"
#include "Monster.h"
#include "ClientInstance.h"
#include "Khazan_Spear.h"
#include "Sequence_HeinMach_Start.h"

#pragma region MAP OBJECT
#include "MapObject_Header.h"
#pragma endregion

#pragma region UI OBJECT
#include "UI_Atlas_Icon.h"
#include "UI_BackGround.h"
#include "Damage_Text.h"
#include "UI_Announce_MapName.h"
#pragma endregion

CLevel_HeinMach::CLevel_HeinMach(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_pClientInstance(CClientInstance::GetInstance())
{
	Safe_AddRef(m_pClientInstance);
}

HRESULT CLevel_HeinMach::Initialize()
{
    CHECK_FAILED(Ready_Layer_UI(), E_FAIL);
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("HeinMach"),
			HEINMACH_1ST_BLADENEXUS, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		return S_OK;
		}));

	m_pGameInstance->Add_FireTask([this]() {
		CHECK_FAILED(Ready_Layer_Player(TEXT("Layer_Creature_Player")), E_FAIL);
		if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
			return E_FAIL;
		CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("HeinMach"),
			HEINMACH_YETUGA, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		CHECK_FAILED(Ready_Layer_Monster(TEXT("Layer_Yetuga")), E_FAIL);
		CHECK_FAILED(Ready_Trigger(TEXT("Layer_Trigger"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		CHECK_FAILED(Ready_Layer_Effect(TEXT("Layer_Effect")), E_FAIL); 
		return S_OK;
		});

	
	m_pGameInstance->Add_FireTask([this]() {

		CHECK_FAILED(Ready_Lights(TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

		CHECK_FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

		CHECK_FAILED(Ready_Layer_Cloud(TEXT("Layer_Sky"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		return S_OK;
		});

	
	m_pGameInstance->Add_FireTask([this]() {
		for (_uint i = 0; i < HEINMACH_SUBLV; ++i)
		{
			// 첫번째 서브 레벨 로드 주석 해제하면 여기서 스킵
			if (HEINMACH_1ST_BLADENEXUS == i)
				continue;

			// 두번째 서브 레벨 로드 주석 해제하면 여기서 스킵
			//if (HEINMACH_2ND_BLADENEXUS == i)
			//	continue;

			// 세번째 서브 레벨 로드 주석 해제하면 여기서 스킵
			//if (HEINMACH_3RD_BLADENEXUS == i)
			//	continue;

			// 예투가 보스 맵 서브 레벨 로드 주석 해제하면 여기서 스킵
			if (HEINMACH_YETUGA == i)
				continue;
			
			CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("HeinMach"), i, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		}		
		return S_OK;
		});

	m_pGameInstance->Add_FireTask([this]() mutable { 
		CHECK_FAILED(Ready_Layer_MapObject_Interactive(TEXT("Layer_MapObject_Interact"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Layer_MapObject_Inst"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL); 
		return S_OK;
		});


	CClientInstance::GetInstance()->Fade_Out();

    if (!Wait_All_Futures())
        return E_FAIL;

	m_futures.clear();
	return S_OK;
}

void CLevel_HeinMach::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_Q))
	{
		m_pGameInstance->isPickRenderTargetPixel(TEXT("Target_Normal"));
	}

	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::HEINMACH), ENUM_CLASS(CAMERATYPE::FREE));
	}
	else if (m_pGameInstance->Key_Down(DIK_F2))
	{
		m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::HEINMACH), ENUM_CLASS(CAMERATYPE::PLAYER));
	}


	//if (m_pGameInstance->Key_Down(DIK_RETURN))
	//	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::VIPER))));

    if (!m_isStart)
    {
        m_isStart = true;
        CSequence_HeinMach_Start* pSequence = CSequence_HeinMach_Start::Create();

        SEQ_REQ_PLAY_DESC tPlayDesc{};
        tPlayDesc.tId.iSeq = 100000;
        tPlayDesc.pAsset = L"HeinMach_Start";
        tPlayDesc.fStartTime = 0.f;

        m_pGameInstance->SEQ_AdoptAndPlay(pSequence, tPlayDesc);
    }


	return;
}

HRESULT CLevel_HeinMach::Render()
{
	SetWindowText(g_hWnd, TEXT("스테이지1 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	//(LightDesc.Diffuse * MtrlDesc.Diffuse) * (fShade(0 ~ 1) + (LightDesc.Ambient * MtrlDesc.Ambient))
	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(TEXT("Directional_Stage1"), ENUM_CLASS(LEVEL::HEINMACH), LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Compre::CAMERA_COMPRE_DESC	CameraFreeDesc{};

	CameraFreeDesc.vEye = _float4(0.39f, 3.97f, -1.79f, 1.f);
	CameraFreeDesc.vAt = _float4(-0.26f, -0.1f, 0.96f, 1.f);
	CameraFreeDesc.fFovy = XMConvertToRadians(60.0f);
	CameraFreeDesc.fNear = 0.1f;
	CameraFreeDesc.fFar = 6000.f;
	CameraFreeDesc.fSpeedPerSec = 40.f;
	CameraFreeDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraFreeDesc.fMouseSensor = 0.2f;
	CameraFreeDesc.iCameraType = ENUM_CLASS(CAMERATYPE::FREE);

	CCamera_Compre* pCamera_Free = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Camera_Compre"), &CameraFreeDesc));
	pCamera_Free->Set_IsActive(false);

	m_pClientInstance->Add_Camera(ENUM_CLASS(LEVEL::HEINMACH), pCamera_Free);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag, pCamera_Free);

	CCamera_Compre::CAMERA_COMPRE_DESC	PlayerCameraDesc{};

	PlayerCameraDesc.vEye = _float4(0.39f, 3.97f, -1.79f, 1.f);
	PlayerCameraDesc.vAt = _float4(-0.26f, -0.1f, 0.96f, 1.f);
	PlayerCameraDesc.fFovy = XMConvertToRadians(60.0f);
	PlayerCameraDesc.fNear = 0.1f;
	PlayerCameraDesc.fFar = 6000.f;
	PlayerCameraDesc.fSpeedPerSec = 10.f;
	PlayerCameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	PlayerCameraDesc.fMouseSensor = 0.2f;
	PlayerCameraDesc.iCameraType = ENUM_CLASS(CAMERATYPE::PLAYER);

	CCamera_Compre* pCamera_Player = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Camera_Compre"), &PlayerCameraDesc));
	pCamera_Player->Set_IsActive(false);
	CGameObject* pPlayer = m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Creature_Player"));
	pCamera_Player->Set_ObjMatrix(dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr());

    static_cast<CKhazan_Spear*>(pPlayer)->Set_Camera(pCamera_Player);

	m_pClientInstance->Add_Camera(ENUM_CLASS(LEVEL::HEINMACH), pCamera_Player);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag, pCamera_Player);

	m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::HEINMACH), ENUM_CLASS(CAMERATYPE::FREE));



	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
		ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Khazan_Spear"), TIME_CHANNEL::PLAYER)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Monster(const _wstring& strLayerTag)
{

    CMonster::MONSTER_DESC MonsterDesc{};
    MonsterDesc.fAttack = 10.f;
    MonsterDesc.fMaxHP = 100.f;
    MonsterDesc.fMaxStamina = 100.f;
    MonsterDesc.fMoveSpeed = 10.f;
    MonsterDesc.fSpeedPerSec = 3.f;
    MonsterDesc.fRotationPerSec = 180.f;
    MonsterDesc.strName = "Yetuga";
    //MonsterDesc.strName = "Gomdol";
    //MonsterDesc.strName = "ImpRange";
    

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Yetuga"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
        return E_FAIL;


    /*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Gomdol"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
        return E_FAIL;*/


  /*  if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Imp_Range"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
        return E_FAIL;*/


    return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Effect(const _wstring& strLayerTag)
{
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpearWind"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust"), 3);

    return S_OK;
}

//HRESULT CLevel_HeinMach::Ready_Layer_MapObject(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
//{
//	_wstring strDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };
//
//	switch (eMap)
//	{
//	case KHAZAN_MAP::HEINMACH:
//		strDataFilePath += TEXT("HeinMach/");
//		break;
//	case KHAZAN_MAP::CREVICE:
//		strDataFilePath += TEXT("Crevice/");
//		break;
//	case KHAZAN_MAP::EMBARS:
//		strDataFilePath += TEXT("Embars/");
//		break;
//	case KHAZAN_MAP::VIPER:
//		strDataFilePath += TEXT("Viper/");
//		break;
//	default:
//		break;
//	}
//
//	strDataFilePath += pDataFileName;
//
//	// 동일한 파일명의 _objects.dat 불러오기
//	strDataFilePath += TEXT("_object.dat");
//
//	DWORD dwByte = {};
//
//	HANDLE hFile = CreateFile(strDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//
//	if (hFile == INVALID_HANDLE_VALUE)
//	{
//		return E_FAIL;
//	}
//	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("데이터 파일이 없거나 박준영 문제"), E_FAIL);
//
//	// 1. 오브젝트의 총 개수
//	_uint iObjectCnt = {};
//	CHECK_FALSE(ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);
//
//	// 오브젝트 총 개수만큼 순회
//	for (_uint i = 0; i < iObjectCnt; ++i)
//	{
//		CProp_Object::PROP_OBJECT_DESC ObjectDesc = {};
//
//		ObjectDesc.eLevel = eCurrentLevel;
//
//		// 2. 프로토 타입 태그 길이 불러오기
//		_uint iPrototypeTagLen = {};
//		CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);
//
//		// 3. 프로토 타입 태그 이름 불러오기
//		_tchar szPrototypeTag[MAX_PATH] = {};
//		CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);
//
//		// 불러온 태그 카피
//		memcpy(ObjectDesc.szModelName, szPrototypeTag, sizeof(ObjectDesc.szModelName));
//
//		// 4. 객체당 월드 행렬 때오기
//		_float4x4 WorldMatrix = {};
//		CHECK_FALSE(ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr), E_FAIL);
//
//		ObjectDesc.WorldMatrix = WorldMatrix;
//
//		// 5. 객체의 속성 불러오기
//		MAPOBJECT_PROPERTIES PropProperties = {};
//		CHECK_FALSE(ReadFile(hFile, &PropProperties, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr), false);
//
//		ObjectDesc.Properties = PropProperties;
//
//		// 일단 단일 오브젝트로 배치하고 추후에 인스턴스, 인터렉티브, 다이나믹 으로 나누겠습니다.
//		//m_pGameInstance->Add_FireTask([this, objDesc = ObjectDesc, curLevel = eCurrentLevel]() mutable {
//		//	CHECK_FAILED(
//		//		m_pGameInstance->Add_GameObject_ToLayer(
//		//			ENUM_CLASS(objDesc.eLevel),
//		//			TEXT("Layer_MapObject"),
//		//			ENUM_CLASS(curLevel),
//		//			TEXT("Prototype_GameObject_Prop_Object"),
//		//			&objDesc // 캡처된 값의 주소 -> 안전
//		//		),
//		//		E_FAIL
//		//	);
//		//	});
//		m_futures.push_back(m_pGameInstance->Add_Task([this, CurLevel = eCurrentLevel, Desc = ObjectDesc, WorldMat = WorldMatrix, LayerTag = strLayerTag]() mutable {
//			lock_guard<mutex> lock(m_Mutex);
//			CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc));
//			if (!pObject)
//				return E_FAIL;
//			_bool isAdd = m_pGameInstance->AddStaticObject(pObject, { WorldMat._41, WorldMat._42, WorldMat._43 }, 3.f);
//			//Safe_Release(pObject);
//			/*CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(CurLevel), LayerTag,
//				ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc), E_FAIL);*/
//			if (isAdd)
//				Safe_Release(pObject);
//			else
//				return E_FAIL;
//
//
//			return S_OK;
//			}));
//		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
//			ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
//
//		//CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc));
//
//		//m_pGameInstance->AddStaticObject(pObject, { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43 }, 10.f);
//
//		//Safe_Release(pObject);
//	}
//
//	CloseHandle(hFile);
//
//	return S_OK;
//}

HRESULT CLevel_HeinMach::Ready_Layer_MapObject_SubLV(const _wstring& strLayerTag, const _tchar* pDataFileName, _uint iSubLV, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

		//if (iSubLV == HEINMACH_1ST_BLADENEXUS)
		//{

		//	// 일단 단일 오브젝트로 배치하고 추후에 인스턴스, 인터렉티브, 다이나믹 으로 나누겠습니다.
		//	m_futures.push_back(m_pGameInstance->Add_Task([this, CurLevel = eCurrentLevel, Desc = ObjectDesc, WorldMat = WorldMatrix, LayerTag = strLayerTag]() mutable {
		//		
  //                  
		//		return S_OK;
		//		}));
		//}
		//else {
		//	m_pGameInstance->Add_FireTask([this, CurLevel = eCurrentLevel, Desc = ObjectDesc, WorldMat = WorldMatrix, LayerTag = strLayerTag]() mutable {
		//		lock_guard<mutex> lock(m_Mutex);
		//		CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc));
		//		if (!pObject)
		//			return E_FAIL;
		//		_bool isAdd = m_pGameInstance->AddStaticObject(pObject, { WorldMat._41, WorldMat._42, WorldMat._43 }, 3.f);
		//		//Safe_Release(pObject);
		//		/*CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(CurLevel), LayerTag,
		//			ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc), E_FAIL);*/
		//		if (isAdd)
		//			Safe_Release(pObject);
		//		else
		//			return E_FAIL;
		//		return S_OK;
		//		});
		//}

        lock_guard<mutex> lock(m_Mutex);
        CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc));
        if (!pObject)
            return E_FAIL;
        _bool isAdd = m_pGameInstance->AddStaticObject(pObject, { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43 }, 3.f);

        if (isAdd)
            Safe_Release(pObject);
        else
        {
            Safe_Release(pObject);
            return E_FAIL;
        }

	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_MapObject_Interactive(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)

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
		default:
			MSG_BOX(TEXT("잉 있으면 안되는디"));
			break;
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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
        CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag, ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Static"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
		//m_pGameInstance->Add_FireTask([this, CurLevel = eCurrentLevel, Desc = ObjectDesc, LayerTag = strLayerTag]() mutable {
		//	lock_guard<mutex> lock(m_Mutex);
		//	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(CurLevel), LayerTag, ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Static"), TIME_CHANNEL::MAP, &Desc), E_FAIL);
		//	return S_OK;
		//	});
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_HeinMach::Ready_Trigger(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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
			ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_HeinMach_Trigger"), TIME_CHANNEL::MAP, &TriggerDesc), E_FAIL);
	}

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_UI()
{
    CUIObject::UIOBJECT_DESC Desc = {};
    Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
    Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "LogoBG";
    Desc.fDepth = 2;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"),
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Announce_Result"), TIME_CHANNEL::WORLD, &Desc)))
        return E_FAIL;

    Desc.vLocalSize = { 1042.f, 168.f };
    Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "LogoBG";
    Desc.fDepth = 2;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"),
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Announce_Over"), TIME_CHANNEL::WORLD, &Desc)))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/HUD.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/Inven.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/MainMenu.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/BladeNexus.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/State.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/ItemInfo_Other.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/ItemInfo_Weapon.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/ItemInfo_Equip.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/BossHp.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/HUD_Amount.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/BladeNexus_Map.json"))))
        return E_FAIL;


    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/Skill.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/Skill_Info.json"))))
        return E_FAIL;

    if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
        TEXT("../Bin/Resources/UI/UIData/SkillQuickSlot.json"))))
        return E_FAIL;

	return S_OK;
}

_bool CLevel_HeinMach::Wait_All_Futures()
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

HRESULT CLevel_HeinMach::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
	//	ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
	//	ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Sky"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Sky(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_HeinMach::Ready_Layer_Cloud(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

	CloudSphereDesc .eLevel = eCurrentLevel;

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

		CloudSphereDesc .fRotationPerSec = XMConvertToRadians(0.f);

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_CloudSphere"), TIME_CHANNEL::WORLD, &CloudSphereDesc), E_FAIL);

		CloseHandle(hFile);
	}

	return S_OK;
}

CLevel_HeinMach* CLevel_HeinMach::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_HeinMach* pInstance = new CLevel_HeinMach(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_HeinMach"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_HeinMach::Free()
{
	__super::Free();

	Safe_Release(m_pClientInstance);
}
