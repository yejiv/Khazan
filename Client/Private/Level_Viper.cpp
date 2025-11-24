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
#include "Camera_Compre.h"
#include "Sequence_Viper_SecondPhase.h"

CLevel_Viper::CLevel_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_pClientInstance(CClientInstance::GetInstance())
{
	Safe_AddRef(m_pClientInstance);
}

HRESULT CLevel_Viper::Initialize()
{

    CHECK_FAILED(Ready_Sequence(), E_FAIL);

	// 플레이어, 카메라, 트리거
    
    CHECK_FAILED(Ready_Layer_Effect(TEXT("Layer_Effect")), E_FAIL);

	m_pGameInstance->Add_FireTask([this]() {
	    CHECK_FAILED(Ready_Layer_Player(TEXT("Layer_Creature_Player")), E_FAIL);
        CHECK_FAILED(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);
		CHECK_FAILED(Ready_Trigger(TEXT("Layer_Trigger"), TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);
        if (FAILED(Ready_Layer_Monster_Viper(TEXT("Layer_Viper"))))
            return E_FAIL;

		return S_OK;
		});

	// 우선 맵 오브젝트 서브 레벨 로드
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("Viper"),
			0, LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);
		return S_OK;
		}));

	// 조명, 스카이박스 설정
	m_pGameInstance->Add_FireTask([this]() {

		CHECK_FAILED(Ready_Lights(TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);

		CHECK_FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"), TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);

		CHECK_FAILED(Ready_Layer_Cloud(TEXT("Layer_Sky"), TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);

		return S_OK;
		});

	// 맵 오브젝트 서브 레벨 로드
	m_pGameInstance->Add_FireTask([this]() {
		for (_uint i = 0; i < VIPER_SUBLV; ++i)
		{
			if (0 == i)
				continue;

			CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("Viper"), i, LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);
		}

		return S_OK;
		});

	m_pGameInstance->Add_FireTask([this]() mutable {
		//CHECK_FAILED(Ready_Layer_MapObject_Interactive(TEXT("Layer_MapObject_Interact"), TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);
		CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Layer_MapObject_Inst"), TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER), E_FAIL);
		return S_OK;
		});



	CClientInstance::GetInstance()->Fade_Out();

	while (true) {
		bool all_ready = true;

		for (auto it = m_futures.begin(); it != m_futures.end(); /* no ++ here */) {
			// 1) invalid면 지워버려서 다시는 접근하지 않게
			if (!it->valid()) {
				it = m_futures.erase(it);
				continue;
			}

			// 2) 아직 준비 안됐으면 플래그만 내리기
			if (it->wait_for(0ms) != std::future_status::ready) {
				all_ready = false;
			}
			++it;
		}

		if (all_ready) break;
		// 너무 바쁘게 돌지 않도록 살짝 양보(필요시)
		std::this_thread::sleep_for(1ms);
	}

	bool all_ok = true;
	for (auto& f : m_futures) {
		if (!f.valid()) continue; // 이미 소비/무효면 스킵
		try {
			HRESULT hr = f.get();          // get()은 딱 한번만!
			if (FAILED(hr)) all_ok = false;
		}
		catch (const std::future_error& e) {
			// e.code()가 no_state인지, broken_promise인지 로깅
			all_ok = false;
		}
		catch (...) {
			all_ok = false;
		}
	}

   
	m_futures.clear();


	CClientInstance::GetInstance()->Fade_In();

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

	if (m_pGameInstance->Key_Down(DIK_Q))
	{
		m_pGameInstance->isPickRenderTargetPixel(TEXT("Target_Normal"));
	}

	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::VIPER), ENUM_CLASS(CAMERATYPE::FREE));
	}
	else if (m_pGameInstance->Key_Down(DIK_F2))
	{
		m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::VIPER), ENUM_CLASS(CAMERATYPE::PLAYER));
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
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag,
		ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Khazan_Spear"), TIME_CHANNEL::PLAYER)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Viper::Ready_Layer_Camera(const _wstring& strLayerTag)
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

	CCamera_Compre* pCamera_Free = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Camera_Compre"), &CameraFreeDesc));
	pCamera_Free->Set_IsActive(false);

	m_pClientInstance->Add_Camera(ENUM_CLASS(LEVEL::VIPER), pCamera_Free);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag, pCamera_Free);

	CCamera_Compre::CAMERA_COMPRE_DESC	CameraSpringDesc{};

	CameraFreeDesc.vEye = _float4(0.39f, 3.97f, -1.79f, 1.f);
	CameraFreeDesc.vAt = _float4(-0.26f, -0.1f, 0.96f, 1.f);
	CameraSpringDesc.fFovy = XMConvertToRadians(60.0f);
	CameraSpringDesc.fNear = 0.1f;
	CameraSpringDesc.fFar = 6000.f;
	CameraSpringDesc.fSpeedPerSec = 10.f;
	CameraSpringDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraSpringDesc.fMouseSensor = 0.2f;
	CameraSpringDesc.iCameraType = ENUM_CLASS(CAMERATYPE::PLAYER);


	CCamera_Compre* pCamera_Spring = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Camera_Compre"), &CameraSpringDesc));
	pCamera_Spring->Set_IsActive(false);
	CGameObject* pPlayer = m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Creature_Player"));
	pCamera_Spring->Set_ObjMatrix(dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr());
	m_pClientInstance->Add_Camera(ENUM_CLASS(LEVEL::VIPER), pCamera_Spring);

	m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag, pCamera_Spring);

	m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::VIPER), ENUM_CLASS(CAMERATYPE::FREE));

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

HRESULT CLevel_Viper::Ready_Layer_MapObject(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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
	strDataFilePath += TEXT("_object.dat");

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
		m_futures.push_back(m_pGameInstance->Add_Task([this, CurLevel = eCurrentLevel, Desc = ObjectDesc, WorldMat = WorldMatrix, LayerTag = strLayerTag]() mutable {
			lock_guard<mutex> lock(m_Mutex);
			CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc));
			if (!pObject)
				return E_FAIL;
			_bool isAdd = m_pGameInstance->AddStaticObject(pObject, { WorldMat._41, WorldMat._42, WorldMat._43 }, 3.f);
			//Safe_Release(pObject);
			/*CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(CurLevel), LayerTag,
				ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc), E_FAIL);*/
			if (isAdd)
				Safe_Release(pObject);
			else
				return E_FAIL;


			return S_OK;
			}));
		CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
			ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);

		//CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc));

		//m_pGameInstance->AddStaticObject(pObject, { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43 }, 10.f);

		//Safe_Release(pObject);
	}

	CloseHandle(hFile);

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

        CSequence_Viper_SecondPhase* pSeq = dynamic_cast<CSequence_Viper_SecondPhase*>(m_pClientInstance->Find_Sequence(TEXT("Viper_SecondPhase")));

        if (wcscmp(ObjectDesc.szModelName, L"Prototype_Component_Model_WP_TDL_Bridge_Collision_004") == 0)
        {
            m_pGameInstance->Add_FireTask([this, CurLevel = eCurrentLevel, WMat = WorldMatrix, strLayerTag = strLayerTag, iDestIndex = iDestIndex, pSeq = pSeq]() {

                CProp_Destructible::PROP_DEST_DESC ObeliskDesc = {};

                ObeliskDesc.eLevel = CurLevel;

                MAPOBJECT_PROPERTIES PropProperties4 = {};

                ObeliskDesc.Properties = PropProperties4;
                //XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
                //WorldMatrix._43 = 10.f;
                ObeliskDesc.WorldMatrix = WMat;
                ObeliskDesc.WorldMatrix._42 -= 400.f;
                ObeliskDesc.iIndex = iDestIndex;
                CObelisk* pObelisk = dynamic_cast<CObelisk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Prop_Obelisk"), &ObeliskDesc));
                pSeq->Push_Obelisk(pObelisk);
                m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag, pObelisk);

                if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag,
                    ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Prop_Obelisk"), TIME_CHANNEL::WORLD, &ObeliskDesc)))
                    return E_FAIL;



                });

            iDestIndex++;

            continue;
        }

		if (iSubLV == 0)
		{

			// 일단 단일 오브젝트로 배치하고 추후에 인스턴스, 인터렉티브, 다이나믹 으로 나누겠습니다.
			m_futures.push_back(m_pGameInstance->Add_Task([this, CurLevel = eCurrentLevel, Desc = ObjectDesc, WorldMat = WorldMatrix, LayerTag = strLayerTag]() mutable {
				lock_guard<mutex> lock(m_Mutex);
				CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc));
				if (!pObject)
					return E_FAIL;
				_bool isAdd = m_pGameInstance->AddStaticObject(pObject, { WorldMat._41, WorldMat._42, WorldMat._43 }, 3.f);
				//Safe_Release(pObject);
				/*CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(CurLevel), LayerTag,
					ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc), E_FAIL);*/
				if (isAdd)
					Safe_Release(pObject);
				else
					return E_FAIL;
				return S_OK;
				}));
		}
		else {
			m_pGameInstance->Add_FireTask([this, CurLevel = eCurrentLevel, Desc = ObjectDesc, WorldMat = WorldMatrix, LayerTag = strLayerTag]() mutable {
				lock_guard<mutex> lock(m_Mutex);
				CGameObject* pObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc));
				if (!pObject)
					return E_FAIL;
				_bool isAdd = m_pGameInstance->AddStaticObject(pObject, { WorldMat._41, WorldMat._42, WorldMat._43 }, 3.f);
				//Safe_Release(pObject);
				/*CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(CurLevel), LayerTag,
					ENUM_CLASS(CurLevel), TEXT("Prototype_GameObject_Prop_Object"), &Desc), E_FAIL);*/
				if (isAdd)
					Safe_Release(pObject);
				else
					return E_FAIL;
				return S_OK;
				});
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
			ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_HeinMach_Trigger"), TIME_CHANNEL::MAP, &TriggerDesc), E_FAIL);
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

        if ("Viper_Phase1" == MonsterData.MonsterKey[i])
        {
            CMonster::MONSTER_DESC MonsterDesc{};
            MonsterDesc.fAttack = 10.f;
            MonsterDesc.fMaxHP = 100.f;
            MonsterDesc.fMaxStamina = 100.f;
            MonsterDesc.fMoveSpeed = 10.f;
            MonsterDesc.fSpeedPerSec = 3.f;
            MonsterDesc.fRotationPerSec = 180.f;

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
    CMonster::MONSTER_DESC MonsterDesc{};
    MonsterDesc.fAttack = 10.f;
    MonsterDesc.fMaxHP = 100.f;
    MonsterDesc.fMaxStamina = 100.f;
    MonsterDesc.fMoveSpeed = 10.f;
    MonsterDesc.fSpeedPerSec = 3.f;
    MonsterDesc.fRotationPerSec = 180.f;
    XMStoreFloat4x4(&MonsterDesc.WorldMatrix,XMMatrixIdentity());
    MonsterDesc.strName = "Viper";
    MonsterDesc.iLevelIndex = ENUM_CLASS(LEVEL::VIPER);
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag,
        ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Monster_Viper"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
        return E_FAIL; 

  /*  CMonster::MONSTER_DESC MonsterDesc{};
    MonsterDesc.fAttack = 10.f;
    MonsterDesc.fMaxHP = 100.f;
    MonsterDesc.fMaxStamina = 100.f;
    MonsterDesc.fMoveSpeed = 10.f;
    MonsterDesc.fSpeedPerSec = 3.f;
    MonsterDesc.fRotationPerSec = 180.f;

    XMStoreFloat4x4(&MonsterDesc.WorldMatrix, XMMatrixIdentity());
    MonsterDesc.strName = "Dragonian_Melee";
    MonsterDesc.iLevelIndex = ENUM_CLASS(LEVEL::VIPER);

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), strLayerTag,
        ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Monster_Dragonian_Melee"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
        return E_FAIL;*/
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

    return S_OK;
}

HRESULT CLevel_Viper::Ready_Sequence()
{
    CSequence_Viper_SecondPhase* pSequence = CSequence_Viper_SecondPhase::Create();
    m_pClientInstance->Push_Sequence(TEXT("Viper_SecondPhase"), pSequence);


    return S_OK;
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
