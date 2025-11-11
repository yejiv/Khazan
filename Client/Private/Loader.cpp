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
#include "Khazan_Spear.h"
#include "Body_Khazan_Spear.h"
#include "Spear_Khazan_Spear.h"
#pragma endregion

#pragma region MAP OBJECT
#include "MapObject_Header.h"
#pragma endregion

#pragma region Monster
#include "Yetuga.h"
#include "Body_Yetuga.h"
#include "Head_Yetuga.h"
#include "Projectile_Yetuga.h"
#include "Projectile_Rock_Yetuga.h"

#include "Gomdol.h"
#include "Body_Gomdol.h"

#include "Imp_Range.h"
#include "Body_Imp_Range.h"
#include "Imp_Wand.h"
#include "Projectile_Imp_MagicBall.h"
#include "Projectile_Boomarang.h"

#pragma endregion

#pragma region UI
#include "Logo_BG.h"
#include "UI_Logo.h"
#pragma endregion
#include "Effect_Prefab.h"
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
			return false; 
	}
	return true;
}

void CLoader::Update()
{
	if (!AllReady(m_futures)) {
		
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
		lstrcpy(m_szLoadingText, TEXT("성공"));
	else
		lstrcpy(m_szLoadingText, TEXT("실패"));
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
	case LEVEL::TEST:
		m_futures.push_back(m_pGameInstance->Add_Task([this]() {
			return Loading_For_Test_Level();
			}));
		break;
	case LEVEL::HEINMACH:
		m_futures.push_back(m_pGameInstance->Add_Task([this]() {
			return Loading_For_HeinMach_Level();
			}));
		break;
	case LEVEL::CREVICE:
		break;
	case LEVEL::EMBARS:
		break;
	case LEVEL::VIPER:
		m_futures.push_back(m_pGameInstance->Add_Task([this]() {
			return Loading_For_Viper_Level();
			}));
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}


HRESULT CLoader::Loading_For_Title_Level()
{
	lstrcpy(m_szLoadingText, TEXT("�ؽ��ĸ� �ε����Դϴ�."));

	lstrcpy(m_szLoadingText, TEXT("���� �ε����Դϴ�."));

	lstrcpy(m_szLoadingText, TEXT("���̴��� �ε����Դϴ�."));

	lstrcpy(m_szLoadingText, TEXT("���ӿ�����Ʈ�� �ε����Դϴ�."));

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_Logo_BG"),
		CLogo_BG::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::TITLE))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_UI_Logo"),
		CUI_Logo::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::TITLE))), E_FAIL);

	lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));
	
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Test_Level()
{
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Test_Texture();
		}));

	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		Loading_For_Test_Model();
		Loading_For_Test_GameObject();
		return E_FAIL;
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Test_Shader();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("HeinMach"), LEVEL::TEST), E_FAIL);
		}));

	

	return S_OK;
}

HRESULT CLoader::Loading_For_Test_Texture()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Test_Model()
{

	/* Prototype_Component_Model_Khazan_Sample*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_Component_Model_Khazan_Sample"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Khazan_Sample.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Spear_Khazan_Sample*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_Component_Model_Spear_Khazan_Sample"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Spear/Spear.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Spear*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_Component_Model_Khazan_Spear"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Spear/Khazan_Spear.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Test_Shader()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Test_GameObject()
{
	/* Prototype_GameObject_Prop_Object */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_Static */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Prop_Static"),
		CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Camera_Compre */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Camera_Compre"),
		CCamera_Compre::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Khazan_Sample */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Khazan_Sample"),
		CKhazan_Sample::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Body_Khazan_Sample */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Body_Khazan_Sample"),
		CBody_Khazan_Sample::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Spear_Khazan_Sample */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Spear_Khazan_Sample"),
		CSpear_Khazan_Sample::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("SpaceTime_SpearBlood"),
		CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/bloodInv"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TEST), TEXT("SpearWind"),
		CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Spear"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_HeinMach_Level()
{

	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_HeinMach_Texture();
		}));
	
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_HeinMach_Model();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_HeinMach_Shader();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_HeinMach_GameObject();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Loading_Prototype_MapObject_Inst_From_DAT(TEXT("HeinMach"), LEVEL::HEINMACH, KHAZAN_MAP::HEINMACH), E_FAIL);
		}));

	//lstrcpy(m_szLoadingText, TEXT("�ε��� �Ϸ�Ǿ����ϴ�."));

	//m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_HeinMach_Texture()
{

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


	return S_OK;
}

HRESULT CLoader::Loading_For_HeinMach_Model()
{

#pragma region KHAZAN

	/* Prototype_Component_Model_Spear_Khazan_Sample*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Spear_Khazan_Sample"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Spear/Spear.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Spear*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Spear"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Spear/Khazan_Spear.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Torso1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso1/Prisoner_Torso1.dat"))))
		return E_FAIL;
	/* Prototype_Component_Model_Khazan_Prisoner_Torso2*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso2/Prisoner_Torso2.dat"))))
		return E_FAIL;
	/* Prototype_Component_Model_Khazan_Prisoner_Torso3*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso3/Prisoner_Torso3.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Shoes1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Shoes/Prisoner_Shoes1/Prisoner_Shoes1.dat"))))
		return E_FAIL;
	///* Prototype_Component_Model_Khazan_Prisoner_Shoes2*/
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes2"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Shoes/Prisoner_Shoes2/Prisoner_Shoes2.dat"))))
	//	return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Leg1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg1/Prisoner_Leg1.dat"))))
		return E_FAIL;
	/* Prototype_Component_Model_Khazan_Prisoner_Leg2*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg2/Prisoner_Leg2.dat"))))
		return E_FAIL;
	/* Prototype_Component_Model_Khazan_Prisoner_Leg3*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg3/Prisoner_Leg3.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Hair1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Hair1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Hair/Prisoner_Hair1/Prisoner_Hair1.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Face1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Face1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Face/Prisoner_Face1/Prisoner_Face1.dat"))))
		return E_FAIL;


	/* Prototype_Component_Model_Khazan_Prisoner_Arm1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Arm/Prisoner_Arm1/Prisoner_Arm1.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Arm2*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Arm/Prisoner_Arm2/Prisoner_Arm2.dat"))))
		return E_FAIL;
#pragma endregion

#pragma region YETUGA

	// Prototype_Component_Model_Yetuga
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Yetuga/Yetuga.dat"))))
		return E_FAIL;

	// Prototype_Component_Model_Yetuga_Stone
 	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga_Stone"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Yetuga/Yetuga_Stone/Yetuga_Stone.dat"))))
		return E_FAIL;

	// Prototype_Component_Model_Yetuga_Rock
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga_Rock"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Yetuga/Yetuga_Rock/Yetuga_Rock.dat"))))
		return E_FAIL;

#pragma endregion

#pragma region Gomdol
	//// Prototype_Component_Model_Gomdel
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Gomdol"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/BigBear/BigBear.dat"))))
	//	return E_FAIL;
#pragma endregion

#pragma region Imp_Range

    //Goblin_Range.dat
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Goblin_Range"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Goblin_Range/Goblin_Range.dat"))))
        return E_FAIL;

    
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_ImpWand"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Goblin_Range/ImpWand/ImpWand.dat"))))
        return E_FAIL;

#pragma endregion

#pragma region Imp_Melee
    //Goblin_Melee.dat
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Goblin_Melee"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Goblin_Melee/Goblin_Melee.dat"))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_ImpSword"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Goblin_Melee/ImpSword/ImpSword.dat"))))
        return E_FAIL;

#pragma endregion

#pragma region �� ���� : ��ȣ �ۿ� �� ������Ʈ
	/* Prototype_Component_Model_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_BladeNexus"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_DamagedTS/WIP_COM_DamagedTS.dat")), E_FAIL);

	/* Prototype_Component_Model_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_BigChest"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_BigChest_Open_003/WIP_COM_BigChest_Open_003.dat")), E_FAIL);
#pragma endregion

#pragma region �� ���� : Ʈ����
	/* Prototype_Component_Model_Trigger */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Trigger"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Base/Cube/Cube.dat")), E_FAIL);
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_HeinMach_Shader()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_HeinMach_GameObject()
{
	/* Prototype_GameObject_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Camera_Compre */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Camera_Compre"),
		CCamera_Compre::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma region YETUGA

	/* Prototype_GameObject_Monster_Yetuga */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Yetuga"),
		CYetuga::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_PartObject_Yetuga_Body
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Yetuga_Body"),
		CBody_Yetuga::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_PartObject_Yetuga_Body
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Yetuga_Head"),
		CHead_Yetuga::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Yetuga_Stone"),
		CProjectile_Yetuga::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Yetuga_Rock"),
		CProjectile_Rock_Yetuga::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region GOMDOL

	/* Prototype_GameObject_Monster_Yetuga */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Gomdol"),
		CGomdol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_PartObject_Yetuga_Body
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Gomdol_Body"),
		CBody_Gomdol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//// Prototype_PartObject_Yetuga_Body
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Yetuga_Head"),
	//	CHead_Yetuga::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

#pragma endregion

#pragma region Imp_Range
    

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Imp_Range"),
        CImp_Range::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Range_Body"),
        CBody_Imp_Range::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Range_Wand"),
        CImp_Wand::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Imp_Magic_Ball"),
        CProjectile_Imp_MagicBall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Imp_Boomarang"),
        CProjectile_Boomarang::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    

#pragma endregion



	/* Prototype_GameObject_Prop_Object */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_Static */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Static"),
		CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region ���� ������Ʈ ���� : ��ȣ �ۿ� �� ������Ʈ
	/* Prototype_GameObject_Prop_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_BladeNexus"),
		CBladeNexus::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_BigChest"),
		CBigChest::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region ���� ������Ʈ ���� : Ʈ����
	/* Prototype_GameObject_Prop_Trigger */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_HeinMach_Trigger"),
		CHeinMach_Trigger::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

	/* Prototype_GameObject_Khazan_Spear */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Khazan_Spear"),
		CKhazan_Spear::Create(m_pDevice, m_pContext))))
 		return E_FAIL;

	/* Prototype_GameObject_Body_Khazan_Spear */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Body_Khazan_Spear"),
		CBody_Khazan_Spear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Spear_Khazan_Spear */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Spear_Khazan_Spear"),
		CSpear_Khazan_Spear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Effect 
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("SpearWind"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Spear"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Blust"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Blust"))))
        return E_FAIL;
#pragma endregion


	return S_OK;
}

HRESULT CLoader::Loading_For_Crevice_Level()
{
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Crevice_Texture();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Crevice_Model();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Crevice_Shader();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Crevice_GameObject();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("Crevice"), LEVEL::CREVICE, KHAZAN_MAP::CREVICE), E_FAIL);
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Loading_Prototype_MapObject_Inst_From_DAT(TEXT("Crevice"), LEVEL::CREVICE, KHAZAN_MAP::CREVICE), E_FAIL);
		}));

	return S_OK;
}

HRESULT CLoader::Loading_For_Crevice_Texture()
{

	return S_OK;
}


HRESULT CLoader::Loading_For_Crevice_Model()
{
	/* Prototype_Component_Model_Khazan_Sample*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_Component_Model_Khazan_Sample"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Khazan_Sample.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Spear_Khazan_Sample*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_Component_Model_Spear_Khazan_Sample"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Spear/Spear.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Spear*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_Component_Model_Khazan_Spear"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Spear/Khazan_Spear.dat"))))
		return E_FAIL;

#pragma region �� ���� : ��ȣ �ۿ� �� ������Ʈ
	/* Prototype_Component_Model_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_Component_Model_BladeNexus"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_DamagedTS/WIP_COM_DamagedTS.dat")), E_FAIL);

	/* Prototype_Component_Model_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_Component_Model_BigChest"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_BigChest_Open_003/WIP_COM_BigChest_Open_003.dat")), E_FAIL);

	/* Prototype_Component_Model_TombStone */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_Component_Model_TombStone"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_BigTombStone_Destruct_001/WIP_BGQ_BigTombStone_Destruct_001.dat")), E_FAIL);
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_Crevice_Shader()
{

	return S_OK;
}

HRESULT CLoader::Loading_For_Crevice_GameObject()
{
	/* Prototype_GameObject_Camera_Compre */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_Camera_Compre"),
		CCamera_Compre::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region ���� ������Ʈ : ��

	/* Prototype_GameObject_Prop_Object */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_Static */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_Prop_Static"),
		CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region ���� ������Ʈ ���� : ��ȣ �ۿ� �� ������Ʈ
	/* Prototype_GameObject_Prop_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_Prop_BladeNexus"),
		CBladeNexus::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_Prop_BigChest"),
		CBigChest::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_TombStone */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_Prop_TombStone"),
		CTombStone::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion


#pragma region Khazna
	/* Prototype_GameObject_Khazan_Spear */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_Khazan_Spear"),
		CKhazan_Spear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Body_Khazan_Spear */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_Body_Khazan_Spear"),
		CBody_Khazan_Spear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Spear_Khazan_Spear */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_Spear_Khazan_Spear"),
		CSpear_Khazan_Spear::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region ����Ʈ �׽�Ʈ ��!
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_TestParticle"),
		CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/test1"))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_Viper_Level()
{
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Viper_Texture();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Viper_Model();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Viper_Shader();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Viper_GameObject();
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_Prototype_MapObject_From_DAT(TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER);
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
        return Loading_Prototype_MapObject_Inst_From_DAT(TEXT("Viper"), LEVEL::VIPER, KHAZAN_MAP::VIPER);
		}));

	return S_OK;
}

HRESULT CLoader::Loading_For_Viper_Texture()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Viper_Model()
{
#pragma region KHAZAN
	/* Prototype_Component_Model_Khazan_Sample*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Sample"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Khazan_Sample.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Spear_Khazan_Sample*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Spear_Khazan_Sample"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Spear/Spear.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Spear*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Spear"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Spear/Khazan_Spear.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Torso1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso1/Prisoner_Torso1.dat"))))
		return E_FAIL;
	/* Prototype_Component_Model_Khazan_Prisoner_Torso2*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso2/Prisoner_Torso2.dat"))))
		return E_FAIL;
	/* Prototype_Component_Model_Khazan_Prisoner_Torso3*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso3/Prisoner_Torso3.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Shoes1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Shoes/Prisoner_Shoes1/Prisoner_Shoes1.dat"))))
		return E_FAIL;
	///* Prototype_Component_Model_Khazan_Prisoner_Shoes2*/
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes2"),
	//	CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Shoes/Prisoner_Shoes2/Prisoner_Shoes2.dat"))))
	//	return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Leg1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg1/Prisoner_Leg1.dat"))))
		return E_FAIL;
	/* Prototype_Component_Model_Khazan_Prisoner_Leg2*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg2/Prisoner_Leg2.dat"))))
		return E_FAIL;
	/* Prototype_Component_Model_Khazan_Prisoner_Leg3*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg3"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg3/Prisoner_Leg3.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Hair1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Hair1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Hair/Prisoner_Hair1/Prisoner_Hair1.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Face1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Face1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Face/Prisoner_Face1/Prisoner_Face1.dat"))))
		return E_FAIL;


	/* Prototype_Component_Model_Khazan_Prisoner_Arm1*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm1"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Arm/Prisoner_Arm1/Prisoner_Arm1.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Prisoner_Arm2*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm2"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Arm/Prisoner_Arm2/Prisoner_Arm2.dat"))))
		return E_FAIL;
#pragma endregion

#pragma region �� ���� : ��ȣ �ۿ� �� ������Ʈ
	/* Prototype_Component_Model_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_BladeNexus"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_DamagedTS/WIP_COM_DamagedTS.dat")), E_FAIL);

	/* Prototype_Component_Model_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_BigChest"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_BigChest_Open_003/WIP_COM_BigChest_Open_003.dat")), E_FAIL);
#pragma endregion

#pragma region �� ���� : Ʈ����
	/* Prototype_Component_Model_Trigger */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Trigger"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Base/Cube/Cube.dat")), E_FAIL);
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_Viper_Shader()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_Viper_GameObject()
{
	/* Prototype_GameObject_Camera_Compre */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Camera_Compre"),
		CCamera_Compre::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Prop_Object */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_Static */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Prop_Static"),
		CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region ���� ������Ʈ ���� : ��ȣ �ۿ� �� ������Ʈ
	/* Prototype_GameObject_Prop_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Prop_BladeNexus"),
		CBladeNexus::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Prop_BigChest"),
		CBigChest::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region ���� ������Ʈ ���� : Ʈ����
	/* Prototype_GameObject_Prop_Trigger */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Prop_HeinMach_Trigger"),
		CHeinMach_Trigger::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

	/* Prototype_GameObject_Khazan_Spear */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Khazan_Spear"),
		CKhazan_Spear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Body_Khazan_Spear */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Body_Khazan_Spear"),
		CBody_Khazan_Spear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Spear_Khazan_Spear */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Spear_Khazan_Spear"),
		CSpear_Khazan_Spear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Prototype_MapObject_From_DAT(const _tchar* pPrototypeDataFileName, LEVEL eLevel, KHAZAN_MAP eMap)
{
	// Dat ���� �⺻ ���
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
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
	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("[DAT ERROR]"), E_FAIL);

	// 1. ������ Ÿ���� �� ����
	_uint iPrototypeCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iPrototypeCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

	// ������ Ÿ���� �� ������ŭ ��ȸ
	for (_uint i = 0; i < iPrototypeCnt; ++i)
	{
		// CModel �� ����� �ϴ� ��� ( Instance X )
		// 2. ������ Ÿ�� �±� ���� ����
		_uint iPrototypeTagLen = {};
		CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 3. ������ Ÿ�� �±� �̸� ����
		_tchar szPrototypeTag[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

		// 4. �� ��� ���� ����
		_uint iModelPathLen = {};
		CHECK_FALSE(ReadFile(hFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 5. �� ��� �̸� ����
		_char szModelPath[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szModelPath, sizeof(_char) * iModelPathLen, &dwByte, nullptr), E_FAIL);

 		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(eLevel), szPrototypeTag,
			CModel::Create(m_pDevice, m_pContext, szModelPath))))
		{
			CloseHandle(hFile);
			MSG_BOX(TEXT("[DAT ERROR] ( CModel )"));
			return E_FAIL;
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLoader::Loading_Prototype_MapObject_Inst_From_DAT(const _tchar* pPrototypeDataFileName, LEVEL eLevel, KHAZAN_MAP eMap)
{
	// Dat ���� �⺻ ���
	_wstring pDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

	switch (eMap)
	{
	case KHAZAN_MAP::HEINMACH:
		pDataFilePath += TEXT("HeinMach/");
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
	CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("[DAT ERROR]"), E_FAIL);

	// 1. ������ Ÿ���� �� ����
	_uint iPrototypeCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iPrototypeCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

	// ������ Ÿ���� �� ������ŭ ��ȸ
	for (_uint i = 0; i < iPrototypeCnt; ++i)
	{
		// CModel �� ����� �ϴ� ��� ( Instance X )

		// 2. ������ Ÿ�� �±� ���� ����
		_uint iPrototypeTagLen = {};
		CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 3. ������ Ÿ�� �±� �̸� ����
		_tchar szPrototypeTag[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

		// 4. �� ��� ���� ����
		_uint iModelPathLen = {};
		CHECK_FALSE(ReadFile(hFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 5. �� ��� �̸� ����
		_char szModelPath[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szModelPath, sizeof(_char) * iModelPathLen, &dwByte, nullptr), E_FAIL);

		// 6. �ν��Ͻ� ��� �� ���� ����
		_uint iNumInstances = {};
		CHECK_FALSE(ReadFile(hFile, &iNumInstances, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// �ν��Ͻ� ���� �ѱ��
		CModelMesh_Instance::MODELMESH_INSTANCE_DESC MeshInstanceDesc = {};

		MeshInstanceDesc.iNumInstance = iNumInstances;

		// 7. ��� ������ŭ ���� resize �� read file
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
			MSG_BOX(TEXT("[DAT ERROR]  ( CModel )"));
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
