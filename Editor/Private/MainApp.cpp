#include "MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "JOH_EditorModelTest.h"
#include "Editor_Model.h"
#include "Debug_Controller.h"

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
	EngineDesc.iWinSizeX_Imgui = g_iWinSizeX_Imgui_TYJ;
	EngineDesc.iWinSizeY_Imgui = g_iWinSizeY_Imgui_TYJ;
	EngineDesc.iNumJoltObjectLayer = ENUM_CLASS(COLLISION_LAYER::END);

	list<_wstring> Imgui_Menu;
	Imgui_Menu.push_back(TEXT("Default"));
	Imgui_Menu.push_back(TEXT("Debug"));
	Imgui_Menu.push_back(TEXT("Map"));
	Imgui_Menu.push_back(TEXT("Effect"));
	Imgui_Menu.push_back(TEXT("Model"));
	Imgui_Menu.push_back(TEXT("UI"));
	Imgui_Menu.push_back(TEXT("Shader"));
	Imgui_Menu.push_back(TEXT("Camera"));
	Imgui_Menu.push_back(TEXT("AI"));

	EngineDesc.Menu_Imgui = Imgui_Menu;

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_ForStatic()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::EDITOR)))
		return E_FAIL;

	if (FAILED(Ready_ObjectLayer()))
		return E_FAIL;
	
	Ready_DefaultImgui();

	Ready_Debug();

	return S_OK;
}

void CMainApp::Update(TIME_DELTA tTimeDelta)
{
	m_pGameInstance->Update_Engine(tTimeDelta);
	m_pDebug->Update();
#ifdef _DEBUG
	m_fTimeAcc += tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)];

#endif
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

	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Terrain/Height.bmp")))))
		return E_FAIL;

	// Shader
	/* Prototype_Component_Shader_VtxPosTex_Edit_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Edit_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex_Edit_UI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosTex_UI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosSpriteTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosSpriteTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxSphere */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxSphere"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxSphere.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxInstance_PointParticle*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxInstance_PointParticle.hlsl"), VTXPOINTPARTICLE::Elements, VTXPOINTPARTICLE::iNumElements))))
		return E_FAIL;
	
	/* Prototype_Component_Shader_VtxInstance_MeshParticle*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Particle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxInstance_Particle.hlsl"), VTXPARTICLE::Elements, VTXPARTICLE::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex_Font */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Font"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosTex_Font.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* Prototype_GameObject_Editor_Animation_TestModel */
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Editor_Animation_TestModel"),
			CJOH_EditorModelTest::Create(m_pDevice, m_pContext))))
			return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ObjectLayer()
{
	m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
	m_pGameInstance->Set_ObjectToBP(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

	m_pGameInstance->Set_ObjectFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(COLLISION_LAYER::MONSTER));

	m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::PLAYER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));
	m_pGameInstance->Set_ObjectVsBPFilter(ENUM_CLASS(COLLISION_LAYER::MONSTER), ENUM_CLASS(JOLT_BP_LAYER::MOVING));

	m_pGameInstance->Set_PhysicsSystem();

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::EDITOR), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Ready_DefaultImgui()
{
	m_pGameInstance->AddWidget(TEXT("Default"), [this]() {
		ImGui::Begin("Default");
		ImGui::Text("Select Level");
		if (ImGui::Button("Map", ImVec2(120, 32))) { 
			m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::MAP));
		}
		ImGui::SameLine();
		if (ImGui::Button("Animation", ImVec2(120, 32))) {
			m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::ANIMATION));
		}
		ImGui::SameLine();
		if (ImGui::Button("Effect", ImVec2(120, 32))) {
			m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::EFFECT));
		}
		ImGui::SameLine();
		if (ImGui::Button("UI", ImVec2(120, 32))) {
			m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::UI));
		}
		ImGui::SameLine();
		if (ImGui::Button("Shader", ImVec2(120, 32))) {
			m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::SHADER));
		}
		ImGui::SameLine();
		if (ImGui::Button("AI", ImVec2(120, 32))) {
			m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::AI));
		}
		ImGui::SameLine();
		if (ImGui::Button("Camera", ImVec2(120, 32))) {
			m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::CAMERA));
		}
		ImGui::End();
		});
}

void CMainApp::Ready_Debug()
{
	m_pDebug = CDebug_Controller::Create();
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
	Safe_Release(m_pDebug);
	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}
