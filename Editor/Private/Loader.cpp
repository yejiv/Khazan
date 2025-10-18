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

	// Prototype_Component_Texture_Terrain_Grid
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Terrain_Grid"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Terrain/Tile0.dds"), 1)), E_FAIL);

#pragma endregion

#pragma region 모델 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	// Prototype_Component_VIBuffer_Terrain
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 100, 100)), E_FAIL);

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

	// Prototype_GameObject_Prop_Object
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	// Prototype_GameObject_Prop_Static
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Static"),
		CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);

	// Prototype_GameObject_Prop_Export
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Export"),
		CProp_Export::Create(m_pDevice, m_pContext)), E_FAIL);

	// Prototype_GameObject_Camera_Map
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Camera_Map"),
		CCamera_Map::Create(m_pDevice, m_pContext)), E_FAIL);

	// Prototype_GameObject_Map_Terrain
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Map_Terrain"),
		CMap_Terrain::Create(m_pDevice, m_pContext)), E_FAIL);

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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/particle/particle%d.png"), 2))))
		return E_FAIL;
	
	// Prototype_Component_Texture_TestSpriteImage
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Sprite_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/Sprite/Sprite%d.png"), 2))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Atlas_Test"),
		CTexture_Atlas::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Test/Test.json"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Atlas_SlotTest"),
		CTexture_Atlas::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Test/SlotTest.json"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Tex_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Test/T_Hud_BG_Deco_Pathfinder_01.png"), 1))))
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
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Texture_Terrain_Shader"),
	//		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Terrain/Tile0.jpg"), 1))))
	//		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	// Prototype_Component_VIBuffer_Terrain
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_VIBuffer_Terrain"),
	//		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 50, 50))))
	//		return E_FAIL;

	// Prototype_Component_Model_WP_WOD_Ground_Base_004
	_matrix PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_WP_WOD_Ground_Base_004"),
		CEditor_Model::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, "../../Client/Bin/Resources/Map/Prop/Ground/WP_WOD_Ground_Base_004.fbx", PreTransformMatrix))))
		return E_FAIL;
	
	// Prototype_Component_Editor_Model_Test
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Editor_Model_Test"),
		CEditor_Model::Create(m_pDevice, m_pContext, MODELTYPE::ANIM, "../../Client/Bin/Resources/Test_Models/Test_Player/Test.fbx", PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트를 로딩중입니다."));

	// Prototype_GameObject_Camera_Shader
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Camera_Shader"),
		CCamera_Shader::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_Terrain_Shader
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Terrain_Shader"),
	//		CTerrain_Shader::Create(m_pDevice, m_pContext))))
	//		return E_FAIL;

	// Prototype_GameObject_Prop_Test
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Prop_Test"),
		CProp_Test::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_Editor_Animation_TestModel
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Editor_Animation_TestModel"),
		CJOH_EditorModelTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

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
