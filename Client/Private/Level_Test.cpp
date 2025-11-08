#include "Level_Test.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Event_Defines.h"
//#include "Camera_Free.h"
#include "Player.h"
#include "Camera_Compre.h"
#include "ClientInstance.h"


#pragma region MAP OBJECT
#include "MapObject_Header.h"
#pragma endregion


CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_pClientInstance(CClientInstance::GetInstance())
{
	Safe_AddRef(m_pClientInstance);
}

HRESULT CLevel_Test::Initialize()
{
	CHECK_FAILED(Ready_Lights(TEXT("HeinMach"), LEVEL::TEST, KHAZAN_MAP::HEINMACH), E_FAIL);

	CHECK_FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"), TEXT("HeinMach"), LEVEL::TEST, KHAZAN_MAP::HEINMACH), E_FAIL);

	CHECK_FAILED(Ready_Layer_Cloud(TEXT("Layer_Sky"), TEXT("HeinMach"), LEVEL::TEST, KHAZAN_MAP::HEINMACH), E_FAIL);

	CHECK_FAILED(Ready_Layer_Test(TEXT("Layer_Creature_Test")), E_FAIL);

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("Test_Level"), 0, LEVEL::TEST), E_FAIL);

	if (FAILED(Ready_Layer_TestEffect(TEXT("Layer_EffectTest"))))
		return E_FAIL;

	CClientInstance::GetInstance()->Fade_Out();

	CClientInstance::GetInstance()->Fade_In();

	return S_OK;
}

void CLevel_Test::Update(_float fTimeDelta)
{
	//if (GetKeyState(VK_RETURN) & 0x8000)
	//	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
	//		return;
	//}

	if (m_pGameInstance->Key_Down(DIK_Q))
	{
		m_pGameInstance->isPickRenderTargetPixel(TEXT("Target_Normal"));
	}

	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::TEST), ENUM_CLASS(CAMERATYPE::FREE));
	}
	else if (m_pGameInstance->Key_Down(DIK_F2))
	{
		m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::TEST), ENUM_CLASS(CAMERATYPE::PLAYER));
	}

	/*Effect test => 혹시 보게되면 지우셔도 됩니다!!!!!!!!! */
	if (m_pGameInstance->Key_Down(DIK_I))
		m_pGameInstance->Spwan_Effect(ENUM_CLASS(LEVEL::TEST), TEXT("SpaceTime_SpearBlood"), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//Test End

	return;
}

HRESULT CLevel_Test::Render()
{
	SetWindowText(g_hWnd, TEXT("테스트 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Test::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	//(LightDesc.Diffuse * MtrlDesc.Diffuse) * (fShade(0 ~ 1) + (LightDesc.Ambient * MtrlDesc.Ambient))
	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(TEXT("Directional_Stage1"), ENUM_CLASS(LEVEL::TEST), LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Camera(const _wstring& strLayerTag)
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

	CCamera_Compre* pCamera_Free = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Camera_Compre"), &CameraFreeDesc));
	pCamera_Free->Set_IsActive(false);

	m_pClientInstance->Add_Camera(ENUM_CLASS(LEVEL::TEST), pCamera_Free);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST), strLayerTag, pCamera_Free);

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

	CCamera_Compre* pCamera_Player = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Camera_Compre"), &PlayerCameraDesc));
	pCamera_Player->Set_IsActive(false);
	CGameObject* pPlayer = m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::TEST), TEXT("Layer_Creature_Test"));
	pCamera_Player->Set_ObjMatrix(dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr());
	m_pClientInstance->Add_Camera(ENUM_CLASS(LEVEL::TEST), pCamera_Player);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST), strLayerTag, pCamera_Player);

	m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::TEST), ENUM_CLASS(CAMERATYPE::FREE));



	return S_OK;
}


HRESULT CLevel_Test::Ready_Layer_Test(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST), strLayerTag,
		ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Khazan_Sample"), TIME_CHANNEL::PLAYER)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_TestEffect(const _wstring& strLayerTag)
{

	m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::TEST), TEXT("SpaceTime_SpearBlood"), 3);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_MapObject_SubLV(const _wstring& strLayerTag, const _tchar* pDataFileName, _uint iSubLV, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

		
		m_pGameInstance->Add_FireTask([this, CurLevel = eCurrentLevel, Desc = ObjectDesc, WorldMat = WorldMatrix, LayerTag = strLayerTag]() mutable {
			lock_guard<mutex> lock(m_Mutex);
			CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc));
			if (!pObject)
				return E_FAIL;
			_bool isAdd = m_pGameInstance->AddStaticObject(pObject, { WorldMat._41, WorldMat._42, WorldMat._43 }, 3.f);

			if (isAdd)
				Safe_Release(pObject);
			else
				return E_FAIL;
			return S_OK;
			});


	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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
		/*m_pGameInstance->Add_FireTask([this, szLightTag = szLightTag, eCurrentLevel = ENUM_CLASS(eCurrentLevel), LightDesc = LightDesc]() mutable {
			m_pGameInstance->Add_Light(szLightTag, ENUM_CLASS(eCurrentLevel), LightDesc, true);
			return S_OK;
			});*/

	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Sky(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_Test::Ready_Layer_Cloud(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

CLevel_Test* CLevel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Test* pInstance = new CLevel_Test(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Test"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Test::Free()
{
	__super::Free();

	Safe_Release(m_pClientInstance);
}
