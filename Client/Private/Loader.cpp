#include "Loader.h"

#include "GameInstance.h"


#include "Player.h"
#include "Body_Player.h"
#include "Camera_Free.h"
#include "Sky.h"
#include "Terrain.h"
#include "Monster.h"
#include "Dummy.h"

#include "JOH_Test1.h"

#pragma region MAP OBJECT
#include "MapObject_Header.h"
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
	//if (0 == m_hThread)
	//	return E_FAIL;

	return Loading();
}

void CLoader::Update()
{
	if (m_isFinished.load()) return;

	FlushCommits();

	bool all_done = true;
	HRESULT first_fail = S_OK;

	for (auto& f : m_futures)
	{
		if (!f.valid()) continue;
		using namespace chrono_literals;
		auto st = f.wait_for(0ms);
		if (st == future_status::ready) {
			HRESULT hr = any_cast<HRESULT>(f.get());
			if (FAILED(hr) && SUCCEEDED(first_fail)) first_fail = hr;
		}
		else {
			all_done = false;
		}
	}

	bool commit_empty = false;
	{
		lock_guard<mutex> lg(m_CommitMutex);
		commit_empty = m_Commits.empty();
	}

	if (all_done && commit_empty)
	{
		if (SUCCEEDED(first_fail))
			lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
		else
			lstrcpy(m_szLoadingText, TEXT("로딩 실패"));

		m_isFinished = true;
	}
}

HRESULT CLoader::Loading()
{
	//EnterCriticalSection(&m_CriticalSection);

	//CoInitializeEx(nullptr, 0);

	m_isFinished = false;
	m_futures.clear();

	HRESULT			hr = {};

	switch(m_eNextLevelID)
	{
	case LEVEL::TITLE:
		m_futures.emplace_back(
			m_pGameInstance->EnqueueAny([this]() -> any {
				CoInitGuard co;
				return any(Loading_For_Title_Level());
				})
		);
		break;
	case LEVEL::STAGE1:
		hr = Loading_For_Stage1_Level();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	//LeaveCriticalSection(&m_CriticalSection);

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
	
	m_futures.emplace_back(m_pGameInstance->EnqueueAny([this]() -> any { return any(Loading_For_Stage1_Texture()); }));

	m_futures.emplace_back(m_pGameInstance->EnqueueAny([this]() -> any { return any(Loading_For_Stage1_Model()); }));

	m_futures.emplace_back(m_pGameInstance->EnqueueAny([this]() -> any { return any(Loading_For_Stage1_Shader()); }));

	m_futures.emplace_back(m_pGameInstance->EnqueueAny([this]() -> any { return any(Loading_For_Stage1_GameObject()); }));

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1_Texture()
{
	CoInitGuard co;

	//lock_guard<mutex> gpu_lock(g_GpuGate);

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sky/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Mask_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Texture_Mask_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain/TerrainMask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain/Brush.png"), 1))))
		return E_FAIL;


	//vector<const _tchar*> TextureList;
	//TextureList.reserve(2);
	//TextureList.push_back(TEXT("T_BG_GrandFlores.png"));
	//TextureList.push_back(TEXT("T_BG_ValleyOfTheFallenSouls.png"));

	///* Prototype_Component_Texture_Test */
 //  	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Texture_Test"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Textures/UI/BackGround/"), TextureList))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Stage1_Model()
{
	CoInitGuard co;

	//lock_guard<mutex> gpu_lock(g_GpuGate);

	///* Prototype_Component_Model_Fiona */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Test/Fiona/Fiona.dat"))))
		return E_FAIL;

	/////* Prototype_Component_Model_WP_WOD_Ground_Base_004 */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Model_WP_WOD_Ground_Base_004"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Environment/Prop/Ground/WP_WOD_Ground_Base_004.dat"))))
	//	return E_FAIL;

	///* Prototype_Component_Model_JOH_TestModel */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Model_JOH_TestModel"),
	//	CModel::Create(m_pDevice, m_pContext, "../Data/Test/Test_Player/Test_Player.dat"))))
	//	return E_FAIL;

	///* Prototype_Component_Model_파일명 */
	//CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("Test"), LEVEL::STAGE1), E_FAIL);

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
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* Prototype_GameObject_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Player"),
	//	CPlayer::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_Body_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Body_Player"),
	//	CBody_Player::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* Prototype_GameObject_Dummy */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Dummy"),
	//	CDummy::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/////* Prototype_GameObject_Prop_Test */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Prop_Test"),
	//	CProp_Test::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* Prototype_GameObject_Prop_Object */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Prop_Object"),
	//	CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_JOH_Test1 */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_JOH_Test1"),
		CJOH_Test1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Prototype_MapObject_From_DAT(const _tchar* pPrototypeDataFileName, LEVEL eLevel, KHAZAN_MAP eMap)
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

	pDataFilePath += pPrototypeDataFileName;

	pDataFilePath += TEXT("_prototypes.dat");

	DWORD dwByte = {};

	HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("[DAT ERROR] 바이너리 파일 오픈 문제"), E_FAIL);

	// 1. 프로토 타입의 총 개수
	_uint iPrototypeCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iPrototypeCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

	// 프로토 타입의 총 개수만큼 순회
	for (_uint i = 0; i < iPrototypeCnt; ++i)
	{
		// 2. MapObject 타입 가져오기 ( _ushort형으로 저장해서 형변환 후 사용 )
		_ushort sMapObjectType = {};
		CHECK_FALSE(ReadFile(hFile, &sMapObjectType, sizeof(_ushort), &dwByte, nullptr), E_FAIL);

		MAPOBJECT_TYPE eMapObjType = static_cast<MAPOBJECT_TYPE>(sMapObjectType);

		// MapObject 타입에 따른 조건문
		if (MAPOBJECT_TYPE::OBJECT == eMapObjType ||
			MAPOBJECT_TYPE::INTERACTIVE == eMapObjType ||
			MAPOBJECT_TYPE::DYNAMIC == eMapObjType)
		{
			// CModel 을 열어야 하는 경우 ( Instance X )

			// 3. 프로토 타입 태그 길이 저장
			_uint iPrototypeTagLen = {};
			CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

			// 4. 프로토 타입 태그 이름 저장
			_tchar szPrototypeTag[MAX_PATH] = {};
			CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

			// 5. 모델 경로 길이 저장
			_uint iModelPathLen = {};
			CHECK_FALSE(ReadFile(hFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

			// 6. 모델 경로 이름 저장
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
		else if (MAPOBJECT_TYPE::STATIC_INST == eMapObjType || MAPOBJECT_TYPE::ANIMATED_INST == eMapObjType)
		{
			// CModel_Instance 를 열어야 하는 경우 ( Instance O )
			// 
			// 3. 프로토 타입 태그 길이 저장
			_uint iPrototypeTagLen = {};
			CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

			// 4. 프로토 타입 태그 이름 저장
			_tchar szPrototypeTag[MAX_PATH] = {};
			CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

			// 5. 모델 경로 길이 저장
			_uint iModelPathLen = {};
			CHECK_FALSE(ReadFile(hFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

			// 6. 모델 경로 이름 저장
			_char szModelPath[MAX_PATH] = {};
			CHECK_FALSE(ReadFile(hFile, &szModelPath, sizeof(_char) * iModelPathLen, &dwByte, nullptr), E_FAIL);

			// 추후에 인스턴스 추가해야하는 코드 부분 ( vector<MESH_INSTANCE_DATA> )
			CModelMesh_Instance::MODELMESH_INSTANCE_DESC InstanceDesc = {};

			// 7. 인스턴스 개수
			_uint iNumInstance = {};
			CHECK_FALSE(ReadFile(hFile, &iNumInstance, sizeof(_uint), &dwByte, nullptr), E_FAIL);

			for (_uint j = 0; j < InstanceDesc.iNumInstance; ++j)
			{
				// 8. 인스턴스 개수 만큼 순회하면서 벡터에 Push_Back ( MapEditor에서 사용한 InstanceID는 빼고 파일 입출력해도 괜찮을 거 같음 )
				_matrix InstanceMatrix = {};
				CHECK_FALSE(ReadFile(hFile, &InstanceMatrix, sizeof(_matrix), &dwByte, nullptr), E_FAIL);

				MESH_INSTANCE_DATA InstanceData = {};
				InstanceData.InstanceMatrix = InstanceMatrix;
				InstanceData.InstanceID = j;

				InstanceDesc.InstanceData.push_back(InstanceData);
			}

			if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(eLevel), szPrototypeTag,
				CModel_Instance::Create(m_pDevice, m_pContext, szModelPath, &InstanceDesc))))
			{
				CloseHandle(hFile);
				MSG_BOX(TEXT("[DAT ERROR] 맵 오브젝트 프로토타입 등록 실패 ( CModel_Instance )"));
				return E_FAIL;
			}
		}
		else
		{
			CloseHandle(hFile);
			MSG_BOX(TEXT("[DAT ERROR] DAT 파일 읽는중 TYPE 문제 ( 박준영 문제 )"));
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
