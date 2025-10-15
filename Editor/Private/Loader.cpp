#include "Loader.h"

#include "GameInstance.h"
#include "Editor_Model.h"


#pragma region Anmaition
#include "JOH_EditorModelTest.h"
#include "Editor_TempCamera.h"
#pragma endregion


#pragma region Effect
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "Camera_Effect.h"
#include "Terrain_Grid.h"
#include "TestParticle.h"
#pragma endregion

#pragma region MapEditor
#include "MapEditor_Header.h"
#pragma endregion

#pragma region UI
#include "Camera_UI.h"
#include "Edit_Button.h"
#include "Edit_Panel.h"
#include "Edit_ProgressBar.h"
#include "Edit_TextBox.h"
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

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Terrain_Grid"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Tile0.jpg"), 1)), E_FAIL);

#pragma endregion

#pragma region 모델 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	// Prototype_Component_VIBuffer_Terrain
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 100, 100)), E_FAIL);

#pragma endregion

#pragma region 쉐이더 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Shader_ModelMeshInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_ModelMeshInstance.hlsl"),
			MESH_INSTANCING::Elements, MESH_INSTANCING::iNumElements)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"),
			VTXMESH::Elements, VTXMESH::iNumElements)), E_FAIL);
	
#pragma endregion

#pragma region 게임오브젝트 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Static"),
		CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Camera_Map"),
		CCamera_Map::Create(m_pDevice, m_pContext)), E_FAIL);

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
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model_JOH_TestModel */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_Component_Model_JOH_TestModel"),
		CEditor_Model::Create(m_pDevice, m_pContext, MODELTYPE::ANIM, "../../Client/Bin/Models/Creature/Test/AnimNude_Player/Test.fbx", PreTransformMatrix))))
		return E_FAIL;

		/* Prototype_Component_Model_JOH_TestModel */
	//PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_Component_Model_JOH_TestModel"),
	//	CEditor_Model::Create(m_pDevice, m_pContext, MODELTYPE::ANIM, "../../Client/Bin/Resources/Models/Fiona/Fiona.fbx", PreTransformMatrix))))
		//return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));
	
	/* Prototype_GameObject_JOH_TestModel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::ANIMATION), TEXT("Prototype_GameObject_JOH_TestModel"),
		CJOH_EditorModelTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_JOH_TempCamera*/
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
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_TestParticle"),
	//		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Effect/TestParticle.png"), 1))))
	//		return E_FAIL;
	
	// Prototype_Component_Texture_TestParticle
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_TestParticle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Models/Effect/LowPolyFoodLiteTexture_01.png"), 1))))
		return E_FAIL;

	// Prototype_Component_Texture_Terrain_Grid
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Terrain_Grid"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Tile0.jpg"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	// Prototype_Component_VIBuffer_Terrain
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 50, 50))))
		return E_FAIL;

	// Prototype_Component_Particle_Spread
	//	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC SpreadDesc{};
	//	SpreadDesc.iNumInstance = 10000;
	//	SpreadDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//	SpreadDesc.vRange = _float3(0.f, 0.f, 0.f);
	//	SpreadDesc.vSize = _float2(0.5f, 1.f);
	//	SpreadDesc.vLifeTime = _float2(0.5f, 2.f);
	//	SpreadDesc.vPivot = _float3(0.f, 0.f, 0.f);
	//	SpreadDesc.vSpeed = _float2(0.5f, 1.f);
	//	SpreadDesc.isLoop = true;
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Particle_Spread"),
	//		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &SpreadDesc))))
	//		return E_FAIL;

	CVIBuffer_Mesh_Instance::MESH_INSTANCE_DESC MeshDesc{};
	MeshDesc.iNumInstance = 100;
	MeshDesc.vCenter = _float3(0.f, 0.f, 0.f);
	MeshDesc.vRange = _float3(0.f, 0.f, 0.f);
	MeshDesc.vSize = _float2(1.f, 2.f);
	MeshDesc.vDirection = _float3(0.f, 1.f, 0.f);
	MeshDesc.vVectorScale = _float3(1.f, 1.f, 1.f);
	MeshDesc.vSpeed = _float2(1.f, 2.f);
	MeshDesc.vLifeTime = _float2(1.f, 2.f);
	MeshDesc.isLoop = true;
	MeshDesc.isRandomVector = true;
	MeshDesc.vMinAngle = _float3(0.f, 0.f, 0.f);
	MeshDesc.vMaxAngle = _float3(360.f, 360.f, 360.f);

	_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Particle_Spread"),
		CVIBuffer_Mesh_Instance::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/Models/Effect/TestMesh.fbx", &MeshDesc, PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트를 로딩중입니다."));

	// Prototype_GameObject_ParticleSystem
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_ParticleSystem"),
		CParticleSystem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_ParticleEmitter
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_ParticleEmitter"),
		CParticleEmitter::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	// Prototype_GameObject_Camera_Effect
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Camera_Effect"),
		CCamera_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_Terrain_Grid
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Terrain_Grid"),
		CTerrain_Grid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_TestParticle
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_TestParticle"),
		CTestParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_UI_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_GameObject_Camera_UI"),
		CCamera_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_UIObject_Edit_Panel"),
		CEdit_Panel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_UIObject_Edit_Button"),
		CEdit_Button::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_UIObject_Edit_ProgressBar"),
		CEdit_ProgressBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_UIObject_Edit_TextBox"),
		CEdit_TextBox::Create(m_pDevice, m_pContext))))
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
