#include "Level_Stage1.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Event_Defines.h"
//#include "Camera_Free.h"
#include "Player.h"
#include "Camera_Compre.h"
#include "Dummy.h"
#include "Monster.h"
#include "ClientInstance.h"

#pragma region MAP OBJECT
#include "MapObject_Header.h"
#pragma endregion

#pragma region UI OBJECT
#include "UI_Atlas_Icon.h"
#include "UI_BackGround.h"
#pragma endregion

CLevel_Stage1::CLevel_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_pClientInstance(CClientInstance::GetInstance())
{
	Safe_AddRef(m_pClientInstance);
}

HRESULT CLevel_Stage1::Initialize()
{

	/*if (FAILED(Ready_Lights()))
		return E_FAIL;*/

	CHECK_FAILED(Ready_Lights(TEXT("HeinMach"), LEVEL::STAGE1, KHAZAN_MAP::HEINMACH), E_FAIL);

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	CHECK_FAILED(Ready_Layer_Test(TEXT("Layer_Test")), E_FAIL);

	/* 상호 작용 오브젝트 호출 함수 */
	// m_pGameInstance->Add_FireTask([this]() {
	// 	CHECK_FAILED(Ready_Layer_MapObject_Interactive(TEXT("Layer_MapObject_Interact"), TEXT("HeinMach_Test"), LEVEL::STAGE1), E_FAIL);
	// 	});
	
	//m_pGameInstance->Add_FireTask([this]() {
	//	CHECK_FAILED(Ready_Layer_MapObject_Test(TEXT("Layer_Test")), E_FAIL);
	//	});
	//
	//m_pGameInstance->Add_FireTask([this]() {
	//	CHECK_FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"), TEXT("HeinMach"), LEVEL::STAGE1, KHAZAN_MAP::HEINMACH), E_FAIL);
	//	});

	//m_pGameInstance->Add_FireTask([this]() {
	//	CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Layer_MapObject_Inst"), TEXT("HeinMach"), LEVEL::STAGE1, KHAZAN_MAP::HEINMACH), E_FAIL);
	//	});
	CHECK_FAILED(Ready_Layer_MapObject_Test(TEXT("Layer_Test")), E_FAIL);
	CHECK_FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"), TEXT("HeinMach"), LEVEL::STAGE1, KHAZAN_MAP::HEINMACH), E_FAIL);
	CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Layer_MapObject_Inst"), TEXT("HeinMach"), LEVEL::STAGE1, KHAZAN_MAP::HEINMACH), E_FAIL);
	//m_pGameInstance->Jolt_Test();

	return S_OK;
}

void CLevel_Stage1::Update(_float fTimeDelta)
{
	//if (GetKeyState(VK_RETURN) & 0x8000)
	//{
	//	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
	//		return;
	//}
	if (m_pGameInstance->Key_Down(DIK_Q))
	{
		m_pGameInstance->isPickRenderTargetPixel(TEXT("Target_Normal"));
	}

	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_pGameInstance->Change_Camera(ENUM_CLASS(LEVEL::STAGE1), ENUM_CLASS(CAMERATYPE::FREE));
	}
	else if (m_pGameInstance->Key_Down(DIK_F2))
	{
		m_pGameInstance->Change_Camera(ENUM_CLASS(LEVEL::STAGE1), ENUM_CLASS(CAMERATYPE::SPRING));
	}

	return;
}

HRESULT CLevel_Stage1::Render()
{
	SetWindowText(g_hWnd, TEXT("스테이지1 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	//(LightDesc.Diffuse * MtrlDesc.Diffuse) * (fShade(0 ~ 1) + (LightDesc.Ambient * MtrlDesc.Ambient))
	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(TEXT("Directional_Stage1"), ENUM_CLASS(LEVEL::STAGE1), LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Compre::CAMERA_COMPRE_DESC	CameraFreeDesc{};

 	CameraFreeDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraFreeDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraFreeDesc.fFovy = XMConvertToRadians(60.0f);
	CameraFreeDesc.fNear = 0.1f;
	CameraFreeDesc.fFar = 6000.f;
	CameraFreeDesc.fSpeedPerSec = 40.f;
	CameraFreeDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraFreeDesc.fMouseSensor = 0.2f;
	CameraFreeDesc.iCameraType = ENUM_CLASS(CAMERATYPE::FREE);

	CCamera_Compre* pCamera_Free = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Camera_Compre"), &CameraFreeDesc));
	pCamera_Free->Set_IsActive(false);

	m_pGameInstance->Add_Camera(ENUM_CLASS(LEVEL::STAGE1), pCamera_Free);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag, pCamera_Free);

	CCamera_Compre::CAMERA_COMPRE_DESC	CameraSpringDesc{};

	CameraSpringDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraSpringDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraSpringDesc.fFovy = XMConvertToRadians(60.0f);
	CameraSpringDesc.fNear = 0.1f;
	CameraSpringDesc.fFar = 6000.f;
	CameraSpringDesc.fSpeedPerSec = 10.f;
	CameraSpringDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraSpringDesc.fMouseSensor = 0.2f;
	CameraSpringDesc.iCameraType = ENUM_CLASS(CAMERATYPE::SPRING);

	
	CCamera_Compre* pCamera_Spring = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Camera_Compre"), &CameraSpringDesc));
	pCamera_Spring->Set_IsActive(false);
	CGameObject* pPlayer = m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::STAGE1), TEXT("Layer_Player"));
	pCamera_Spring->Set_ObjMatrix(dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr());
	m_pGameInstance->Add_Camera(ENUM_CLASS(LEVEL::STAGE1), pCamera_Spring);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag, pCamera_Spring);

	m_pGameInstance->Change_Camera(ENUM_CLASS(LEVEL::STAGE1), ENUM_CLASS(CAMERATYPE::FREE));

	

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
		ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Monster(const _wstring& strLayerTag)
{

	CMonster::MONSTER_DESC MonsterDesc{};
	MonsterDesc.fAttack = 10.f;
	MonsterDesc.fMaxHP = 100.f;
	MonsterDesc.fMaxStamina = 100.f;
	MonsterDesc.fMoveSpeed = 10.f;
	MonsterDesc.fSpeedPerSec = 3.f;
	MonsterDesc.fRotationPerSec = 180.f;
	MonsterDesc.strName = "Yetuga";

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
			ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Monster_Yetuga"),&MonsterDesc)))
			return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Test(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
		ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_JOH_Test1"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_MapObject_Test(const _wstring& strLayerTag)
{
	//CProp_Test::PROP_TEST_DESC ObjectDesc = {};

	//memcpy(ObjectDesc.szModelName, TEXT("Prototype_Component_Model_WP_WOD_Ground_Base_004"), sizeof(ObjectDesc.szModelName));
	//ObjectDesc.eLevel = LEVEL::STAGE1;

	//CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
	//	ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Prop_Test"), &ObjectDesc), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_MapObject(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
		break;
	case KHAZAN_MAP::YETUGA:
		pDataFilePath += TEXT("Yetuga/");
		break;
	case KHAZAN_MAP::THECREVICE:
		pDataFilePath += TEXT("TheCrevice/");
		break;
	case KHAZAN_MAP::EMBARS:
		pDataFilePath += TEXT("Embars/");
		break;
	case KHAZAN_MAP::VIPER:
		pDataFilePath += TEXT("Viper/");
		break;
	default:
		break;
	}

	pDataFilePath += pDataFileName;

	// 동일한 파일명의 _objects.dat 불러오기
	pDataFilePath += TEXT("_object.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

		// 일단 단일 오브젝트로 배치하고 추후에 인스턴스, 인터렉티브, 다이나믹 으로 나누겠습니다.
		//m_pGameInstance->Add_FireTask([this, objDesc = ObjectDesc, curLevel = eCurrentLevel]() mutable {
		//	CHECK_FAILED(
		//		m_pGameInstance->Add_GameObject_ToLayer(
		//			ENUM_CLASS(objDesc.eLevel),
		//			TEXT("Layer_MapObject"),
		//			ENUM_CLASS(curLevel),
		//			TEXT("Prototype_GameObject_Prop_Object"),
		//			&objDesc // 캡처된 값의 주소 -> 안전
		//		),
		//		E_FAIL
		//	);
		//	});
		/*CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc), E_FAIL);*/

		CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc));

		m_pGameInstance->AddStaticObject(pObject, { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43 }, 10.f);
	}

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_MapObject_Interactive(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
		break;
	case KHAZAN_MAP::YETUGA:
		pDataFilePath += TEXT("Yetuga/");
		break;
	case KHAZAN_MAP::THECREVICE:
		pDataFilePath += TEXT("TheCrevice/");
		break;
	case KHAZAN_MAP::EMBARS:
		pDataFilePath += TEXT("Embars/");
		break;
	case KHAZAN_MAP::VIPER:
		pDataFilePath += TEXT("Viper/");
		break;
	default:
		break;
	}

	pDataFilePath += pDataFileName;

	// 동일한 파일명의 _objects.dat 불러오기
	pDataFilePath += TEXT("_interactive.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
		if (INTERACTIVE_TYPE::CHECKPOINT == eType)
		{
			CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_BladeNexus"), &ObjectDesc), E_FAIL);
		}

		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc), E_FAIL);
	}

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
		break;
	case KHAZAN_MAP::YETUGA:
		pDataFilePath += TEXT("Yetuga/");
		break;
	case KHAZAN_MAP::THECREVICE:
		pDataFilePath += TEXT("TheCrevice/");
		break;
	case KHAZAN_MAP::EMBARS:
		pDataFilePath += TEXT("Embars/");
		break;
	case KHAZAN_MAP::VIPER:
		pDataFilePath += TEXT("Viper/");
		break;
	default:
		break;
	}

	pDataFilePath += pDataFileName;

	// 동일한 파일명의 _inst.dat 불러오기
	pDataFilePath += TEXT("_inst.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
			ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Static"), &ObjectDesc), E_FAIL);
	}

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
	// Dat 기본 경로
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
		break;
	case KHAZAN_MAP::YETUGA:
		pDataFilePath += TEXT("Yetuga/");
		break;
	case KHAZAN_MAP::THECREVICE:
		pDataFilePath += TEXT("TheCrevice/");
		break;
	case KHAZAN_MAP::EMBARS:
		pDataFilePath += TEXT("Embars/");
		break;
	case KHAZAN_MAP::VIPER:
		pDataFilePath += TEXT("Viper/");
		break;
	default:
		break;
	}

	pDataFilePath += pDataFileName;

	pDataFilePath += TEXT("_lights.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
		m_pGameInstance->Add_FireTask([this, szLightTag = szLightTag, eCurrentLevel = ENUM_CLASS(eCurrentLevel), LightDesc = LightDesc]() mutable {
			m_pGameInstance->Add_Light(szLightTag, ENUM_CLASS(eCurrentLevel), LightDesc, true);
			return S_OK;
			});
		
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
	//	ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), strLayerTag,
		ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}


CLevel_Stage1* CLevel_Stage1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage1* pInstance = new CLevel_Stage1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Stage1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Stage1::Free()
{
	__super::Free();

	Safe_Release(m_pClientInstance);
}
