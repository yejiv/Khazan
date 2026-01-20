#include "Shader_Controller.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Body_Khazan_Spear.h"
#include "Khazan_Spear.h"
#include "ContainerObject.h"
#include "Creature.h"
#include "Yetuga.h"
#include "Body_Yetuga.h"
#include "Spear_Khazan_Spear.h"

#include "Khazan_GSword.h"
#include "Body_Khazan_GS.h"
#include "GSword_Khazan_GS.h"
#include "Elamein.h"
#include "Dragonian_Melee.h"
#include "Dragonian_Rampage.h"
#include "Projectile_Imp_MagicBall.h"
#include "Imp_Melee.h"
#include "Halberd.h"
#include "Viper.h"
#include "SkySphere.h"
#include "CloudSphere.h"

CShader_Controller::CShader_Controller()
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pClientInstance{ CClientInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pClientInstance);
}

HRESULT CShader_Controller::Initialize()
{
	//  m_CascadeConfig.Splits.resize(m_iNumCascades);
#ifdef _DEBUG
	//  m_CascadeConfig = m_pGameInstance->Get_CascadeConfig();
#endif
	m_SSAOConfig = m_pGameInstance->Get_SSAOConfig();
	m_FogConfig = m_pGameInstance->Get_FogConfig();
	//  m_iNumCascades = m_pGameInstance->Get_NumCascades();
    m_InitFogConfig = m_FogConfig;
    m_RadialBlurDesc = m_pGameInstance->Get_RadialBlurDesc();
    m_MotionBlurDesc = m_pGameInstance->Get_MotionBlurDesc();
    m_RimLightDesc = m_pGameInstance->Get_RimLightDesc();
    m_ShadowDesc = m_pGameInstance->Get_ShadowDesc();
    m_VignetteConfig = m_pGameInstance->Get_VignetteConfig();

    //  m_TargetLightDesc.vDiffuse = _float4(10.f, 9.f, 8.f, 1.f);
    //  m_TargetLightDesc.vAmbient = _float4(1.f, 0.9f, 0.8f, 1.f);
    //  m_TargetLightDesc.vSpecular = m_TargetLightDesc.vDiffuse;

    m_DecalDesc.fLifeTime = 5.f;
    m_DecalDesc.vFadeTime = { 0.5f, 0.5f };
    m_DecalDesc.vScale = { 30.f, 3.f, 30.f };
    m_DecalDesc.vColor = _float3(0.2745f, 0.08f, 0.08f);

    m_DecalDesc.EmissiveDesc.vBaseColor = _float3(0.15f, 0.02f, 0.01f);
    m_DecalDesc.EmissiveDesc.vEmissiveColor = _float3(1.f, 0.15f, 0.15f);
    m_DecalDesc.EmissiveDesc.vBorderColor = _float3(0.f, 0.f, 0.f);
    m_DecalDesc.EmissiveDesc.fEmissiveMaskPower = 3.f;
    m_DecalDesc.EmissiveDesc.fEmissiveIntensity = 5.f;

    m_DistortionDesc = m_pGameInstance->Get_DistortionDesc();

    m_GaussianBlurConfig = m_pGameInstance->Get_GaussianBlurConfig();

	Ready_Level();
	Ready_Shader();

	return S_OK;
}

void CShader_Controller::Update(_float fTimeDelta)
{
    // Picking Test
    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::WB))
    {
        _float3 vPos, vNorm;
        m_pGameInstance->isPicked(&vPos, &vNorm);
        m_DecalDesc.vPosition = vPos;

        if (FAILED(m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Decal"), m_DecalDesc)))
            return;
    }
}

void CShader_Controller::Ready_Level()
{
#ifdef _DEBUG

#endif
}

void CShader_Controller::Ready_Shader()
{
#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Shader"), [&]()
		{
			ImGui::Begin("Shader Settings");

            if (ImGui::CollapsingHeader("Select Level"))
            {
                Select_Level();
                ImGui::Separator();
            }

            if (ImGui::CollapsingHeader("Debug"))
            {
                Debug_Settings();
                ImGui::Separator();
            }

            if (ImGui::CollapsingHeader("Lighting Calculation"))
            {
                Lighting_Calculation_Settings();
                Cartoon_Rendering_Settings();
                ImGui::Separator();
            }

            if (ImGui::CollapsingHeader("Ambient Occlusion"))
            {
                SSAO_Settings();
                ImGui::Separator();
            }

            if (ImGui::CollapsingHeader("Cascade Shadow"))
            {
                Shadow_Settings();
                ImGui::Separator();
            }

			if (ImGui::CollapsingHeader("Post Processing"))
			{
                Fog_Settings();
                Bloom_Settings();
                MotionBlur_Settings();
                LUT_Settings();
                ImGui::Separator();
            }

            if (ImGui::CollapsingHeader("VFX"))
            {
                MotionTrail_Settings();
                Decal_Settings();
                RadialBlur_Settings();
                Distortion_Settings();
                Vignette_Settings();
                ImGui::Separator();
            }

            if (ImGui::CollapsingHeader("Light Settings"))
            {
                Light_Settings();
                ImGui::Separator();
            }

            if (ImGui::CollapsingHeader("External System Settings"))
            {
                MeshTrail_Settings();
                Environment_Settings();
            }

			ImGui::End();
		});
#endif
}

void CShader_Controller::Select_Level()
{
    static const struct { const char* label; LEVEL value; } kItems[] = {
                { "HEINMACH", LEVEL::HEINMACH },
                { "CREVICE",  LEVEL::CREVICE  },
                { "EMBARS",   LEVEL::EMBARS   },
                { "VIPER",    LEVEL::VIPER    },
    };

    // 현재 선택을 int로 보유(라디오 그룹은 int 포인터 사용 권장)
    int iCurrent = static_cast<int>(m_eCurrentLevel);

    for (const auto& it : kItems)
    {
        const int v_button = static_cast<int>(it.value);
        // 동일 int*를 공유하면 "그룹"으로 동작 (하나만 선택됨)
        if (ImGui::RadioButton(it.label, &iCurrent, v_button))
            m_eCurrentLevel = static_cast<LEVEL>(iCurrent);
    }
}

void CShader_Controller::Debug_Settings()
{
    if (ImGui::Button("Raw Render Mode"))
        Set_RawRenderMode();

    if (ImGui::Checkbox("UI Render", &m_isVisibleUI))
        CClientInstance::GetInstance()->Set_UIAllRenderSet(m_isVisibleUI);
}

void CShader_Controller::Set_RawRenderMode()
{
    m_pGameInstance->Set_UnlitMode(true);
    m_pGameInstance->Set_LitMode(false);
    m_pGameInstance->Set_EnableSpecular(false);
    m_pGameInstance->Set_EnableRimLight(false);
    m_pGameInstance->Set_EnableToonShade(false);

    CKhazan_Spear* pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
    CBody_Khazan_Spear* pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
    pBody->Set_EnableEdge(false);

    m_pGameInstance->Set_EnableSSAO(false);
    m_pGameInstance->Set_EnableShadow(false);
    m_pGameInstance->Set_EnablePCF(false);
    m_pGameInstance->Set_EnableFog(false);
    m_pGameInstance->Set_EnableBloom(false);
    m_pGameInstance->Set_EnableMotionBlur(false);
    m_pGameInstance->Set_EnableLUT(false);
    m_pGameInstance->Set_EnableVignette(false);

    m_isRenderShadow = { false };
    m_isRenderSSAO = { false };
    m_isEnableFog = { false };
    m_isEnableToonShade = { false };
    m_isEnableLUT = { false };
    m_isEnableRadialBlur = { false };
    m_isEnableMotionBlur = { false };
    m_isEnableEdge = { false };
    m_isEnableRimLight = { false };
    m_isEnableMotionTrail = {};
    m_isUnlitMode = { true };
    m_isLitMode = { false };
    m_isEnableSpecular = { false };
    m_isEnableBloom = { false };
    m_isEnableShadowPCF = { false };
    m_isEnableVignette = { false };
}

void CShader_Controller::Lighting_Calculation_Settings()
{
    //  ImGui::SeparatorText("Test SeparatorText");

    //  ImGui::Indent(); // 수동으로 조금 더 밀어 넣고 싶을 때 사용
    //  Code
    //  ImGui::Unindent();

    //  if (ImGui::TreeNode("Test Node")) 
    //  {
    //      ImGui::SliderFloat("RimLight Power", &m_RimLightDesc.fPower, 0.f, 10.f, "%.2f");
    //      ImGui::SliderFloat("RimLight Power", &m_RimLightDesc.fPower, 0.f, 10.f, "%.2f");
    //      ImGui::SliderFloat("RimLight Power", &m_RimLightDesc.fPower, 0.f, 10.f, "%.2f");
    //      ImGui::TreePop();
    //  }

    if (ImGui::Checkbox("Lit Mode", &m_isLitMode))
    {
        m_pGameInstance->Set_UnlitMode(true);
        m_pGameInstance->Set_LitMode(m_isLitMode);
    }

    if (ImGui::Checkbox("Specular", &m_isEnableSpecular))
        m_pGameInstance->Set_EnableSpecular(m_isEnableSpecular);

    if (ImGui::Checkbox("RimLight", &m_isEnableRimLight))
        m_pGameInstance->Set_EnableRimLight(m_isEnableRimLight);

    if (m_isEnableRimLight)
    {
        ImGui::Indent();

        if (ImGui::TreeNode("Settings###RimLight"))
        {
            if (ImGui::SliderFloat("Power###RimLight0", &m_RimLightDesc.fPower, 0.f, 10.f, "%.2f"))
                m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);

            if (ImGui::SliderFloat("Intensity###RimLight1", &m_RimLightDesc.fIntensity, 0.f, 1.f, "%.2f"))
                m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);

            if (ImGui::Checkbox("Toon Light", &m_RimLightDesc.isToonLight))
                m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);

            if (m_RimLightDesc.isToonLight)
                if (ImGui::SliderFloat("Toon Threshold###RimLight2", &m_RimLightDesc.fToonThreshold, 0.f, 1.f, "%.2f"))
                    m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);
            
            ImGui::TreePop();
        }

        ImGui::Unindent();
    }
}

void CShader_Controller::Cartoon_Rendering_Settings()
{
    ImGui::SeparatorText("Cartoon Rendering");

    if (ImGui::Checkbox("Toon Shading", &m_isEnableToonShade))
        m_pGameInstance->Set_EnableToonShade(m_isEnableToonShade);

    if (m_isEnableToonShade)
    {
        ImGui::Indent();

        if (ImGui::TreeNode("Settings###ToonShading"))
        {
            if (ImGui::SliderFloat("Toon Shade Level", &m_fToonShadeLevel, 1.f, 5.f, "%.0f"))
                m_pGameInstance->Set_ToonShadeLevel(m_fToonShadeLevel);

            ImGui::TreePop();
        }

        ImGui::Unindent();
    }

    ImGui::SeparatorText("Default Shade");

    DefaultShade_Settings();
}

void CShader_Controller::DefaultShade_Settings()
{
    const _char* ObjectTags[] = { "Khazan_Spear", "Khazan_GS", "Yetuga", "Viper", "Danjin" };
    ImGui::Combo("GameObject List", &m_iSelectedDSGameObjectIndex, ObjectTags, IM_ARRAYSIZE(ObjectTags));

    CKhazan_Spear* pKhazan = {};
    CBody_Khazan_Spear* pBody = {};

    switch (m_iSelectedDSGameObjectIndex)
    {
    case 0:
        pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
        pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
        m_fEdgeIntensity = pBody->Get_EdgeIntensity();
        m_fShadeIntensity = pBody->Get_ShadeIntensity();
        break;

    default:
        m_iSelectedDSGameObjectIndex = -1;
        break;
    }

    if (0 <= m_iSelectedDSGameObjectIndex)
    {
        if (ImGui::Checkbox("Enable###DefaultShade3", &m_isEnableEdge))
        {
            switch (m_iSelectedDSGameObjectIndex)
            {
            case 0:
                pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                pBody->Set_EnableEdge(m_isEnableEdge);
                break;
            }
        }

        if (m_isEnableEdge)
        {
            if (ImGui::SliderFloat("Edge Intensity###DefaultShade1", &m_fEdgeIntensity, 0.f, 1.f, "%.2f"))
            {
                switch (m_iSelectedDSGameObjectIndex)
                {
                case 0:
                    pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                    pBody->Set_EdgeIntensity(m_fEdgeIntensity);
                    break;
                }
            }

            if (ImGui::SliderFloat("Shade Intensity###DefaultShade2", &m_fShadeIntensity, 0.f, 1.f, "%.2f"))
            {
                switch (m_iSelectedDSGameObjectIndex)
                {
                case 0:
                    pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                    pBody->Set_ShadeIntensity(m_fShadeIntensity);
                    break;
                }
            }
        }
    }
}

void CShader_Controller::SSAO_Settings()
{
    if (ImGui::Checkbox("SSAO", &m_isRenderSSAO))
        m_pGameInstance->Set_EnableSSAO(m_isRenderSSAO);

    if (m_isRenderSSAO)
    {
        _int iNumKernels = static_cast<_int>(m_SSAOConfig.iNumKernels);
        if (ImGui::InputInt("Sample Count###SSAO0", &iNumKernels, 4, 16))
        {
            m_SSAOConfig.iNumKernels = iNumKernels;
            m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);
        }

        if (ImGui::SliderFloat("Sample Radius###SSAO1", &m_SSAOConfig.fRadius, 0.1f, 2.f))
            m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);

        if (ImGui::SliderFloat("Intensity###SSAO2", &m_SSAOConfig.fIntensity, 0.5f, 4.f))
            m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);

        if (ImGui::SliderFloat("Contrast###SSAO3", &m_SSAOConfig.fConstrast, 0.5f, 2.f))
            m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);
    }
}

void CShader_Controller::Shadow_Settings()
{
    if (ImGui::Checkbox("Shadow", &m_isRenderShadow))
    {
        m_pGameInstance->Set_EnableShadow(m_isRenderShadow);
        m_pGameInstance->Set_UnlitMode(false);
    }

    if (m_isRenderShadow)
    {
        if (ImGui::SliderFloat3("Light Direction###Shadow0", reinterpret_cast<_float*>(&m_ShadowDesc.vLightDir), -1.f, 1.f))
            m_pGameInstance->Set_ShadowDesc(m_ShadowDesc);

        if (ImGui::SliderFloat("Split###Shadow1", &m_ShadowDesc.fSplit, 0.1f, 100.f))
            m_pGameInstance->Set_ShadowDesc(m_ShadowDesc);

        if (ImGui::SliderFloat("Bias###Shadow2", &m_ShadowDesc.fBias, 0.001f, 0.01f))
            m_pGameInstance->Set_ShadowDesc(m_ShadowDesc);

        if (ImGui::SliderFloat("Intensity###Shadow3", &m_ShadowDesc.fIntensity, 0.f, 1.f))
            m_pGameInstance->Set_ShadowDesc(m_ShadowDesc);
    }

    if (ImGui::Checkbox("3x3 Percentage Closer Filtering", &m_isEnableShadowPCF))
        m_pGameInstance->Set_EnablePCF(m_isEnableShadowPCF);
}

void CShader_Controller::Fog_Settings()
{
    if (ImGui::Checkbox("Fog", &m_isEnableFog))
    {
        m_pGameInstance->Set_EnableFog(m_isEnableFog);
        m_pGameInstance->Set_UnlitMode(false);
    }

    if (m_isEnableFog)
    {
        _bool isChanged = {};
        _int iFogMode = static_cast<_int>(m_FogConfig.eType);

        isChanged |= ImGui::RadioButton("Linear###Fog0", &iFogMode, static_cast<_int>(FOG_CONFIG::LINEAR));
        ImGui::SameLine();
        isChanged |= ImGui::RadioButton("Exp###Fog1", &iFogMode, static_cast<_int>(FOG_CONFIG::EXP));
        ImGui::SameLine();
        isChanged |= ImGui::RadioButton("Exp Spuare###Fog2", &iFogMode, static_cast<_int>(FOG_CONFIG::EXPSQUARE));

        if (true == isChanged)
        {
            m_FogConfig.eType = static_cast<FOG_CONFIG::TYPE>(iFogMode);
            m_pGameInstance->Set_FogConfig(m_FogConfig);
        }

        if (FOG_CONFIG::LINEAR == m_FogConfig.eType)
        {
            if (ImGui::SliderFloat("Near###Fog3", &m_FogConfig.fNear, 0.1f, 100.f))
                m_pGameInstance->Set_FogConfig(m_FogConfig);

            if (ImGui::SliderFloat("Far###Fog4", &m_FogConfig.fFar, m_FogConfig.fNear + 0.1f, 1000.f))
                m_pGameInstance->Set_FogConfig(m_FogConfig);

            if (m_FogConfig.fFar <= m_FogConfig.fNear)
            {
                m_FogConfig.fFar = m_FogConfig.fNear + 0.1f;
                m_pGameInstance->Set_FogConfig(m_FogConfig);
            }
        }
        else
        {
            if (ImGui::SliderFloat("Density###Fog5", &m_FogConfig.fDensity, 0.0001f, 0.05f, "%.4f"))
                m_pGameInstance->Set_FogConfig(m_FogConfig);
        }

        if (ImGui::SliderFloat("Bias###Fog6", &m_FogConfig.fBias, 0.f, 1.f, "%.1f"))
            m_pGameInstance->Set_FogConfig(m_FogConfig);

        if (ImGui::ColorEdit4("Color###Fog7", reinterpret_cast<_float*>(&m_FogConfig.vColor)))
            m_pGameInstance->Set_FogConfig(m_FogConfig);

        if (ImGui::SliderFloat("Light Bleed Strength", &m_FogConfig.fLightBleedStrength, 0.f, 1.f, "%.2f"))
            m_pGameInstance->Set_FogConfig(m_FogConfig);

        // Use Height
        if (ImGui::Checkbox("Height Fog", &m_FogConfig.isUseHeight))
            m_pGameInstance->Set_FogConfig(m_FogConfig);

        if (true == m_FogConfig.isUseHeight)
        {
            ImGui::Indent();

            if (ImGui::TreeNodeEx("Settings###Fog8", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::SliderFloat("Base Height###Fog9", &m_FogConfig.fBaseHeight, -1000.f, 3000.f))
                    m_pGameInstance->Set_FogConfig(m_FogConfig);

                if (ImGui::SliderFloat("Height Density###Fog10", &m_FogConfig.fHeightDensity, 0.001f, 1.f))
                    m_pGameInstance->Set_FogConfig(m_FogConfig);
                
                ImGui::TreePop();
            }

            ImGui::Unindent();
        }

        // Use Color Lerp
        if (ImGui::Checkbox("Sub Color###Fog11", &m_FogConfig.isUseSubColor))
            m_pGameInstance->Set_FogConfig(m_FogConfig);

        if (true == m_FogConfig.isUseSubColor)
        {
            ImGui::Indent();

            if (ImGui::TreeNodeEx("Settings###Fog12", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::SliderFloat("Start Height###Fog13", &m_FogConfig.fSubColorStartHeight, -1000.f, 3000.f))
                    m_pGameInstance->Set_FogConfig(m_FogConfig);

                if (ImGui::ColorEdit4("Sub Color###Fog14", reinterpret_cast<_float*>(&m_FogConfig.vSubColor)))
                    m_pGameInstance->Set_FogConfig(m_FogConfig);

                ImGui::TreePop();
            }

            ImGui::Unindent();
        }

        if (ImGui::Checkbox("Noise###Fog15", &m_FogConfig.Noise.isEnable))
            m_pGameInstance->Set_FogConfig(m_FogConfig);

        if (m_FogConfig.Noise.isEnable)
        {
            ImGui::Indent();

            if (ImGui::TreeNodeEx("Settings###Fog11", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::SliderFloat2("Scroll Speed###Fog15", reinterpret_cast<_float*>(&m_FogConfig.Noise.vSpeed), -1.f, 1.f, "%.2f"))
                    m_pGameInstance->Set_FogConfig(m_FogConfig);

                if (ImGui::SliderFloat2("Noise Scale###Fog16", reinterpret_cast<_float*>(&m_FogConfig.Noise.vScale), 1.f, 5.f, "%.2f"))
                    m_pGameInstance->Set_FogConfig(m_FogConfig);

                if (ImGui::SliderFloat("Noise Strength###Fog17", &m_FogConfig.Noise.fStrength, 0.0f, 1.0f, "%.2f"))
                    m_pGameInstance->Set_FogConfig(m_FogConfig);

                if (ImGui::SliderFloat("Noise Contrast###Fog18", &m_FogConfig.Noise.fContrast, 0.1f, 5.0f, "%.2f"))
                    m_pGameInstance->Set_FogConfig(m_FogConfig);

                ImGui::SeparatorText("Noise Textures");

                ImGui::BeginChild("Noise Texture###Fog19", ImVec2(0, 68), true, ImGuiWindowFlags_HorizontalScrollbar);

                for (_uint i = 0; i < m_pGameInstance->Get_NumFogNoiseTextures(); ++i)
                {
                    ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_FogNoiseTexture(i);

                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                        m_pGameInstance->Set_FogNoiseTextureIndex(i);

                    ImGui::SameLine();
                }

                ImGui::EndChild();

                ImGui::TreePop();
            }

            ImGui::Unindent();
        }

        ImGui::SeparatorText("Fog Transition");

        ImGui::SliderFloat("Target Density###Fog21", &m_TargetFogDesc.fDensity, 0.0001f, 0.05f, "%.4f");
        ImGui::SliderFloat("Target Bias###Fog22", &m_TargetFogDesc.fBias, 0.f, 1.f, "%.2f");
        ImGui::ColorEdit4("Target Color###Fog23", reinterpret_cast<_float*>(&m_TargetFogDesc.vColor));
        ImGui::SliderFloat("Duration###Fog20", &m_fFogTransDuration, 0.1f, 10.f, "%.2f");

        if (ImGui::Button("Start Fog Transition"))
        {
            m_pGameInstance->Start_FogTransition(m_fFogTransDuration, m_TargetFogDesc);
            m_FogConfig.fDensity = m_TargetFogDesc.fDensity;
            m_FogConfig.vColor = m_TargetFogDesc.vColor;
            m_FogConfig.fBias = m_TargetFogDesc.fBias;
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset Fog"))
        {
            m_FogConfig = m_InitFogConfig;
            m_pGameInstance->Set_FogConfig(m_InitFogConfig);
        }

        ImGui::SameLine();

        if (ImGui::Button("Get Current Fog"))
            m_FogConfig = m_pGameInstance->Get_FogConfig();

        //  ImGui::Separator();
    }
}

void CShader_Controller::Bloom_Settings()
{
    ImGui::SeparatorText("Blur");

    if (ImGui::Checkbox("Bloom", &m_isEnableBloom))
        m_pGameInstance->Set_EnableBloom(m_isEnableBloom);

    if (m_isEnableBloom)
    {
        ImGui::Indent();

        if (ImGui::TreeNodeEx("Gaussian Blur Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 가우시안 블러 범위(반경)
            if (ImGui::InputInt("Radius###GaussianBlur0", &m_GaussianBlurConfig.iRadius, 2, 4))
                m_pGameInstance->Set_GaussianBlurConfig(m_GaussianBlurConfig);

            // 가우시안 블러 가중치 밀집도
            if (ImGui::SliderFloat("Concentration###GaussianBlur1", &m_GaussianBlurConfig.fSigma, 1.f, 10.f))
                m_pGameInstance->Set_GaussianBlurConfig(m_GaussianBlurConfig);

            // 가우시안 블러 가중치 합 정규화 수치
            if (ImGui::SliderFloat("Normalization###GaussianBlur2", &m_GaussianBlurConfig.fNormalization, 0.f, 15.f))
                m_pGameInstance->Set_GaussianBlurConfig(m_GaussianBlurConfig);

            ImGui::TreePop();
        }

        ImGui::Unindent();
    }
}

void CShader_Controller::MotionBlur_Settings()
{
    if (ImGui::Checkbox("Motion Blur", &m_isEnableMotionBlur))
        m_pGameInstance->Set_EnableMotionBlur(m_isEnableMotionBlur);

    if (m_isEnableMotionBlur)
    {
        ImGui::Indent();

        if (ImGui::TreeNodeEx("Settings###MotionBlur0", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 깊이 바이어스
            if (ImGui::SliderFloat("Depth Bias###MotionBlur1", &m_MotionBlurDesc.fDepthBias, 0.f, 0.1f, "%.3f"))
                m_pGameInstance->Set_MotionBlurDesc(m_MotionBlurDesc);

            // 샘플 개수
            _int iNumSamples = static_cast<_int>(m_MotionBlurDesc.iNumSamples);
            if (ImGui::InputInt("Sample Count###MotionBlur2", &iNumSamples, 2, 4))
            {
                m_MotionBlurDesc.iNumSamples = iNumSamples;
                m_pGameInstance->Set_MotionBlurDesc(m_MotionBlurDesc);
            }

            // 블러 강도
            if (ImGui::SliderFloat("Strength###MotionBlur3", &m_MotionBlurDesc.fStrength, 0.f, 1.f, "%.1f"))
                m_pGameInstance->Set_MotionBlurDesc(m_MotionBlurDesc);

            ImGui::TreePop();
        }

        ImGui::Unindent();
    }
}

void CShader_Controller::LUT_Settings()
{
    ImGui::SeparatorText("Color Grading");

    if (ImGui::Checkbox("LUT", &m_isEnableLUT))
        m_pGameInstance->Set_EnableLUT(m_isEnableLUT);

    if (m_isEnableLUT)
    {
        ImGui::Indent();

        if (ImGui::TreeNodeEx("Settings##LUT0", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // LUT 강도
            if (ImGui::SliderFloat("Intensity##LUT1", &m_fLUTIntensity, 0.f, 1.f, "%.2f"))
                m_pGameInstance->Set_LUTIntensity(m_fLUTIntensity);

            ImGui::TreePop();
        }

        ImGui::Unindent();
    }
}

void CShader_Controller::MotionTrail_Settings()
{
    if (ImGui::TreeNode("Motion Trail"))
    {
        const _char* ObjectTags[] = { "Khazan_Spear", "Spear", "Khazan_GS", "GSword", "Viper" };
        ImGui::Combo("GameObject List", &m_iCurrentGameObjectIndex, ObjectTags, IM_ARRAYSIZE(ObjectTags));

        // 고르면 해당 객체의 모션 트레일 정보 Get해서 띄우기
        CKhazan_Spear* pKhazan = {};
        CBody_Khazan_Spear* pBody = {};
        CSpear_Khazan_Spear* pWeapon = {};

        CKhazan_GSword* pKhazanGS = {};
        CBody_Khazan_GS* pGSBody = {};
        CGSword_Khazan_GS* pWeaponGS = {};

        switch (m_iCurrentGameObjectIndex)
        {
        case 0:
            pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
            pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
            m_MotionTrailConfig = pBody->Get_MotionTrailConfig();
            m_isEnableMotionTrail = pBody->isEnableMotionTrail();
            break;

        case 1:
            pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
            pWeapon = dynamic_cast<CSpear_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Weapon_Spear")));
            m_MotionTrailConfig = pWeapon->Get_MotionTrailConfig();
            m_isEnableMotionTrail = pWeapon->isEnableMotionTrail();
            break;

        case 2:
            pKhazanGS = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
            pGSBody = dynamic_cast<CBody_Khazan_GS*>(pKhazanGS->Find_PartObject(TEXT("Part_Body")));
            m_MotionTrailConfig = pGSBody->Get_MotionTrailConfig();
            m_isEnableMotionTrail = pGSBody->isEnableMotionTrail();
            break;

        case 3:
            pKhazanGS = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
            pWeaponGS = dynamic_cast<CGSword_Khazan_GS*>(pKhazanGS->Find_PartObject(TEXT("Part_Weapon")));
            m_MotionTrailConfig = pWeaponGS->Get_MotionTrailConfig();
            m_isEnableMotionTrail = pWeaponGS->isEnableMotionTrail();
            break;

        default:
            m_iCurrentGameObjectIndex = -1;
            m_isEnableMotionTrail = false;
            break;
        }

        if (0 <= m_iCurrentGameObjectIndex)
        {
            // 특정 설정이 바뀔 때 Set
            _bool isChanged = false;

            // bool Enable(체크박스)
            if (ImGui::Checkbox("Enable###MotionTrail0", &m_isEnableMotionTrail))
            {
                switch (m_iCurrentGameObjectIndex)
                {
                case 0:
                    pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                    pBody->Set_EnableMotionTrail(m_isEnableMotionTrail);
                    break;

                case 1:
                    pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pWeapon = dynamic_cast<CSpear_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Weapon_Spear")));
                    pWeapon->Set_EnableMotionTrail(m_isEnableMotionTrail);
                    break;

                case 2:
                    pKhazanGS = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pGSBody = dynamic_cast<CBody_Khazan_GS*>(pKhazanGS->Find_PartObject(TEXT("Part_Body")));
                    pGSBody->Set_EnableMotionTrail(m_isEnableMotionTrail);
                    break;

                case 3:
                    pKhazanGS = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pWeaponGS = dynamic_cast<CGSword_Khazan_GS*>(pKhazanGS->Find_PartObject(TEXT("Part_Weapon")));
                    pWeaponGS->Set_EnableMotionTrail(m_isEnableMotionTrail);
                    break;
                }
            }

            isChanged |= ImGui::SliderFloat("LifeTime###MotionTrail1", &m_MotionTrailConfig.vLifeTime.y, 0.f, 10.f, "%.3f");

            isChanged |= ImGui::ColorEdit3("Start Color###MotionTrail2", reinterpret_cast<_float*>(&m_MotionTrailConfig.vStartColor));
            isChanged |= ImGui::ColorEdit3("Target Color###MotionTrail3", reinterpret_cast<_float*>(&m_MotionTrailConfig.vTargetColor));

            isChanged |= ImGui::SliderFloat("Rim Power###MotionTrail4", &m_MotionTrailConfig.fRimPower, 0.f, 10.f, "%.2f");
            isChanged |= ImGui::SliderFloat("Rim Intensity###MotionTrail5", &m_MotionTrailConfig.fRimIntensity, 0.f, 10.f, "%.2f");
            isChanged |= ImGui::SliderFloat("Emissive Intensity###MotionTrail6", &m_MotionTrailConfig.fEmissiveIntensity, 0.f, 5.f, "%.2f");

            isChanged |= ImGui::SliderFloat("Interval###MotionTrail8", &m_MotionTrailConfig.fInterval, 0.f, 5.f, "%.3f");
            isChanged |= ImGui::SliderInt("Max Frame###MotionTrail9", reinterpret_cast<_int*>(&m_MotionTrailConfig.iMaxFrames), 1, 10);

            isChanged |= ImGui::Checkbox("Individual Color", &m_MotionTrailConfig.isIndividualColor);

            if (false == m_MotionTrailConfig.isIndividualColor)
                isChanged |= ImGui::SliderFloat("Color Update Speed###MotionTrail7", &m_MotionTrailConfig.fColorUpdateSpeed, 1.f, 1000.f, "%.1f");

            if (true == isChanged)
            {
                switch (m_iCurrentGameObjectIndex)
                {
                case 0:
                    pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                    pBody->Set_MotionTrailConfig(m_MotionTrailConfig);
                    break;

                case 1:
                    pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pWeapon = dynamic_cast<CSpear_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Weapon_Spear")));
                    pWeapon->Set_MotionTrailConfig(m_MotionTrailConfig);
                    break;

                case 2:
                    pKhazanGS = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pGSBody = dynamic_cast<CBody_Khazan_GS*>(pKhazanGS->Find_PartObject(TEXT("Part_Body")));
                    pGSBody->Set_MotionTrailConfig(m_MotionTrailConfig);
                    break;

                case 3:
                    pKhazanGS = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pWeaponGS = dynamic_cast<CGSword_Khazan_GS*>(pKhazanGS->Find_PartObject(TEXT("Part_Weapon")));
                    pWeaponGS->Set_MotionTrailConfig(m_MotionTrailConfig);
                    break;
                }
            }

            ImGui::SeparatorText("Duration Mode");

            ImGui::SliderFloat("Duration###MotionTrail10", &m_fMotionTrailDuration, 0.f, 10.f, "%.2f");

            if (ImGui::Button("Start Motion Trail"))
            {
                switch (m_iCurrentGameObjectIndex)
                {
                case 0:
                    pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                    pBody->Start_MotionTrail(m_fMotionTrailDuration);
                    break;

                case 1:
                    pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pWeapon = dynamic_cast<CSpear_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Weapon_Spear")));
                    pWeapon->Start_MotionTrail(m_fMotionTrailDuration);
                    break;

                case 2:
                    pKhazanGS = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pGSBody = dynamic_cast<CBody_Khazan_GS*>(pKhazanGS->Find_PartObject(TEXT("Part_Body")));
                    pGSBody->Start_MotionTrail(m_fMotionTrailDuration);
                    break;

                case 3:
                    pKhazanGS = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
                    pWeaponGS = dynamic_cast<CGSword_Khazan_GS*>(pKhazanGS->Find_PartObject(TEXT("Part_Weapon")));
                    pWeaponGS->Start_MotionTrail(m_fMotionTrailDuration);
                    break;
                }
            }
        }

        ImGui::TreePop();
    }

    //ImGui::Unindent();

    ImGui::Separator();
}

void CShader_Controller::Decal_Settings()
{
    if (ImGui::TreeNode("Decal"))
    {
        // 데칼 타입
        _bool isChanged = {};
        _int iDecalType = static_cast<_int>(m_DecalDesc.eType);

        isChanged |= ImGui::RadioButton("Blood Linear", &iDecalType, static_cast<_int>(DECALTYPE::LINEAR));
        ImGui::SameLine();
        isChanged |= ImGui::RadioButton("Blood Circle", &iDecalType, static_cast<_int>(DECALTYPE::CIRCLE));
        ImGui::SameLine();
        isChanged |= ImGui::RadioButton("Blood Curve", &iDecalType, static_cast<_int>(DECALTYPE::CURVE));
        ImGui::SameLine();
        isChanged |= ImGui::RadioButton("Emissive Decal", &iDecalType, static_cast<_int>(DECALTYPE::EMISSIVE));

        if (true == isChanged)
            m_DecalDesc.eType = static_cast<DECALTYPE>(iDecalType);

        // 라이프 타임
        ImGui::SliderFloat("LifeTime###Decal0", &m_DecalDesc.fLifeTime, 1.f, 20.f, "%.0f");

        // 페이드 타임
        ImGui::SliderFloat2("FadeTime###Decal1", reinterpret_cast<_float*>(&m_DecalDesc.vFadeTime), 0.1f, 10.f, "%.1f");

        // 바운딩 박스 사이즈
        ImGui::SliderFloat3("Bounding Box Size###Decal2", reinterpret_cast<_float*>(&m_DecalDesc.vScale), 1.f, 50.f, "%.0f");

        // 컬러
        if (DECALTYPE::EMISSIVE == m_DecalDesc.eType)
        {
            ImGui::ColorEdit3("Base Color###Decal3", reinterpret_cast<_float*>(&m_DecalDesc.EmissiveDesc.vBaseColor));
            ImGui::ColorEdit3("Emissive Color###Decal4", reinterpret_cast<_float*>(&m_DecalDesc.EmissiveDesc.vEmissiveColor));
            ImGui::ColorEdit3("Border Color###Decal5", reinterpret_cast<_float*>(&m_DecalDesc.EmissiveDesc.vBorderColor));
            ImGui::SliderFloat("Emissive Mask Power###Decal6", &m_DecalDesc.EmissiveDesc.fEmissiveMaskPower, 1.f, 10.f, "%.2f");
            ImGui::SliderFloat("Emissive Intensity###Decal7", &m_DecalDesc.EmissiveDesc.fEmissiveIntensity, 1.f, 10.f, "%.2f");
        }
        else
            ImGui::ColorEdit3("Color###Decal8", reinterpret_cast<_float*>(&m_DecalDesc.vColor));

        ImGui::Checkbox("Random Texture###Decal9", &m_DecalDesc.isRandomTexture);

        if (false == m_DecalDesc.isRandomTexture)
        {
            ImGui::SeparatorText("Decal Textures");

            ImGui::BeginChild("Decal Texture", ImVec2(0, 55), true, ImGuiWindowFlags_HorizontalScrollbar);

            for (_uint i = 0; i < m_pGameInstance->Get_NumDecalTextures(m_DecalDesc.eType); ++i)
            {
                ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_DecalTexture(m_DecalDesc.eType, i);

                if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                    m_DecalDesc.iTextureIndex = i;

                ImGui::SameLine();
            }

            ImGui::EndChild();
        }

        ImGui::TreePop();
    }

    //ImGui::Unindent();

    ImGui::Separator();
}

void CShader_Controller::RadialBlur_Settings()
{
    //ImGui::Indent();

    if (ImGui::TreeNode("RadialBlur"))
    {
        // 중심 UV
        ImGui::SliderFloat2("Center UV###RadialBlur0", reinterpret_cast<_float*>(&m_RadialBlurDesc.vCenterUV), 0.f, 1.f, "%.1f");

        // 반경
        ImGui::SliderFloat("Blur Radius###RadialBlur1", &m_RadialBlurDesc.fSampleRadius, 0.01f, 1.5f, "%.2f");

        // 마스크 반경
        ImGui::SliderFloat("Mask Radius Iner###RadialBlur2", &m_RadialBlurDesc.vMaskRadius.x, 0.f, 1.f, "%.3f");
        ImGui::SliderFloat("Mask Radius Outer###RadialBlur3", &m_RadialBlurDesc.vMaskRadius.y, 0.f, 1.f, "%.3f");

        // 마스크 지수(곡선 강화)
        ImGui::SliderFloat("Mask Exponent###RadialBlur4", &m_RadialBlurDesc.fExponent, 1.f, 5.f, "%.2f");

        // 샘플 개수
        _int iNumSamples = static_cast<_int>(m_RadialBlurDesc.iNumSamples);
        if (ImGui::InputInt("Sample Count###RadialBlur5", &iNumSamples, 2, 4))
            m_RadialBlurDesc.iNumSamples = iNumSamples;

        // 감쇠
        ImGui::SliderFloat("Attenuation###RadialBlur6", &m_RadialBlurDesc.fAttenuation, 0.1f, 5.f, "%.2f");

        // 블러 강도
        ImGui::SliderFloat("Strength###RadialBlur7", &m_RadialBlurDesc.fStrength, 0.f, 1.f, "%.2f");

        // 지속 시간
        ImGui::SliderFloat("Duration###RadialBlur8", &m_RadialBlurDesc.fDuration, 0.f, 10.f, "%.2f");

        // 페이드 시간
        ImGui::SliderFloat2("Fade Time###RadialBlur9", reinterpret_cast<_float*>(&m_RadialBlurDesc.vFadeTime), 0.f, 5.f, "%.1f");

        if (ImGui::Button("Start Radial Blur"))
            m_pGameInstance->Start_RadialBlur(m_RadialBlurDesc);

        ImGui::TreePop();
    }

    //ImGui::Unindent();

    ImGui::Separator();
}

void CShader_Controller::Distortion_Settings()
{
    //ImGui::Indent();

    if (ImGui::TreeNode("Distortion"))
    {
        ImGui::SliderFloat("Range###Distortion0", &m_DistortionDesc.fRange, 0.f, 1.f, "%.2f");

        ImGui::SliderFloat("Power###Distortion1", &m_DistortionDesc.fPower, 0.f, 10.f, "%.2f");

        ImGui::SliderFloat2("FadeTime###Distortion2", reinterpret_cast<_float*>(&m_DistortionDesc.vFadeTime), 0.1f, 10.f, "%.1f");

        ImGui::SliderFloat("Scroll Speed###Distortion3", &m_DistortionDesc.fSpeed, 0.f, 10.f, "%.2f");

        ImGui::SliderFloat("Duration###Distortion4", &m_DistortionDesc.fDuration, 0.f, 20.f, "%.1f");

        ImGui::SeparatorText("Noise Textures");

        ImGui::BeginChild("Distortion Noise", ImVec2(0, 68), true, ImGuiWindowFlags_HorizontalScrollbar);

        for (_uint i = 0; i < m_pGameInstance->Get_NumDistortionNoiseTextures(); ++i)
        {
            ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_DistortionNoiseTexture(i);

            if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                m_DistortionDesc.iNoiseIndex = i;

            ImGui::SameLine();
        }

        ImGui::EndChild();

        if (ImGui::Button("Start Distortion"))
            m_pGameInstance->Start_Distortion(m_DistortionDesc);

        ImGui::TreePop();
    }

    //ImGui::Unindent();

    ImGui::Separator();
}

void CShader_Controller::Vignette_Settings()
{
    //ImGui::Indent();

    if (ImGui::TreeNode("Vignette"))
    {
        if (ImGui::Checkbox("Enable###Vignette0", &m_isEnableVignette))
            m_pGameInstance->Set_EnableVignette(m_isEnableVignette, 5.f);

        if (ImGui::ColorEdit3("Color###Vignette1", reinterpret_cast<_float*>(&m_VignetteConfig.vColor)))
            m_pGameInstance->Set_VignetteConfig(m_VignetteConfig);

        if (ImGui::SliderFloat("Power###Vignette2", &m_VignetteConfig.fPower, 0.f, 10.f, "%.2f"))
            m_pGameInstance->Set_VignetteConfig(m_VignetteConfig);

        if (ImGui::SliderFloat("Min Intensity###Vignette3", &m_VignetteConfig.fMinIntensity, 0.f, 10.f, "%.2f"))
            m_pGameInstance->Set_VignetteConfig(m_VignetteConfig);

        if (ImGui::SliderFloat("Max Intensity###Vignette4", &m_VignetteConfig.fMaxIntensity, 0.f, 10.f, "%.2f"))
            m_pGameInstance->Set_VignetteConfig(m_VignetteConfig);

        // 노이즈 사용 여부
        if (ImGui::Checkbox("Noise###Vignette7", &m_VignetteConfig.isUseNoise))
            m_pGameInstance->Set_VignetteConfig(m_VignetteConfig);

        if (true == m_VignetteConfig.isUseNoise)
        {
            // 텍스처 인덱스
            if (ImGui::SliderInt("Texture Index###Vignette8", reinterpret_cast<_int*>(&m_VignetteConfig.iTextureIndex), 0, 3))
                m_pGameInstance->Set_VignetteConfig(m_VignetteConfig);
            // 대비
            if (ImGui::SliderFloat("Contrast###Vignette9", &m_VignetteConfig.fContrast, 0.f, 10.f, "%.2f"))
                m_pGameInstance->Set_VignetteConfig(m_VignetteConfig);
        }

        ImGui::SeparatorText("Duration Mode");

        // 듀레이션
        if (ImGui::SliderFloat("Duration###Vignette5", &m_VignetteConfig.fDuration, 0.f, 5.f, "%.2f"))
            m_pGameInstance->Set_VignetteConfig(m_VignetteConfig);

        if (ImGui::SliderFloat2("Fade Time###Vignette6", reinterpret_cast<_float*>(&m_VignetteConfig.vFadeTime), 0.f, 5.f, "%.1f"))
            m_pGameInstance->Set_VignetteConfig(m_VignetteConfig);

        ImGui::Checkbox("Return Off###Vignette10", &m_isVignetteReturnOff);

        // 스타트 버튼
        if (ImGui::Button("Start Vignette"))
            m_pGameInstance->Start_VignetteAnimation(m_VignetteConfig, m_isVignetteReturnOff);

        ImGui::TreePop();
    }

    //ImGui::Unindent();
}

void CShader_Controller::Light_Settings()
{
    _uint iCurrentLevelIndex = ENUM_CLASS(m_eCurrentLevel);

    // 1. 현재 레벨의 조명 태그 목록 가져오기 (매 프레임 호출되지만 가벼운 복사)
    vector<_wstring> CurrentLightTags = m_pGameInstance->Get_LightTags(iCurrentLevelIndex);

    // 2. 캐시 업데이트 로직: 크기가 다를 때만 갱신 (조명 추가/삭제 시에만 진입)
    if (m_wstrLightTags.size() != CurrentLightTags.size())
    {
        // 컨테이너 초기화
        m_wstrLightTags.clear();
        m_strLightTags.clear();
        m_szLightTags.clear();

        // 원본 태그 캐시 (Deep Copy)
        m_wstrLightTags = CurrentLightTags;

        // 문자열 변환 및 캐싱
        for (const auto& Tag : m_wstrLightTags)
        {
            _int iBufferSize = WideCharToMultiByte(CP_ACP, 0, Tag.c_str(), -1, nullptr, 0, nullptr, nullptr);
            string strLightTag(iBufferSize, 0);
            WideCharToMultiByte(CP_ACP, 0, Tag.c_str(), -1, &strLightTag[0], iBufferSize, nullptr, nullptr);

            m_strLightTags.push_back(strLightTag);
        }

        // ImGui용 포인터 배열 캐싱
        for (const auto& narrowTag : m_strLightTags)
        {
            m_szLightTags.push_back(narrowTag.c_str());
        }
    }

    // 3. 조명이 없을 때 예외 처리
    if (m_wstrLightTags.empty())
    {
        ImGui::Text("No light registered in this level");
    }
    else
    {
        // 현재 선택된 조명의 인덱스 찾기 (문자열 비교)
        _int iCurrentLightIndex = -1;
        for (_uint i = 0; i < m_wstrLightTags.size(); ++i)
        {
            if (m_wstrLightTags[i] == m_strSelectedLightTag)
            {
                iCurrentLightIndex = static_cast<_int>(i);
                break;
            }
        }

        //  ImGui::SeparatorText("Light List");

        // 리스트 박스 렌더링 (캐시된 m_szLightTags 사용)
        if (ImGui::ListBox("Light List", &iCurrentLightIndex, m_szLightTags.data(), static_cast<_int>(m_szLightTags.size())))
        {
            if (iCurrentLightIndex >= 0 && iCurrentLightIndex < m_wstrLightTags.size())
            {
                m_strSelectedLightTag = m_wstrLightTags[iCurrentLightIndex];
            }
        }

        //  ImGui::Separator();

        // 4. 선택된 조명 속성 편집
        if (!m_strSelectedLightTag.empty())
        {
            const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(m_strSelectedLightTag, iCurrentLevelIndex);
            m_isEnableLight = m_pGameInstance->Is_LightEnable(m_strSelectedLightTag, iCurrentLevelIndex);

            _bool isChanged = { false };

            if (pLightDesc)
            {
                // 데이터 수정을 위한 복사본 생성
                LIGHT_DESC EditedDesc = *pLightDesc;

                // New
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.f, 0.f, 0.f, 1.0f)); // 어두운 배경
                ImGui::BeginChild("SelectedInfo", ImVec2(0, 58), true);

                ImGui::TextColored(ImVec4(1.f, 1.f, 0.25f, 1.f), "Selected Light Tag : %ws", m_strSelectedLightTag.c_str());

                const char* typeStr = (EditedDesc.eType == LIGHT_DESC::DIRECTIONAL) ? "DIRECTIONAL" : "POINT";
                
                ImGui::TextColored(ImVec4(0.5f, 1.f, 0.5f, 1.f), "Type : %s", typeStr);

                ImGui::EndChild();
                ImGui::PopStyleColor();

                //  ImGui::Text("Type: %s", (EditedDesc.eType == LIGHT_DESC::DIRECTIONAL) ? "DIRECTIONAL" : "POINT");
                //  
                //  ImGui::TextColored(ImVec4(0.2f, 1.f, 0.2f, 1.f), "Selected: %ws", m_strSelectedLightTag.c_str());

                if (ImGui::Checkbox("Enable###Light0", &m_isEnableLight))
                {
                    m_pGameInstance->Set_LightEnable(m_strSelectedLightTag, iCurrentLevelIndex, m_isEnableLight);
                }

                // ImGuiColorEditFlags_HDR: HDR 모드 활성화 (1.0 초과 값 허용)
                // ImGuiColorEditFlags_Float: 0~255가 아닌 실수형 입력
                //  ImGuiColorEditFlags HDRFlags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float;

                isChanged |= ImGui::ColorEdit4("Diffuse###Light1", reinterpret_cast<_float*>(&EditedDesc.vDiffuse));
                isChanged |= ImGui::ColorEdit4("Ambient###Light2", reinterpret_cast<_float*>(&EditedDesc.vAmbient));
                isChanged |= ImGui::ColorEdit4("Specular###Light3", reinterpret_cast<_float*>(&EditedDesc.vSpecular));

                // 유형별 속성
                if (EditedDesc.eType == LIGHT_DESC::DIRECTIONAL)
                {
                    ImGui::Text("Direction : ");
                    isChanged |= ImGui::SliderFloat3("Direction###Light4", reinterpret_cast<_float*>(&EditedDesc.vDirection), -1.f, 1.f);
                }
                else if (EditedDesc.eType == LIGHT_DESC::POINT)
                {
                    isChanged |= ImGui::SliderFloat3("Position###Light5", reinterpret_cast<_float*>(&EditedDesc.vPosition), -300.f, 300.f, "%.1f");

                    isChanged |= ImGui::SliderFloat("Range###Light6", &EditedDesc.fRange, 0.0f, 100.f, "%.1f");
                }

                // 변경사항 적용
                if (true == isChanged)
                    m_pGameInstance->Set_LightDesc(m_strSelectedLightTag, iCurrentLevelIndex, EditedDesc);

                if (ImGui::Button("Backup Selected Light"))
                    m_pGameInstance->Backup_LightDesc(m_strSelectedLightTag, iCurrentLevelIndex);
            }
            else
            {
                ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Light Desc Not Found!");
                // 존재하지 않는 조명이면 선택 해제
                m_strSelectedLightTag = TEXT("");
            }
        }

        Light_Transition_Settings();
    }
}

void CShader_Controller::Light_Transition_Settings()
{
    ImGui::SeparatorText("Light Transition");
    
    //  ImGuiColorEditFlags HDRFlags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float;

    ImGui::ColorEdit4("Target Diffuse###LightTrans2", reinterpret_cast<_float*>(&m_TargetLightDesc.vDiffuse));
    ImGui::ColorEdit4("Target Ambient###LightTrans3", reinterpret_cast<_float*>(&m_TargetLightDesc.vAmbient));
    ImGui::ColorEdit4("Target Specular###LightTrans4", reinterpret_cast<_float*>(&m_TargetLightDesc.vSpecular));

    ImGui::SliderInt("Blink Count###LightTrans5", reinterpret_cast<_int*>(&m_TargetLightDesc.iBlinkCount), 0, 10);

    ImGui::SliderFloat("Duration###LightTrans0", &m_TargetLightDesc.fDuration, 0.f, 20.f, "%.1f");

    ImGui::SliderFloat2("FadeTime###LightTrans1", reinterpret_cast<_float*>(&m_TargetLightDesc.vFadeTime), 0.1f, 10.f, "%.1f");

    ImGui::Checkbox("Return To Start###LightTrans6", &m_TargetLightDesc.isReturnToStart);
    ImGui::SameLine();
    ImGui::Checkbox("Restore Light###LightTrans7", &m_isRestoreLight);

    if (ImGui::Button("Start Light Transition"))
        m_pGameInstance->Start_LightTransition(m_strSelectedLightTag, ENUM_CLASS(m_eCurrentLevel), m_TargetLightDesc, m_isRestoreLight);
}

void CShader_Controller::MeshTrail_Settings()
{
    if (ImGui::TreeNode("Mesh Trail"))
    {
        const _char* ObjectTags[] = { "Elamein", "Dragonian_Melee", "Dragonian_Rampage", "Khazan_Spear", "Khazan_GS", "Beomsoo", "Viper" };
        ImGui::Combo("Owner List###MeshTrail0", &m_iTrailOwnerIndex, ObjectTags, IM_ARRAYSIZE(ObjectTags));

        // 고르면 해당 객체의 모션 트레일 정보 Get해서 띄우기
        CElamein* pElamein = {};
        CDragonian_Melee* pDragonianMelee = {};
        CDragonian_Rampage* pDragonianRampage = {};
        CKhazan_Spear* pKhazanSpear = {};
        CBody_Khazan_Spear* pBodyKhazanSpear = {};
        CKhazan_GSword* pKhazanGS = {};
        CBody_Khazan_GS* pBodyKhazanGS = {};
        CHalberd* pHalberd = {};
        CViper* pViper = {};

        switch (m_iTrailOwnerIndex)
        {
        case 0:
            pElamein = dynamic_cast<CElamein*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Monster"), 0));
            m_TrailConfig = pElamein->Get_TrailConfig();
            break;

        case 1:
            pDragonianMelee = dynamic_cast<CDragonian_Melee*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Monster"), 0));
            m_TrailConfig = pDragonianMelee->Get_TrailConfig();
            break;

        case 2:
            pDragonianRampage = dynamic_cast<CDragonian_Rampage*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Monster"), 0));
            m_TrailConfig = pDragonianRampage->Get_TrailConfig();
            break;

        case 3:
            pKhazanSpear = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
            pBodyKhazanSpear = dynamic_cast<CBody_Khazan_Spear*>(pKhazanSpear->Find_PartObject(TEXT("Part_Body")));
            m_TrailConfig = pBodyKhazanSpear->Get_TrailConfig();
            break;

        case 4:
            pKhazanGS = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Player"), 0));
            pBodyKhazanGS = dynamic_cast<CBody_Khazan_GS*>(pKhazanGS->Find_PartObject(TEXT("Part_Body")));
            m_TrailConfig = pBodyKhazanGS->Get_TrailConfig();
            break;
        case 5:
            pHalberd = dynamic_cast<CHalberd*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Monster"), 0));
            m_TrailConfig = pHalberd->Get_TrailConfig();
            break;
        case 6:
            pViper = dynamic_cast<CViper*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Viper"), 0));
            m_TrailConfig = pViper->Get_TrailConfig();
            break;

        default:
            m_iTrailOwnerIndex = -1;
            break;
        }

        if (0 <= m_iTrailOwnerIndex)
        {
            // 특정 설정이 바뀔 때 Set
            _bool isChanged = false;

            isChanged |= ImGui::SliderFloat("LifeTime###MeshTrail1", &m_TrailConfig.fLifeTime, 0.f, 3.f, "%.3f");
            isChanged |= ImGui::SliderInt("Division Count###MeshTrail2", reinterpret_cast<_int*>(&m_TrailConfig.iDivisionCount), 1, 10);
            isChanged |= ImGui::ColorEdit4("Main Color###MeshTrail3", reinterpret_cast<_float*>(&m_TrailConfig.vColor));
            isChanged |= ImGui::ColorEdit4("Sub Color###MeshTrail4", reinterpret_cast<_float*>(&m_TrailConfig.vSubColor));

            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.5f, 1.f), "Selected Texture Index : %d", m_TrailConfig.iTextureIdx);

            ImGui::BeginChild("Trail Texture", ImVec2(0, 68), true, ImGuiWindowFlags_HorizontalScrollbar);

            switch (m_iTrailOwnerIndex)
            {
            case 0:
                for (_uint i = 0; i < pElamein->Get_NumTrailTextures(); ++i)
                {
                    ID3D11ShaderResourceView* pSRV = pElamein->Get_TrailTexture(i);

                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                    {
                        isChanged = true;
                        m_TrailConfig.iTextureIdx = i;
                    }

                    ImGui::SameLine();
                }
                break;

            case 1:
                for (_uint i = 0; i < pDragonianMelee->Get_NumTrailTextures(); ++i)
                {
                    ID3D11ShaderResourceView* pSRV = pDragonianMelee->Get_TrailTexture(i);

                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                    {
                        isChanged = true;
                        m_TrailConfig.iTextureIdx = i;
                    }

                    ImGui::SameLine();
                }
                break;

            case 2:
                for (_uint i = 0; i < pDragonianRampage->Get_NumTrailTextures(); ++i)
                {
                    ID3D11ShaderResourceView* pSRV = pDragonianRampage->Get_TrailTexture(i);

                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                    {
                        isChanged = true;
                        m_TrailConfig.iTextureIdx = i;
                    }

                    ImGui::SameLine();
                }
                break;

            case 3:
                for (_uint i = 0; i < pBodyKhazanSpear->Get_NumTrailTextures(); ++i)
                {
                    ID3D11ShaderResourceView* pSRV = pBodyKhazanSpear->Get_TrailTexture(i);

                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                    {
                        isChanged = true;
                        m_TrailConfig.iTextureIdx = i;
                    }

                    ImGui::SameLine();
                }
                break;

            case 4:
                for (_uint i = 0; i < pBodyKhazanGS->Get_NumTrailTextures(); ++i)
                {
                    ID3D11ShaderResourceView* pSRV = pBodyKhazanGS->Get_TrailTexture(i);

                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                    {
                        isChanged = true;
                        m_TrailConfig.iTextureIdx = i;
                    }

                    ImGui::SameLine();
                }
                break;
            case 5:
                for (_uint i = 0; i < pHalberd->Get_NumTrailTextures(); ++i)
                {
                    ID3D11ShaderResourceView* pSRV = pHalberd->Get_TrailTexture(i);
                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                    {
                        isChanged = true;
                        m_TrailConfig.iTextureIdx = i;
                    }

                    ImGui::SameLine();
                }
            case 6:
                for (_uint i = 0; i < pViper->Get_NumTrailTextures(); ++i)
                {
                    ID3D11ShaderResourceView* pSRV = pViper->Get_TrailTexture(i);

                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                    {
                        isChanged = true;
                        m_TrailConfig.iTextureIdx = i;
                    }

                    ImGui::SameLine();
                }
                break;
            }

            ImGui::EndChild();

            if (true == isChanged)
            {
                switch (m_iTrailOwnerIndex)
                {
                case 0: pElamein->Set_TrailConfig(m_TrailConfig); break;
                case 1: pDragonianMelee->Set_TrailConfig(m_TrailConfig); break;
                case 2: pDragonianRampage->Set_TrailConfig(m_TrailConfig); break;
                case 3: pBodyKhazanSpear->Set_TrailConfig(m_TrailConfig); break;
                case 4: pBodyKhazanGS->Set_TrailConfig(m_TrailConfig); break;
                case 5: pHalberd->Set_TrailConfig(m_TrailConfig); break;
                case 6: pViper->Set_TrailConfig(m_TrailConfig); break;
                }
            }
        }

        ImGui::TreePop();
    }

    ImGui::Separator();
}

void CShader_Controller::Environment_Settings()
{
    if (ImGui::TreeNode("Environment"))
    {
        _bool isChanged = false;

        ImGui::SeparatorText("Sky");

        isChanged |= ImGui::ColorEdit3("Sky Top Color", reinterpret_cast<_float*>(&m_SkyDesc.vNebulaColorR));
        isChanged |= ImGui::ColorEdit3("Sky Middle Color", reinterpret_cast<_float*>(&m_SkyDesc.vNebulaColorG));
        isChanged |= ImGui::ColorEdit3("Sky Bottom Color", reinterpret_cast<_float*>(&m_SkyDesc.vNebulaColorB));
        isChanged |= ImGui::SliderFloat("Star Strength", &m_SkyDesc.fStarStrength, 0.f, 5.f, "%.2f");
        isChanged |= ImGui::SliderFloat("Moon Size", &m_SkyDesc.fMoonSize, 0.f, 5.f, "%.2f");
        isChanged |= ImGui::SliderFloat3("Moon Direction", reinterpret_cast<_float*>(&m_SkyDesc.vMoonDirection), -1.f, 1.f);
        isChanged |= ImGui::ColorEdit3("Moon Color", reinterpret_cast<_float*>(&m_SkyDesc.vMoonColor));
        isChanged |= ImGui::SliderFloat("Moon Intensity", &m_SkyDesc.fMoonIntensity, 0.f, 5.f, "%.2f");

        ImGui::SeparatorText("Cloud");

        isChanged |= ImGui::ColorEdit3("Color###Cloud0", reinterpret_cast<_float*>(&m_CloudDesc.vCloudColor));
        isChanged |= ImGui::SliderFloat("Speed###Cloud1", &m_CloudDesc.fCloudSpeed, 0.f, 5.f, "%.3f");
        isChanged |= ImGui::SliderFloat("Scale###Cloud2", &m_CloudDesc.fCloudScale, 0.f, 5.f, "%.3f");
        isChanged |= ImGui::SliderFloat("Density###Cloud3", &m_CloudDesc.fCloudDensity, 0.f, 5.f, "%.3f");
        isChanged |= ImGui::SliderFloat("Light Intensity###Cloud4", &m_CloudDesc.fCloudLightIntensity, 0.f, 5.f, "%.3f");
        isChanged |= ImGui::SliderFloat3("Light Direction###Cloud5", reinterpret_cast<_float*>(&m_CloudDesc.vLightDir), -1.f, 1.f);
        isChanged |= ImGui::SliderFloat("Dynamic###Cloud6", &m_CloudDesc.fDynamic, 0.f, 1.f, "%.0f");

        if (isChanged)
        {
            if (m_fSkyCloudDuration == 0.f)
            {
                static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Sky"), 0))->Start_LerpSky(m_SkyDesc, m_fSkyCloudDuration);
                static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Sky"), 1))->Start_LerpCloud(m_CloudDesc, m_fSkyCloudDuration);
            }
        }

        ImGui::SeparatorText("Environment Transition");

        ImGui::SliderFloat("Duration###Environment", &m_fSkyCloudDuration, 0.f, 10.f, "%.2f");

        if (ImGui::Button("Start Environment Transition"))
        {
            static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Sky"), 0))->Start_LerpSky(m_SkyDesc, m_fSkyCloudDuration);
            static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Sky"), 1))->Start_LerpCloud(m_CloudDesc, m_fSkyCloudDuration);
        }

        ImGui::SameLine();

        if (ImGui::Button("Get Current Environment Info"))
        {
            m_SkyDesc = static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Sky"), 0))->Get_SkyDesc();
            m_CloudDesc = static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Sky"), 1))->Get_CloudDesc();
        }

        ImGui::TreePop();
    }

    ImGui::Separator();
}

CShader_Controller* CShader_Controller::Create()
{
	CShader_Controller* pInstance = new CShader_Controller();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CShader_Controller"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShader_Controller::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pClientInstance);
}
