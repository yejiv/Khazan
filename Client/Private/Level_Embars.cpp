#include "Level_Embars.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Level_Loading.h"
#include "Event_Defines.h"
#include "Player.h"
#include "Camera_Compre.h"
#include "Dummy.h"
#include "Monster.h"
#include "Khazan_Spear.h"
#include "Khazan_GSword.h"
#include "Sequence_Embars_Puzzle_First.h"
#include "Sequence_Embars_Puzzle_Second.h"

#pragma region MAP OBJECT
#include "MapObject_Header.h"
#pragma endregion

#include "UI_Announce_MapName.h"
#include "Dragonian_Melee.h"
#include "Dragonian_Rampage.h"
#include "Interaction_Item.h"

CLevel_Embars::CLevel_Embars(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
    , m_pClientInstance(CClientInstance::GetInstance())
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CLevel_Embars::Initialize()
{
#pragma region 수정된 코드

    // 연동되지 않는 것들을 쓰레드풀로 돌리기
    //m_futures.push_back(m_pGameInstance->Add_Task([this]() {

        //return S_OK;
        //}));
    CHECK_FAILED(Ready_Layer_Item(), E_FAIL);

    CHECK_FAILED(Ready_Layer_Player(TEXT("Layer_Creature_Player")), E_FAIL);

    CHECK_FAILED(Ready_Layer_Camera(TEXT("Layer_Camera")), E_FAIL);

    CHECK_FAILED(Ready_Layer_Effect(TEXT("Layer_Effect")), E_FAIL);

    /*m_futures.push_back(m_pGameInstance->Add_Task([this]() {

        }));*/


    CHECK_FAILED(Ready_Lights(TEXT("Embars"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);
    CHECK_FAILED(Ready_Lights(), E_FAIL);

    CHECK_FAILED(Ready_FireLights(TEXT("Embars_Point"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);

    CHECK_FAILED(Ready_BrazierLights(TEXT("Embars_Brazier"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);

    //CHECK_FAILED(Ready_Map_Decal(TEXT("Layer_Decal"), TEXT("Embars"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);        

    for (_uint i = 0; i < EMBARS_SUBLV; ++i)
    {
        CHECK_FAILED(Ready_Layer_MapObject_SubLV(TEXT("Layer_MapObject"), TEXT("Embars"), i, LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);
        CHECK_FAILED(Ready_Layer_Monster_SubLV(TEXT("Layer_MapObject"), TEXT("Embars"), i, LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);
    }

    CHECK_FAILED(Ready_Layer_MapObject_Interactive(TEXT("Layer_MapObject_Interact"), TEXT("Embars"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);

    CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Layer_MapObject_Inst"), TEXT("Embars"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);

    CHECK_FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"), TEXT("Embars"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);

    CHECK_FAILED(Ready_Layer_Cloud(TEXT("Layer_Sky"), TEXT("Embars"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);

    CHECK_FAILED(Ready_Trigger(TEXT("Layer_Trigger"), TEXT("Embars"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);

    CHECK_FAILED(Ready_Shader_Settings(), E_FAIL);
    CHECK_FAILED(Ready_Layer_Decal(), E_FAIL);

    CClientInstance::GetInstance()->Fade_In([this]() {Start_Event(); });

    CHECK_FAILED(Ready_SoundSetting(), E_FAIL);

    if (!Wait_All_Futures())
        return E_FAIL;

    m_futures.clear();    

    m_iEventID = m_pGameInstance->Subscribe_Event<EVENT_LEVEL_CHANGE>(ENUM_CLASS(EVENT_TYPE::LEVEL_CHANGE), [&](const EVENT_LEVEL_CHANGE& e)
        {
            m_eNextLevel = static_cast<LEVEL>(e.iLevel);
        });

    m_pClientInstance->Set_PlayerInput(true);
#pragma endregion

    CHECK_FAILED(Ready_Layer_Pet(TEXT("Layer_Pet")), E_FAIL);
	return S_OK;
}

void CLevel_Embars::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_F1, INPUT_TYPE::FORCE))
    {
        m_pClientInstance->Camera_Switch_CameraMode(CAMERATYPE::FREE);
    }
    else if (m_pGameInstance->Key_Down(DIK_F2, INPUT_TYPE::FORCE))
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

	return;
}

HRESULT CLevel_Embars::Render()
{
	SetWindowText(g_hWnd, TEXT("엠바스 유적지 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Embars::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Embars::Ready_Layer_UI()
{
	
	return S_OK;
}

HRESULT CLevel_Embars::Ready_Layer_Player(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc;
    Desc.iLevelIndex = ENUM_CLASS(LEVEL::VIPER);

  /*  if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EMBARS), strLayerTag,
        ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Khazan_Spear"), TIME_CHANNEL::PLAYER, &Desc)))
        return E_FAIL;*/

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EMBARS), strLayerTag,
        ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Khazan_GSword"), TIME_CHANNEL::PLAYER, &Desc)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_Embars::Ready_Layer_Camera(const _wstring& strLayerTag)
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

    CCamera_Compre* pCamera_Player = dynamic_cast<CCamera_Compre*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Camera_Compre"), &PlayerCameraDesc));
    pCamera_Player->Set_IsActive(true);
    CGameObject* pPlayer = m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::EMBARS), TEXT("Layer_Creature_Player"));
    pCamera_Player->Set_ObjMatrix(dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrixPtr());

    //static_cast<CKhazan_Spear*>(pPlayer)->Set_Camera(pCamera_Player);
    static_cast<CKhazan_GSword*>(pPlayer)->Set_Camera(pCamera_Player);

    m_pClientInstance->Add_Camera(ENUM_CLASS(LEVEL::EMBARS), pCamera_Player);

    m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::EMBARS), strLayerTag, pCamera_Player);

    m_pClientInstance->Change_Camera(ENUM_CLASS(LEVEL::EMBARS), ENUM_CLASS(CAMERATYPE::PLAYER));

    return S_OK;
}


HRESULT CLevel_Embars::Ready_Layer_Effect(const _wstring& strLayerTag)
{
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("SpearWind"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("SpiralSpear_SpearFX"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Blust"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Blust2"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Blust3"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Blust4"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Blust5"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Blust6"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Blust9"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Stamp"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("BlustSmall"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Fire"), 42);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Spawn"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("BloodHit"), 100);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Open"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("GhostKnight"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("GhostKnight_static"), 4);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("GhostKnight_static_connect"), 4);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Brazier"), 10);

    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("DoorOpen"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("labber"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Statue_Dust"), 5);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("LeverGear_On_Static"), 4);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("LeverGear_On"), 4);

    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Jump"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Land_Shield"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Land_Sword"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Spark_Loop"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Elamein_Sword_Wind"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Smoke_Small"), 10);

    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Gacha_Fail"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Gacha_Suceess"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("ITEM_FX"), 5);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("ITEM_RARE_FX"), 5);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("ITEM_UNIQUE_FX"), 5);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item_normal_Gacha"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("item_rare_Gacha"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("item_unique_Gacha"), 1);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("item_blust"), 1);


    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("FerociousMomentum0"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("SpiningCharger0"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("SpiningCharger1"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("SpiningCharger2"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("SpiningCharger_Smoke"), 50);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("SpiningCharger_Trail"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Manifest_Strength_Land"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("SpiningCharger_Smoke_Red"), 50);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("SpiningCharger_Trail_V"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Giant_Hunt_Land"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Giant_Roar"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("DarkShadow_Land_1"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("DarkShadow_Land_2"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Body_Wind"), 4);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("particle"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("particle2"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Inner_Range_Ground"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Dawn_BloodTrail1"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Dawn_BloodTrail2"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("GS_StrongATK"), 2);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Ghost_Dark_Shadow_Land"), 1);

    // [Player Ect]
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Guard"), 100);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("PerfectGaurd"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Lachryma"), 3);
    m_pGameInstance->Add_Effect_ToPool(ENUM_CLASS(LEVEL::EMBARS), TEXT("Lachryma_Arm"), 3);

    return S_OK;
}

HRESULT CLevel_Embars::Ready_Shader_Settings()
{
    // 그림자
    SHADOW_DESC ShadowDesc{};
    ShadowDesc.fSplit = 35.f;
    ShadowDesc.vLightDir = _float3(-0.1f, -1.f, -0.1f);
    ShadowDesc.fBias = 0.001f;
    ShadowDesc.fIntensity = 0.8f;
    m_pGameInstance->Set_ShadowDesc(ShadowDesc);

    // 초기 Fog
    //  FOG_TRANSITION_DESC FogDesc{};
    //  FogDesc.fDensity = 0.05f;
    //  FogDesc.fBias = 0.8f;
    //  FogDesc.vColor = _float4(0.f, 0.176f, 0.341f, 1.f);
    //  FogDesc.isUseHeight = false;
    //  FogDesc.isUseNoise = false;
    //  m_pGameInstance->Start_FogTransition(0.f, FogDesc);

    // 포그 라이트 블리드 1 설정
    FOG_CONFIG FogConfig{};
    FogConfig.eType = FOG_CONFIG::EXP;
    FogConfig.fDensity = 0.05f;
    FogConfig.fBias = 0.8f;
    FogConfig.vColor = _float4(0.f, 0.176f, 0.341f, 1.f);
    FogConfig.Noise.isEnable = false;
    FogConfig.isUseHeight = false;
    FogConfig.isUseSubColor = false;
    FogConfig.fLightBleedStrength = 1.f;
    m_pGameInstance->Set_FogConfig(FogConfig);

    // 림 라이트 강도 줄이기
    RIM_LIGHT_DESC RimDesc{};
    RimDesc.fPower = 5.f;
    RimDesc.isToonLight = false;
    RimDesc.fToonThreshold = 1.f;
    RimDesc.fIntensity = 0.15f;
    m_pGameInstance->Set_RimLightDesc(RimDesc);

    // 스페큘러 강도 줄이기
    m_pGameInstance->Set_SpecularAttenuation(1.5f);

    return S_OK;
}

HRESULT CLevel_Embars::Ready_Layer_Item()
{
    CGameObject::GAMEOBJECT_DESC desc{};

    desc.iLevelIndex = ENUM_CLASS(LEVEL::EMBARS);

    m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Item"), ENUM_CLASS(LEVEL::EMBARS), TEXT("Item"), &desc, 10);

    CInteraction_Item* pItem = dynamic_cast<CInteraction_Item*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::EMBARS), TEXT("Item")));

    pItem->Special_Item(TEXT("Handwriting"), XMVectorSet(109.18f, -83.451f, 52.09f, 1.f));

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::EMBARS), TEXT("Layer_Item"), pItem);

    return S_OK;
}

HRESULT CLevel_Embars::Ready_SoundSetting()
{
    // 사운드 매니저 글로벌 볼륨
    _float fGlobalVolume = m_pGameInstance->Get_Gloval_Volume();

    // 글로벌 볼륨 세팅 후 환경음, BGM 사운드 세팅 및 재생
    CClientInstance::GetInstance()->Set_Volume_BGM(0.65f);
    CClientInstance::GetInstance()->Set_Volume_AMB(0.65f);
    CClientInstance::GetInstance()->BGM_HeinMach_Entry();

    return S_OK;
}

HRESULT CLevel_Embars::Ready_Layer_Sky(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_Embars::Ready_Layer_Cloud(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_Embars::Ready_Layer_MapObject_SubLV(const _wstring& strLayerTag, const _tchar* pDataFileName, _uint iSubLV, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_Embars::Ready_Layer_Monster_SubLV(const _wstring& strLayerTag, const _tchar* pDataFileName, _uint iSubLV, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

        if ("Dragonian_Melee" == MonsterData.MonsterKey[i])
        {
            CDragonian_Melee::DRAGON_MELEE_MONSTER_DESC MonsterDesc{};
            MonsterDesc.fAttack = m_pGameInstance->Rand(50.f, 100.f);
            MonsterDesc.fMaxHP = m_pGameInstance->Rand(2000.f, 2500.f);
            MonsterDesc.fMaxStamina = m_pGameInstance->Rand(80.f, 140.f);
            MonsterDesc.fMoveSpeed = 10.f;
            MonsterDesc.fSpeedPerSec = 3.f;
            MonsterDesc.fRotationPerSec = 180.f;

            MonsterDesc.WorldMatrix = WorldMatrix;
            MonsterDesc.strName = "Dragonian_Melee";
            MonsterDesc.iLevelIndex = ENUM_CLASS(eCurrentLevel);
            MonsterDesc.isSleep = false;
            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
                ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Monster_Dragonian_Melee"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
                return E_FAIL;
        }
        else if ("Dragonian_Claw" == MonsterData.MonsterKey[i])
        {
            CDragonian_Rampage::DRAGON_RAMPAGE_MONSTER_DESC MonsterDesc{};
            MonsterDesc.fAttack = m_pGameInstance->Rand(80.f, 120.f);
            MonsterDesc.fMaxHP = m_pGameInstance->Rand(3500.f, 4000.f);
            MonsterDesc.fMaxStamina = m_pGameInstance->Rand(100.f, 150.f);
            MonsterDesc.fMoveSpeed = 10.f;
            MonsterDesc.fSpeedPerSec = 3.f;
            MonsterDesc.fRotationPerSec = 180.f;

            MonsterDesc.WorldMatrix = WorldMatrix;

            MonsterDesc.strName = "Dragonian_Rampage";
            MonsterDesc.iLevelIndex = ENUM_CLASS(eCurrentLevel);
            MonsterDesc.isSleep = false;
            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
                ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Monster_Dragonian_Rampage"), TIME_CHANNEL::ENEMY, &MonsterDesc)))

                return E_FAIL;
        }
        else if ("Dragonian_Melee_Sleep" == MonsterData.MonsterKey[i])
        {
            CDragonian_Melee::DRAGON_MELEE_MONSTER_DESC MonsterDesc{};
            MonsterDesc.fAttack = m_pGameInstance->Rand(50.f, 100.f);
            MonsterDesc.fMaxHP = m_pGameInstance->Rand(2000.f, 2500.f);
            MonsterDesc.fMaxStamina = m_pGameInstance->Rand(80.f, 140.f);
            MonsterDesc.fMoveSpeed = 10.f;
            MonsterDesc.fSpeedPerSec = 3.f;
            MonsterDesc.fRotationPerSec = 180.f;

            MonsterDesc.WorldMatrix = WorldMatrix;
            MonsterDesc.strName = "Dragonian_Melee";
            MonsterDesc.iLevelIndex = ENUM_CLASS(eCurrentLevel);
            MonsterDesc.isSleep = true;
            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
                ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Monster_Dragonian_Melee"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
                return E_FAIL;
        }
        else if ("Dragonian_Claw_Sleep" == MonsterData.MonsterKey[i])
        {
            CDragonian_Rampage::DRAGON_RAMPAGE_MONSTER_DESC MonsterDesc{};
            MonsterDesc.fAttack = m_pGameInstance->Rand(80.f, 120.f);
            MonsterDesc.fMaxHP = m_pGameInstance->Rand(3500.f, 4000.f);
            MonsterDesc.fMaxStamina = m_pGameInstance->Rand(100.f, 150.f);
            MonsterDesc.fMoveSpeed = 10.f;
            MonsterDesc.fSpeedPerSec = 3.f;
            MonsterDesc.fRotationPerSec = 180.f;

            MonsterDesc.WorldMatrix = WorldMatrix;

            MonsterDesc.strName = "Dragonian_Rampage";
            MonsterDesc.iLevelIndex = ENUM_CLASS(eCurrentLevel);
            MonsterDesc.isSleep = true;
            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
                ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Monster_Dragonian_Rampage"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
                return E_FAIL;
        }
        else if ("Elamein" == MonsterData.MonsterKey[i])
        {
            CMonster::MONSTER_DESC MonsterDesc{};
            MonsterDesc.fAttack = 100.f;
            MonsterDesc.fMaxHP = 12000.f;
            MonsterDesc.fMaxStamina = 1200.f;
                                                                                            
            MonsterDesc.fMoveSpeed = 10.f;
            MonsterDesc.fSpeedPerSec = 3.f;
            MonsterDesc.fRotationPerSec = 180.f;

            MonsterDesc.WorldMatrix = WorldMatrix;

            MonsterDesc.strName = "Elamein";
            MonsterDesc.iLevelIndex = ENUM_CLASS(eCurrentLevel);

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
                ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Monster_Elamein"), TIME_CHANNEL::ENEMY, &MonsterDesc)))

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

HRESULT CLevel_Embars::Ready_Layer_MapObject_Interactive(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

    _uint iStatueIndex0 = {};
    _uint iStatueIndex1 = {};

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
        case INTERACTIVE_TYPE::TOMBSTONE:
        {
            _int iTombStoneID = {};
            CHECK_FALSE(ReadFile(hFile, &iTombStoneID, sizeof(_int), &dwByte, nullptr), E_FAIL);
            ObjectDesc.pOtherDesc = &iTombStoneID;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_TombStone"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
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
        case INTERACTIVE_TYPE::LEVER:
        {
            _int iEventID = {};
            CHECK_FALSE(ReadFile(hFile, &iEventID, sizeof(_int), &dwByte, nullptr), E_FAIL);
            ObjectDesc.pOtherDesc = &iEventID;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Lever"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::GEAR1:
        {
            _int iEventID = {};
            CHECK_FALSE(ReadFile(hFile, &iEventID, sizeof(_int), &dwByte, nullptr), E_FAIL);
            ObjectDesc.pOtherDesc = &iEventID;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Lever_Gear"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::GEAR2:
        {
            CGearGate::DOOR_GEAR_EVENTID EventIDs = {};
            CHECK_FALSE(ReadFile(hFile, &EventIDs, sizeof(CGearGate::DOOR_GEAR_EVENTID), &dwByte, nullptr), E_FAIL);
            ObjectDesc.pOtherDesc = &EventIDs;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_GearGate"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::STATUE:
        {
            CStatue::STATUE_DESC StatueDesc = {};

            CHECK_FALSE(ReadFile(hFile, &StatueDesc.iEventID, sizeof(_int), &dwByte, nullptr), E_FAIL);
            
            CHECK_FALSE(ReadFile(hFile, &StatueDesc.StatueRotation, sizeof(CStatue::STATUE_ROTATION), &dwByte, nullptr), E_FAIL);

            switch (StatueDesc.iEventID)
            {
            case 0:
                StatueDesc.iStatueIndex = iStatueIndex0++;
                break;
            case 1:
                StatueDesc.iStatueIndex = iStatueIndex1++;
                break;
            }

            ObjectDesc.pOtherDesc = &StatueDesc;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Statue"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::VERTICALGATE:
        {
            _int iEventID = {};
            CHECK_FALSE(ReadFile(hFile, &iEventID, sizeof(_int), &dwByte, nullptr), E_FAIL);
            ObjectDesc.pOtherDesc = &iEventID;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_VerticalGate"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::IRONGATE:
        {
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_IronGate"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::LADDER:
        {
            CLadder::LADDER_OFFSET LadderOffSet = {};
            CHECK_FALSE(ReadFile(hFile, &LadderOffSet.fOffSetHeight, sizeof(_float), &dwByte, nullptr), E_FAIL);
            CHECK_FALSE(ReadFile(hFile, &LadderOffSet.iSegmentCount, sizeof(_int), &dwByte, nullptr), E_FAIL);
            ObjectDesc.pOtherDesc = &LadderOffSet;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Ladder"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::UNLOCKGEAR:
        {
            _int iEventID = {};
            CHECK_FALSE(ReadFile(hFile, &iEventID, sizeof(_int), &dwByte, nullptr), E_FAIL);
            ObjectDesc.pOtherDesc = &iEventID;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_UnLockGear"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::LARGEELEVATOR:
        {
            CElevatorL::LARGE_ELEVATOR_POS ElevatorPos = {};
            CHECK_FALSE(ReadFile(hFile, &ElevatorPos, sizeof(CElevatorL::LARGE_ELEVATOR_POS), &dwByte, nullptr), E_FAIL);
            ObjectDesc.pOtherDesc = &ElevatorPos;
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_LargeElevator"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::GIANTGATE:
        {
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_GiantGate"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::GACHANPC:
        {
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_NPC_Gacha"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::DAPHRONA:
        {
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_NPC_Daphrona"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::DUIMUK:
        {
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_NPC_Duimuk"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        case INTERACTIVE_TYPE::DANJINJAR:
        {
            CDanjinJar::DANJINJAR_INFO JarInfo = {};

            CHECK_FALSE(ReadFile(hFile, &JarInfo.eJarType, sizeof(CDanjinJar::DANJINJAR_TYPE), &dwByte, nullptr), E_FAIL);

            CHECK_FALSE(ReadFile(hFile, &JarInfo.StepPosition, sizeof(CDanjinJar::DANJINJAR_STEP), &dwByte, nullptr), E_FAIL);

            ObjectDesc.pOtherDesc = &JarInfo;

            _wstring strPrototypeTag = {};

            switch (JarInfo.eJarType)
            {
            case CDanjinJar::DANJINJAR_TYPE::A:
                strPrototypeTag = TEXT("Prototype_GameObject_Prop_NPC_Jar_1st");
                break;
            case CDanjinJar::DANJINJAR_TYPE::B:
                strPrototypeTag = TEXT("Prototype_GameObject_Prop_NPC_Jar_2nd");
                break;
            case CDanjinJar::DANJINJAR_TYPE::C:
                strPrototypeTag = TEXT("Prototype_GameObject_Prop_NPC_Jar_3rd");
                break;
            case CDanjinJar::DANJINJAR_TYPE::D:
                strPrototypeTag = TEXT("Prototype_GameObject_Prop_NPC_Jar_4th");
                break;
            case CDanjinJar::DANJINJAR_TYPE::E:
                strPrototypeTag = TEXT("Prototype_GameObject_Prop_NPC_Jar_5th");
                break;
            case CDanjinJar::DANJINJAR_TYPE::F:
                strPrototypeTag = TEXT("Prototype_GameObject_Prop_NPC_Jar_6th");
                break;
            case CDanjinJar::DANJINJAR_TYPE::G:
                strPrototypeTag = TEXT("Prototype_GameObject_Prop_NPC_Jar_7th");
                break;
            case CDanjinJar::DANJINJAR_TYPE::H:
                strPrototypeTag = TEXT("Prototype_GameObject_Prop_NPC_Jar_8th");
                break;
            }

            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), strPrototypeTag, TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);

            break;
        }
        case INTERACTIVE_TYPE::DESTRUCTIBLE:
        {
            CProp_Destructible::MODEL_TYPE eModelType = {};

            CHECK_FALSE(ReadFile(hFile, &eModelType, sizeof(CProp_Destructible::MODEL_TYPE), &dwByte, nullptr), E_FAIL);

            _matrix WorldMatrix = { XMLoadFloat4x4(&ObjectDesc.WorldMatrix) };

            WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
            WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
            WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

            switch (eModelType)
            {
            case CProp_Destructible::MODEL_TYPE::FENCE:
            {
                CFence::PROP_FENCE_DESC FenceDesc = {};

                FenceDesc.eLevel = eCurrentLevel;

                XMStoreFloat4x4(&FenceDesc.WorldMatrix, WorldMatrix);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel),
                    TEXT("Prototype_GameObject_Prop_Fence"), TIME_CHANNEL::MAP, &FenceDesc), E_FAIL);
                break;
            }
            case CProp_Destructible::MODEL_TYPE::POT:
            {
                CPot::PROP_POT_DESC PotDesc = {};

                PotDesc.eLevel = eCurrentLevel;

                XMStoreFloat4x4(&PotDesc.WorldMatrix, WorldMatrix);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel),
                    TEXT("Prototype_GameObject_Prop_Pot"), TIME_CHANNEL::MAP, &PotDesc), E_FAIL);
                break;
            }
            case CProp_Destructible::MODEL_TYPE::BARREL:
            {
                CBarrel::PROP_BARREL_DESC BarrelDesc = {};

                BarrelDesc.eLevel = eCurrentLevel;

                XMStoreFloat4x4(&BarrelDesc.WorldMatrix, WorldMatrix);

                CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel),
                    TEXT("Prototype_GameObject_Prop_Barrel"), TIME_CHANNEL::MAP, &BarrelDesc), E_FAIL);
                break;
            }
            }
            break;
        }
        case INTERACTIVE_TYPE::ILLUSION_WALL:
        {
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Illusion_Wall"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
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

HRESULT CLevel_Embars::Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_Embars::Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Lantern"), ENUM_CLASS(LEVEL::EMBARS), LightDesc1, false)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Embars::Ready_Lights()
{
    LIGHT_DESC LightDesc = {};
    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vDiffuse = _float4(2.f, 1.6f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 0.8f, 0.5f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 1.f;

    if (FAILED(m_pGameInstance->Add_Light(TEXT("DanjinJar_1"), ENUM_CLASS(LEVEL::EMBARS), LightDesc)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("DanjinJar_2"), ENUM_CLASS(LEVEL::EMBARS), LightDesc)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("DanjinJar_3"), ENUM_CLASS(LEVEL::EMBARS), LightDesc)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("DanjinJar_4"), ENUM_CLASS(LEVEL::EMBARS), LightDesc)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("DanjinJar_5"), ENUM_CLASS(LEVEL::EMBARS), LightDesc)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("DanjinJar_6"), ENUM_CLASS(LEVEL::EMBARS), LightDesc)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("DanjinJar_7"), ENUM_CLASS(LEVEL::EMBARS), LightDesc)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("DanjinJar_8"), ENUM_CLASS(LEVEL::EMBARS), LightDesc, false)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Light(TEXT("GachaSelect"), ENUM_CLASS(LEVEL::EMBARS), LightDesc, false)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Light(TEXT("GachaSelect1"), ENUM_CLASS(LEVEL::EMBARS), LightDesc, false)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Light(TEXT("GachaSelect2"), ENUM_CLASS(LEVEL::EMBARS), LightDesc, false)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Light(TEXT("GachaSelect3"), ENUM_CLASS(LEVEL::EMBARS), LightDesc, false)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Light(TEXT("DanjinJar_Pet"), ENUM_CLASS(LEVEL::EMBARS), LightDesc, false)))
        return E_FAIL;

    LightDesc.vDiffuse = _float4(2.5f, 0.8f, 0.8f, 1.f);
    LightDesc.vPosition = _float4(-67.325f, -90.f, -41.831f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 0.1f);
    LightDesc.fRange = 17.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Mirroball"), ENUM_CLASS(LEVEL::EMBARS), LightDesc, false)))
        return E_FAIL;

    LightDesc = {};
    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 5.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("BladeNexus_ActivateLight"), ENUM_CLASS(LEVEL::EMBARS), LightDesc, false)))
        return E_FAIL;

    LightDesc = {};
    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 3.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Player_GuardLight"), ENUM_CLASS(LEVEL::EMBARS), LightDesc, false)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Embars::Ready_FireLights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Fire"), XMLoadFloat4(&LightDesc.vPosition));
    }

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_Embars::Ready_BrazierLights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("Brazier"), XMLoadFloat4(&LightDesc.vPosition));
    }

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_Embars::Ready_Trigger(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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
            ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Embars_Trigger"), TIME_CHANNEL::MAP, &TriggerDesc), E_FAIL);
    }

    return S_OK;
}

HRESULT CLevel_Embars::Ready_Map_Decal(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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
        CDecal_Static* pDecal = static_cast<CDecal_Static*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Decal_Static")));
        CHECK_NULLPTR(pDecal, E_FAIL);

        STATIC_DECAL_DESC DecalDesc = {};
        // 2. 데칼의 구조체 불러오기
        CHECK_FALSE(ReadFile(hFile, &DecalDesc, sizeof(STATIC_DECAL_DESC), &dwByte, nullptr), false);
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
        pDecal-> Set_WorldMatrix(WorldMatrix);

        m_pGameInstance->Batch_Decal(pDecal);
    }

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_Embars::Ready_Layer_Decal()
{
    // Decal
    if (FAILED(m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Decal"),
        ENUM_CLASS(LEVEL::EMBARS), TEXT("Pool_Decal"), nullptr, 100)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Embars::Ready_Layer_Pet(const _wstring& strLayerTag)
{
    CMonster::MONSTER_DESC MonsterDesc{};
    MonsterDesc.fAttack = 100.f;
    MonsterDesc.fMaxHP = 100.f;
    MonsterDesc.fMaxStamina = 250.f;
    MonsterDesc.fMoveSpeed = 10.f;
    MonsterDesc.fSpeedPerSec = 3.f;
    MonsterDesc.fRotationPerSec = 180.f;
    XMStoreFloat4x4(&MonsterDesc.WorldMatrix, XMMatrixIdentity());
    MonsterDesc.WorldMatrix.m[3][0] = -58.26f;
    MonsterDesc.WorldMatrix.m[3][1] = -92.f;
    MonsterDesc.WorldMatrix.m[3][2] = -38.42f;

    MonsterDesc.strName = "Pet_Danjinjar";
    MonsterDesc.iLevelIndex = ENUM_CLASS(LEVEL::EMBARS);
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EMBARS), strLayerTag,
        ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Pet_Danjinjar"), TIME_CHANNEL::ENEMY, &MonsterDesc)))
        return E_FAIL;

    return S_OK;
}

_bool CLevel_Embars::Wait_All_Futures()
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

void CLevel_Embars::Start_Event()
{
    //NPC 대사
    m_pGameInstance->Emit_Event<EVENT_ANNOUNCE_TALK>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_TALK), EVENT_ANNOUNCE_TALK{ 20 });

    //맵 이름 표시
    EVENT_ANNOUNCE_MAPNAME Desc = {};
    Desc.fTime = 2.f;
    Desc.iMapType = ENUM_CLASS(CUI_Announce_MapName::MAP_TYPE::EMBARS);
    Desc.fFadeOutTime = 2.5f;
    Desc.isDissovle = true;
    m_pGameInstance->Emit_Event<EVENT_ANNOUNCE_MAPNAME>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_MAPNAME), Desc);

    m_pClientInstance->Camera_MouseOnOff(true);
    m_pGameInstance->Decal_OnOff(true);
}

CLevel_Embars* CLevel_Embars::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Embars* pInstance = new CLevel_Embars(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Embars"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Embars::Free()
{
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::LEVEL_CHANGE), m_iEventID);

    __super::Free();

    Safe_Release(m_pClientInstance);
}
