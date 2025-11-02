#include "Loader.h"

#include "GameInstance.h"


#include "Player.h"
#include "Body_Player.h"
#include "Camera.h"
#include "Camera_Free.h"
#include "Camera_Compre.h"
#include "Sky.h"
#include "SkySphere.h"
#include "Terrain.h"
#include "Dummy.h"

#pragma region Khazan
#include "JOH_Test1.h"
#include "Khazan_Sample.h"
#include "Body_Khazan_Sample.h"
#include "Spear_Khazan_Sample.h"
#pragma endregion

#pragma region MAP OBJECT
#include "MapObject_Header.h"
#pragma endregion

#pragma region Monster
#include "Yetuga.h"
#include "Body_Yetuga.h"
#include "Projectile_Yetuga.h"
#pragma endregion

//static mutex g_GpuGate;

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}


HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	//InitializeCriticalSection(&m_CriticalSection);

	//m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	/*if (0 == m_hThread)
		return E_FAIL;*/

	return Loading();
}

_bool CLoader::AllReady(const std::vector<std::future<HRESULT>>& futures)
{
	for (auto const& f : futures) {
		if (f.wait_for(0ms) != std::future_status::ready)
			return false; // 하나라도 준비 안 됐으면 즉시 false
	}
	return true;
}

void CLoader::Update()
{
	if (!AllReady(m_futures)) {
		// 아직 로딩 중
		return;
	}

	_bool all_ok = true;
	for (auto& f : m_futures) {
		try {
			const HRESULT hr = f.get();
			if (FAILED(hr)) all_ok = false;
		}
		catch (...) {
			all_ok = false;
		}
	}
	m_futures.clear();

	m_isFinished = all_ok;
	if (m_isFinished)
		lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	else
		lstrcpy(m_szLoadingText, TEXT("로딩 실패하였습니다."));
}

HRESULT CLoader::Loading()
{

	HRESULT			hr = {};

	switch(m_eNextLevelID)
	{
	case LEVEL::TITLE:		
		m_futures.push_back(m_pGameInstance->Add_Task([this]() {
			return Loading_For_Title_Level();
			}));
		break;
	case LEVEL::HEINMACH:
		m_futures.push_back(m_pGameInstance->Add_Task([this]() {
			return Loading_For_Stage1_Level();
			}));
		break;
	case LEVEL::CREVICE:
		break;
	case LEVEL::EMBARS:
		break;
	case LEVEL::VIPER:
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}


HRESULT CLoader::Loading_For_Title_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1_Level()
{

	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Stage1_Texture();
		}));
	
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Stage1_Model();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Stage1_Shader();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Stage1_GameObject();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Loading_Prototype_MapObject_Inst_From_DAT(TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		}));

	//Loading_For_Stage1_Model();

	//Loading_For_Stage1_Shader();

	//Loading_For_Stage1_GameObject();

	//CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

	//CHECK_FAILED(Loading_Prototype_MapObject_Inst_From_DAT(TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);

	//lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	//m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1_Texture()
{
	CoInitGuard co;

	//lock_guard<mutex> gpu_lock(g_GpuGate);

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sky/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Mask_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Mask_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain/TerrainMask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain/Brush.png"), 1))))
		return E_FAIL;

#pragma region SKY SPHERE TEXTURES
	/* Prototype_Component_Texture_Cloud_Dist_Gradation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Cloud_Dist_Gradation"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/DistanceGradation.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Cloud_LookUp */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Cloud_LookUp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FTW_Sky_Cloud_LookUp.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Cloud_Normal */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Cloud_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FTW_Sky_Cloud_Nomal.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Cloud_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Cloud_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FTW_Sky_Cloud_Distortion.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky_Nebula */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Sky_Nebula"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FTW_Sky_Nebula.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky_Star_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Sky_Star_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FTW_Sky_Star_Mask_001.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Moon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Moon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FT_Rgb_Moon_001.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Ring */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FT_Refraction_Ring.dds"), 1))))
		return E_FAIL;
#pragma endregion


	///* Prototype_Component_Texture_Brush */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_BackGround"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/BG/T_Hud_BG_Deco_Pathfinder_01.png"), 1))))
	//	return E_FAIL;

	//vector<const _tchar*> TextureList;
	//TextureList.reserve(2);
	//TextureList.push_back(TEXT("T_BG_GrandFlores.png"));
	//TextureList.push_back(TEXT("T_BG_ValleyOfTheFallenSouls.png"));

	///* Prototype_Component_Texture_Test */
 //  	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Test"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Textures/UI/BackGround/"), TextureList))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1_Model()
{
	CoInitGuard co;

	//lock_guard<mutex> gpu_lock(g_GpuGate);

	/* Prototype_Component_Model_Fiona */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Test/Fiona/Fiona.dat"))))
	//	return E_FAIL;

#pragma region SKY SPHERE MODELS
	/* Prototype_Component_Model_SkyMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_SkyMesh"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Map/Prop/NonAnim/SkySphere/SkyMesh.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_CloudMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_CloudMesh"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Map/Prop/NonAnim/SkySphere/CloudMesh.dat"))))
		return E_FAIL;
#pragma endregion

	/* Prototype_Component_Model_Khazan_Sample*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Sample"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Khazan_Sample.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Spear_Khazan_Sample*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Spear_Khazan_Sample"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Spear/Spear.dat"))))
		return E_FAIL;

#pragma region YETUGA

	// Prototype_Component_Model_Yetuga
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Yetuga/Yetuga.dat"))))
		return E_FAIL;

	// Prototype_Component_Model_Yetuga_Rock
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga_Rock"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Yetuga/Yetuga_Rock/Yetuga_Rock.dat"))))
		return E_FAIL;

#pragma endregion
	/////* Prototype_Component_Model_Khazan */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Test/Khazan/Khazan.dat"))))
	//	return E_FAIL;

	/////* Prototype_Component_Model_WP_WOD_Ground_Base_004 */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_WP_WOD_Ground_Base_004"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Map/Test/WP_WOD_Ground_Base_004/WP_WOD_Ground_Base_004.dat"))))
	//	return E_FAIL;

	/* Prototype_Component_Model_JOH_TestModel */
 	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_JOH_TestModel"),
		//CModel::Create(m_pDevice, m_pContext, "../Data/Test/Test_Player/Test_Player.dat"))))
		//return E_FAIL;

#pragma region 모델 원형 : 상호 작용 맵 오브젝트
	/* Prototype_Component_Model_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_BladeNexus"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_DamagedTS/WIP_COM_DamagedTS.dat")), E_FAIL);

	/* Prototype_Component_Model_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_BigChest"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_BigChest_Open_003/WIP_COM_BigChest_Open_003.dat")), E_FAIL);
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1_Shader()
{
	CoInitGuard co;

	//lock_guard<mutex> gpu_lock(g_GpuGate);

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1_GameObject()
{
	CoInitGuard co;

	//lock_guard<mutex> gpu_lock(g_GpuGate);

	/* Prototype_GameObject_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_SkyShpere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_SkySphere"),
		CSkySphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* Prototype_GameObject_Camera_Free */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Camera_Free"),
	//	CCamera_Free::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* Prototype_GameObject_Camera_Compre */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Camera_Compre"),
		CCamera_Compre::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* Prototype_GameObject_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Player"),
	//	CPlayer::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_Body_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Body_Player"),
	//	CBody_Player::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

#pragma region YETUGA

	/* Prototype_GameObject_Monster_Yetuga */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Yetuga"),
		CYetuga::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_PartObject_Yetuga_Body
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Yetuga_Body"),
		CBody_Yetuga::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Yetuga_Rock"),
		CProjectile_Yetuga::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	/////* Prototype_GameObject_Dummy */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Dummy"),
	//	CDummy::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_Prop_Test */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Test"),
	//	CProp_Test::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* Prototype_GameObject_Prop_Object */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_Static */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Static"),
		CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region 게임 오브젝트 원형 : 상호 작용 맵 오브젝트
	/* Prototype_GameObject_Prop_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_BladeNexus"),
		CBladeNexus::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_BigChest"),
		CBigChest::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

	/* Prototype_GameObject_JOH_Test1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_JOH_Test1"),
		CJOH_Test1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Khazan_Sample */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Khazan_Sample"),
		CKhazan_Sample::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Body_Khazan_Sample */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Body_Khazan_Sample"),
		CBody_Khazan_Sample::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Spear_Khazan_Sample */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Spear_Khazan_Sample"),
		CSpear_Khazan_Sample::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Prototype_MapObject_From_DAT(const _tchar* pPrototypeDataFileName, LEVEL eLevel, KHAZAN_MAP eMap)
{
	// Dat 파일 기본 경로
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
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

	pDataFilePath += pPrototypeDataFileName;

	pDataFilePath += TEXT("_prototype.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("[DAT ERROR] 바이너리 파일 오픈 문제"), E_FAIL);

	// 1. 프로토 타입의 총 개수
	_uint iPrototypeCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iPrototypeCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

	// 프로토 타입의 총 개수만큼 순회
	for (_uint i = 0; i < iPrototypeCnt; ++i)
	{
		// CModel 을 열어야 하는 경우 ( Instance X )
		// 2. 프로토 타입 태그 길이 저장
		_uint iPrototypeTagLen = {};
		CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 3. 프로토 타입 태그 이름 저장
		_tchar szPrototypeTag[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

		// 4. 모델 경로 길이 저장
		_uint iModelPathLen = {};
		CHECK_FALSE(ReadFile(hFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 5. 모델 경로 이름 저장
		_char szModelPath[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szModelPath, sizeof(_char) * iModelPathLen, &dwByte, nullptr), E_FAIL);

 		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(eLevel), szPrototypeTag,
			CModel::Create(m_pDevice, m_pContext, szModelPath))))
		{
			CloseHandle(hFile);
			MSG_BOX(TEXT("[DAT ERROR] 맵 오브젝트 프로토타입 등록 실패 ( CModel )"));
			return E_FAIL;
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLoader::Loading_Prototype_MapObject_Inst_From_DAT(const _tchar* pPrototypeDataFileName, LEVEL eLevel, KHAZAN_MAP eMap)
{
	// Dat 파일 기본 경로
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
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

	pDataFilePath += pPrototypeDataFileName;

	pDataFilePath += TEXT("_prototype_inst.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("[DAT ERROR] 바이너리 파일 오픈 문제"), E_FAIL);

	// 1. 프로토 타입의 총 개수
	_uint iPrototypeCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iPrototypeCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

	// 프로토 타입의 총 개수만큼 순회
	for (_uint i = 0; i < iPrototypeCnt; ++i)
	{
		// CModel 을 열어야 하는 경우 ( Instance X )

		// 2. 프로토 타입 태그 길이 저장
		_uint iPrototypeTagLen = {};
		CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 3. 프로토 타입 태그 이름 저장
		_tchar szPrototypeTag[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

		// 4. 모델 경로 길이 저장
		_uint iModelPathLen = {};
		CHECK_FALSE(ReadFile(hFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 5. 모델 경로 이름 저장
		_char szModelPath[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szModelPath, sizeof(_char) * iModelPathLen, &dwByte, nullptr), E_FAIL);

		// 6. 인스턴스 행렬 총 개수 저장
		_uint iNumInstances = {};
		CHECK_FALSE(ReadFile(hFile, &iNumInstances, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 인스턴스 정보 넘기기
		CModelMesh_Instance::MODELMESH_INSTANCE_DESC MeshInstanceDesc = {};

		MeshInstanceDesc.iNumInstance = iNumInstances;

		// 7. 행렬 개수만큼 벡터 resize 및 read file
		MeshInstanceDesc.InstanceData.resize(static_cast<size_t>(iNumInstances));

		for (_uint i = 0; i < iNumInstances; ++i)
		{
			CHECK_FALSE(ReadFile(hFile, &MeshInstanceDesc.InstanceData[i].vRight, sizeof(_float4), &dwByte, nullptr), E_FAIL);
			CHECK_FALSE(ReadFile(hFile, &MeshInstanceDesc.InstanceData[i].vUp, sizeof(_float4), &dwByte, nullptr), E_FAIL);
			CHECK_FALSE(ReadFile(hFile, &MeshInstanceDesc.InstanceData[i].vLook, sizeof(_float4), &dwByte, nullptr), E_FAIL);
			CHECK_FALSE(ReadFile(hFile, &MeshInstanceDesc.InstanceData[i].vTranslation, sizeof(_float4), &dwByte, nullptr), E_FAIL);
			CHECK_FALSE(ReadFile(hFile, &MeshInstanceDesc.InstanceData[i].iID, sizeof(_uint), &dwByte, nullptr), E_FAIL);
		}

		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(eLevel), szPrototypeTag,
			CModel_Instance::Create(m_pDevice, m_pContext, szModelPath, &MeshInstanceDesc))))
		{
			CloseHandle(hFile);
			MSG_BOX(TEXT("[DAT ERROR] 맵 오브젝트 프로토타입 등록 실패 ( CModel )"));
			return E_FAIL;
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
