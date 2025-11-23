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
#include "Lantern_Khazan_Spear.h"
#include "Khazan_GSword.h"
#include "Body_Khazan_GS.h"
#include "GSword_Khazan_GS.h"
#include "Lantern_Khazan_GS.h"

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
#include "Projectile_Breath_Yetuga.h"

#include "Gomdol.h"
#include "Body_Gomdol.h"

#include "Imp_Range.h"
#include "Body_Imp_Range.h"
#include "Imp_Wand.h"
#include "Projectile_Imp_MagicBall.h"
#include "Projectile_Boomarang.h"

#include "Imp_Melee.h"
#include "Body_Imp_Melee.h"
#include "Imp_Sword.h"


#include "Viper.h"
#include "Body_Viper.h"

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
        m_futures.push_back(m_pGameInstance->Add_Task([this]() {
            return Loading_For_Embars_Level();
            }));
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
        return S_OK;
		}));

	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		Loading_For_Test_Model();
		Loading_For_Test_GameObject();
        return S_OK;
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Test_Shader();
        return S_OK;
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("HeinMach"), LEVEL::TEST), E_FAIL);
        return S_OK;
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

HRESULT CLoader::Loading_For_Effect_Static(_uint level)
{
    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("SpearWind"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Spear"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Blust"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Blust"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Blust2"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Blust2"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Blust3"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Blust3"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Blust4"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Blust4"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Blust5"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Blust5"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Blust6"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Blust6"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("BlustSmall"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/blustSmall"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Fire"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/fire1"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Open"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/TreasureBox"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Spawn"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Spawn"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("BloodHit"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/bloodhit"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("SpiralSpear_SpearFX"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Bloodhand_Big"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Stamp"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/stamp"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Snow"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Snow"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Snow_Once"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/snow_once"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("GhostKnight"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/GhostKnight"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("GhostKnight_static"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/GhostKnight_static"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("GhostKnight_static_connect"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/GhostKnight_static_connect"))))
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

	///* Prototype_Component_Texture_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Sky/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain/Tile%d.dds"), 2))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Mask_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Mask_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain/TerrainMask.dds"), 1))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Brush */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Texture_Brush"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain/Brush.png"), 1))))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading_For_HeinMach_Model()
{

#pragma region KHAZAN
    /* Prototype_Component_Model_Khazan_Lantern*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Lantern"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/APC/Lantern/Lantern.dat"))))
        return E_FAIL;

	/* Prototype_Component_Model_Spear_Khazan_Sample*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Spear_Khazan_Sample"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Spear/Spear.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_Khazan_Spear*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_Spear"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Spear/Khazan_Spear.dat"))))
		return E_FAIL;

    /* Prototype_Component_Model_Khazan_GSword*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_GSword"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_GSword/Khazan_GSword.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_GSword_Meteor*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_GSword_Meteor"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/APC/GSword_Meteor/GSword_Meteor.dat"))))
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

    /* Prototype_Component_Model_Khazan_DanJin_Hair*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Khazan_DanJin_Hair"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Hair/Danjin_Hair/Danjin_Hair/Danjin_Hair.dat"))))
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
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Goblin_Melee/Imp_Sword/Imp_Sword.dat"))))
        return E_FAIL;


#pragma endregion

#pragma region �� ���� : ��ȣ �ۿ� �� ������Ʈ
	/* Prototype_Component_Model_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_BigChest"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_BigChest_Open_003/WIP_COM_BigChest_Open_003.dat")), E_FAIL);

#pragma region 엘리베이터 ( 스몰 사이즈 )
    /* Prototype_Component_Model_SmallElevator */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_SmallElevator"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_TDl_Elevator_Move_001_a/WIP_TDl_Elevator_Move_001_a.dat")), E_FAIL);

    /* Prototype_Component_Model_Elevator_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Elevator_Gear"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_TDl_Elevator_Turn_001/WIP_TDl_Elevator_Turn_001.dat")), E_FAIL);

    /* Prototype_Component_Model_Slate_Switch */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Slate_Switch"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Slate_Switch_001/WIP_BGQ_Slate_Switch_001.dat")), E_FAIL);
#pragma endregion

#pragma endregion

#pragma region �� ���� : Ʈ����
	/* Prototype_Component_Model_Trigger */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Trigger"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Base/Cube/Cube.dat")), E_FAIL);
#pragma endregion


#pragma region DEST
    /* Prototype_Component_Model_Fence_Chunk */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Fence_Chunk_1"),
    CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Fence/Fence_Chunk1/Fence_Chunk1.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Fence_Chunk_2"),
    CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Fence/Fence_Chunk2/Fence_Chunk2.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Fence_Chunk_3"),
    CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Fence/Fence_Chunk3/Fence_Chunk3.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Fence_Chunk_4"),
    CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Fence/Fence_Chunk4/Fence_Chunk4.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Fence_Chunk_5"),
    CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Fence/Fence_Chunk5/Fence_Chunk5.dat")), E_FAIL);

    /* Prototype_Component_Model_Pot_Chunk */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Pot_Chunk_1"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Box/Pot_Chunk1/Pot_Chunk1.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Pot_Chunk_2"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Box/Pot_Chunk2/Pot_Chunk2.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Pot_Chunk_3"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Box/Pot_Chunk3/Pot_Chunk3.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Pot_Chunk_4"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Box/Pot_Chunk4/Pot_Chunk4.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Pot_Chunk_5"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Box/Pot_Chunk5/Pot_Chunk5.dat")), E_FAIL);

    /* Prototype_Component_Model_Barrel_Chunk */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Barrel_Chunk_1"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Box/Barrel_Chunk1/Barrel_Chunk1.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Barrel_Chunk_2"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Box/Barrel_Chunk2/Barrel_Chunk2.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Barrel_Chunk_3"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Box/Barrel_Chunk3/Barrel_Chunk3.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Barrel_Chunk_4"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Box/Barrel_Chunk4/Barrel_Chunk4.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Barrel_Chunk_5"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Box/Barrel_Chunk5/Barrel_Chunk5.dat")), E_FAIL);

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

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Yetuga_Breath"),
        CProjectile_Breath_Yetuga::Create(m_pDevice, m_pContext))))
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

  
#pragma region Imp_Melee

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Monster_Imp_Melee"),
        CImp_Melee::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Melee_Body"),
        CBody_Imp_Melee::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Melee_Sword"),
        CImp_Sword::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#pragma endregion
	/* Prototype_GameObject_Prop_Object */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Object"),
		CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_Static */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Static"),
		CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region _Prop_BladeNexus, _Prop_BigChest
	/* Prototype_GameObject_Prop_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_BladeNexus"),
		CBladeNexus::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Prop_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_BigChest"),
		CBigChest::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region 엘리베이터 ( 스몰 사이즈 )
    /* Prototype_GameObject_Prop_SmallElevator */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_SmallElevator"),
        CElevatorS::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Elevator_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Elevator_Gear"),
        CElevator_Gear::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Slate_Switch */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Slate_Switch"),
        CSlate_Switch::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 파괴
    /* Prototype_GameObject_Prop_Chunk */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"),
        CProp_Chunk::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Fence */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Fence"),
        CFence::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Pot */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Pot"),
        CPot::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Barrel */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Barrel"),
        CBarrel::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma endregion



#pragma endregion

#pragma region _Prop_Trigger
	/* Prototype_GameObject_Prop_Trigger */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_HeinMach_Trigger"),
		CHeinMach_Trigger::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region Khazan
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

    /* Prototype_GameObject_Lantern_Khazan_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Lantern_Khazan_Spear"),
        CLantern_Khazan_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_Khazan_GSword */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Khazan_GSword"),
        CKhazan_GSword::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_Body_Khazan_GS */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Body_Khazan_GS"),
        CBody_Khazan_GS::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_GSword_Khazan_GS */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_GSword_Khazan_GS"),
        CGSword_Khazan_GS::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_Lantern_Khazan_GS */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Lantern_Khazan_GS"),
        CLantern_Khazan_GS::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#pragma endregion


#pragma region Effect 
    Loading_For_Effect_Static(ENUM_CLASS(LEVEL::HEINMACH));
   
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Yetuga_Snow"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Snow_Cam"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/snow_cam"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Yetuga_SnowUp"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Big"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Yetuga_Snow_Big"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Snow_Small"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Yetuga_Snow_Small"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Boomarang"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Boomerang"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("MagicBall"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/MonsterSphere"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Breath"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Breath"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Focus"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Yetuga_Focus"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Ice"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Yetuga_Ice"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Ice_Disappear"),
        CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/Yetuga_Ice_Disappear"))))
        return E_FAIL;
#pragma endregion

#pragma region Shader
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Decal"),
        CDecal::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_Crevice_Level()
{
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Crevice_Texture();
        return S_OK;
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Crevice_Model();
        return S_OK;
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Crevice_Shader();
        return S_OK;
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		return Loading_For_Crevice_GameObject();
        return S_OK;
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("Crevice"), LEVEL::CREVICE, KHAZAN_MAP::CREVICE), E_FAIL);
        return S_OK;
		}));
	m_futures.push_back(m_pGameInstance->Add_Task([this]() {
		CHECK_FAILED(Loading_Prototype_MapObject_Inst_From_DAT(TEXT("Crevice"), LEVEL::CREVICE, KHAZAN_MAP::CREVICE), E_FAIL);
        return S_OK;
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


#pragma region Khazan
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

    /* Prototype_GameObject_Lantern_Khazan_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_Lantern_Khazan_Spear"),
        CLantern_Khazan_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion

#pragma region ����Ʈ �׽�Ʈ ��!
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::CREVICE), TEXT("Prototype_GameObject_TestParticle"),
		CEffect_Prefab::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Effect/Baked/test1"))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Loading_For_Embars_Level()
{
    m_futures.push_back(m_pGameInstance->Add_Task([this]() {
        return Loading_For_Embars_Texture();
        }));
    m_futures.push_back(m_pGameInstance->Add_Task([this]() {
        return Loading_For_Embars_Model();
        }));
    m_futures.push_back(m_pGameInstance->Add_Task([this]() {
        return Loading_For_Embars_Shader();
        }));
    m_futures.push_back(m_pGameInstance->Add_Task([this]() {
        return Loading_For_Embars_GameObject();
        }));
    m_futures.push_back(m_pGameInstance->Add_Task([this]() {
        CHECK_FAILED(Loading_Prototype_MapObject_From_DAT(TEXT("Embars"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);
        }));
    m_futures.push_back(m_pGameInstance->Add_Task([this]() {
        CHECK_FAILED(Loading_Prototype_MapObject_Inst_From_DAT(TEXT("Embars"), LEVEL::EMBARS, KHAZAN_MAP::EMBARS), E_FAIL);
        }));

    return S_OK;
}

HRESULT CLoader::Loading_For_Embars_Texture()
{
    return S_OK;
}

HRESULT CLoader::Loading_For_Embars_Model()
{
#pragma region KHAZAN
    /* Prototype_Component_Model_Khazan_Lantern*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Lantern"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/APC/Lantern/Lantern.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Spear_Khazan_Sample*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Spear_Khazan_Sample"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Sample/Spear/Spear.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Spear*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Spear"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Spear/Khazan_Spear.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Torso1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso1"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso1/Prisoner_Torso1.dat"))))
        return E_FAIL;
    /* Prototype_Component_Model_Khazan_Prisoner_Torso2*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso2"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso2/Prisoner_Torso2.dat"))))
        return E_FAIL;
    /* Prototype_Component_Model_Khazan_Prisoner_Torso3*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso3"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Torso/Prisoner_Torso3/Prisoner_Torso3.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Shoes1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes1"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Shoes/Prisoner_Shoes1/Prisoner_Shoes1.dat"))))
        return E_FAIL;
    ///* Prototype_Component_Model_Khazan_Prisoner_Shoes2*/
    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes2"),
    //	CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Shoes/Prisoner_Shoes2/Prisoner_Shoes2.dat"))))
    //	return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Leg1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg1"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg1/Prisoner_Leg1.dat"))))
        return E_FAIL;
    /* Prototype_Component_Model_Khazan_Prisoner_Leg2*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg2"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg2/Prisoner_Leg2.dat"))))
        return E_FAIL;
    /* Prototype_Component_Model_Khazan_Prisoner_Leg3*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg3"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Leg/Prisoner_Leg3/Prisoner_Leg3.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Hair1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Hair1"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Hair/Prisoner_Hair1/Prisoner_Hair1.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Face1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Face1"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Face/Prisoner_Face1/Prisoner_Face1.dat"))))
        return E_FAIL;


    /* Prototype_Component_Model_Khazan_Prisoner_Arm1*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm1"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Arm/Prisoner_Arm1/Prisoner_Arm1.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_Prisoner_Arm2*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm2"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Arm/Prisoner_Arm2/Prisoner_Arm2.dat"))))
        return E_FAIL;

    /* Prototype_Component_Model_Khazan_DanJin_Hair*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Khazan_DanJin_Hair"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Hair/Danjin_Hair/Danjin_Hair/Danjin_Hair.dat"))))
        return E_FAIL;
#pragma endregion

#pragma region 상호작용 오브젝
    /* Prototype_Component_Model_BigChest */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_BigChest"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_BigChest_Open_003/WIP_COM_BigChest_Open_003.dat")), E_FAIL);

#pragma region 엘리베이터 ( 스몰 사이즈 )
    /* Prototype_Component_Model_SmallElevator */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_SmallElevator"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_TDl_Elevator_Move_001_a/WIP_TDl_Elevator_Move_001_a.dat")), E_FAIL);

    /* Prototype_Component_Model_Elevator_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Elevator_Gear"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_TDl_Elevator_Turn_001/WIP_TDl_Elevator_Turn_001.dat")), E_FAIL);

    /* Prototype_Component_Model_Slate_Switch */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Slate_Switch"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Slate_Switch_001/WIP_BGQ_Slate_Switch_001.dat")), E_FAIL);
#pragma endregion

#pragma region 레버, 기어
    /* Prototype_Component_Model_Lever */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Lever"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Gear_Switch_001/WIP_BGQ_Gear_Switch_001.dat")), E_FAIL);

    /* Prototype_Component_Model_Lever_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Lever_Gear"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Machine_Loop_001/WIP_BGQ_Machine_Loop_001.dat")), E_FAIL);

    /* Prototype_Component_Model_Door_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Door_Gear"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_BGQ_Gear_Loop_001/WIP_BGQ_Gear_Loop_001.dat")), E_FAIL);
#pragma endregion

#pragma region 조각상 퍼즐
    /* Prototype_Component_Model_Statue */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Statue"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Base_StatueTable_Rotate_001/WIP_Base_StatueTable_Rotate_001.dat")), E_FAIL);

    /* Prototype_Component_Model_Statue_Plate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Statue_Plate"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_TDL_StatueTable_Static_001/WIP_TDL_StatueTable_Static_001.dat")), E_FAIL);

    /* Prototype_Component_Model_Statue_Deco */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Statue_Deco"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Symbol/WP_CTR_Statue_Stone_002/WP_CTR_Statue_Stone_002.dat")), E_FAIL);
#pragma endregion

#pragma region 수직 차단봉
    /* Prototype_Component_Model_VerticalGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_VerticalGate"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_TDL_Gate_Open_004/WIP_TDL_Gate_Open_004.dat")), E_FAIL);
#pragma endregion

#pragma region 잠긴 철문
    /* Prototype_Component_Model_IronGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_IronGate"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Base_MiddleDoor_Open_001_b/WIP_Base_MiddleDoor_Open_001_b.dat")), E_FAIL);

    /* Prototype_Component_Model_IronGate_Lock */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_IronGate_Lock"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Base_MiddleDoor_Lock_001/WIP_Base_MiddleDoor_Lock_001.dat")), E_FAIL);

    /* Prototype_Component_Model_IronGate_Part */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_IronGate_Part"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Building_B/WP_BTP_Door_Metal_004/WP_BTP_Door_Metal_004.dat")), E_FAIL);
#pragma endregion

#pragma region 사다리
    /* Prototype_Component_Model_Ladder_Top */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Ladder_Top"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Top_Ladder_Metal_01/WIP_Top_Ladder_Metal_01.dat")), E_FAIL);

    /* Prototype_Component_Model_Ladder_Middle */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Ladder_Middle"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Middle_Ladder_Metal_01/WIP_Middle_Ladder_Metal_01.dat")), E_FAIL);

    /* Prototype_Component_Model_Ladder_Bottom */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Ladder_Bottom"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Bottom_Ladder_Metal_01/WIP_Bottom_Ladder_Metal_01.dat")), E_FAIL);

    /* Prototype_Component_Model_Ladder_Support */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Ladder_Support"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_Bottom_Ladder_Support_Static_Metal_01/WIP_Bottom_Ladder_Support_Static_Metal_01.dat")), E_FAIL);
#pragma endregion

#pragma endregion

#pragma region 트리거
    /* Prototype_Component_Model_Trigger */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_Trigger"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Base/Cube/Cube.dat")), E_FAIL);
#pragma endregion

    return S_OK;
}

HRESULT CLoader::Loading_For_Embars_Shader()
{
    return S_OK;
}

HRESULT CLoader::Loading_For_Embars_GameObject()
{
    /* Prototype_GameObject_Camera_Compre */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Camera_Compre"),
        CCamera_Compre::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#pragma region 맵 오브젝트 원형
    /* Prototype_GameObject_Prop_Object */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Object"),
        CProp_Object::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Static */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Static"),
        CProp_Static::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 게임오브젝트 원형 로딩 ( 상호 작용 객체 )
    /* Prototype_GameObject_Prop_BladeNexus */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_BladeNexus"),
        CBladeNexus::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_BigChest */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_BigChest"),
        CBigChest::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_TombStone */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_TombStone"),
        CTombStone::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma region 엘리베이터 ( 스몰 사이즈 )
    /* Prototype_GameObject_Prop_SmallElevator */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_SmallElevator"),
        CElevatorS::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Elevator_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Elevator_Gear"),
        CElevator_Gear::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Slate_Switch */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Slate_Switch"),
        CSlate_Switch::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 레버, 기어
    /* Prototype_GameObject_Prop_Lever */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Lever"),
        CLever::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Lever_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Lever_Gear"),
        CLever_Gear::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Door_Gear */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Door_Gear"),
        CDoor_Gear::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 조각상 퍼즐
    /* Prototype_GameObject_Prop_Statue */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Statue"),
        CStatue::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Statue_Plate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Statue_Plate"),
        CStatue_Plate::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Statue_Deco */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Statue_Deco"),
        CStatue_Deco::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 수직 차단봉
    /* Prototype_GameObject_Prop_VerticalGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_VerticalGate"),
        CVerticalGate::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 잠긴 철문
    /* Prototype_GameObject_Prop_IronGate */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_IronGate"),
        CIronGate::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_IronGate_Lock */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_IronGate_Lock"),
        CIronGate_Lock::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_IronGate_Part_L */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_IronGate_Part_L"),
        CIronGate_Part_L::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_IronGate_Part_R */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_IronGate_Part_R"),
        CIronGate_Part_R::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 사다리
    /* Prototype_GameObject_Prop_Ladder */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Ladder"),
        CLadder::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Ladder_Top */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Ladder_Top"),
        CLadder_Top::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Ladder_Middle */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Ladder_Middle"),
        CLadder_Middle::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Ladder_Bottom */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Ladder_Bottom"),
        CLadder_Bottom::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Ladder_Support */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Ladder_Support"),
        CLadder_Support::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma endregion

#pragma region 게임오브젝트 원형 로딩 ( 트리거 및 몬스터 )
    /* Prototype_GameObject_Prop_Trigger */
    // CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Prop_Trigger"),
    //     CTrigger::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 게임오브젝트 원형 로딩 ( 데칼 )
    /* Prototype_GameObject_Decal */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Decal"),
        CDecal::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion


    /* Prototype_GameObject_Khazan_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Khazan_Spear"),
        CKhazan_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_Body_Khazan_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Body_Khazan_Spear"),
        CBody_Khazan_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_Spear_Khazan_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Spear_Khazan_Spear"),
        CSpear_Khazan_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_Lantern_Khazan_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_GameObject_Lantern_Khazan_Spear"),
        CLantern_Khazan_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;
     
#pragma region EFFECT
    Loading_For_Effect_Static(ENUM_CLASS(LEVEL::EMBARS));
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

    /* Prototype_Component_Model_Khazan_Lantern*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_Lantern"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/APC/Lantern/Lantern.dat"))))
        return E_FAIL;


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


    /* Prototype_Component_Model_Khazan_DanJin_Hair*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Khazan_DanJin_Hair"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Khazan/Khazan_Parts/Hair/Danjin_Hair/Danjin_Hair/Danjin_Hair.dat"))))
        return E_FAIL;


#pragma endregion

#pragma region 상호작용 오브젝
	/* Prototype_Component_Model_BladeNexus */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_BladeNexus"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_DamagedTS/WIP_COM_DamagedTS.dat")), E_FAIL);

	/* Prototype_Component_Model_BigChest */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_BigChest"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_BigChest_Open_003/WIP_COM_BigChest_Open_003.dat")), E_FAIL);
#pragma endregion

#pragma region 트리거
	/* Prototype_Component_Model_Trigger */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Trigger"),
		CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Base/Cube/Cube.dat")), E_FAIL);
#pragma endregion


#pragma region VIPER

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Viper_Phase1"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Monster/Model/Viper_Phase1/Viper_Phase1.dat")), E_FAIL);

#pragma endregion


#pragma region CHUNK
    /* Prototype_Component_Model_Obelisk_Chunk */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_1"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk1/Obelisk_Chunk1.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_2"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk2/Obelisk_Chunk2.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_3"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk3/Obelisk_Chunk3.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_4"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk4/Obelisk_Chunk4.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_5"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk5/Obelisk_Chunk5.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_6"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk6/Obelisk_Chunk6.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_7"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk7/Obelisk_Chunk7.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_8"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk8/Obelisk_Chunk8.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_9"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk9/Obelisk_Chunk9.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_10"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk10/Obelisk_Chunk10.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_11"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk11/Obelisk_Chunk11.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_12"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk12/Obelisk_Chunk12.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_13"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk13/Obelisk_Chunk13.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_14"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk14/Obelisk_Chunk14.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_15"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk15/Obelisk_Chunk15.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_16"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk16/Obelisk_Chunk16.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_17"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk17/Obelisk_Chunk17.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_18"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk18/Obelisk_Chunk18.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_19"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk19/Obelisk_Chunk19.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_20"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk20/Obelisk_Chunk20.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_21"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk21/Obelisk_Chunk21.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_22"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk22/Obelisk_Chunk22.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_23"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk23/Obelisk_Chunk23.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_24"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk24/Obelisk_Chunk24.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_25"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk25/Obelisk_Chunk25.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_26"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk26/Obelisk_Chunk26.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_27"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk27/Obelisk_Chunk27.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_28"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk28/Obelisk_Chunk28.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_29"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk29/Obelisk_Chunk29.dat")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Obelisk_Chunk_30"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/Prop/NonAnim/Obelisk/Obelisk_Chunk30/Obelisk_Chunk30.dat")), E_FAIL);
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

    /* Prototype_GameObject_Prop_Chunk */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Prop_Chunk"),
        CProp_Chunk::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_GameObject_Prop_Obelisk */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Prop_Obelisk"),
        CObelisk::Create(m_pDevice, m_pContext)), E_FAIL);
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

    /* Prototype_GameObject_Lantern_Khazan_Spear */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Lantern_Khazan_Spear"),
        CLantern_Khazan_Spear::Create(m_pDevice, m_pContext))))
        return E_FAIL;


#pragma region VIPER

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Monster_Viper"),
        CViper::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_PartObject_Body_Viper"),
        CBody_Viper::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#pragma endregion

#pragma region EFFECT
    Loading_For_Effect_Static(ENUM_CLASS(LEVEL::VIPER));
#pragma endregion


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

	// 1. 프로토 타입 총 개수 불러오기
	_uint iPrototypeCnt = {};
	CHECK_FALSE(ReadFile(hFile, &iPrototypeCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

	// 프로토 타입 개수만큼 LOOP
	for (_uint i = 0; i < iPrototypeCnt; ++i)
	{
		// CModel �� ����� �ϴ� ��� ( Instance X )

		// 2. 프로토 타입 태그 길이
		_uint iPrototypeTagLen = {};
		CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 3. 프로토 타입 태그
		_tchar szPrototypeTag[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

		// 4. 모델 주소 길이
		_uint iModelPathLen = {};
		CHECK_FALSE(ReadFile(hFile, &iModelPathLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 5. 모델 주소
		_char szModelPath[MAX_PATH] = {};
		CHECK_FALSE(ReadFile(hFile, &szModelPath, sizeof(_char) * iModelPathLen, &dwByte, nullptr), E_FAIL);

		// 6. 인스턴싱할 개수
		_uint iNumInstances = {};
		CHECK_FALSE(ReadFile(hFile, &iNumInstances, sizeof(_uint), &dwByte, nullptr), E_FAIL);

		// 인스턴싱 정보 구조체
		CModelMesh_Instance::MODELMESH_INSTANCE_DESC MeshInstanceDesc = {};

		MeshInstanceDesc.iNumInstance = iNumInstances;

		// 인스턴싱할 개수만큼 백터 Resize
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
