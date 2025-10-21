#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "HeaderGroup_UI.h"

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
	EngineDesc.Menu_Imgui = Imgui_Menu;
	
	//MakeSpriteFont "넥슨Lv1고딕 Bold" "153.SpriteFont"
	/*MakeSpriteFont "넥슨Lv1고딕 Bold" /FontSize:20 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 153ex.spritefont */

	if(FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_ForStatic()))
		return E_FAIL;

	if (Ready_Prototype_ForStatic_UI())
		return E_FAIL;

	CHECK_FAILED(Ready_ClientInstance(&m_pDevice, &m_pContext), E_FAIL);

	if (FAILED(Start_Level(LEVEL::TITLE)))
		return E_FAIL;

	if (FAILED(Ready_ObjectLayer()))
		return E_FAIL;
	CHECK_FAILED(Ready_DB(), E_FAIL);
	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
	m_pClientInstance->Update(fTimeDelta);

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif
}

HRESULT CMainApp::Render()
{
	_float4		vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

#ifdef _DEBUG
	++m_iRenderCount;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS:%d"), m_iRenderCount);
		m_fTimeAcc = 0.f;
		m_iRenderCount = 0;
	}
	m_pGameInstance->DrawText(TEXT("Font_153"), m_szFPS, _float2(100.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
#endif

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForStatic()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_153"), TEXT("../Bin/Resources/Font/153ex.SpriteFont"))))
		return E_FAIL;

	// VIBuffer

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
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

	/* Prototype_Component_Shader_VtxInstance_Point_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Point_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point_UI.hlsl"), UI_INSTANCING::Elements, UI_INSTANCING::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_UI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex_UIMask*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Mask"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_UIMask.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_RigidBody*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* Prototype_Component_CharacterVirtual*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
		CCharacterVirtual::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* Prototype_Component_Body*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
		CBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_ForStatic_UI()
{
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"),
		CUI_Atlas_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"),
		CUI_BackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Gague_Tip"),
		CUI_Gague_Tip::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_HUD"),
		CUI_HUD::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC)))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Inven"),
		CUI_Inven::Create(m_pDevice, m_pContext, ENUM_CLASS(LEVEL::STATIC)))))
		return E_FAIL;

	//플레이어 HUD 텍스처
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hud_HPGauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Hud/State/T_Hud_Gauge_HP_0%d.png"), 7))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hud_HPGauge_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Hud/State/T_Hud_Gauge_HP_BG_0%d.png"), 10))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hud_Player_HPGauge_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Hud/State/T_Hud_Hp_Bg_0%d.png"), 5))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_DB()
{
	CHECK_FAILED(m_pClientInstance->Load_Data(DATATYPE::ITEM, TEXT("../Bin/Data/DB/Item_DB.csv")),E_FAIL);
	CHECK_FAILED(m_pClientInstance->Load_Data(DATATYPE::EQUIPEFFECT, TEXT("../Bin/Data/DB/EquipItem_DB.csv")), E_FAIL);
	CHECK_FAILED(m_pClientInstance->Load_Data(DATATYPE::OTHEREFFECT, TEXT("../Bin/Data/DB/OtherItem_DB.csv")), E_FAIL);
	
	_wstring test = m_pClientInstance->Get_Data<ITEM_DATA>(1010)->strName;
	
	MSG_BOX(test.c_str());
	
	return S_OK;
}

HRESULT CMainApp::Ready_ObjectLayer()
{
	// Static 지형
	m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MAP), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
	// 동적 물체
	m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
	m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
	m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::CAMERA), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

	// 동적-동적 & 동적-지형 & 동적-트리거
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MONSTER));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(COLLISION_LAYER::PLAYER));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MAP));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(COLLISION_LAYER::MAP));
	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::CAMERA), ENUM_CLASS(COLLISION_LAYER::MAP));

	// PLAYER
	m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
	m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
	// MONSTER
	m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(JOLT_BP_LAYER::NON_MOVING));
	m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

	m_pGameInstance->Set_PhysicsSystem();

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

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pClientInstance);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}
