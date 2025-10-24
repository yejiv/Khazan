#include "Level_Effect.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Effect.h"
#include "Effect_Point_Instance.h"
#include "Effect_Mesh_Instance.h"
#include "Effect_Sprite.h"

CLevel_Effect::CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Effect::Initialize()
{
    if (FAILED(Ready_Layer_BackGround()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera()))
        return E_FAIL;

    if (FAILED(Ready_Layer_GameObject()))
        return E_FAIL;

    m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
        {
            ImGui::Begin("[Create Effect to Prefab]");

            _float time = m_PrefabPrototype->Get_Time();
            ImGui::SliderFloat("TimeTrack", &time, 0.f, m_PrefabPrototype->Get_MaxTrack());

            if (ImGui::Button("Start"))
                m_PrefabPrototype->ResetChildren();
            if (ImGui::Button("Stop"))
                ;

            m_PrefabPrototype->Setting_Loop();
            ImGui::Separator();

            Edit_Options();

            ImGui::End();
        });

    m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
        {
			vector<string> Names;
			vector<const char*> cstr_Names;

			_uint size = m_PrefabPrototype->Get_ChildrenSize();

			for (_uint i = 0; i < size; ++i)
				Names.push_back("Element" + to_string(i));

			for (const auto& name : Names)
				cstr_Names.push_back(name.c_str());

			ImGui::Begin("[Effect Elements]");

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

			ImGui::End();
        });

	m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
		{
			_int EventType = ENUM_CLASS(m_WorkingTrackData.eEventType);

			ImGui::Begin("[Time Track]");

			ImGui::RadioButton("Active", &EventType, ENUM_CLASS(CEffect_Prefab::EffectEventType::ACTIVATE));
			ImGui::RadioButton("Spread", &EventType, ENUM_CLASS(CEffect_Prefab::EffectEventType::ANIMATE_SPREAD));
			ImGui::RadioButton("Rotate", &EventType, ENUM_CLASS(CEffect_Prefab::EffectEventType::ANIMATE_ROTATE));
			ImGui::RadioButton("Twinkle", &EventType, ENUM_CLASS(CEffect_Prefab::EffectEventType::ANIMATE_TWINLKE));
			ImGui::RadioButton("Up", &EventType, ENUM_CLASS(CEffect_Prefab::EffectEventType::ANIMATE_LINEAR_MOVE));
			ImGui::RadioButton("Dissolve", &EventType, ENUM_CLASS(CEffect_Prefab::EffectEventType::DISSOLVE));

			m_WorkingTrackData.eEventType = (CEffect_Prefab::EffectEventType)EventType;

			ImGui::InputInt("Element ID : ", reinterpret_cast<int*>(&m_WorkingTrackData.iElementIdx));
			ImGui::InputFloat("StartTime : ", reinterpret_cast<_float*>(&m_WorkingTrackData.fStartTime));
			ImGui::InputFloat("fDuration : ", reinterpret_cast<_float*>(&m_WorkingTrackData.fDuration));

			switch (m_WorkingTrackData.eEventType)
			{
			case CEffect_Prefab::EffectEventType::ANIMATE_SPREAD:
				ImGui::InputFloat2("Spread speed : ", reinterpret_cast<_float*>(&m_WorkingTrackData.fSpreadSpeed));
				ImGui::InputFloat3("pivot : ", reinterpret_cast<_float*>(&m_WorkingTrackData.fPivot));
				ImGui::Checkbox("Gravity", &m_bGravity);
				break;

			case CEffect_Prefab::EffectEventType::ANIMATE_ROTATE:
				ImGui::InputFloat2("Rotate speed : ", reinterpret_cast<_float*>(&m_WorkingTrackData.fRotationSpeed));
				ImGui::InputFloat3("pivot : ", reinterpret_cast<_float*>(&m_WorkingTrackData.fPivot));
				break;

			case CEffect_Prefab::EffectEventType::ANIMATE_TWINLKE:
				ImGui::InputFloat2("Scale speed : ", reinterpret_cast<_float*>(&m_WorkingTrackData.fScaleSpeed));
				break;

			case CEffect_Prefab::EffectEventType::ANIMATE_LINEAR_MOVE:
				ImGui::InputFloat2("Upward speed : ", reinterpret_cast<_float*>(&m_WorkingTrackData.fUpwardSpeed));
				break;
			}

			if (ImGui::Button("Add TimeTrack"))
			{
				m_WorkingTrackData.bGravity = m_bGravity;
				m_PrefabPrototype->Add_TimeTrack(m_WorkingTrackData); 
			}

			ImGui::End();
		});

	m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
		{
			m_PrefabPrototype->Edit_TimeTrack(m_iChildrenIdx); 
		});

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

void CLevel_Effect::Edit_Options()
{
	ImGui::RadioButton("Point Effect", &m_EffectType, 0);
	ImGui::RadioButton("Mesh Effect", &m_EffectType, 1);
	ImGui::RadioButton("Sprite Effect", &m_EffectType, 2);
	ImGui::Separator();

	if (m_EffectType != 2)	//Instancing Effect
	{
		ImGui::RadioButton("Spawn_BoundingBox", &m_SpawnType, 0);
		ImGui::RadioButton("Spawn_Circle", &m_SpawnType, 1);
		ImGui::Separator();

		if (m_SpawnType == 0)
			Create_Box_Spawn();
		else if (m_SpawnType == 1)
			Create_Circle_Spawn();

		ImGui::Checkbox("Element Loop", &m_bLoop);
		ImGui::InputScalar("Instance Num : ", ImGuiDataType_U32, &m_iInstanceNum);
		ImGui::InputFloat2("LifeTime : ", m_fLifeTime);
		ImGui::InputFloat2("Scrolling Speed : ", reinterpret_cast<_float*>(&m_fScrollSpeed));
	}
	else // Sprite Effect
	{
		const char* textures[] = { "test0","test1" };
		ImGui::ListBox("Particles", reinterpret_cast<int*>(&m_iTextureIdx), textures, IM_ARRAYSIZE(textures));

		ImGui::InputFloat("Sprite Speed : ", reinterpret_cast<_float*>(&m_fSpriteSpeed));
		ImGui::InputInt("Scaling Value : ", reinterpret_cast<int*>(&m_fScalingValue));
		ImGui::InputInt("Col : ", reinterpret_cast<int*>(&m_iCol));
		ImGui::InputInt("Row : ", reinterpret_cast<int*>(&m_iRow));
		ImGui::Checkbox("Sprite Loop", &m_bLoop);
	}

	ImGui::InputFloat2("Size : ", m_fSize);
	ImGui::InputFloat("Size Ratio : ", &m_fSizeRatio);

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

	ImGui::ColorEdit4("MyColorWithAlpha", (float*)&m_fColor);

	if (prevIdx != m_EffectType)
		m_iTextureIdx = 0;

	if (m_EffectType == 0)
	{
		const char* textures[] = { "test0", "test1", "test2",  "test3",  "test4",  "test5" };
		ImGui::ListBox("Point Particles Textures", &m_iTextureIdx, textures, IM_ARRAYSIZE(textures));
	}
	else if (m_EffectType == 1)
	{
		const char* textures[] = { "test0", "test1", "test2",  "test3",  "test4",  "test5",  "test6" ,  "test7" ,  "test8" ,  "test9" ,  "test10" ,  "test11" ,  "test12" ,  "test13",  "test14" ,  "test15" ,  "test16" ,  "test17" ,  "test18" ,  "test19" ,  "test20" ,  "test21",  "test22" ,  "test23" };
		ImGui::ListBox("Mesh Textures", &m_iTextureIdx, textures, IM_ARRAYSIZE(textures));

		const char* Meshes[] = { "Mesh1", "Mesh2", "Mesh3",  "Mesh4",  "Mesh5",  "Mesh6" };
		ImGui::ListBox("Mesh Shape", &m_iMeshTypeIdx, Meshes, IM_ARRAYSIZE(Meshes));
	}

	prevIdx = m_EffectType;
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

	m_PrefabPrototype->Add_Effect_Element(m_EffectType, &data);
}

HRESULT CLevel_Effect::Ready_Layer_BackGround()
{

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Camera()
{
    return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_GameObject()
{
    m_PrefabPrototype = CEffect_Prefab::Create(m_pDevice, m_pContext); 

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
		ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Camera_Effect"), &Desc), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Effect_Terrain"),
		ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Effect_Terrain")), E_FAIL);

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
