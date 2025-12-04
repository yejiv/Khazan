#include "Loader.h"

#include "GameInstance.h"
#include "Editor_Model.h"

#pragma region AI
#include "Edit_Monster.h"
#pragma endregion


#pragma region Anmaition
#include "JOH_EditorModelTest.h"
#include "Editor_TempCamera.h"
#pragma endregion


#pragma region Effect
#include "Camera_Effect.h"
#include "Effect_Terrain.h"
#include "LineTrail.h"
#include "MeshTrail.h"
#include "ScreenTrail.h"
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
#include "E_Khazan_Spear.h"
#include "E_Body_Khazan_Spear.h"
#pragma endregion

#pragma region Camera
#include "Camera_Compre.h"
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
	case LEVEL::CAMERA:
		hr = Loading_For_Camera_Level();
		break;
	case LEVEL::AI:
		hr = Loading_For_AI_Level();
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/Textures/Tile.dds"), 1)), E_FAIL);



#pragma region SKY SPHERE TEXTURES
	/* Prototype_Component_Texture_Cloud_Dist_Gradation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Cloud_Dist_Gradation"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/Prop/Sky/DistanceGradation.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Cloud_LookUp */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Cloud_LookUp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/Prop/Sky/FTW_Sky_Cloud_LookUp.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Cloud_Normal */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Cloud_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/Prop/Sky/FTW_Sky_Cloud_Nomal.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Cloud_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Cloud_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/Prop/Sky/FTW_Sky_Cloud_Distortion.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky_Nebula */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Sky_Nebula"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/Prop/Sky/FTW_Sky_Nebula.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky_Star_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Sky_Star_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/Prop/Sky/FTW_Sky_Star_Mask_001.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Moon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Moon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/Prop/Sky/FT_Rgb_Moon_001.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Ring */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Map/Prop/Sky/FT_Refraction_Ring.dds"), 1))))
		return E_FAIL;
#pragma endregion

#pragma endregion

#pragma region 모델 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	/* Prototype_Component_VIBuffer_Terrain */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 100, 100)), E_FAIL);

	/* Prototype_Component_Model_PlayerTest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_PlayerTest"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Sample/Khazan_Sample.dat")), E_FAIL);

#pragma region SKY SPHERE MODELS
	/* Prototype_Component_Model_SkyMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_SkyMesh"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/SkySphere/SkyMesh.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_CloudMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_CloudMesh"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/SkySphere/CloudMesh.dat"))))
		return E_FAIL;
#pragma endregion

#pragma region 상호 작용 모델 원형 ( 추가 추가 하면 추가 추가 )
	/* Prototype_Component_Model_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_BladeNexus"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_DamagedTS/WIP_COM_DamagedTS.dat")), E_FAIL);

	/* Prototype_Component_Model_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_BigChest"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_BigChest_Open_003/WIP_COM_BigChest_Open_003.dat")), E_FAIL);

	/* Prototype_Component_Model_TombStone */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_TombStone"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_BigTombStone_Destruct_001/WIP_BGQ_BigTombStone_Destruct_001.dat")), E_FAIL);

#pragma region 엘리베이터 ( 스몰 사이즈 )
    /* Prototype_Component_Model_SmallElevator */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_SmallElevator"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_TDl_Elevator_Move_001_a/WIP_TDl_Elevator_Move_001_a.dat")), E_FAIL);

    /* Prototype_Component_Model_Elevator_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Elevator_Gear"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_TDl_Elevator_Turn_001/WIP_TDl_Elevator_Turn_001.dat")), E_FAIL);

    /* Prototype_Component_Model_Slate_Switch */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Slate_Switch"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Slate_Switch_001/WIP_BGQ_Slate_Switch_001.dat")), E_FAIL);
#pragma endregion

#pragma region 레버, 기어
    /* Prototype_Component_Model_Lever */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Lever"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Gear_Switch_001/WIP_BGQ_Gear_Switch_001.dat")), E_FAIL);

    /* Prototype_Component_Model_Lever_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Lever_Gear"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Machine_Loop_001/WIP_BGQ_Machine_Loop_001.dat")), E_FAIL);
#pragma endregion

#pragma region 조각상 퍼즐
    /* Prototype_Component_Model_Statue */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Statue"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Base_StatueTable_Rotate_001/WIP_Base_StatueTable_Rotate_001.dat")), E_FAIL);

    /* Prototype_Component_Model_Statue_Plate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Statue_Plate"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_TDL_StatueTable_Static_001/WIP_TDL_StatueTable_Static_001.dat")), E_FAIL);

    /* Prototype_Component_Model_Statue_Deco */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Statue_Deco"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Symbol/WP_CTR_Statue_Stone_002/WP_CTR_Statue_Stone_002.dat")), E_FAIL);
#pragma endregion

#pragma region 수직 차단봉
    /* Prototype_Component_Model_VerticalGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_VerticalGate"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_TDL_Gate_Open_004/WIP_TDL_Gate_Open_004.dat")), E_FAIL);
#pragma endregion

#pragma region 잠긴 철문
    /* Prototype_Component_Model_IronGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_IronGate"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Base_MiddleDoor_Open_001_b/WIP_Base_MiddleDoor_Open_001_b.dat")), E_FAIL);

    /* Prototype_Component_Model_IronGate_Lock */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_IronGate_Lock"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Base_MiddleDoor_Lock_001/WIP_Base_MiddleDoor_Lock_001.dat")), E_FAIL);

    /* Prototype_Component_Model_IronGate_Part */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_IronGate_Part"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Building_B/WP_BTP_Door_Metal_004/WP_BTP_Door_Metal_004.dat")), E_FAIL);
#pragma endregion

#pragma region 사다리
    /* Prototype_Component_Model_Ladder_Top */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Ladder_Top"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Top_Ladder_Metal_01/WIP_Top_Ladder_Metal_01.dat")), E_FAIL);

    /* Prototype_Component_Model_Ladder_Middle */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Ladder_Middle"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Middle_Ladder_Metal_01/WIP_Middle_Ladder_Metal_01.dat")), E_FAIL);

    /* Prototype_Component_Model_Ladder_Bottom */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Ladder_Bottom"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Bottom_Ladder_Metal_01/WIP_Bottom_Ladder_Metal_01.dat")), E_FAIL);

    /* Prototype_Component_Model_Ladder_Support */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Ladder_Support"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Bottom_Ladder_Support_Static_Metal_01/WIP_Bottom_Ladder_Support_Static_Metal_01.dat")), E_FAIL);
#pragma endregion

#pragma region 기어 게이트
    /* Prototype_Component_Model_GearGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_GearGate"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Gate_Open_001_b/WIP_BGQ_Gate_Open_001_b.dat")), E_FAIL);

    /* Prototype_Component_Model_Door_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Door_Gear"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Gear_Loop_001/WIP_BGQ_Gear_Loop_001.dat")), E_FAIL);
#pragma endregion

#pragma region 엘리베이터 잠금 해제 기어
    /* Prototype_Component_Model_UnLockGear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_UnLockGear"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_ERM_Gear_Switch_001/WIP_ERM_Gear_Switch_001.dat")), E_FAIL);
#pragma endregion

#pragma region 엘리베이터 3단 합체 모델
    /* Prototype_Component_Model_LargeElevator */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_LargeElevator"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Elevator_Move_001_a/WIP_BGQ_Elevator_Move_001_a.dat")), E_FAIL);

    /* Prototype_Component_Model_Elevator_Inner */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Elevator_Inner"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Building_A/WP_BGQ_Elevator_Move_001_01/WP_BGQ_Elevator_Move_001_01.dat")), E_FAIL);

    /* Prototype_Component_Model_Elevator_Mid */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Elevator_Mid"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Building_A/WP_BGQ_Elevator_Move_001_02/WP_BGQ_Elevator_Move_001_02.dat")), E_FAIL);

    /* Prototype_Component_Model_Elevator_Outer */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Elevator_Outer"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Building_A/WP_BGQ_Elevator_Move_001_03/WP_BGQ_Elevator_Move_001_03.dat")), E_FAIL);
#pragma endregion

#pragma region 바이퍼 가는 문
    /* Prototype_Component_Model_GiantGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_GiantGate"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Base_GiantDoor_Open_001/WIP_Base_GiantDoor_Open_001.dat")), E_FAIL);

    /* Prototype_Component_Model_GiantGate_Part */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_GiantGate_Part"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Building_A/WIP_BGQ_Door_Wood_001/WIP_BGQ_Door_Wood_001.dat")), E_FAIL);
#pragma endregion

#pragma region NPC ( 야매 )
    /* Prototype_Component_Model_NPC_Daphrona */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_NPC_Daphrona"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/NPC/C_NPC_Seona/C_NPC_Seona.dat")), E_FAIL);

    /* Prototype_Component_Model_NPC_Duimuk */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_NPC_Duimuk"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/NPC/C_NPC_Duimuk/C_NPC_Duimuk.dat")), E_FAIL);

    /* Prototype_Component_Model_NPC_Duimuk_Part */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_NPC_Duimuk_Part"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/NPC/C_I_Duimuk_Abacus/C_I_Duimuk_Abacus.dat")), E_FAIL);

    /* Prototype_Component_Model_NPC_Danjin */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_NPC_Danjin"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/NPC/C_NPC_Danjin001/C_NPC_Danjin001.dat")), E_FAIL);
#pragma endregion

#pragma region 항아리 요정

    /* Prototype_Component_Model_NPC_DanjinJar_A */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_NPC_DanjinJar_A"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/NPC/C_M_DanjinJar/C_M_DanjinJar.dat")), E_FAIL);

    /* Prototype_Component_Model_NPC_DanjinJar_B */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_NPC_DanjinJar_B"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/NPC/C_M_DanjinJar_B/C_M_DanjinJar_B.dat")), E_FAIL);

    /* Prototype_Component_Model_NPC_DanjinJar_C */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_NPC_DanjinJar_C"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/NPC/C_M_DanjinJar_C/C_M_DanjinJar_C.dat")), E_FAIL);

#pragma endregion

#pragma endregion

#pragma region 트리거 모델 원형 ( 야매? )

    /* Prototype_Component_Model_Trigger */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Trigger"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Base/Cube/Cube.dat")), E_FAIL);

#pragma endregion

#pragma region 몬스터 모델 원형 ( 야매 )

    /* Prototype_Component_Model_Monster */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Monster"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Symbol/WP_TDL_Statue_Stone_002_b/WP_TDL_Statue_Stone_002_b.dat")), E_FAIL);

#pragma endregion

#pragma region 조명 모델 원형

    /* Prototype_Component_Model_Cube */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Cube"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Base/Cube/Cube.dat")), E_FAIL);

#pragma endregion

#pragma endregion

#pragma region 쉐이더 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	// Prototype_Component_Shader_ModelMeshInstance
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Shader_ModelMeshInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_ModelMeshInstance.hlsl"),
			MESH_INSTANCING::Elements, MESH_INSTANCING::iNumElements)), E_FAIL);
	
#pragma endregion

#pragma region 게임오브젝트 원형 로딩

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	/* Prototype_GameObject_SkyShpere */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_SkySphere"),
		CSkySphere::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_CloudShpere */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_CloudSphere"),
		CCloudSphere::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_Object */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_Static */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Static"),
		CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_Preview */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Preview"),
		CProp_Preview::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Camera_Map */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Camera_Map"),
		CCamera_Map::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Map_Terrain */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Map_Terrain"),
		CMap_Terrain::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Map_TestPlayer */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Map_TestPlayer"),
		CMap_TestPlayer::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region 게임오브젝트 원형 로딩 ( 상호 작용 객체 )
	/* Prototype_GameObject_Prop_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_BladeNexus"),
		CBladeNexus::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_BigChest"),
		CBigChest::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_TombStone */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_TombStone"),
        CTombStone::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region 엘리베이터 ( 스몰 사이즈 )
    /* Prototype_GameObject_Prop_SmallElevator */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_SmallElevator"),
        CElevatorS::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Elevator_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Elevator_Gear"),
        CElevator_Gear::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Slate_Switch */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Slate_Switch"),
        CSlate_Switch::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 레버, 기어
    /* Prototype_GameObject_Prop_Lever */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Lever"),
        CLever::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Lever_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Lever_Gear"),
        CLever_Gear::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 조각상 퍼즐
    /* Prototype_GameObject_Prop_Statue */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Statue"),
        CStatue::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Statue_Plate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Statue_Plate"),
        CStatue_Plate::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Statue_Deco */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Statue_Deco"),
        CStatue_Deco::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 수직 차단봉
    /* Prototype_GameObject_Prop_VerticalGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_VerticalGate"),
        CVerticalGate::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 잠긴 철문
    /* Prototype_GameObject_Prop_IronGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_IronGate"),
        CIronGate::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_IronGate_Lock */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_IronGate_Lock"),
        CIronGate_Lock::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_IronGate_Part_L */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_IronGate_Part_L"),
        CIronGate_Part_L::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_IronGate_Part_R */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_IronGate_Part_R"),
        CIronGate_Part_R::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 사다리
    /* Prototype_GameObject_Prop_Ladder */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Ladder"),
        CLadder::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Ladder_Top */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Ladder_Top"),
        CLadder_Top::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Ladder_Middle */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Ladder_Middle"),
        CLadder_Middle::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Ladder_Bottom */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Ladder_Bottom"),
        CLadder_Bottom::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Ladder_Support */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Ladder_Support"),
        CLadder_Support::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 기어 게이트
    /* Prototype_GameObject_Prop_GearGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_GearGate"),
        CGearGate::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Door_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Door_Gear"),
        CDoor_Gear::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 엘리베이터 잠금 해제 기어
    /* Prototype_GameObject_Prop_UnLockGear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_UnLockGear"),
        CUnLockGear::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 엘리베이터 3단 합체 모델
    /* Prototype_GameObject_Prop_LargeElevator */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_LargeElevator"),
        CElevatorL::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Elevator_Inner */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Elevator_Inner"),
        CElevator_Inner::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Elevator_Mid */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Elevator_Mid"),
        CElevator_Mid::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Elevator_Outer */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Elevator_Outer"),
        CElevator_Outer::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 바이퍼 가는 문
    /* Prototype_GameObject_Prop_GiantGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_GiantGate"),
        CGiantGate::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_GiantGate_Part_L */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_GiantGate_Part_L"),
        CGiantGate_Part_L::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_GiantGate_Part_R */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_GiantGate_Part_R"),
        CGiantGate_Part_R::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region NPC ( 야매 )
    /* Prototype_GameObject_Prop_NPC_Daphrona */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_NPC_Daphrona"),
        CNPC_Daphrona::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_NPC_Duimuk */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_NPC_Duimuk"),
        CNPC_Duimuk::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_NPC_Duimuk_Part */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_NPC_Duimuk_Part"),
        CDuimuk_Part::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_NPC_Danjin */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_NPC_Danjin"),
        CNPC_Danjin::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 항아리 요정
    /* Prototype_GameObject_Prop_DanjinJar */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_DanjinJar"),
        CDanjinJar::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma endregion

#pragma region 게임오브젝트 원형 로딩 ( 트리거 및 몬스터 )
    /* Prototype_GameObject_Prop_Trigger */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Trigger"),
        CTrigger::Create(m_pDevice, m_pContext)), E_FAIL);
    /* Prototype_GameObject_Prop_Spawn */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Spawn"),
        CMap_Spawn::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 게임오브젝트 원형 로딩 ( 데칼 )
    /* Prototype_GameObject_Decal */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Decal"),
        CDecal::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 게임오브젝트 원형 로딩 ( 점 조명 )
    /* Prototype_GameObject_Prop_Light */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_GameObject_Prop_Light"),
        CMap_Light::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/particle/particle%d.png"), 12))))
		return E_FAIL;
	
	// Prototype_Component_Texture_TestSpriteImage
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Sprite_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/Sprite/Sprite%d.png"), 12))))
		return E_FAIL;

	// Prototype_Component_Texture_MeshEffect(Diffuse)
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Slash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/SowardTrailTexture/Slash_%d.png"), 39))))
		return E_FAIL;

	// Prototype_Component_Texture_MeshEffect(Masking)
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_MeshEffect_Masking"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/Mask/Masking%d.png"), 15))))
		return E_FAIL;

	// Prototype_Component_Texture_MeshEffect(Dissolve)
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_MeshEffect_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/Dissolve/Dissolve%d.png"), 6))))
		return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_MeshEffect_Normal"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Effect/Normal/Normal%d.png"), 3))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_VIBuffer_QuadTrail"),
		CVIBuffer_QuadTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_VIBuffer_LineTrail"),
		CVIBuffer_LineTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_Camera_Effect
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Camera_Effect"),
		CCamera_Effect::Create(m_pDevice, m_pContext)), E_FAIL);

	// Prototype_GameObject_Terrain_Effect
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Effect_Terrain"),
		CEffect_Terrain::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_LineTrail"),
		CLineTrail::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_MeshTrail"),
		CMeshTrail::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_ScreenTrail"),
		CScreenTrail::Create(m_pDevice, m_pContext)), E_FAIL);


#pragma region 테스트용 ( 프로토타입 모델 생성 ) ( 박준영이 남기고간거 필요없으면 삭제 )
    CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("Test"), LEVEL::EFFECT), E_FAIL);
    CHECK_FAILED(Loading_Prototype_MapObject_Inst_From_DAT(TEXT("Test"), LEVEL::EFFECT), E_FAIL);

    // Prototype_GameObject_Prop_Object
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Prop_Object"),
        CProp_Object::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    // Prototype_GameObject_Prop_Static
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Prop_Static"),
        CProp_Static::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_UI_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Atlas"),
		CTexture_Atlas::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Atlas/Atlas_%d.json"), 5))))
		return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Texture_UI_BackGround"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/BG/T_Texture_Bg_UI.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_FX_Mask"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Mask/T_Fx_%d.png"), 14))))
        return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Hud_HPGauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Hud/State/T_Hud_Gauge_HP_0%d.png"), 7))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Hud_HPGauge_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Hud/State/T_Hud_Gauge_HP_BG_0%d.png"), 10))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Hud_Player_HPGauge_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Hud/State/T_Hud_Hp_Bg_0%d.png"), 9))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_Hud_ComBatSpirit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Hud/State/T_Hud_Gauge_CombatSpirit_0%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_Common_MenuList"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Common/T_Img_List_Menu_%d.png"), 8))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_State_Arrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/State/ArrowButton_UI_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_State_Button"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/State/NormalButton_%d.png"), 10))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_ItemInfo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Hud/ItemInfo/T_BG_ItemInfo_B_%d.png"), 7))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_ItemInfo_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/ItemInfo/T_BG_ItemInfo_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_ItemInfo_Top"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/ItemInfo/T_BG_ItemInfo_Top_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_ItemInfo_Bottom"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/ItemInfo/T_BG_ItemInfo_Bottom_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_Log_Tex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Logo/Logo_Tex_%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_GuidePage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/TutorialGuide/GuidePage_%d.dds"), 6))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_GuideTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/TutorialGuide/GuideTex_%d.dds"), 3))))
		return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_TS_Tex"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Map/Map_Tex_%d.png"), 9))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_TS_BG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Map/Map_Bg_%d.png"), 2))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_Flag"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Flag/T_SkillTreeBg_0%d.png"), 2))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Component_UI_Collect_Line"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/UI/Common/T_Img_MemoryCollect_Line.png"), 1))))
        return E_FAIL;

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

    /* Prototype_Component_Model_BladeNexus */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_BladeNexus"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_DamagedTS/WIP_COM_DamagedTS.dat")), E_FAIL);


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
	//	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Editor_Model_Test"),
	//		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Test/Test_Player/Test_Player.dat"))))
	//		return E_FAIL;

	/* Prototype_Component_Editor_Model_Test */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Editor_Model_Test"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Sample/Khazan_Sample.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Rock */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Rock"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Rock/WP_COM_Rock_Base_001/WP_COM_Rock_Base_001.dat"))))
		return E_FAIL;

#pragma region 테스트용 ( 프로토타입 모델 생성 ) ( 박준영이 남기고간거 필요없으면 삭제 )
	CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("Test"), LEVEL::SHADER), E_FAIL);
	CHECK_FAILED(Loading_Prototype_MapObject_Inst_From_DAT(TEXT("Test"), LEVEL::SHADER), E_FAIL);

    /* Prototype_GameObject_Prop_BladeNexus */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Prop_BladeNexus"),
        CBladeNexus::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

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

	// Prototype_GameObject_Prop_Object
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_Decal
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Decal"),
		CDecal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_Editor_Animation_TestModel
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Editor_Animation_TestModel"),
		CJOH_EditorModelTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Khazan
    /* Prototype_GameObject_Khazan_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Khazan_Spear"),
        CE_Khazan_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_Body_Khazan_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Body_Khazan_Spear"),
        CE_Body_Khazan_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    /* Prototype_Component_Model_Khazan_Lantern*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Lantern"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/APC/Lantern/Lantern.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Spear_Khazan_Sample*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Spear_Khazan_Sample"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Sample/Spear/Spear.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Spear*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Spear"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Spear/Khazan_Spear.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Torso1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso1"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso1/Prisoner_Torso1.dat"))))
        return E_FAIL;
    /* Prototype_Component_Model_Khazan_Prisoner_Torso2*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso2"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso2/Prisoner_Torso2.dat"))))
        return E_FAIL;
    /* Prototype_Component_Model_Khazan_Prisoner_Torso3*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso3"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso3/Prisoner_Torso3.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Shoes1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes1"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Shoes/Prisoner_Shoes1/Prisoner_Shoes1.dat"))))
        return E_FAIL;
    ///* Prototype_Component_Model_Khazan_Prisoner_Shoes2*/
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes2"),
    //	CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Shoes/Prisoner_Shoes2/Prisoner_Shoes2.dat"))))
    //	return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Leg1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg1"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg1/Prisoner_Leg1.dat"))))
        return E_FAIL;
    /* Prototype_Component_Model_Khazan_Prisoner_Leg2*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg2"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg2/Prisoner_Leg2.dat"))))
        return E_FAIL;
    /* Prototype_Component_Model_Khazan_Prisoner_Leg3*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg3"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg3/Prisoner_Leg3.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Hair1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Hair1"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Hair/Prisoner_Hair1/Prisoner_Hair1.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Face1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Face1"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Face/Prisoner_Face1/Prisoner_Face1.dat"))))
        return E_FAIL;


    /* Prototype_Component_Model_Khazan_Prisoner_Arm1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm1"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Arm/Prisoner_Arm1/Prisoner_Arm1.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Arm2*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm2"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Arm/Prisoner_Arm2/Prisoner_Arm2.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_DanJin_Hair*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Model_Khazan_DanJin_Hair"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Khazan/Khazan_Parts/Hair/Danjin_Hair/Danjin_Hair/Danjin_Hair.dat"))))
        return E_FAIL;
#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Camera_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_AI_Level()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트를 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::AI), TEXT("Prototype_GameObject_Monster"),
		CEdit_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
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
	case KHAZAN_MAP::YETUGA:
		pDataFilePath += TEXT("Yetuga/");
		break;
	case KHAZAN_MAP::CREVICE:
		pDataFilePath += TEXT("Crevice/");
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
	case KHAZAN_MAP::YETUGA:
		pDataFilePath += TEXT("Yetuga/");
		break;
	case KHAZAN_MAP::CREVICE:
		pDataFilePath += TEXT("Crevice/");
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

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
