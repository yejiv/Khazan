#include "MainApp.h"
#include "Level_Loading.h"
#include "HeaderGroup_UI.h"

#include "MapObject_Header.h"
#include "Camera_Free.h"
#include "ScreenTrail.h"
#include "MeshTrail.h"
#include "LineTrail.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{	

	ENGINE_DESC		EngineDesc{};

	EngineDesc.hInst = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);
	EngineDesc.iWinSizeX_Imgui = g_iWinSizeX_Imgui;
	EngineDesc.iWinSizeY_Imgui = g_iWinSizeY_Imgui;
	EngineDesc.iNumJoltObjectLayer = ENUM_CLASS(COLLISION_LAYER::END);

	list<_wstring> Imgui_Menu;
	Imgui_Menu.push_back(TEXT("Client"));
	Imgui_Menu.push_back(TEXT("Debug"));
	Imgui_Menu.push_back(TEXT("Camera"));
	Imgui_Menu.push_back(TEXT("Shader"));
	EngineDesc.Menu_Imgui = Imgui_Menu;
	

	if(FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_ForStatic()))
		return E_FAIL;

	CHECK_FAILED(Ready_ClientInstance(&m_pDevice, &m_pContext), E_FAIL);
	CHECK_FAILED(Ready_DB(), E_FAIL);

	if (Ready_Prototype_ForStatic_UI())
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::TITLE)))
		return E_FAIL;

	CHECK_FAILED(Ready_Prototype_ForStatic_Effect(), E_FAIL);
	CHECK_FAILED(Ready_Font(), E_FAIL);
	//CHECK_FAILED(Ready_DebugTool(), E_FAIL); 

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_LCONTROL))
	{
		//m_pClientInstance->ActiveCamera_Shaking(1.5f, 1.f);

		//m_pGameInstance->Start_HitStop(TIME_CHANNEL::PLAYER, 0.3f, 0.003f, 3.f);
		//m_pGameInstance->Fix_HitStop(TIME_CHANNEL::ENEMY);
		//FOVModifier tMod{};

		// PRIORITY
		//tMod.strID = TEXT("Hit");
		//tMod.eMode = FOVModifier::FOV_MODE::PRIORITY;
		//tMod.fDuration = 0.f;
		//tMod.fFrom = 0.f;
		//tMod.fTo = XMConvertToRadians(50.f);
		//tMod.iPriority = 5.f;
		//tMod.Ease = EaseOutQuad;

		// ADD
		//tMod.eMode = FOVModifier::FOV_MODE::ADD;
		//tMod.fDuration = 5.f;
		//tMod.fFrom = 0.f;
		//tMod.fTo = XMConvertToRadians(80.f);
		//tMod.iPriority = 5.f;
		//tMod.Ease = EaseOutQuad;

		// Multiply
		//tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
		//tMod.fDuration = 5.f;
		//tMod.fFrom = XMConvertToRadians(60.f);
		//tMod.fTo = XMConvertToRadians(80.f);
		//tMod.iPriority = 5.f;
		//tMod.Ease = EaseOutQuad;

		//m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);

        //m_pClientInstance->Find_MapBladeNexus(KHAZAN_MAP::HEINMACH);
	}
	if (m_pGameInstance->Key_Down(DIK_RCONTROL))
	{
		//m_pClientInstance->ActiveCamera_KillFov(L"Hit");
		//m_pGameInstance->UnFix_HitStop(TIME_CHANNEL::ENEMY);
	}

	TIME_DELTA      tTimeDelta = {};

	const _float fDt_World = m_pGameInstance->Get_ScaledDelta(TEXT("Timer_60"), TIME_CHANNEL::WORLD);
	const _float fDt_Player = m_pGameInstance->Get_ScaledDelta(TEXT("Timer_60"), TIME_CHANNEL::PLAYER);
	const _float fDt_Enemy = m_pGameInstance->Get_ScaledDelta(TEXT("Timer_60"), TIME_CHANNEL::ENEMY);
	const _float fDt_UI = m_pGameInstance->Get_ScaledDelta(TEXT("Timer_60"), TIME_CHANNEL::MAP);
	const _float fDt_FX = m_pGameInstance->Get_ScaledDelta(TEXT("Timer_60"), TIME_CHANNEL::EFFECT);
	tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)] = fDt_World;
	tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::PLAYER)] = fDt_Player;
	tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::ENEMY)] = fDt_Enemy;
	tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::MAP)] = fDt_UI;
	tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::EFFECT)] = fDt_FX;

	m_pGameInstance->Update_Engine(tTimeDelta);
	m_pClientInstance->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);
	
}

HRESULT CMainApp::Render()
{
	_float4		vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForStatic()
{
	// VIBuffer

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

    /* Prototype_Component_VIBuffer_Point */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Point"),
        CVIBuffer_Point::Create(m_pDevice, m_pContext))))
        return E_FAIL;

	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain/Height.bmp")))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Shader
	/* Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	
	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxSphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxSphere"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxSphere.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxInstance_PointParticle*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_PointParticle.hlsl"), VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
		return E_FAIL;

    /* Prototype_Component_Shader_VtxPosTex */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosSpriteTex"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosSpriteTex.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
        return E_FAIL;

	/* Prototype_Component_Shader_ModelMeshInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_ModelMeshInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ModelMeshInstance.hlsl"), MESH_INSTANCING::Elements, MESH_INSTANCING::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxInstance_Point_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Point_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point_UI.hlsl"), UI_INSTANCING::Elements, UI_INSTANCING::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_UI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Cursor"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_Cursor.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex_UIMask*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Mask"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_UIMask.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex_Font */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Font"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_Font.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex_Font */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_UI_Effect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_RigidBody*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_CharacterVirtual*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
		CCharacterVirtual::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Body*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
		CBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

    /* Prototype_Component_SoftBody*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SoftBody"),
        CSoftBody::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_Component_ClothBody*/
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_ClothBody"),
        CClothBody::Create(m_pDevice, m_pContext))))
        return E_FAIL;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Prototype_Component_DeferredShader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_DeferredShader_VtxMesh"),
		CDeferredShader::Create(m_pDevice, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements, m_pGameInstance->Get_ThreadCount()))))
		return E_FAIL;

	/* Prototype_Component_DeferredShader_ModelMeshInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_DeferredShader_ModelMeshInstance"),
		CDeferredShader::Create(m_pDevice, TEXT("../Bin/ShaderFiles/Shader_ModelMeshInstance.hlsl"), MESH_INSTANCING::Elements, MESH_INSTANCING::iNumElements, m_pGameInstance->Get_ThreadCount()))))
		return E_FAIL;

#pragma region FOR LOADING & SKY BOX

#pragma region SKY SPHERE TEXTURES
	/* Prototype_Component_Texture_Cloud_Dist_Gradation */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cloud_Dist_Gradation"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/DistanceGradation.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Cloud_LookUp */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cloud_LookUp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FTW_Sky_Cloud_LookUp.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Cloud_Normal */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cloud_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FTW_Sky_Cloud_Nomal.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Cloud_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Cloud_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FTW_Sky_Cloud_Distortion.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky_Nebula */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky_Nebula"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FTW_Sky_Nebula.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky_Star_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sky_Star_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FTW_Sky_Star_Mask_001.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Moon */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Moon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FT_Rgb_Moon_001.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Ring */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Map/Prop/Sky/FT_Refraction_Ring.dds"), 1))))
		return E_FAIL;
#pragma endregion
	
#pragma region SKY SPHERE MODELS
	/* Prototype_Component_Model_SkyMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_SkyMesh"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Map/Prop/NonAnim/SkySphere/SkyMesh.dat"))))
		return E_FAIL;

	/* Prototype_Component_Model_CloudMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_CloudMesh"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Map/Prop/NonAnim/SkySphere/CloudMesh.dat"))))
		return E_FAIL;
#pragma endregion

#pragma region 귀검 모델 원형
    /* Prototype_Component_Model_BladeNexus */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_BladeNexus"),
        CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Data/Map/InteractiveProp/WIP_COM_DamagedTS/WIP_COM_DamagedTS.dat")), E_FAIL);
#pragma endregion

#pragma region 하늘 구름 객체 원형
	/* Prototype_GameObject_SkyShpere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkySphere"),
		CSkySphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_CloudSphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_CloudSphere"),
		CCloudSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region 귀검 객체 원형
    /* Prototype_GameObject_Loading_BladeNexus */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Loading_BladeNexus"),
        CLoadingObj_BN::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

#pragma region 카메라
	/* Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region 맵 데칼
    /* Prototype_GameObject_Decal_Static */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Decal_Static"),
        CDecal::Create(m_pDevice, m_pContext)), E_FAIL);
#pragma endregion

    /* Prototype_GameObject_Decal */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Decal"),
        CDecal::Create(m_pDevice, m_pContext)), E_FAIL);

    /* Prototype_Component_MotionTrail */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_MotionTrail"),
        CMotionTrail::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_Component_Texture_Monster_Dissolve */
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Dissolve"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Dissolve/FT_Noise_Dissolve_0.png"), 1))))
        return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForStatic_UI()
{
	//텍스처
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_World_Talk"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Talk/Talk_Bg_%d.png"), 3)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_GuideTex"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/GuideTex_%d.dds"), 3)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UI_BackGround"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/BG/T_Texture_Bg_UI.png"), 1)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hud_HPGauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Hud/State/T_Hud_Gauge_HP_0%d.png"), 7)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hud_HPGauge_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Hud/State/T_Hud_Gauge_HP_BG_0%d.png"), 10)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hud_Player_HPGauge_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Hud/State/T_Hud_Hp_Bg_0%d.png"), 9)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_MenuList"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Common/T_Img_List_Menu_%d.png"), 8)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_FX_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Mask/T_Fx_%d.png"), 14)), E_FAIL);

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_State_Button"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("..//Bin/Resources/UI/State/NormalButton_%d.png"), 10))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Guide_Circle"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Common/Guide_Circle.png"), 1))))
        return E_FAIL;
	//아이템 인포 텍스처
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_ItemInfo_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/ItemInfo/T_BG_ItemInfo_%d.png"), 2)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_ItemInfo_Top"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/ItemInfo/T_BG_ItemInfo_Top_%d.png"), 6)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_ItemInfo_Bottom"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/ItemInfo/T_BG_ItemInfo_Bottom_%d.png"), 6)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Collect_Line"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Common/T_Img_MemoryCollect_Line.png"), 1)), E_FAIL);

	//UI 객체
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"),
		CUI_Atlas_Icon::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"),
		CUI_BackGround::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Default_Tex"),
		CUI_Default_Tex::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Guide_Icon"),
		CUI_Guide_Icon::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Default_Button"),
		CUI_Default_Button::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_TextBox"),
		CUI_TextBox::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Gague_Tip"),
		CUI_Gague_Tip::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_HUD"),
		CUI_HUD::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Inven"),
		CUI_Inven::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Item"),
		CItem_Slot::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);
	
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_MainMenu"),
		CUI_MainMenu::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BladeNexus"),
		CUI_BladeNexus::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_MainMenu_Deco"),
		CMainMune_Deco::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Cursor"),
		CCursor::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_ScreenTrail"),
		CScreenTrail::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"),
		CMeshTrail::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_LineTrail"),
		CLineTrail::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_DamageText"),
		CDamage_Text::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Mon_HP"),
		CMon_HP::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Interaction_Guide"),
		CInteraction_Guide::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);
		
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_State"),
		CUI_State::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Announce_MapName"),
		CUI_Announce_MapName::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_ItemInfo_Other"),
		CItemInfo_Other::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_ItemInfo_Weapon"),
		CItemInfo_Weapon::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BossHp"),
		CBossHp::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Amount"),
		CAmount::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Target_LockOn"),
		CTarget_LockOn::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Target_BrutalAttack"),
		CTarget_BrutalAttack::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Loading"),
		CUI_Loading::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Announce_Talk"),
		CUI_Announce_Talk::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Tutorial"),
		CUI_Tutorial::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_SkillTree"),
		CUI_SkillTree::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Skill_Info"),
        CSkill_Info::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Skill_QuickSlot"),
        CSkill_QuickSlot::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Announce_Result"),
        CUI_Announce_Result::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Announce_Over"),
        CUI_Announce_Over::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Announce_Warning"),
        CUI_Annouce_Warning::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BladeNexus_Map"),
        CUI_BladeNexus_Map::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Slot_Smoke"),
        CUI_Slot_Smoke::Create(m_pDevice, m_pContext)), E_FAIL);
    
    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_QuickSlot_Skill_fx"),
        CUI_QuickSlot_Skill_Fx::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Slot_Over_Fx"),
        CUI_Slot_Over_Fx::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Slot_Selete_Fx"),
        CUI_Slot_Selete_Fx::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldTex"),
        CUI_WorldTex::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldText"),
        CUI_WorldTextBox::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldList"),
        CUI_WorldList::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldFX"),
        CUI_WorldFX::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Talk_Daphrona"),
        CUI_Talk_Daphrona::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Talk_Dangin"),
        CUI_Talk_Dangin::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Talk_Trader"),
        CUI_Talk_Trader::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Amount_Info"),
        CAmount_Info::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Store"),
        CUI_Store::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Store_Slot"),
        CStore_Slot::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Popup_Reset"),
        CPopup_Reset::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Popup_Item"),
        CPopup_Item::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Collection_Info"),
        CCollection_Info::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_SkipButton"),
        CSkipButton::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC))), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_TalkDanjinjar"),
        CUI_Talk_Danjinjar::Create(m_pDevice, m_pContext)), E_FAIL);


	CUIObject::UIOBJECT_DESC AnnounceDesc = {};
	AnnounceDesc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	AnnounceDesc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
	AnnounceDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	AnnounceDesc.szName = "Announce";
	AnnounceDesc.fDepth = 8.f;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Announce_MapName"), TIME_CHANNEL::WORLD, &AnnounceDesc), E_FAIL);

	AnnounceDesc.vLocalSize = { 660.f, 100.f };
	AnnounceDesc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
	AnnounceDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	AnnounceDesc.szName = "Announce";
	AnnounceDesc.fDepth = 8.f;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Announce_Talk"), TIME_CHANNEL::WORLD, &AnnounceDesc), E_FAIL);

    AnnounceDesc.vLocalSize = { 660.f, 100.f };
    AnnounceDesc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
    AnnounceDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AnnounceDesc.szName = "Announce";
    AnnounceDesc.fDepth = 1.f;

    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"),
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Announce_Warning"), TIME_CHANNEL::WORLD, &AnnounceDesc), E_FAIL);

 
	CUIObject::UIOBJECT_DESC Desc = {};
	Desc.vLocalSize = { 64.f, 64.f };
	Desc.vLocalPos = { 0.f, 0.f };
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "DamageText";
	Desc.fDepth = 7.9f;

	CHECK_FAILED(m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_DamageText"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text"), &Desc, 40), E_FAIL);

	Desc.vLocalSize = { 106.f, 18.f };
	Desc.vLocalPos = { 0.f, 0.f };
	Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
	Desc.szName = "MonHP";
	Desc.fDepth = 9.f;

	CHECK_FAILED(m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Mon_HP"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP"), &Desc, 60), E_FAIL);

	Desc.vLocalSize = { 48.f, 48.f };
	Desc.vLocalPos = { 0.f, 0.f };
	Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
	Desc.szName = "KeyGuide";
	Desc.fDepth = 8.f;

	CHECK_FAILED(m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Interaction_Guide"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide"), &Desc, 50), E_FAIL);

	Desc.vLocalSize = { 52.f, 52.f };
	Desc.vLocalPos = { 0.f, 0.f };
	Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
	Desc.szName = "BrutalAttack";
	Desc.fDepth = 9.f;

	CHECK_FAILED(m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Target_BrutalAttack"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack"), &Desc, 10), E_FAIL);

	CUIObject::UIOBJECT_DESC LockOnDesc = {};
	LockOnDesc.vLocalSize = { 84.f, 84.f };
	LockOnDesc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
	LockOnDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	LockOnDesc.szName = "LockOn";
	LockOnDesc.fDepth = 9.f;

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Target_LockOn"), TIME_CHANNEL::WORLD, &LockOnDesc), E_FAIL);

	if (FAILED(CClientInstance::GetInstance()->Load_UIData(ENUM_CLASS(LEVEL::STATIC), TEXT("Layer_UI"), ENUM_CLASS(LEVEL::STATIC),
		TEXT("../Bin/Resources/UI/UIData/Loading.json"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForStatic_Effect()
{
	/* Prototype_Component_Shader_VtxInstance_PointParticle*/
	// 이거 혹시 다른사람 쓰는 지 확인 필요
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_PointParticle.hlsl"), VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
	//	return E_FAIL;

	/* Prototype_Component_Shader_VtxInstance_MeshParticle*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Particle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Particle.hlsl"), VTXPARTICLE::Elements, VTXPARTICLE::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_LineTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_LineTrail"),
		CVIBuffer_LineTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_QuadTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_QuadTrail"),
		CVIBuffer_QuadTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Texture_Trail */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Slash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/SowardTrailTexture/Slash_%d.png"), 47)), E_FAIL);

	// Prototype_Component_Texture_TestParticle
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Particle_Prototype"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/particle/particle%d.png"), 12)), E_FAIL);

	// Prototype_Component_Texture_TestSpriteImage
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Sprite_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Sprite/Sprite%d.png"), 12)), E_FAIL);

	// Prototype_Component_Texture_MeshEffect(Masking)
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MeshEffect_Masking"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Mask/Masking%d.png"), 15)), E_FAIL);

	// Prototype_Component_Texture_MeshEffect(Dissolve)
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MeshEffect_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Dissolve/Dissolve%d.png"), 6))))
		return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_MeshEffect_Normal"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Normal/Normal%d.png"), 3))))
        return E_FAIL; 

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	CHECK_FAILED(m_pGameInstance->Font_Load_Data("../Bin/Data/Font/FontData.json"), E_FAIL);
	return S_OK;
}

HRESULT CMainApp::Ready_DB()
{
	CHECK_FAILED(m_pClientInstance->Load_Data(DATATYPE::ITEM, TEXT("../Bin/Data/DB/Item_DB.csv")),E_FAIL);
	CHECK_FAILED(m_pClientInstance->Load_Data(DATATYPE::EQUIPEFFECT, TEXT("../Bin/Data/DB/EquipItem_DB.csv")), E_FAIL);
	CHECK_FAILED(m_pClientInstance->Load_Data(DATATYPE::OTHEREFFECT, TEXT("../Bin/Data/DB/OtherItem_DB.csv")), E_FAIL);
	CHECK_FAILED(m_pClientInstance->Load_Data(DATATYPE::STATE, TEXT("../Bin/Data/DB/State_DB.csv")), E_FAIL);
	CHECK_FAILED(m_pClientInstance->Load_Data(DATATYPE::ANNOUNCE_TALK, TEXT("../Bin/Data/DB/Announce_Talk_DB.csv")), E_FAIL);
	CHECK_FAILED(m_pClientInstance->Load_Data(DATATYPE::SKill, TEXT("../Bin/Data/DB/Skill_DB.csv")), E_FAIL);
    CHECK_FAILED(m_pClientInstance->Load_Data(DATATYPE::DANJINJAL, TEXT("../Bin/Data/DB/Danjinjar_DB.csv")), E_FAIL);

	return S_OK;
}



HRESULT CMainApp::Ready_ClientInstance(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pClientInstance = CClientInstance::GetInstance();

	m_pClientInstance->Initialize(ppDevice, ppContext);

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_DebugTool()
{

		
	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	m_pClientInstance->Release_Client();

	Safe_Release(m_pClientInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}
