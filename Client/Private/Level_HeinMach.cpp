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
#include "UI_HUD.h"
#pragma endregion

CLevel_HeinMach::CLevel_HeinMach(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
	, m_pClientInstance(CClientInstance::GetInstance())
{
	Safe_AddRef(m_pClientInstance);
}

HRESULT CLevel_HeinMach::Initialize()
{
#pragma region 수정된 코드

    // 연동되지 않는 것들을 쓰레드풀로 돌리기
    //m_futures.push_back(m_pGameInstance->Add_Task([this]() {

        //return S_OK;
        //}));

    m_pGameInstance->Add_FireTask([&]() {
        CHECK_FAILED(Ready_Layer_MapObject_DEST(TEXT("Layer_DEST"), TEXT(""), LEVEL::HEINMACH), E_FAIL);
        return S_OK;
        });

    CHECK_FAILED(Ready_Layer_Effect(TEXT("Layer_Effect")), E_FAIL);

    m_futures.push_back(m_pGameInstance->Add_Task([this]() {

        //CHECK_FAILED(Ready_Layer_UI(), E_FAIL);

        CHECK_FAILED(Ready_Layer_Decal(), E_FAIL);

        CHECK_FAILED(Ready_Lights(TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

        CHECK_FAILED(Ready_FireLights(TEXT("HeinMach_Point"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

        CHECK_FAILED(Ready_Map_Decal(TEXT("Layer_Decal"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

        return S_OK;
        }));

    CHECK_FAILED(Ready_Layer_Player(TEXT("Layer_Creature_Player")), E_FAIL);

    CHECK_FAILED(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL); 

    for (_uint i = 0; i < HEINMACH_SUBLV; ++i)
    {
        CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("HeinMach"), i, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
        CHECK_FAILED(Ready_Layer_Monster_SubLV(TEXT("Layer_MapObject"), TEXT("HeinMach"), i, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
    }

    CHECK_FAILED(Ready_Layer_MapObject_Interactive(TEXT("Layer_MapObject_Interact"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
    
    CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Layer_MapObject_Inst"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

    CHECK_FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

    CHECK_FAILED(Ready_Layer_Cloud(TEXT("Layer_Sky"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

    CHECK_FAILED(Ready_Trigger(TEXT("Layer_Trigger"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

    CClientInstance::GetInstance()->Fade_Out();

    if (!Wait_All_Futures())
        return E_FAIL;

    m_futures.clear();

    m_iEventID = m_pGameInstance->Subscribe_Event<EVENT_LEVEL_CHANGE>(ENUM_CLASS(EVENT_TYPE::LEVEL_CHANGE), [&](const EVENT_LEVEL_CHANGE& e)
        {
            m_eNextLevel = static_cast<LEVEL>(e.iLevel);
        });

#pragma endregion

#pragma region 기존 코드

    /*
    CHECK_FAILED(Ready_Layer_UI(), E_FAIL);
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("HeinMach"), HEINMACH_1ST_BLADENEXUS, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		return S_OK;
		}));

	m_pGameInstance->Add_FireTask([this]() {
		CHECK_FAILED(Ready_Layer_Player(TEXT("Layer_Creature_Player")), E_FAIL);
		if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
			return E_FAIL;
		CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("HeinMach"), HEINMACH_YETUGA, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
        CHECK_FAILED(Ready_Map_Decal(TEXT("Layer_Decal"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		CHECK_FAILED(Ready_Trigger(TEXT("Layer_Trigger"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
        if (FAILED(Ready_Layer_Decal()))
            return E_FAIL;
        CHECK_FAILED(Ready_Layer_Monster_SubLV(TEXT("Layer_Monster"), TEXT("HeinMach"), HEINMACH_1ST_BLADENEXUS, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
        CHECK_FAILED(Ready_Layer_Monster_SubLV(TEXT("Layer_Monster"), TEXT("HeinMach"), HEINMACH_YETUGA, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		return S_OK;
		});

    m_pGameInstance->Add_FireTask([this]() { 
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
            CHECK_FAILED(Ready_Layer_Monster_SubLV(TEXT("Layer_Monster"), TEXT("HeinMach"), i, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		}		
		return S_OK;
		});

    for (_uint i = 0; i < HEINMACH_SUBLV; ++i)
    {
        //CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("HeinMach"), i, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
        //CHECK_FAILED(Ready_Layer_Monster_SubLV(TEXT("Layer_MapObject"), TEXT("HeinMach"), i, LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
    }

	m_pGameInstance->Add_FireTask([this]() mutable { 
		CHECK_FAILED(Ready_Layer_MapObject_Interactive(TEXT("Layer_MapObject_Interact"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Layer_MapObject_Inst"), TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL); 
		return S_OK;
		});


	CClientInstance::GetInstance()->Fade_Out();

    if (!Wait_All_Futures())
        return E_FAIL;

	m_futures.clear();
    */
    //Ready_Layer_NorMonster(TEXT("Normal"));

#pragma endregion

	return S_OK;
}

void CLevel_HeinMach::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_F1))
    {
        m_pClientInstance->Camera_Switch_CameraMode(CAMERATYPE::FREE);
    }
    else if (m_pGameInstance->Key_Down(DIK_F2))
    {
        m_pClientInstance->Camera_Switch_CameraMode(CAMERATYPE::PLAYER);
    }

    if (m_eNextLevel != LEVEL::END)
    {      
        if (!m_isOpenLevel) {
            m_pGameInstance->StopAll();
            if (FAILED(m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, m_eNextLevel))))
                return;
            m_isOpenLevel = true;
        }
    }



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


    LIGHT_DESC LightDesc1 = {};
    LightDesc1.eType = LIGHT_DESC::POINT;
    LightDesc1.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc1.vDiffuse = _float4(0.98f, 0.96f, 0.88f, 1.f);
    LightDesc1.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
    LightDesc1.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc1.fRange = 5.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Lantern"), ENUM_CLASS(LEVEL::HEINMACH), LightDesc1, true)))
        return E_FAIL;

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Camera(const _wstring& strLayerTag)
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

    CCamera_Compre* pCamera_Player = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Camera_Compre"), &PlayerCameraDesc));
    pCamera_Player->Set_IsActive(false);
    CGameObject* pPlayer = m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Creature_Player"));
    pCamera_Player->Set_ObjMatrix(dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr());

    static_cast<CKhazan_Spear*>(pPlayer)->Set_Camera(pCamera_Player);

    m_pClientInstance->Add_Camera(ENUM_CLASS(LEVEL::HEINMACH), pCamera_Player);

    m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag, pCamera_Player);

    m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::HEINMACH), ENUM_CLASS(CAMERATYPE::PLAYER));

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Player(const _wstring& strLayerTag)
{
	  /*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
	  	ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Khazan_GSword"), TIME_CHANNEL::PLAYER)))
	  	return E_FAIL;*/

    // Motion Trail Test
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Khazan_Spear"), TIME_CHANNEL::PLAYER)))
        return E_FAIL;

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Monster(const _wstring& strLayerTag)
{

    CMonster::MONSTER_DESC MonsterDesc{};
    MonsterDesc.fAttack = 10.f;
    MonsterDesc.fMaxHP = 5000.f;
    MonsterDesc.fMaxStamina = 100.f;
    MonsterDesc.fMoveSpeed = 10.f;
    MonsterDesc.fSpeedPerSec = 3.f;
    MonsterDesc.fRotationPerSec = 180.f;
    MonsterDesc.strName = "Yetuga";
    //MonsterDesc.strName = "Gomdol";
    //MonsterDesc.strName = "ImpRange";
    //MonsterDesc.strName = "ImpMelee";

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Yetuga"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
        return E_FAIL;


    /*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Gomdol"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
        return E_FAIL;*/

    /*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Imp_Range"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
        return E_FAIL;*/

   /* if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Imp_Melee"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
        return E_FAIL;*/
     
    return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Effect(const _wstring& strLayerTag)
{
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpearWind"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiralSpear_SpearFX"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust2"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust3"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust4"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust5"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust6"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Stamp"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("BlustSmall"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Fire"), 10);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Spawn"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"), 30);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), 30);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"), 30);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"), 10);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Ice"), 20);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Ice_Disappear"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("BloodHit"), 100);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Open"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Breath"), 100);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Focus"), 1); 
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Snow"), 4); 
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Snow_Once"), 20);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Snow_Cam"), 1);
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Snow"), XMVectorSet(240.f, 6.f, 150.f, 1.f));
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Snow"), XMVectorSet(343.f, 8.f, 172.f, 1.f));
    m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Snow_Cam"), XMVectorSet(167.f,43.f, 209.f, 1.f)); 
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("GhostKnight"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("GhostKnight_static"), 4);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("GhostKnight_static_connect"), 4);

    // [GS] 

    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("FerociousMomentum0"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiningCharger0"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiningCharger1"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiningCharger2"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiningCharger_Smoke"), 50);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiningCharger_Trail"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Manifest_Strength_Land"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiningCharger_Smoke_Red"), 50);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpiningCharger_Trail_V"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Giant_Hunt_Land"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Giant_Roar"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("DarkShadow_Land_1"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::HEINMACH), TEXT("DarkShadow_Land_2"), 2);

    return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Decal()
{
    // Decal
    if (FAILED(m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Decal"),
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Pool_Decal"), nullptr, 100)))
        return E_FAIL;

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

    _tchar szDone[MAX_PATH] = {};
    _wstring strDone = { TEXT("MAP LV : %d LOAD DONE\n") };

    wsprintf(szDone, strDone.c_str(), iSubLV);

    OutputDebugStringW(szDone);

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_Monster_SubLV(const _wstring& strLayerTag, const _tchar* pDataFileName, _uint iSubLV, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

        if ("Yetuga" == MonsterData.MonsterKey[i])
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
            MonsterDesc.iLevelIndex = ENUM_CLASS(LEVEL::HEINMACH);

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
                ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Yetuga"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
                return E_FAIL;
        }
        else if ("ImpRange" == MonsterData.MonsterKey[i])
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
            MonsterDesc.iLevelIndex = ENUM_CLASS(LEVEL::HEINMACH);

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
                ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Imp_Range"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
                return E_FAIL;
        }
        else if ("ImpMelee" == MonsterData.MonsterKey[i])
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
            MonsterDesc.iLevelIndex = ENUM_CLASS(LEVEL::HEINMACH);

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
                ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Imp_Melee"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
                return E_FAIL;
        }
    }

    _tchar szDone[MAX_PATH] = {};
    _wstring strDone = { TEXT("MONSTER LV : %d LOAD DONE\n") };

    wsprintf(szDone, strDone.c_str(), iSubLV);

#ifdef _DEBUG

    OutputDebugStringW(szDone);

#endif // _DEBUG

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
        case INTERACTIVE_TYPE::ELEVATOR:
        {
            CElevatorS::ELEVATOR_POS ElevatorPos = {};
            CHECK_FALSE(ReadFile(hFile, &ElevatorPos, sizeof(CElevatorS::ELEVATOR_POS), &dwByte, nullptr), E_FAIL);
            ObjectDesc.pOtherDesc = &ElevatorPos;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_SmallElevator"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::DANJIN:
        {
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_NPC_Danjin"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
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


    LIGHT_DESC LightDesc1 = {};
    LightDesc1.eType = LIGHT_DESC::POINT;
    LightDesc1.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc1.vDiffuse = _float4(0.98f, 0.96f, 0.88f, 1.f);
    LightDesc1.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
    LightDesc1.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc1.fRange = 2.45f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Lantern"), ENUM_CLASS(LEVEL::HEINMACH), LightDesc1, false)))
        return E_FAIL;

	return S_OK;
}

HRESULT CLevel_HeinMach::Ready_FireLights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Fire"), XMLoadFloat4(&LightDesc.vPosition));
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

HRESULT CLevel_HeinMach::Ready_Map_Decal(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

    strDataFilePath += TEXT("_decals.dat");

    DWORD dwByte = {};

    HANDLE hFile = CreateFile(strDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return E_FAIL;
    }
    CHECK_EQUAL(INVALID_HANDLE_VALUE, hFile, E_FAIL);

    // 1. 데칼의 총 개수
    _uint iDecalCnt = {};
    CHECK_FALSE(ReadFile(hFile, &iDecalCnt, sizeof(_uint), &dwByte, nullptr), false);

    // 데칼 총 개수만큼 순회
    for (_uint i = 0; i < iDecalCnt; ++i)
    {
        CDecal* pDecal = static_cast<CDecal*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Decal")));
        CHECK_NULLPTR(pDecal, E_FAIL);

        DECAL_DESC DecalDesc = {};
        // 2. 데칼의 구조체 불러오기
        CHECK_FALSE(ReadFile(hFile, &DecalDesc, sizeof(DECAL_DESC), &dwByte, nullptr), false);
        pDecal->Set_Desc(DecalDesc);

        _float fThreshold = {};
        // 3. 데칼의 쓰레스 홀드 불러오기 ( 마스크 )
        CHECK_FALSE(ReadFile(hFile, &fThreshold, sizeof(_float), &dwByte, nullptr), false);
        pDecal->Set_Threshold(fThreshold);

        _uint iTextureIndex = {};
        // 4. 데칼의 텍스쳐 인덱스 불러오기
        CHECK_FALSE(ReadFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr), false);
        pDecal->Set_TextureIndex(iTextureIndex);

        _float4x4 WorldMatrix = {};
        // 5. 데칼의 월드 행렬 불러오기
        CHECK_FALSE(ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr), false);
        pDecal->Set_WorldMatrix(WorldMatrix);

        // 데코용 데칼 true
        pDecal->Set_EnableDecoration(true);

        m_pGameInstance->Batch_Decal(pDecal);
    }

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_MapObject_DEST(const _wstring& strLayerTag, const _tchar* pDataFilename, LEVEL eCurrentLevel)
{
    CProp_Object::PROP_OBJECT_DESC FenceDesc = {};

    FenceDesc.eLevel = eCurrentLevel;

    MAPOBJECT_PROPERTIES PropProperties1 = {};

    FenceDesc.Properties = PropProperties1;
    _float4x4 WorldMatrix{};
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    WorldMatrix._41 = 4.f;
    WorldMatrix._43 = 4.f;
    FenceDesc.WorldMatrix = WorldMatrix;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Fence"), TIME_CHANNEL::WORLD, &FenceDesc)))
        return E_FAIL;

    CProp_Object::PROP_OBJECT_DESC PotDesc = {};

    PotDesc.eLevel = eCurrentLevel;

    MAPOBJECT_PROPERTIES PropProperties2 = {};

    PotDesc.Properties = PropProperties2;
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    WorldMatrix._43 = 4.f;
    PotDesc.WorldMatrix = WorldMatrix;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Pot"), TIME_CHANNEL::WORLD, &PotDesc)))
        return E_FAIL;

    CProp_Object::PROP_OBJECT_DESC BarrelDesc = {};

    BarrelDesc.eLevel = eCurrentLevel;

    MAPOBJECT_PROPERTIES PropProperties3 = {};

    BarrelDesc.Properties = PropProperties3;
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    WorldMatrix._43 = 10.f;
    BarrelDesc.WorldMatrix = WorldMatrix;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), strLayerTag,
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Barrel"), TIME_CHANNEL::WORLD, &BarrelDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_HeinMach::Ready_Layer_UI()
{

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
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::LEVEL_CHANGE), m_iEventID);

	__super::Free();

	Safe_Release(m_pClientInstance);
}
