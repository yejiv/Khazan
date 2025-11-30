#include "Level_Effect.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Effect.h"
#include "Effect_Point_Instance.h"
#include "Effect_Mesh_Instance.h"
#include "Effect_Sprite.h"
#include <shobjidl.h>
#include "Prop_Object.h"


#include "Edit_Interface_UI.h"

CLevel_Effect::CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Effect::Initialize()
{
    //if (FAILED(Ready_Layer_BackGround()))
    //    return E_FAIL;

    if (FAILED(Ready_Layer_Camera()))
        return E_FAIL;

    if (FAILED(Ready_Layer_GameObject()))
        return E_FAIL;

	strcpy_s(FilePath, sizeof(FilePath), "../../Client/Bin/Data/Effect/Baked/");
	strcpy_s(SaveFileName, sizeof(SaveFileName), "../../Client/Bin/Data/Effect/Baked/");
	strcpy_s(LoadFileName, sizeof(LoadFileName), "../../Client/Bin/Data/Effect/Baked/");
	Init_GUI();

    LIGHT_DESC LightDesc = {};
    LightDesc.eType = LIGHT_DESC::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    if (FAILED(m_pGameInstance->Add_Light(TEXT("default"), ENUM_CLASS(LEVEL::EFFECT), LightDesc)))
        return E_FAIL;

    //m_pGameInstance->Set_EnableShadow(false);
    //m_pGameInstance->Set_EnableFog(false);
    //m_pGameInstance->Set_EnableSSAO(false);

	return S_OK;
}

void CLevel_Effect::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	m_PrefabPrototype->Priority_Update(fTimeDelta);
	m_PrefabPrototype->Update(fTimeDelta);
	m_PrefabPrototype->Late_Update(fTimeDelta);

	return;
}

HRESULT CLevel_Effect::Render()
{
	++m_iRenderCount;
	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("Effect Tool :: FPS %d"), m_iRenderCount);
		m_fTimeAcc = 0.f;
		m_iRenderCount = 0;
	}
	SetWindowText(g_hWnd, m_szFPS);

	return S_OK;
}

void CLevel_Effect::Init_GUI()
{
	m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
		{
			ImGui::Begin("[Edit Prefab]");

			//1 Info
			ImGui::BeginChild("PrefabPanel", ImVec2(440, 120), true);
			Prefab_Info();
			ImGui::EndChild();

			ImGui::SameLine();

			//2. Save
			ImGui::BeginChild("Save/LoadPanel", ImVec2(0, 120), true);
			Save_Load();
			ImGui::EndChild();

			//3 Create
			ImGui::BeginChild("CreateElementPanel", ImVec2(440, 0), true);
			Create_Element();
			ImGui::EndChild();

			ImGui::SameLine();
			//4 list
			ImGui::BeginChild("EditElementPanel", ImVec2(440, 0), true);
			Edit_Element_List();
			ImGui::EndChild();

			ImGui::SameLine();
			//5 timetrack
			ImGui::BeginChild("TimeTrackPanel", ImVec2(0, 0), true);
			Edit_Time_Track();
			ImGui::EndChild();

			ImGui::End();
		});
}

void CLevel_Effect::Prefab_Info()
{
	ImGui::Text("[Effect Prefab]");
	ImGui::SameLine();
	ImGui::Separator();

	_float time = m_PrefabPrototype->Get_Time();
	ImGui::SliderFloat("TimeTrack", &time, 0.f, m_PrefabPrototype->Get_MaxTrack());

	if (ImGui::Button("Start"))
		m_PrefabPrototype->ResetChildren();

	ImGui::SameLine();

	if (ImGui::Button("Stop"))
        m_PrefabPrototype->Stop();

	ImGui::SameLine();

	m_PrefabPrototype->Setting_Loop();
}

void CLevel_Effect::Create_Element()
{
	ImGui::Text("[Create Effect Element]");

	if (ImGui::BeginTabBar("EffectType"))
	{
		if (ImGui::BeginTabItem("Point Effect"))
		{
			ImGui::Checkbox("Element Loop", &m_bLoop);
			ImGui::Text("Spawn Type");
			ImGui::SameLine();
			ImGui::Separator();
			ImGui::RadioButton("Spawn_BoundingBox", &m_SpawnType, 0);
			ImGui::RadioButton("Spawn_Circle", &m_SpawnType, 1);
			ImGui::Separator();

			if (m_SpawnType == 0)
				Create_Box_Spawn();
			else if (m_SpawnType == 1)
				Create_Circle_Spawn();

			ImGui::InputScalar("Instance Num : ", ImGuiDataType_U32, &m_iInstanceNum);
			ImGui::InputFloat2("LifeTime : ", m_fLifeTime);
			ImGui::InputFloat2("Scrolling Speed : ", reinterpret_cast<_float*>(&m_fScrollSpeed));
			ImGui::InputFloat2("Size : ", m_fSize);
			ImGui::InputFloat("Size Ratio : ", &m_fSizeRatio);
			GetMaksingScrollData();
			ImGui::Checkbox("Turbulence", &m_bIsTurbulence);

			if (m_bIsTurbulence)
			{
				ImGui::Indent();
				const char* MaskTexture[] = { "texture0", "texture1", "texture2",  "texture3"};
				ImGui::Combo("Turbulence Textures", reinterpret_cast<int*>(&m_iTurbulenceTextureIdx), MaskTexture, IM_ARRAYSIZE(MaskTexture));
				ImGui::InputFloat("Turbulence Speed : ", &m_fTurbulenceSpeed);
				ImGui::InputFloat("Turbulence Sample Size: ", &m_fTurbulenceSampleSize);
				ImGui::Unindent();
			}

			ImGui::InputFloat("Sprite Speed : ", reinterpret_cast<_float*>(&m_fSpriteSpeed));
			ImGui::InputInt("Col : ", reinterpret_cast<int*>(&m_iCol));
			ImGui::InputInt("Row : ", reinterpret_cast<int*>(&m_iRow));

			ImGui::Checkbox("Dissolve", &m_bIsDissolve);
			if (m_bIsDissolve)
			{
                const char* DissolveTex[] = { "DissolveTexture0", "DissolveTexture1", "DissolveTexture2", "DissolveTexture3","DissolveTexture4", "Normal0", "Normal1", "RockNormal" };
                ImGui::Combo("Dissolve Texture", reinterpret_cast<int*>(&m_iDissolveTextureIdx), DissolveTex, IM_ARRAYSIZE(DissolveTex));
				ImGui::InputFloat("Dissolve Edge Width : ", reinterpret_cast<_float*>(&m_fDissolveEdgeWidth));
				ImGui::ColorEdit4("Edge Color", (float*)&m_fDissolveEdgeColor);
			}

			m_EffectType = 0;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Mesh Effect"))
		{
			ImGui::Checkbox("Element Loop", &m_bLoop);
			ImGui::Text("Spwan Type");
			ImGui::SameLine();
			ImGui::Separator();
			ImGui::RadioButton("Spawn_BoundingBox", &m_SpawnType, 0);
			ImGui::RadioButton("Spawn_Circle", &m_SpawnType, 1);
			ImGui::Separator();

			if (m_SpawnType == 0)
				Create_Box_Spawn();
			else if (m_SpawnType == 1)
				Create_Circle_Spawn();
			
			ImGui::InputScalar("Instance Num : ", ImGuiDataType_U32, &m_iInstanceNum);
			ImGui::InputFloat2("LifeTime : ", m_fLifeTime);
			ImGui::InputFloat2("Scrolling Speed : ", reinterpret_cast<_float*>(&m_fScrollSpeed));
			ImGui::InputFloat2("Size : ", m_fSize);
			ImGui::InputFloat("Size Ratio : ", &m_fSizeRatio);
			ImGui::InputFloat3("Rotation : ", m_fRotation);
			GetMaksingScrollData();

			ImGui::Checkbox("Turbulence", &m_bIsTurbulence);

			if (m_bIsTurbulence)
			{
				ImGui::Indent();
				const char* MaskTexture[] = { "texture0", "texture1", "texture2",  "texture3" };
				ImGui::Combo("Turbulence Textures", reinterpret_cast<int*>(&m_iTurbulenceTextureIdx), MaskTexture, IM_ARRAYSIZE(MaskTexture));
				ImGui::InputFloat("Turbulence Speed : ", &m_fTurbulenceSpeed);
				ImGui::InputFloat("Turbulence Sample Size: ", &m_fTurbulenceSampleSize);
				ImGui::Unindent();
			}

			ImGui::Checkbox("Fresnel", &m_bIsFresnel);
			ImGui::Checkbox("Dissolve", &m_bIsDissolve);
			if (m_bIsDissolve)
			{
				ImGui::Indent();
                const char* DissolveTex[] = { "DissolveTexture0", "DissolveTexture1", "DissolveTexture2", "DissolveTexture3","DissolveTexture4", "Normal0", "Normal1", "RockNormal" };
                ImGui::Combo("Dissolve Texture", reinterpret_cast<int*>(&m_iDissolveTextureIdx), DissolveTex, IM_ARRAYSIZE(DissolveTex));
				ImGui::InputFloat("Dissolve Edge Width : ", reinterpret_cast<_float*>(&m_fDissolveEdgeWidth));
				ImGui::ColorEdit4("Edge Color", (float*)&m_fDissolveEdgeColor);
				ImGui::Unindent();
			}
			m_EffectType = 1;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Sprite Effect"))
		{
            const char* textures[] = { "test0", "test1", "test2",  "test3",  "flare", "Fire_Sprite", "Fire_Sprite2",  "flare", "blood" };
            ImGui::Combo("Textures", reinterpret_cast<int*>(&m_iTextureIdx), textures, IM_ARRAYSIZE(textures));

			ImGui::InputFloat("Sprite Speed : ", reinterpret_cast<_float*>(&m_fSpriteSpeed));
			ImGui::InputInt("Col : ", reinterpret_cast<int*>(&m_iCol));
			ImGui::InputInt("Row : ", reinterpret_cast<int*>(&m_iRow));
			ImGui::Checkbox("Sprite Loop", &m_bLoop);
			ImGui::InputFloat2("Size : ", m_fSize);
			ImGui::InputFloat("Size Ratio : ", &m_fSizeRatio);
			GetMaksingScrollData();
			m_EffectType = 2;
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	GetParticleColor();

	if (ImGui::Button("Create"))
	{
		switch (m_EffectType)
		{
		case ENUM_CLASS(CEffect_Prefab::EffectType::POINT_INSTANCE):
			Create_PointInstance_Element();
			break;

		case ENUM_CLASS(CEffect_Prefab::EffectType::MESH_INSTANCE):
			Create_MeshInstance_Element();
			break;

		case ENUM_CLASS(CEffect_Prefab::EffectType::SPRITE):
			Create_Sprite_Element();
			break;

		default:
			MSG_BOX(TEXT("Effect Type Error"));
		}
	}
}

void CLevel_Effect::Edit_Element_List()
{
	ImGui::Text("[Edit Effect Elements]");
	ImGui::SameLine();
	ImGui::Separator();

	vector<string> Names;
	vector<const char*> cstr_Names;

	_uint size = m_PrefabPrototype->Get_ChildrenSize();

	for (_uint i = 0; i < size; ++i)
		Names.push_back("Element" + to_string(i));

	for (const auto& name : Names)
		cstr_Names.push_back(name.c_str());

	// 제목 쓰기

	if (cstr_Names.size() > 0)
	{
		ImGui::ListBox("Effect Elements", &m_iChildrenIdx, cstr_Names.data(), (_int)cstr_Names.size());
		if (m_iPrevChildrenIdx != m_iChildrenIdx)
			m_PrefabPrototype->RevertChanges(m_iPrevChildrenIdx);
		m_PrefabPrototype->Edit_Element(m_iChildrenIdx);
		ImGui::Separator();
		if (ImGui::Button("Delete"))
		{
			m_PrefabPrototype->RemoveEffect(m_iChildrenIdx);
			m_iChildrenIdx = 0;
		}
		m_iPrevChildrenIdx = m_iChildrenIdx;
	}
}

void CLevel_Effect::Edit_Time_Track()
{
	ImGui::Text("[Edit Time Track]");
	ImGui::SameLine();
	ImGui::Separator();

	if (ImGui::Button("Add TimeTrack", ImVec2(300, 30)))
	{
		CEffect_Prefab::EFFECT_EVENT newTimeTrack {};
		newTimeTrack.iElementIdx = m_iChildrenIdx;
		m_PrefabPrototype->Add_TimeTrack(newTimeTrack);
	}

	m_PrefabPrototype->Edit_TimeTrack(m_iChildrenIdx);
}

void CLevel_Effect::Save_Load()
{
	ImGui::Text("[Save / Load]");
	ImGui::SameLine();
	ImGui::Separator();
	
	if (ImGui::Button("Select File Path....", ImVec2(200, 0))) 
		SelectFilePath();
	
	ImGui::InputText("Save Path : ", SaveFileName, IM_ARRAYSIZE(SaveFileName));
	ImGui::SameLine();
	if (ImGui::Button("Save"))
		m_PrefabPrototype->Save(SaveFileName);
	
	ImGui::InputText("Load Path : ", LoadFileName, IM_ARRAYSIZE(LoadFileName));
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		m_PrefabPrototype->Load(LoadFileName);
}

void CLevel_Effect::Create_Box_Spawn()
{
	ImGui::InputFloat3("Center : ", m_fCenter);
	ImGui::InputFloat3("Range : ", m_fRange);
}

void CLevel_Effect::Create_Circle_Spawn()
{
	ImGui::InputFloat("Circle Offset  : ", &m_fOffset);
}

void CLevel_Effect::GetParticleColor()
{
	static _int prevIdx = 0;

	ImGui::Text("Color");
	ImGui::SameLine();
	ImGui::Separator();

	ImGui::ColorEdit4("MyColorWithAlpha", (float*)&m_fColor);

	if (prevIdx != m_EffectType)
		m_iTextureIdx = 0;

	if (m_EffectType == 0)
	{
		const char* textures[] = { "test0", "test1", "test2",  "test3",  "flare", "Fire_Sprite", "Fire_Sprite2",  "flare",  "blood", "BloomParticle"};
		ImGui::Combo("Point Particles Textures", reinterpret_cast<int*>(&m_iTextureIdx), textures, IM_ARRAYSIZE(textures));
	}
	else if (m_EffectType == 1)
	{
		const char* textures[] = { "test0", "test1", "test2",  "test3",  "test4",  "test5",  "test6" ,  "test7" ,  "test8" ,  "test9" ,  "test10" ,  "test11" ,  "test12",  "test13",  "test14",  "test15",  "test16",  "test17",  "test18",  "test19",  "test20",
            "shock", "smoke", "cloud", "blood", "Ice1", "Ice2", "Rock", "Slash28", "Slash29" , "Slash30" , "Slash31", "Slash32", "Slash33" };

		ImGui::Combo("Mesh Textures", reinterpret_cast<int*>(&m_iTextureIdx), textures, IM_ARRAYSIZE(textures));

		const char* Meshes[] = { "Helix0", "Helix1", "Helix2", "Helix3",  "Helix4",  "Helix5",  "Helix6",  "Helix7",  "Helix8",  "Helix9",  "Helix10",  "Helix11",  "Helix12",  "Helix13",  "Helix14",  "Helix15",  "Helix16",  "Helix17",  "Helix18",  "Helix19",  "Helix20",  
									"Spline0",  "Spline1", "Spline2",  "Spline3", "Spline4",  "Spline5", 
									"Spline_Cylinder",  "Spline_Helix0", "Spline_Helix1",  
									"Spline_Twist0", "Spline_Twist1",  "Spline_Twist2", "Spline_Twist3",  "Spline_Twist4", "Spline_Twist5",  "Spline_Twist6", "Spline_Twist7", 
									"TombStone_Spline0", "TombStone_Spline1", "TombStone_Spline2", "TombStone_Spline3", "WorldSpline",
									"Helix21", "absolb_Spiral", "Focus", "HelixSimple", "Lightning0", "Lightning1", "Lightning2" , "Lightning3", "Helix22", "Helix23", "Helix24", "Helix25", "Helix26",
									"Trail", "TwistRing0", "TwistRing1", "Spiral", "Wave0", "Wave1", "windLine", "Mash_Helix_Simple_mid", "T_blunt_slash_test", "Mash_Helix", "Mash_circle_twist",
									"Attack_ShockWave", "AttackTrail_First", "AttackTrail_Fluid", "AttackTrail_Spin", "AttackTrail_Wind", "Curve0", "Curve1" , "Curve2", "Helix27",
									"Lupers_Spear", "Particle_001", "ShockWave_Flow", "Spiral_001", "Wind_001", 
									"IN_Spiral_02", "Swirl_Spine_X", "SwirlHelix",
									"FastAtk_1", "FastAtk_2L", "FastAtk_2R", "FastAtk_3L", "FastAtk_3R", "Grapple_Atk_2", "CounterATK", "DodgeATK", "FastATK1","FastATK2_L", "FastATK2_R",  "FastATK3_L" ,  "FastATK3_R", "FastATK4",
									"StrongAtk0", "StrongAtk1", "FastAtk03_Slash", "GrappleAtk02_Slash", "StrongAtk03_Slash"
									, "Cylinder_003", "Cylinder_003_02", "Cylinder_003_Noise" , "Spine", "Circle_002" ,"Sphere","CircleTwist", "CircleTwist2", "Plane" , "circle001", "circle002", "Ice",  "Rock", "Crystal" };
		ImGui::Combo("Mesh Shape", reinterpret_cast<int*>(&m_iMeshTypeIdx), Meshes, IM_ARRAYSIZE(Meshes));
	}

	prevIdx = m_EffectType;
}

void CLevel_Effect::GetMaksingScrollData()
{
	ImGui::Checkbox("Do Mask Scrolling", &m_bIsMaskScrolling);

	if (m_bIsMaskScrolling)
	{
		ImGui::Indent();
		const char* MaskTexture[] = { "width0", "width1", "width2",  "width3",  "width4",  "width5",  "width6", "width7", "length0" ,  "length1",  "length2" ,  "length3",  "length4" ,  "length5",  "length6" };
		ImGui::Combo("Mask Textures", reinterpret_cast<int*>(&m_iMaskTextureIdx), MaskTexture, IM_ARRAYSIZE(MaskTexture));
		ImGui::InputFloat("Mask Scroll Speed: ", &m_bMaskScrollSpeed);
		ImGui::Checkbox("Is Vecrtical", &m_bIsScrollVertical);
		ImGui::Checkbox("Is Inverse Direction", &m_bScrollDir);
		ImGui::Unindent();
	}
	else
		m_bMaskScrollSpeed = 0.f;
}

void CLevel_Effect::Create_PointInstance_Element()
{
	CEffect_Point_Instance::PARTICLE_DESC data;

	data.IsCircle = m_SpawnType;
	data.iNumInstance = m_iInstanceNum;
	data.vSize = _float2(m_fSize[0], m_fSize[1]);
	data.fSizeRatio = m_fSizeRatio;
	data.vLifeTime = _float2(m_fLifeTime[0], m_fLifeTime[1]);
	data.vCenter = _float3(m_fCenter[0], m_fCenter[1], m_fCenter[2]);
	data.vRange = _float3(m_fRange[0], m_fRange[1], m_fRange[2]);
	data.fOffset = m_fOffset;
	data.vColor = m_fColor;
	data.iTextureIdx = m_iTextureIdx;
	data.iScrollSpeed = m_fScrollSpeed;
	data.bIsLoop = m_bLoop;

	data.iMaskTextureIdx = m_iMaskTextureIdx;
	data.fMaskScrollSpeed = m_bMaskScrollSpeed;
	data.bIsScrollVertical = m_bIsScrollVertical;
	data.bIsScrollInverse = m_bScrollDir;

	data.bIsTurbulence = m_bIsTurbulence;
	data.fTurbulenceSpeed = m_fTurbulenceSpeed;
	data.fTurbulenceSampleSize = m_fTurbulenceSampleSize;
	data.iTurbulenceTextureIdx = m_iTurbulenceTextureIdx;

	data.iCol = m_iCol;
	data.iRow = m_iRow;
	data.fSpriteSpeed = m_fSpriteSpeed;

	data.sDissolveData.bIsDissolve = m_bIsDissolve;
	data.sDissolveData.fDissolveEdgeColor = m_fDissolveEdgeColor;
	data.sDissolveData.fDissolveEdgeWidth = m_fDissolveEdgeWidth;
	data.sDissolveData.iDissolveTextureIdx = m_iDissolveTextureIdx;

	m_PrefabPrototype->Add_Effect_Element(m_EffectType, &data);
}

void CLevel_Effect::Create_MeshInstance_Element()
{
	CEffect_Mesh_Instance::PARTICLE_DESC data;
	
	data.IsCircle = m_SpawnType;
	data.iNumInstance = m_iInstanceNum;
	data.vSize = _float2(m_fSize[0], m_fSize[1]);
	data.fSizeRatio = m_fSizeRatio;
	data.vLifeTime = _float2(m_fLifeTime[0], m_fLifeTime[1]);
	data.vCenter = _float3(m_fCenter[0], m_fCenter[1], m_fCenter[2]);
	data.vRange = _float3(m_fRange[0], m_fRange[1], m_fRange[2]);
	data.fOffset = m_fOffset;
	data.vColor = m_fColor;
	data.iTextureIdx = m_iTextureIdx;
	data.iMeshTypeIdx = m_iMeshTypeIdx;
	data.iScrollSpeed = m_fScrollSpeed;
	data.bIsLoop = m_bLoop;
	data.bIsScrollVertical = m_bIsScrollVertical;
	data.iMaskTextureIdx = m_iMaskTextureIdx;
	data.fMaskScrollSpeed = m_bMaskScrollSpeed;
	data.bIsScrollVertical = m_bIsScrollVertical;
	data.bIsScrollInverse = m_bScrollDir;
	data.bIsFresnel = m_bIsFresnel;
	data.sDissolveData.bIsDissolve = m_bIsDissolve;
	data.sDissolveData.fDissolveEdgeColor = m_fDissolveEdgeColor;
	data.sDissolveData.fDissolveEdgeWidth = m_fDissolveEdgeWidth;
	data.sDissolveData.iDissolveTextureIdx = m_iDissolveTextureIdx;

	data.bIsTurbulence = m_bIsTurbulence;
	data.fTurbulenceSpeed = m_fTurbulenceSpeed;
	data.fTurbulenceSampleSize = m_fTurbulenceSampleSize;
	data.iTurbulenceTextureIdx = m_iTurbulenceTextureIdx;

	data.fRotation = _float3(m_fRotation[0], m_fRotation[1], m_fRotation[2]);

	m_PrefabPrototype->Add_Effect_Element(m_EffectType, &data);
}

void CLevel_Effect::Create_Sprite_Element()
{
	CEffect_Sprite::SPRITE_DESC data;

	data.IsLoop = m_bLoop;
	data.iCol = m_iCol;
	data.iRow = m_iRow;
	data.fSpriteSpeed = m_fSpriteSpeed;
	data.vColor = m_fColor;
	data.fSize = m_fSize[0];
	data.fSizeRatio = m_fSizeRatio;
	data.ScalingValue = 1.f; //tmp;
	data.iTextureIdx = m_iTextureIdx;
	data.iMaskTextureIdx = m_iMaskTextureIdx;
	data.fMaskScrollSpeed = m_bMaskScrollSpeed;
	data.bIsScrollVertical = m_bIsScrollVertical;
	data.bIsScrollInverse = m_bScrollDir;

	m_PrefabPrototype->Add_Effect_Element(m_EffectType, &data);
}

void CLevel_Effect::SelectFilePath()
{
	_char savedDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, savedDir);

	IFileOpenDialog* pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

	if (SUCCEEDED(hr))
	{
		DWORD dwOptions;
		pfd->GetOptions(&dwOptions);
		pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);

		if (SUCCEEDED(pfd->Show(NULL)))
		{
			IShellItem* psiResult;
			if (SUCCEEDED(pfd->GetResult(&psiResult)))
			{
				PWSTR pszPath = NULL;
				if (SUCCEEDED(psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)))
				{
					char exeDirPath[MAX_PATH] = { 0 };
					GetModuleFileNameA(NULL, exeDirPath, MAX_PATH);
					std::filesystem::path relativePath = std::filesystem::relative(pszPath, exeDirPath);
					strcpy_s(FilePath, sizeof(FilePath), relativePath.string().c_str());
					strcpy_s(SaveFileName, sizeof(SaveFileName), FilePath);
					strcpy_s(LoadFileName, sizeof(LoadFileName), FilePath);

					CoTaskMemFree(pszPath); // 메모리 해제
				}
				psiResult->Release();
			}
		}
		pfd->Release();
	}

	SetCurrentDirectoryA(savedDir);
}

HRESULT CLevel_Effect::Ready_Layer_BackGround()
{
    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Effect_Terrain"),
        ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Effect_Terrain")), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Camera()
{
    CCamera_Effect::CAMERA_EFFECT_DESC Desc{};

    Desc.vEye = _float4(0.f, 20.f, -20.f, 1.f);
    Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.fFovy = XMConvertToRadians(60.0f);
    Desc.fNear = 0.1f;
    Desc.fFar = 1000.f;
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
    Desc.fMouseSensor = 0.1f;

    CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Camera"),
        ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Camera_Effect"), TIME_CHANNEL::WORLD, &Desc), E_FAIL);

    return S_OK;

}

HRESULT CLevel_Effect::Ready_Layer_GameObject()
{
    m_PrefabPrototype = CEffect_Prefab::Create(m_pDevice, m_pContext);   


    CHECK_FAILED(Ready_Lights(TEXT("Test"), LEVEL::EFFECT), E_FAIL);

    CHECK_FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"), TEXT("Test"), LEVEL::EFFECT), E_FAIL);

    CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Layer_MapObject_Inst"), TEXT("Test"), LEVEL::EFFECT), E_FAIL);

    return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_MapObject(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

    pDataFilePath += pDataFileName;

    // 동일한 파일명의 _objects.dat 불러오기
    pDataFilePath += TEXT("_object.dat");

    DWORD dwByte = {};

    HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("데이터 파일이 없거나 박준영 문제"), E_FAIL);

    // 1. 오브젝트의 총 개수
    _uint iObjectCnt = {};
    CHECK_FALSE(ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

    // 오브젝트 총 개수만큼 순회
    for (_uint i = 0; i < iObjectCnt; ++i)
    {
        CProp_Object::PROP_OBJECT_DESC ObjectDesc = {};

        ObjectDesc.eLevel = eCurrentLevel;

        // 2. 프로토 타입 태그 길이 불러오기
        _uint iPrototypeTagLen = {};
        CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

        // 3. 프로토 타입 태그 이름 불러오기
        _tchar szPrototypeTag[MAX_PATH] = {};
        CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

        // 불러온 태그 카피
        memcpy(ObjectDesc.szModelName, szPrototypeTag, sizeof(ObjectDesc.szModelName));

        // 4. 객체당 월드 행렬 때오기
        _float4x4 WorldMatrix = {};
        CHECK_FALSE(ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr), E_FAIL);

        ObjectDesc.WorldMatrix = WorldMatrix;

        // 5. 객체의 속성 불러오기
        MAPOBJECT_PROPERTIES PropProperties = {};
        CHECK_FALSE(ReadFile(hFile, &PropProperties, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr), false);

        ObjectDesc.Properties = PropProperties;

        // 일단 단일 오브젝트로 배치하고 추후에 인스턴스, 인터렉티브, 다이나믹 으로 나누겠습니다.
        //	m_pGameInstance->Add_FireTask([this, objDesc = ObjectDesc, curLevel = eCurrentLevel]() mutable {
        //		});

        CHECK_FAILED(
            m_pGameInstance->Add_GameObject_ToLayer(
                ENUM_CLASS(ObjectDesc.eLevel),
                TEXT("Layer_MapObject"),
                ENUM_CLASS(eCurrentLevel),
                TEXT("Prototype_GameObject_Prop_Object"),
                TIME_CHANNEL::WORLD,
                &ObjectDesc // 캡처된 값의 주소 -> 안전
            ),
            E_FAIL
        );

        /*CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(eCurrentLevel), strLayerTag,
            ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_Object"), &ObjectDesc), E_FAIL);*/
    }

    return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

    pDataFilePath += pDataFileName;

    // 동일한 파일명의 _inst.dat 불러오기
    pDataFilePath += TEXT("_inst.dat");

    DWORD dwByte = {};

    HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("데이터 파일이 없거나 박준영 문제"), E_FAIL);

    // 1. 오브젝트의 총 개수
    _uint iObjectCnt = {};
    CHECK_FALSE(ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

    // 오브젝트 총 개수만큼 순회
    for (_uint i = 0; i < iObjectCnt; ++i)
    {
        CProp_Object::PROP_OBJECT_DESC ObjectDesc = {};

        ObjectDesc.eLevel = eCurrentLevel;

        // 2. 프로토 타입 태그 길이 불러오기
        _uint iPrototypeTagLen = {};
        CHECK_FALSE(ReadFile(hFile, &iPrototypeTagLen, sizeof(_uint), &dwByte, nullptr), E_FAIL);

        // 3. 프로토 타입 태그 이름 불러오기
        _tchar szPrototypeTag[MAX_PATH] = {};
        CHECK_FALSE(ReadFile(hFile, &szPrototypeTag, sizeof(_tchar) * iPrototypeTagLen, &dwByte, nullptr), E_FAIL);

        // 불러온 태그 카피
        memcpy(ObjectDesc.szModelName, szPrototypeTag, sizeof(ObjectDesc.szModelName));

        // 4. 객체의 속성 불러오기
        MAPOBJECT_PROPERTIES PropProperties = {};
        CHECK_FALSE(ReadFile(hFile, &PropProperties, sizeof(MAPOBJECT_PROPERTIES), &dwByte, nullptr), false);

        ObjectDesc.Properties = PropProperties;

        // 인스턴스 객체 슈웃
        m_pGameInstance->Add_FireTask([this, objDesc = ObjectDesc, curLevel = eCurrentLevel]() mutable {
            CHECK_FAILED(
                m_pGameInstance->Add_GameObject_ToLayer(
                    ENUM_CLASS(objDesc.eLevel),
                    TEXT("Layer_MapObject_Inst"),
                    ENUM_CLASS(curLevel),
                    TEXT("Prototype_GameObject_Prop_Static"),
                    TIME_CHANNEL::WORLD,
                    &objDesc // 캡처된 값의 주소 -> 안전
                ),
                E_FAIL);
            });
    }

    return S_OK;
}

HRESULT CLevel_Effect::Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
    // Dat 기본 경로
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

    pDataFilePath += pDataFileName;

    pDataFilePath += TEXT("_lights.dat");

    DWORD dwByte = {};

    HANDLE hFile = CreateFile(pDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    CHECK_EQUAL(INVALID_HANDLE_VALUE, hFile, E_FAIL);

    // 1. 조명의 총 개수
    _uint iLightCnt = {};
    CHECK_FALSE(ReadFile(hFile, &iLightCnt, sizeof(_uint), &dwByte, nullptr), false);

    // 조명 총 개수만큼 순회
    for (_uint i = 0; i < iLightCnt; ++i)
    {
        LIGHT_DESC LightDesc = {};

        // 2. 조명 태그 길이 불러오기
        _uint iLightTagLen = {};
        CHECK_FALSE(ReadFile(hFile, &iLightTagLen, sizeof(_uint), &dwByte, nullptr), false);

        // 3. 조명 태그 이름 불러오기
        _tchar szLightTag[MAX_PATH] = {};
        CHECK_FALSE(ReadFile(hFile, &szLightTag, sizeof(_tchar) * iLightTagLen, &dwByte, nullptr), false);

        // 4. 조명 구조체 불러오기
        CHECK_FALSE(ReadFile(hFile, &LightDesc, sizeof(LIGHT_DESC), &dwByte, nullptr), false);

        // 조명 등록
        m_pGameInstance->Add_FireTask([this, szLightTag = szLightTag, eCurrentLevel = ENUM_CLASS(eCurrentLevel), LightDesc = LightDesc]() mutable {
            m_pGameInstance->Add_Light(szLightTag, ENUM_CLASS(eCurrentLevel), LightDesc, true);
            return S_OK;
            });

    }

    CloseHandle(hFile);

    return S_OK;

}

CLevel_Effect* CLevel_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Effect* pInstance = new CLevel_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Effect::Free()
{
	__super::Free();
}
