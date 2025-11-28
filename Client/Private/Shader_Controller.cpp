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

	Ready_Level();
	Ready_Shader();


	return S_OK;
}

void CShader_Controller::Update(_float fTimeDelta)
{

}

void CShader_Controller::Ready_Level()
{
#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Shader"), [&]()
		{
			ImGui::Begin("Select Level");
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
				{
					m_eCurrentLevel = static_cast<LEVEL>(iCurrent);

					if (m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel), TEXT("Layer_Creature_Test")) != nullptr)
					{
                        CKhazan_Spear* pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel),
							TEXT("Layer_Creature_Test"), 0));
						CBody_Khazan_Spear* pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
						OUTLINE_CONFIG PlayerOutlineConfig = pBody->Get_OutlineConfig();
						m_OutlineConfig.vColor = PlayerOutlineConfig.vColor;
						m_OutlineConfig.fSize = PlayerOutlineConfig.fSize;
						OUTLINE_CONFIG RendererOutlineConfig = m_pGameInstance->Get_OutlineConfig();
						m_OutlineConfig.fAlpha = RendererOutlineConfig.fAlpha;
						m_OutlineConfig.fBias = RendererOutlineConfig.fBias;
					}

				}
			}

			ImGui::End();
		});
#endif
}

void CShader_Controller::Ready_Shader()
{
#ifdef _DEBUG
	m_pGameInstance->AddWidget(TEXT("Shader"), [&]()
		{
			ImGui::Begin("Shader Settings");

			if (ImGui::Checkbox("Shadow", &m_isRenderShadow))
				m_pGameInstance->Set_EnableShadow(m_isRenderShadow);

            if (m_isRenderShadow)
            {
                if (ImGui::SliderFloat("Shadow Light Split", &m_ShadowDesc.fSplit, 0.1f, 100.f))
                    m_pGameInstance->Set_ShadowDesc(m_ShadowDesc);

                if (ImGui::SliderFloat3("Shadow Light Direction", reinterpret_cast<_float*>(&m_ShadowDesc.vLightDir), -1.f, 1.f))
                    m_pGameInstance->Set_ShadowDesc(m_ShadowDesc);

                if (ImGui::SliderFloat("Shadow Light Bias", &m_ShadowDesc.fBias, 0.001f, 0.01f))
                    m_pGameInstance->Set_ShadowDesc(m_ShadowDesc);

                if (ImGui::SliderFloat("Shadow Intensity", &m_ShadowDesc.fIntensity, 0.f, 1.f))
                    m_pGameInstance->Set_ShadowDesc(m_ShadowDesc);
            }

			//  if (m_isRenderShadow)
			//  {
            //      if (ImGui::SliderFloat2("Specular", reinterpret_cast<_float*>(&m_vSpecularPower), 0.f, 256.f, "%.0f"))
            //          m_pGameInstance->Set_SpecularPower(m_vSpecularPower);
            //  
			//  	if (ImGui::CollapsingHeader("Shadow Light"), ImGuiTreeNodeFlags_DefaultOpen)
			//  	{
			//  		if (ImGui::SliderFloat3("Direction", reinterpret_cast<_float*>(&m_CascadeConfig.vLightDir), -1.f, 1.f))
			//  			m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);
			//  	}
            //  
			//  	if (ImGui::CollapsingHeader("Cascade"), ImGuiTreeNodeFlags_DefaultOpen)
			//  	{
			//  		ImGui::Text("Manual Split Adjustment");
			//  		ImGui::Separator();
            //  
			//  		for (_uint i = 0; i < m_iNumCascades; ++i)
			//  		{
			//  			_float fMin = (i == 0) ? m_fCameraNear : m_CascadeConfig.Splits[i - 1];
			//  			_float fMax = (i == (m_iNumCascades - 1)) ? m_fCameraFar : m_CascadeConfig.Splits[i + 1];
            //  
			//  			_char szLabel[64] = {};
			//  			sprintf_s(szLabel, "Cascade %d Split Far", i);
            //  
			//  			if (ImGui::SliderFloat(szLabel, &m_CascadeConfig.Splits[i], fMin, fMax))
			//  				m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);
			//  		}
            //  
			//  		ImGui::Separator();
			//  		ImGui::Text("Auto Split Calculation");
            //  
			//  		if (ImGui::SliderFloat("Cascade Mix Lamda", &m_CascadeConfig.fLamda, 0.f, 1.f))
			//  			m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);
            //  
			//  		ImGui::Separator();
            //  
			//  		if (ImGui::SliderFloat("Shadow Bias", &m_CascadeConfig.fBias, 0.0001f, 0.005f, "%.4f"))
			//  			m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);
            //  
			//  		ImGui::Separator();
			//  		ImGui::Text("Shadow Intensity Lerp");
            //  
			//  		ImGui::SliderFloat("Shadow Transition Duration", &m_fShadowTransDuration, 0.1f, 10.f, "%.1f");
			//  		ImGui::SliderFloat("Shadow Target Intensity", &m_fTargetShadowIntensity, 0.f, 1.f, "%.1f");
            //  
			//  		if (ImGui::Button("Start Shadow Intensity Transition"))
			//  			m_pGameInstance->Start_ShadowTransition(m_fShadowTransDuration, m_fTargetShadowIntensity);
            //  
			//  		ImGui::Separator();
			//  	}
			//  }

			if (ImGui::Checkbox("SSAO", &m_isRenderSSAO))
				m_pGameInstance->Set_EnableSSAO(m_isRenderSSAO);

			if (m_isRenderSSAO)
			{
				_int iNumKernels = static_cast<_int>(m_SSAOConfig.iNumKernels);
				if (ImGui::InputInt("Sample Count", &iNumKernels, 4, 16))
				{
					m_SSAOConfig.iNumKernels = iNumKernels;
					m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);
				}

				if (ImGui::SliderFloat("Sample Radius", &m_SSAOConfig.fRadius, 0.1f, 2.f))
					m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);

				if (ImGui::SliderFloat("Intensity", &m_SSAOConfig.fIntensity, 0.5f, 4.f))
					m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);

				if (ImGui::SliderFloat("Contrast", &m_SSAOConfig.fConstrast, 0.5f, 2.f))
					m_pGameInstance->Set_SSAOConfig(m_SSAOConfig);

				ImGui::Separator();
			}

			if (ImGui::CollapsingHeader("Post Processing"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				if (ImGui::Checkbox("Fog", &m_isEnableFog))
					m_pGameInstance->Set_EnableFog(m_isEnableFog);

				if (m_isEnableFog)
				{
					_bool isChanged = {};
					_int iFogMode = static_cast<_int>(m_FogConfig.eType);

					isChanged |= ImGui::RadioButton("Linear", &iFogMode, static_cast<_int>(FOG_CONFIG::LINEAR));
					ImGui::SameLine();
					isChanged |= ImGui::RadioButton("Exp", &iFogMode, static_cast<_int>(FOG_CONFIG::EXP));
					ImGui::SameLine();
					isChanged |= ImGui::RadioButton("Exp Spuare", &iFogMode, static_cast<_int>(FOG_CONFIG::EXPSQUARE));

					if (true == isChanged)
					{
						m_FogConfig.eType = static_cast<FOG_CONFIG::TYPE>(iFogMode);
						m_pGameInstance->Set_FogConfig(m_FogConfig);
					}

					if (FOG_CONFIG::LINEAR == m_FogConfig.eType)
					{
						if (ImGui::SliderFloat("Near", &m_FogConfig.fNear, 0.1f, 100.f))
							m_pGameInstance->Set_FogConfig(m_FogConfig);

						if (ImGui::SliderFloat("Far", &m_FogConfig.fFar, m_FogConfig.fNear + 0.1f, 1000.f))
							m_pGameInstance->Set_FogConfig(m_FogConfig);

						if (m_FogConfig.fFar <= m_FogConfig.fNear)
						{
							m_FogConfig.fFar = m_FogConfig.fNear + 0.1f;
							m_pGameInstance->Set_FogConfig(m_FogConfig);
						}
					}
					else
					{
						if (ImGui::SliderFloat("Density", &m_FogConfig.fDensity, 0.0001f, 0.05f, "%.4f"))
							m_pGameInstance->Set_FogConfig(m_FogConfig);
					}

                    if (ImGui::SliderFloat("Fog Bias", &m_FogConfig.fBias, 0.f, 1.f, "%.1f"))
                        m_pGameInstance->Set_FogConfig(m_FogConfig);

					if (ImGui::ColorEdit4("Fog Color", reinterpret_cast<_float*>(&m_FogConfig.vColor)))
						m_pGameInstance->Set_FogConfig(m_FogConfig);

                    // Use Height
                    if (ImGui::Checkbox("Use Height Fog", &m_FogConfig.isUseHeight))
                        m_pGameInstance->Set_FogConfig(m_FogConfig);

                    if (true == m_FogConfig.isUseHeight)
                    {
                        if (ImGui::SliderFloat("Fog Base Height", &m_FogConfig.fBaseHeight, -1000.f, 3000.f))
                            m_pGameInstance->Set_FogConfig(m_FogConfig);

                        if (ImGui::SliderFloat("Fog Height Density", &m_FogConfig.fHeightDensity, 0.001f, 1.f))
                            m_pGameInstance->Set_FogConfig(m_FogConfig);
                    }

					ImGui::Separator();
					ImGui::Text("Fog Transition Lerp");

					ImGui::SliderFloat("Fog Transition Duration", &m_fFogTransDuration, 0.1f, 10.f, "%.2f");
					ImGui::SliderFloat("Fog Transition Density", &m_TargetFogDesc.fDensity, 0.0001f, 0.05f, "%.4f");
					ImGui::ColorEdit4("Fog Transition Color", reinterpret_cast<_float*>(&m_TargetFogDesc.vColor));

					if (ImGui::Button("Start Fog Transition"))
					{
						m_pGameInstance->Start_FogTransition(m_fFogTransDuration, m_TargetFogDesc);
						m_FogConfig.fDensity = m_TargetFogDesc.fDensity;
						m_FogConfig.vColor = m_TargetFogDesc.vColor;
					}

                    ImGui::SameLine();

                    if (ImGui::Button("Reset Fog"))
                    {
                        m_FogConfig = m_InitFogConfig;
                        m_pGameInstance->Set_FogConfig(m_InitFogConfig);
                    }

					ImGui::Separator();

					if (ImGui::Checkbox("Fog Noise", &m_FogConfig.Noise.isEnable))
					{
						if (m_isWorldSpaceFog)
						{
							m_FogConfig.Noise.vSpeed = { 0.05f, 0.f };
							m_FogConfig.Noise.vScale = { 0.05f, 0.05f };
						}
						else
						{
							m_FogConfig.Noise.vSpeed = { 0.01f, 0.f };
							m_FogConfig.Noise.vScale = { 1.f, 1.f };
						}

						m_pGameInstance->Set_FogConfig(m_FogConfig);
					}

					if (m_FogConfig.Noise.isEnable)
					{
						if (m_isWorldSpaceFog)
						{
							if (ImGui::SliderFloat2("Noise Speed", reinterpret_cast<_float*>(&m_FogConfig.Noise.vSpeed), -0.1f, 0.1f, "%.4f"))
								m_pGameInstance->Set_FogConfig(m_FogConfig);
						}
						else
						{
							if (ImGui::SliderFloat2("Noise Speed", reinterpret_cast<_float*>(&m_FogConfig.Noise.vSpeed), -1.f, 1.f, "%.2f"))
								m_pGameInstance->Set_FogConfig(m_FogConfig);
						}

						if (m_isWorldSpaceFog)
						{
							if (ImGui::SliderFloat2("Noise Scale", reinterpret_cast<_float*>(&m_FogConfig.Noise.vScale), 0.1f, 0.0001f, "%.4f"))
								m_pGameInstance->Set_FogConfig(m_FogConfig);
						}
						else
						{
							if (ImGui::SliderFloat2("Noise Scale", reinterpret_cast<_float*>(&m_FogConfig.Noise.vScale), 1.f, 5.f, "%.2f"))
								m_pGameInstance->Set_FogConfig(m_FogConfig);
						}

						if (ImGui::SliderFloat("Noise Strength", &m_FogConfig.Noise.fStrength, 0.0f, 1.0f, "%.2f"))
							m_pGameInstance->Set_FogConfig(m_FogConfig);

						if (ImGui::SliderFloat("Noise Contrast", &m_FogConfig.Noise.fContrast, 0.1f, 5.0f, "%.2f"))
							m_pGameInstance->Set_FogConfig(m_FogConfig);

						if (ImGui::CollapsingHeader("Noise Texture"), ImGuiTreeNodeFlags_DefaultOpen)
						{
							ImGui::BeginChild("Noise Texture", ImVec2(0, 70), true, ImGuiWindowFlags_HorizontalScrollbar);

							for (_uint i = 0; i < m_pGameInstance->Get_NumFogNoiseTextures(); ++i)
							{
								ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_FogNoiseTexture(i);

								if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
									m_pGameInstance->Set_FogNoiseTextureIndex(i);

								ImGui::SameLine();
							}

							ImGui::EndChild();
						}

						ImGui::NewLine();
					}

					//  _bool isChangedSpace{};
                    //  
					//  isChangedSpace |= ImGui::Checkbox("Fog World Space", &m_isWorldSpaceFog);
                    //  
					//  if (isChangedSpace)
					//  {
					//  	m_pGameInstance->Set_FogNoiseWorldSpace(m_isWorldSpaceFog);
                    //  
					//  	if (m_isWorldSpaceFog)
					//  	{
					//  		m_FogConfig.Noise.vSpeed = { 0.05f, 0.f };
					//  		m_FogConfig.Noise.vScale = { 0.05f, 0.05f };
					//  	}
					//  	else
					//  	{
					//  		m_FogConfig.Noise.vSpeed = { 0.01f, 0.f };
					//  		m_FogConfig.Noise.vScale = { 1.f, 1.f };
					//  	}
					//  }

					//	m_pGameInstance->Set_FogConfig(m_FogConfig);

                    if (ImGui::Checkbox("Edge", &m_isEnableEdge))
                    {
                        CYetuga* pYetuga = dynamic_cast<CYetuga*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel),
                            TEXT("Layer_MapObject"), 12));
                        CBody_Yetuga* pYetugaBody = dynamic_cast<CBody_Yetuga*>(pYetuga->Find_PartObject(TEXT("Part_Body")));
                        pYetugaBody->Set_EnableEdge(m_isEnableEdge);

                        CKhazan_Spear* pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel),
                            TEXT("Layer_Creature_Player"), 0));
                        CBody_Khazan_Spear* pKhazanBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                        pKhazanBody->Set_EnableEdge(m_isEnableEdge);
                    }

                    if (ImGui::Checkbox("Rim Light", &m_isEnableRimLight))
                        m_pGameInstance->Set_EnableRimLight(m_isEnableRimLight);

                    if (m_isEnableRimLight)
                    {
                        if (ImGui::SliderFloat("RimLight Power", &m_RimLightDesc.fPower, 0.f, 10.f, "%.3f"))
                            m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);

                        if (ImGui::Checkbox("Toon Light", &m_RimLightDesc.isToonLight))
                            m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);

                        if (m_RimLightDesc.isToonLight)
                            if (ImGui::SliderFloat("RimLight Toon Threshold", &m_RimLightDesc.fToonThreshold, 0.f, 1.f, "%.3f"))
                                m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);

                        if (ImGui::SliderFloat("RimLight Intensity", &m_RimLightDesc.fIntensity, 0.f, 1.f, "%.4f"))
                            m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);
                    }

					ImGui::Separator();
				}

                if (ImGui::Checkbox("LUT", &m_isEnableLUT))
                    m_pGameInstance->Set_EnableLUT(m_isEnableLUT);

                if (m_isEnableLUT)
                {
                    // LUT 강도
                    if (ImGui::SliderFloat("LUT Intensity", &m_fLUTIntensity, 0.f, 1.f, "%.2f"))
                        m_pGameInstance->Set_LUTIntensity(m_fLUTIntensity);
                }

                ImGui::Checkbox("Radial Blur", &m_isEnableRadialBlur);

                if (m_isEnableRadialBlur)
                {
                    // 중심 UV
                    ImGui::SliderFloat2("Radial Blur Center UV", reinterpret_cast<_float*>(&m_RadialBlurDesc.vCenterUV), 0.f, 1.f, "%.1f");

                    // 반경
                    ImGui::SliderFloat("Radial Blur Radius", &m_RadialBlurDesc.fSampleRadius, 0.01f, 1.5f, "%.2f");

                    // 마스크 반경
                    ImGui::SliderFloat("Radial Blur Mask Radius Iner", &m_RadialBlurDesc.vMaskRadius.x, 0.f, 1.f, "%.3f");
                    ImGui::SliderFloat("Radial Blur Mask Radius Outer", &m_RadialBlurDesc.vMaskRadius.y, 0.f, 1.f, "%.3f");

                    // 마스크 지수(곡선 강화)
                    ImGui::SliderFloat("Radial Blur Mask Exponent", &m_RadialBlurDesc.fExponent, 1.f, 5.f, "%.2f");

                    // 샘플 개수
                    _int iNumSamples = static_cast<_int>(m_RadialBlurDesc.iNumSamples);
                    if (ImGui::InputInt("Radial Blur Num Samples", &iNumSamples, 2, 4))
                        m_RadialBlurDesc.iNumSamples = iNumSamples;

                    // 감쇠
                    ImGui::SliderFloat("Radial Blur Attenuation", &m_RadialBlurDesc.fAttenuation, 0.1f, 5.f, "%.2f");

                    // 블러 강도
                    ImGui::SliderFloat("Radial Blur Strength", &m_RadialBlurDesc.fStrength, 0.f, 1.f, "%.2f");

                    // 지속 시간
                    ImGui::SliderFloat("Radial Blur Duration", &m_RadialBlurDesc.fDuration, 0.f, 10.f, "%.2f");

                    // 페이드 시간
                    ImGui::SliderFloat2("Radial Blur Fade Time", reinterpret_cast<_float*>(&m_RadialBlurDesc.vFadeTime), 0.f, 5.f, "%.1f");

                    if (ImGui::Button("Start Radial Blur Animation"))
                        m_pGameInstance->Start_RadialBlur(m_RadialBlurDesc);

                    ImGui::Separator();
                }

                // ===== Motion Blur =====

                if (ImGui::Checkbox("Motion Blur", &m_isEnableMotionBlur))
                    m_pGameInstance->Set_EnableMotionBlur(m_isEnableMotionBlur);

                if (m_isEnableMotionBlur)
                {
                    // 깊이 바이어스
                    if (ImGui::SliderFloat("Motion Blur Bias", &m_MotionBlurDesc.fDepthBias, 0.f, 0.1f, "%.3f"))
                        m_pGameInstance->Set_MotionBlurDesc(m_MotionBlurDesc);

                    // 샘플 개수
                    _int iNumSamples = static_cast<_int>(m_MotionBlurDesc.iNumSamples);
                    if (ImGui::InputInt("Motion Blur Num Samples", &iNumSamples, 2, 4))
                    {
                        m_MotionBlurDesc.iNumSamples = iNumSamples;
                        m_pGameInstance->Set_MotionBlurDesc(m_MotionBlurDesc);
                    }

                    // 블러 강도
                    if (ImGui::SliderFloat("Motion Blur Strength", &m_MotionBlurDesc.fStrength, 0.f, 1.f, "%.1f"))
                        m_pGameInstance->Set_MotionBlurDesc(m_MotionBlurDesc);

                    ImGui::Separator();
                }
			}

			if (ImGui::CollapsingHeader("Cartoon Rendering"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				if (ImGui::Checkbox("Toon Shading", &m_isEnableToonShade))
					m_pGameInstance->Set_EnableToonShade(m_isEnableToonShade);

				if (m_isEnableToonShade)
				{
					if (ImGui::SliderFloat("Toon Shade Level", &m_fToonShadeLevel, 1.f, 5.f, "%.0f"))
						m_pGameInstance->Set_ToonShadeLevel(m_fToonShadeLevel);
				}

				if (ImGui::Checkbox("Outline", &m_isEnableOutline))
					m_pGameInstance->Set_EnableOutline(m_isEnableOutline);

				if (m_isEnableOutline)
				{
					if (ImGui::SliderFloat("Outline Size", &m_OutlineConfig.fSize, 0.0001f, 0.01f, "%.4f"))
					{
						CKhazan_Spear* pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel),
							TEXT("Layer_Creature_Player"), 0));
						CBody_Khazan_Spear* pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
						pBody->Set_OutlineConfig(m_OutlineConfig);
						/*dynamic_cast<CBody_Khazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel),
							TEXT("Layer_Creature_Test"), 0))->Set_OutlineConfig(m_OutlineConfig);*/
					}
					if (ImGui::ColorEdit3("Outline Color", reinterpret_cast<_float*>(&m_OutlineConfig.vColor)))
					{
						CKhazan_Spear* pKhazan = dynamic_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel),
							TEXT("Layer_Creature_Player"), 0));
						CBody_Khazan_Spear* pBody = dynamic_cast<CBody_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
						pBody->Set_OutlineConfig(m_OutlineConfig);

						/*dynamic_cast<CBody_Khazan_Sample*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(m_eCurrentLevel),
							TEXT("Layer_Creature_Test"), 0))->Set_OutlineConfig(m_OutlineConfig);*/
					}
						

					if (ImGui::SliderFloat("Outline Alpha", &m_OutlineConfig.fAlpha, 0.f, 1.f, "%.2f"))
						m_pGameInstance->Set_OutlineConfig(m_OutlineConfig);

					if (ImGui::SliderFloat("Outline Bias", &m_OutlineConfig.fBias, 0.001f, 0.1f, "%.3f"))
						m_pGameInstance->Set_OutlineConfig(m_OutlineConfig);
				}

				ImGui::Separator();
			}

            if (ImGui::CollapsingHeader("Motion Trail"), ImGuiTreeNodeFlags_DefaultOpen)
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
                    if (ImGui::Checkbox("Enable Motion Trail", &m_isEnableMotionTrail))
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

                    isChanged |= ImGui::SliderFloat("Motion Trail LifeTime", &m_MotionTrailConfig.vLifeTime.y, 0.f, 10.f, "%.3f");

                    isChanged |= ImGui::ColorEdit3("Motion Trail Start Color", reinterpret_cast<_float*>(&m_MotionTrailConfig.vStartColor));
                    isChanged |= ImGui::ColorEdit3("Motion Trail Target Color", reinterpret_cast<_float*>(&m_MotionTrailConfig.vTargetColor));

                    isChanged |= ImGui::SliderFloat("Motion Trail Rim Power", &m_MotionTrailConfig.fRimPower, 0.f, 10.f, "%.2f");
                    isChanged |= ImGui::SliderFloat("Motion Trail Rim Intensity", &m_MotionTrailConfig.fRimIntensity, 0.f, 10.f, "%.2f");
                    isChanged |= ImGui::SliderFloat("Motion Trail Emissive Intensity", &m_MotionTrailConfig.fEmissiveIntensity, 0.f, 5.f, "%.2f");

                    isChanged |= ImGui::Checkbox("Individual Color", &m_MotionTrailConfig.isIndividualColor);

                    if (false == m_MotionTrailConfig.isIndividualColor)
                        isChanged |= ImGui::SliderFloat("Motion Trail Color Update Speed", &m_MotionTrailConfig.fColorUpdateSpeed, 1.f, 1000.f, "%.1f");

                    isChanged |= ImGui::SliderFloat("Motion Trail Interval", &m_MotionTrailConfig.fInterval, 0.f, 5.f, "%.3f");
                    isChanged |= ImGui::SliderInt("Motion Trail Max Frame Snapshot", reinterpret_cast<_int*>(&m_MotionTrailConfig.iMaxFrames), 1, 10);

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

                    ImGui::SliderFloat("Motion Trail Duration", &m_fMotionTrailDuration, 0.f, 10.f, "%.2f");

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

                ImGui::Separator();
            }

            if (ImGui::CollapsingHeader("Light Settings", ImGuiTreeNodeFlags_DefaultOpen))
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

                    // 리스트 박스 렌더링 (캐시된 m_szLightTags 사용)
                    if (ImGui::ListBox("Light List", &iCurrentLightIndex, m_szLightTags.data(), static_cast<_int>(m_szLightTags.size())))
                    {
                        if (iCurrentLightIndex >= 0 && iCurrentLightIndex < m_wstrLightTags.size())
                        {
                            m_strSelectedLightTag = m_wstrLightTags[iCurrentLightIndex];
                        }
                    }

                    ImGui::Separator();

                    // 4. 선택된 조명 속성 편집
                    if (!m_strSelectedLightTag.empty())
                    {
                        const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(m_strSelectedLightTag, iCurrentLevelIndex);
                        m_isEnableLight = m_pGameInstance->Is_LightEnable(m_strSelectedLightTag, iCurrentLevelIndex);

                        _bool isChanged = { false };

                        if (pLightDesc)
                        {
                            ImGui::TextColored(ImVec4(0.2f, 1.f, 0.2f, 1.f), "Selected: %ws", m_strSelectedLightTag.c_str());

                            if (ImGui::Checkbox("Light Enable", &m_isEnableLight))
                            {
                                m_pGameInstance->Set_LightEnable(m_strSelectedLightTag, iCurrentLevelIndex, m_isEnableLight);
                            }

                            // 데이터 수정을 위한 복사본 생성
                            LIGHT_DESC EditedDesc = *pLightDesc;

                            ImGui::Text("Type: %s", (EditedDesc.eType == LIGHT_DESC::DIRECTIONAL) ? "DIRECTIONAL" : "POINT");

                            // ImGuiColorEditFlags_HDR: HDR 모드 활성화 (1.0 초과 값 허용)
                            // ImGuiColorEditFlags_Float: 0~255가 아닌 실수형 입력
                            ImGuiColorEditFlags HDRFlags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float;

                            isChanged |= ImGui::ColorEdit4("Diffuse (HDR)", reinterpret_cast<_float*>(&EditedDesc.vDiffuse), HDRFlags);
                            isChanged |= ImGui::ColorEdit4("Ambient (HDR)", reinterpret_cast<_float*>(&EditedDesc.vAmbient), HDRFlags);
                            isChanged |= ImGui::ColorEdit4("Specular (HDR)", reinterpret_cast<_float*>(&EditedDesc.vSpecular), HDRFlags);

                            // 유형별 속성
                            if (EditedDesc.eType == LIGHT_DESC::DIRECTIONAL)
                            {
                                ImGui::Text("Direction : ");
                                isChanged |= ImGui::SliderFloat3("Dir", reinterpret_cast<_float*>(&EditedDesc.vDirection), -1.f, 1.f);
                            }
                            else if (EditedDesc.eType == LIGHT_DESC::POINT)
                            {
                                isChanged |= ImGui::SliderFloat3("Pos", reinterpret_cast<_float*>(&EditedDesc.vPosition), -300.f, 300.f, "%.1f");

                                isChanged |= ImGui::SliderFloat("Range", &EditedDesc.fRange, 0.0f, 100.f, "%.1f");
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

                    ImGui::Separator();

                    // Light Transition
                    if (ImGui::CollapsingHeader("Light Transition"))
                    {
                        ImGui::SliderFloat("Light Trans Duration", &m_TargetLightDesc.fDuration, 0.f, 20.f, "%.1f");

                        ImGui::SliderFloat2("Light Trans FadeTime (In / Out)", reinterpret_cast<_float*>(&m_TargetLightDesc.vFadeTime), 0.1f, 10.f, "%.1f");

                        ImGuiColorEditFlags HDRFlags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float;

                        ImGui::ColorEdit4("Target Diffuse (HDR)", reinterpret_cast<_float*>(&m_TargetLightDesc.vDiffuse), HDRFlags);
                        ImGui::ColorEdit4("Target Ambient (HDR)", reinterpret_cast<_float*>(&m_TargetLightDesc.vAmbient), HDRFlags);
                        ImGui::ColorEdit4("Target Specular (HDR)", reinterpret_cast<_float*>(&m_TargetLightDesc.vSpecular), HDRFlags);

                        ImGui::SliderInt("Light Blink Count", reinterpret_cast<_int*>(&m_TargetLightDesc.iBlinkCount), 0, 10);

                        ImGui::Checkbox("Return To Start", &m_TargetLightDesc.isReturnToStart);
                        ImGui::SameLine();
                        ImGui::Checkbox("Restore Light", &m_isRestoreLight);

                        if (ImGui::Button("Start Light Transition"))
                            m_pGameInstance->Start_LightTransition(m_strSelectedLightTag, iCurrentLevelIndex, m_TargetLightDesc, m_isRestoreLight);
                    }
                }

                ImGui::Separator();
            }

			ImGui::End();
		});
#endif
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
