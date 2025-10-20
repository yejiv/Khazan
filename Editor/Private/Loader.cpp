#include "Loader.h"

#include "GameInstance.h"
#include "Editor_Model.h"


#pragma region Anmaition
#include "JOH_EditorModelTest.h"
#include "Editor_TempCamera.h"
#pragma endregion


#pragma region Effect
#include "Camera_Effect.h"
#include "Effect_Terrain.h"
#pragma endregion

#pragma region MapEditor
#include "MapEditor_Header.h"
#pragma endregion

#pragma region UI
#include "Camera_UI.h"
#pragma endregion 

#pragma region Shader
#include "Camera_Shader.h"
#include "Terrain_Shader.h"
#include "Player_Shader.h"
#pragma endregion

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
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

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, 0);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::EDITOR:
		hr = Loading_For_Editor_Level();
		break;
	case LEVEL::MAP:
		hr = Loading_For_Map_Level();
		break;
	case LEVEL::ANIMATION:
		hr = Loading_For_Animation_Level();
		break;
	case LEVEL::EFFECT:
		hr = Loading_For_Effect_Level();
		break;
	case LEVEL::UI:
		hr = Loading_For_UI_Level();
		break;
	case LEVEL::SHADER:
		hr = Loading_For_Shader_Level();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

HRESULT CLoader::Loading_For_Editor_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Map_Level()
{
#pragma region 텍스쳐 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* Prototype_Component_Texture_Terrain_Grid */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Terrain_Grid"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Terrain/Tile0.dds"), 1)), E_FAIL);

	/* Prototype_Component_Texture_Map_Snow */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Map_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/Textures/WT_Base_D.dds"), 1)), E_FAIL);

#pragma endregion

#pragma region 모델 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	/* Prototype_Component_VIBuffer_Terrain */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 100, 100)), E_FAIL);

	/* Prototype_Component_Model_PlayerTest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_PlayerTest"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Test/Test/Test.dat")), E_FAIL);

#pragma endregion

#pragma region 쉐이더 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	// Prototype_Component_Shader_ModelMeshInstance
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Shader_ModelMeshInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_ModelMeshInstance.hlsl"),
			MESH_INSTANCING::Elements, MESH_INSTANCING::iNumElements)), E_FAIL);

	// Prototype_Component_Shader_VtxMesh
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"),
			VTXMESH::Elements, VTXMESH::iNumElements)), E_FAIL);
	
#pragma endregion

#pragma region 게임오브젝트 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	/* Prototype_GameObject_Prop_Object */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_Static */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Static"),
		CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Camera_Map */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Camera_Map"),
		CCamera_Map::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Map_Terrain */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Map_Terrain"),
		CMap_Terrain::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Map_TestPlayer */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Map_TestPlayer"),
		CMap_TestPlayer::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Animation_Level()
{

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));
	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_GameObject_JOH_TempCamera"),
		CEditor_TempCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Effect_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	// Prototype_Component_Texture_TestParticle
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Particle_Prototype"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/particle/particle%d.png"), 3))))
		return E_FAIL;
	
	// Prototype_Component_Texture_TestSpriteImage
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Sprite_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/Sprite/Sprite%d.png"), 2))))
		return E_FAIL;

	// Prototype_Component_Texture_MeshEffect(Diffuse)
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Slash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/SowardTrailTexture/Slash_%d.png"), 12))))
		return E_FAIL;

	// Prototype_Component_Texture_MeshEffect(Masking)
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_MeshEffect_Masking"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/Mask/Masking%d.png"), 7))))
		return E_FAIL;

	// Prototype_Component_Texture_Terrain_Texture
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Terrain_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/Tile0.jpg"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트를 로딩중입니다."));

	// Prototype_Component_Buffer_Terrain
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 100, 100)), E_FAIL);

	// Prototype_GameObject_Camera_Effect
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Camera_Effect"),
		CCamera_Effect::Create(m_pDevice, m_pContext)), E_FAIL);

	// Prototype_GameObject_Terrain_Effect
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Effect_Terrain"),
		CEffect_Terrain::Create(m_pDevice, m_pContext)), E_FAIL);

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_UI_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Atlas_Test"),
	//	CTexture_Atlas::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Test/Test.json"), 1))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Atlas_SlotTest"),
	//	CTexture_Atlas::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Test/SlotTest.json"), 1))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Tex_Background"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Test/T_Hud_BG_Deco_Pathfinder_01.png"), 1))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Hud_HPGauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Hud/State/T_Hud_Gauge_HP_0%d.png"), 7))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Hud_HPGauge_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Hud/State/T_Hud_Gauge_HP_BG_0%d.png"), 10))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Hud_Player_HPGauge_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Hud/State/T_Hud_Hp_Bg_0%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Hud_ComBatSpirit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Hud/State/T_Hud_Gauge_CombatSpirit_0%d.png"), 4))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));



	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_GameObject_Camera_UI"),
		CCamera_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Shader_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	// Prototype_Component_Texture_Terrain_Shader
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Texture_Terrain_Shader"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Terrain/Tile0.dds"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	// Prototype_Component_VIBuffer_Terrain
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 50, 50))))
		return E_FAIL;

	// Prototype_Component_Model_WP_WOD_Ground_Base_004
	//	_matrix PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_WP_WOD_Ground_Base_004"),
	//		CEditor_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, "../../Client/Bin/Resources/Map/Prop/Ground/WP_WOD_Ground_Base_004.fbx", PreTransformMatrix))))
	//		return E_FAIL;
	
	// Prototype_Component_Editor_Model_Test
	//	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Editor_Model_Test"),
	//		CEditor_Model::Create(m_pDevice, m_pContext, MODELTYPE::ANIM, "../../Client/Bin/Resources/Test_Models/Test_Player/Test.fbx", PreTransformMatrix))))
	//		return E_FAIL;

	/* Prototype_Component_Editor_Model_Test */
	_matrix PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Editor_Model_Test"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Test/Test_Player/Test_Player.dat"))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트를 로딩중입니다."));

	// Prototype_GameObject_Camera_Shader
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Camera_Shader"),
		CCamera_Shader::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_Terrain_Shader
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Terrain_Shader"),
		CTerrain_Shader::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_Player_Shader
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Player_Shader"),
		CPlayer_Shader::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_Prop_Test
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Prop_Test"),
	//		CProp_Test::Create(m_pDevice, m_pContext))))
	//		return E_FAIL;

	// Prototype_GameObject_Editor_Animation_TestModel
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Editor_Animation_TestModel"),
	//		CJOH_EditorModelTest::Create(m_pDevice, m_pContext))))
	//		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

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

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
