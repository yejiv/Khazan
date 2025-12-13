#include "Level_Shader.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Shader.h"
#include "Prop_Object.h"
#include "JOH_EditorModelTest.h"
#include "Player_Shader.h"
#include "E_Body_Khazan_Spear.h"
#include "E_Khazan_Spear.h"
#include "MapEditor_Header.h"

CLevel_Shader::CLevel_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Shader::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	// Decal
	if (FAILED(m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Decal"),
		ENUM_CLASS(LEVEL::SHADER), TEXT("Pool_Decal"), nullptr, 10)))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

#pragma region 테스트용 ( 박준영이 남기고 간거 )
	//  CHECK_FAILED(Ready_Lights(TEXT("Test"), LEVEL::SHADER), E_FAIL);
	
	CHECK_FAILED(Ready_Layer_MapObject(TEXT("Layer_MapObject"), TEXT("Test"), LEVEL::SHADER), E_FAIL);
	
	CHECK_FAILED(Ready_Layer_MapObject_Inst(TEXT("Layer_MapObject_Inst"), TEXT("Test"), LEVEL::SHADER), E_FAIL);

    //  CHECK_FAILED(Ready_Layer_MapObject_Interactive(TEXT("Layer_Interact"), TEXT("Test"), LEVEL::SHADER), E_FAIL);
#pragma endregion

	m_CascadeConfig.Splits.resize(m_iNumCascades);
	m_CascadeConfig = m_pGameInstance->Get_CascadeConfig();
	m_SSAOConfig = m_pGameInstance->Get_SSAOConfig();
	m_GaussianBlurConfig = m_pGameInstance->Get_GaussianBlurConfig();
	m_FogConfig = m_pGameInstance->Get_FogConfig();
	//  OUTLINE_CONFIG PlayerOutlineConfig = dynamic_cast<CPlayer_Shader*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
	//  	TEXT("Layer_Player"), 0))->Get_OutlineConfig();
	//  m_OutlineConfig.vColor = PlayerOutlineConfig.vColor;
	//  m_OutlineConfig.fSize = PlayerOutlineConfig.fSize;
	//  OUTLINE_CONFIG RendererOutlineConfig = m_pGameInstance->Get_OutlineConfig();
	//  m_OutlineConfig.fAlpha = RendererOutlineConfig.fAlpha;
	//  m_OutlineConfig.fBias = RendererOutlineConfig.fBias;
	m_VignetteConfig = m_pGameInstance->Get_VignetteConfig();
	
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
    m_RadialBlurDesc = m_pGameInstance->Get_RadialBlurDesc();
    m_MotionBlurDesc = m_pGameInstance->Get_MotionBlurDesc();
    m_RimLightDesc = m_pGameInstance->Get_RimLightDesc();

	m_iNumCascades = m_pGameInstance->Get_NumCascades();

    // Test Blink Desc
    m_TargetLightDesc.vDiffuse = _float4(10.f, 9.f, 8.f, 1.f);
    m_TargetLightDesc.vAmbient = _float4(1.f, 0.9f, 0.8f, 1.f);
    m_TargetLightDesc.vSpecular = m_TargetLightDesc.vDiffuse;

    // Fog, Shadow Off
    m_pGameInstance->Set_EnableFog(m_isEnableFog);
    m_pGameInstance->Set_EnableShadow(m_isRenderShadow);

	m_pGameInstance->AddWidget(TEXT("Shader"), [&]()
	{
		ImGui::Begin("Shader Settings");

		if (ImGui::CollapsingHeader("Frame Per Second"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			ImGui::SetWindowFontScale(2.f);
			ImGui::Text("%s", m_szFPS);
			ImGui::SetWindowFontScale(1.f);
			ImGui::Separator();
		}

		if (ImGui::Checkbox("Shadow", &m_isRenderShadow))
			m_pGameInstance->Set_EnableShadow(m_isRenderShadow);

		if (m_isRenderShadow)
		{
			if (ImGui::CollapsingHeader("Shadow Light"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				if (ImGui::SliderFloat3("Direction", reinterpret_cast<_float*>(&m_CascadeConfig.vLightDir), -1.f, 1.f))
					m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);
			}

			if (ImGui::CollapsingHeader("Cascade"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				ImGui::Text("Manual Split Adjustment");
				ImGui::Separator();

				for (_uint i = 0; i < m_iNumCascades; ++i)
				{
					_float fMin = (i == 0) ? m_fCameraNear : m_CascadeConfig.Splits[i - 1];
					_float fMax = (i == (m_iNumCascades - 1)) ? m_fCameraFar : m_CascadeConfig.Splits[i + 1];

					_char szLabel[64] = {};
					sprintf_s(szLabel, "Cascade %d Split Far", i);

					if (ImGui::SliderFloat(szLabel, &m_CascadeConfig.Splits[i], fMin, fMax))
						m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);
				}

				ImGui::Separator();
				ImGui::Text("Auto Split Calculation");

				if (ImGui::SliderFloat("Cascade Mix Lamda", &m_CascadeConfig.fLamda, 0.f, 1.f))
					m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);

				ImGui::Separator();

				if (ImGui::SliderFloat("Shadow Bias", &m_CascadeConfig.fBias, 0.0001f, 0.005f, "%.4f"))
					m_pGameInstance->Set_CascadeConfig(m_CascadeConfig);

				ImGui::Separator();
			}
		}

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
			if (ImGui::Checkbox("Emissive", &m_isEnableEmissive))
				dynamic_cast<CPlayer_Shader*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
					TEXT("Layer_Player"), 0))->Set_EnableEmissive(m_isEnableEmissive);

			if (m_isEnableEmissive)
			{
				if (ImGui::SliderFloat("Brightness", &m_fEmissiveIntensity, 0.f, 10.f))
				{
					dynamic_cast<CPlayer_Shader*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
						TEXT("Layer_Player"), 0))->Set_EmissiveIntensity(m_fEmissiveIntensity);
				}

                // 가우시안 블러 범위(반경)
                if (ImGui::InputInt("Gaussian Blur Radius", &m_GaussianBlurConfig.iRadius, 2, 4))
                    m_pGameInstance->Set_GaussianBlurConfig(m_GaussianBlurConfig);

                // 가우시안 블러 가중치 밀집도
                if (ImGui::SliderFloat("Gaussian Blur Concentration", &m_GaussianBlurConfig.fSigma, 1.f, 10.f))
                    m_pGameInstance->Set_GaussianBlurConfig(m_GaussianBlurConfig);

                // 가우시안 블러 가중치 합 정규화 수치
                if (ImGui::SliderFloat("Gaussian Blur Normalization", &m_GaussianBlurConfig.fNormalization, 0.f, 15.f))
                    m_pGameInstance->Set_GaussianBlurConfig(m_GaussianBlurConfig);

                ImGui::Separator();
			}

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

				if (ImGui::ColorEdit4("Fog Color", reinterpret_cast<_float*>(&m_FogConfig.vColor)))
					m_pGameInstance->Set_FogConfig(m_FogConfig);

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

					if (ImGui::CollapsingHeader("Fog Noise"), ImGuiTreeNodeFlags_DefaultOpen)
					{
						ImGui::BeginChild("Fog Noise", ImVec2(0, 70), true, ImGuiWindowFlags_HorizontalScrollbar);

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

				m_pGameInstance->Set_FogConfig(m_FogConfig);

				ImGui::Separator();
			}

			if (ImGui::Checkbox("Vignette", &m_isEnableVignette))
				m_pGameInstance->Set_EnableVignette(m_isEnableVignette);

			if (m_isEnableVignette)
			{
				// 라디오 버튼으로 애니메이션 모드 고르기
				_bool isChanged = {};
				_int iVignetteMode = static_cast<_int>(m_VignetteConfig.eMode);

				isChanged |= ImGui::RadioButton("Smooth_Smooth", &iVignetteMode, static_cast<_int>(VIGNETTE_CONFIG::SMOOTH_SMOOTH));
				ImGui::SameLine();
				isChanged |= ImGui::RadioButton("Smooth_Intant", &iVignetteMode, static_cast<_int>(VIGNETTE_CONFIG::SMOOTH_INTANT));
				ImGui::SameLine();
				isChanged |= ImGui::RadioButton("Intant_Smooth", &iVignetteMode, static_cast<_int>(VIGNETTE_CONFIG::INTANT_SMOOTH));
				ImGui::SameLine();
				isChanged |= ImGui::RadioButton("None", &iVignetteMode, static_cast<_int>(VIGNETTE_CONFIG::NONE));

				if (true == isChanged)
					m_VignetteConfig.eMode = static_cast<VIGNETTE_CONFIG::ANIMMODE>(iVignetteMode);

                ImGui::ColorEdit3("Vignette Color", reinterpret_cast<_float*>(&m_VignetteConfig.vColor));

                ImGui::SliderFloat("Vignette Power", &m_VignetteConfig.fPower, 0.f, 10.f, "%.2f");

                ImGui::SliderFloat("Vignette Intensity", &m_VignetteConfig.fIntensity, 0.f, 10.f, "%.2f");

				// 최대 강도
                ImGui::SliderFloat("Vignette Max Intensity", &m_VignetteConfig.fMaxIntensity, 0.f, 10.f, "%.2f");
				
                // 노이즈 사용 여부
                ImGui::Checkbox("Use Vignette Noise", &m_VignetteConfig.isUseNoise);

                // 텍스처 인덱스
                ImGui::SliderInt("Vignette Texture Index", reinterpret_cast<_int*>(&m_VignetteConfig.iTextureIndex), 0, 3);

                // 대비
                ImGui::SliderFloat("Vignette Contrast", &m_VignetteConfig.fContrast, 0.f, 10.f, "%.2f");

				// 듀레이션
				ImGui::SliderFloat("Vignette Duration", &m_fVignetteAnimDuration, 0.f, 5.f, "%.2f");

				// 스타트 버튼
				if (ImGui::Button("Start Vignette"))
					m_pGameInstance->Start_VignetteAnimation(m_fVignetteAnimDuration, m_VignetteConfig);
                
                ImGui::Separator();
            }

            if (ImGui::Checkbox("LUT", &m_isEnableLUT))
                m_pGameInstance->Set_EnableLUT(m_isEnableLUT);

            if (m_isEnableLUT)
            {
                // LUT 강도
                if (ImGui::SliderFloat("LUT Intensity", &m_fLUTIntensity, 0.f, 1.f, "%.2f"));
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

            if (ImGui::Checkbox("Deferred Rim Light", &m_isEnableRimLight))
                m_pGameInstance->Set_EnableRimLight(m_isEnableRimLight);

            if (m_isEnableRimLight)
            {
                if (ImGui::SliderFloat("RimLight Power", &m_RimLightDesc.fPower, 0.f, 10.f, "%.2f"))
                    m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);
                
                if (ImGui::Checkbox("Toon Light", &m_RimLightDesc.isToonLight))
                    m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);

                if (m_RimLightDesc.isToonLight)
                    if (ImGui::SliderFloat("RimLight Toon Threshold", &m_RimLightDesc.fToonThreshold, 0.f, 1.f, "%.2f"))
                        m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);

                if (ImGui::SliderFloat("RimLight Intensity", &m_RimLightDesc.fIntensity, 0.f, 1.f, "%.2f"))
                    m_pGameInstance->Set_RimLightDesc(m_RimLightDesc);
            }

            if (ImGui::Checkbox("Edge", &m_isEnableEdge))
            {
                CE_Khazan_Spear* pKhazan = dynamic_cast<CE_Khazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
                    TEXT("Layer_Player"), 0));
                CE_Body_Khazan_Spear* pKhazanBody = dynamic_cast<CE_Body_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                pKhazanBody->Set_EnableEdge(m_isEnableEdge);
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

			//  if (ImGui::Checkbox("Outline", &m_isEnableOutline))
			//  	m_pGameInstance->Set_EnableOutline(m_isEnableOutline);
            //  
			//  if (m_isEnableOutline)
			//  {
			//  	if (ImGui::SliderFloat("Outline Size", &m_OutlineConfig.fSize, 0.001f, 0.01f, "%.3f"))
			//  		dynamic_cast<CPlayer_Shader*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
			//  			TEXT("Layer_Player"), 0))->Set_OutlineConfig(m_OutlineConfig);
            //  
			//  	if (ImGui::ColorEdit3("Outline Color", reinterpret_cast<_float*>(&m_OutlineConfig.vColor)))
			//  		dynamic_cast<CPlayer_Shader*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
			//  			TEXT("Layer_Player"), 0))->Set_OutlineConfig(m_OutlineConfig);
            //  
			//  	if (ImGui::SliderFloat("Outline Alpha", &m_OutlineConfig.fAlpha, 0.f, 1.f, "%.2f"))
			//  		m_pGameInstance->Set_OutlineConfig(m_OutlineConfig);
            //  
			//  	if (ImGui::SliderFloat("Outline Bias", &m_OutlineConfig.fBias, 0.001f, 0.1f, "%.3f"))
			//  		m_pGameInstance->Set_OutlineConfig(m_OutlineConfig);
			//  }

			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader("Decal"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			// 라이프 타임
			ImGui::SliderFloat("Decal LifeTime", &m_DecalDesc.fLifeTime, 1.f, 20.f, "%.0f");

			// 페이드 타임
			ImGui::SliderFloat2("Decal FadeTime (In / Out)", reinterpret_cast<_float*>(&m_DecalDesc.vFadeTime), 0.1f, 10.f, "%.1f");

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

			// 바운딩 박스 사이즈
			ImGui::SliderFloat3("Decal Bounding Box Size", reinterpret_cast<_float*>(&m_DecalDesc.vScale), 1.f, 50.f, "%.0f");
			
			// 컬러
            if (DECALTYPE::EMISSIVE == m_DecalDesc.eType)
            {
                ImGui::ColorEdit3("Base Color", reinterpret_cast<_float*>(&m_DecalDesc.EmissiveDesc.vBaseColor));
                ImGui::ColorEdit3("Emissive Color", reinterpret_cast<_float*>(&m_DecalDesc.EmissiveDesc.vEmissiveColor));
                ImGui::ColorEdit3("Border Color", reinterpret_cast<_float*>(&m_DecalDesc.EmissiveDesc.vBorderColor));
                ImGui::SliderFloat("Decal Emissive Mask Power", &m_DecalDesc.EmissiveDesc.fEmissiveMaskPower, 1.f, 10.f, "%.2f");
                ImGui::SliderFloat("Decal Emissive Intensity", &m_DecalDesc.EmissiveDesc.fEmissiveIntensity, 1.f, 10.f, "%.2f");
            }
            else
			    ImGui::ColorEdit3("Decal Color", reinterpret_cast<_float*>(&m_DecalDesc.vColor));		

            ImGui::Checkbox("Random Texture", &m_DecalDesc.isRandomTexture);

            if (false == m_DecalDesc.isRandomTexture)
            {
                ImGui::BeginChild("Decal Texture", ImVec2(0, 70), true, ImGuiWindowFlags_HorizontalScrollbar);

                for (_uint i = 0; i < m_pGameInstance->Get_NumDecalTextures(m_DecalDesc.eType); ++i)
                {
                    ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_DecalTexture(m_DecalDesc.eType, i);

                    if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
                        m_DecalDesc.iTextureIndex = i;

                    ImGui::SameLine();
                }

                ImGui::EndChild();
            }

			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader("Distortion"), ImGuiTreeNodeFlags_DefaultOpen)
		{
			// ����
			ImGui::SliderFloat("Distortion Range", &m_DistortionDesc.fRange, 0.f, 1.f, "%.2f");

			// ����
			ImGui::SliderFloat("Distortion Power", &m_DistortionDesc.fPower, 0.f, 10.f, "%.2f");

			// ���� �ð�
			ImGui::SliderFloat("Distortion Duration", &m_DistortionDesc.fDuration, 0.f, 20.f, "%.1f");

			// ���̵� Ÿ��
			ImGui::SliderFloat2("Distortion FadeTime (In / Out)", reinterpret_cast<_float*>(&m_DistortionDesc.vFadeTime), 0.1f, 10.f, "%.1f");

			// ���ǵ�
			ImGui::SliderFloat("Distortion fSpeed", &m_DistortionDesc.fSpeed, 0.f, 10.f, "%.2f");
			
			// ������ �ؽ�ó
			if (ImGui::CollapsingHeader("Distortion Noise"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				ImGui::BeginChild("Distortion Noise", ImVec2(0, 70), true, ImGuiWindowFlags_HorizontalScrollbar);

				for (_uint i = 0; i < m_pGameInstance->Get_NumDistortionNoiseTextures(); ++i)
				{
					ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_DistortionNoiseTexture(i);

					if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(pSRV), ImVec2(32, 32)))
						m_DistortionDesc.iNoiseIndex = i;

					ImGui::SameLine();
				}

				ImGui::EndChild();
			}

			if (ImGui::Button("Start Distortion"))
				m_pGameInstance->Start_Distortion(m_DistortionDesc);

			ImGui::Separator();
		}

        if (ImGui::CollapsingHeader("Motion Trail"), ImGuiTreeNodeFlags_DefaultOpen)
        {
            // 리스트 박스로 Player, Yetuga, Viper 등 고르기 (현재는 플레이어밖에 없음)
            const _char* ObjectTags[] = { "Player", "Yetuga", "Viper" };
            ImGui::Combo("GameObject List", &m_iCurrentGameObjectIndex, ObjectTags, IM_ARRAYSIZE(ObjectTags));

            // 고르면 해당 객체의 모션 트레일 정보 Get해서 띄우기
            if (0 == m_iCurrentGameObjectIndex)
            {
                CE_Khazan_Spear* pKhazan = dynamic_cast<CE_Khazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
                    TEXT("Layer_Player"), 0));
                CE_Body_Khazan_Spear* pKhazanBody = dynamic_cast<CE_Body_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                m_MotionTrailConfig = pKhazanBody->Get_MotionTrailConfig();
                m_isEnableMotionTrail = pKhazanBody->isEnableMotionTrail();
            }
            else
            {
                m_iCurrentGameObjectIndex = -1;
                m_isEnableMotionTrail = false;
            }

            if (0 <= m_iCurrentGameObjectIndex)
            {
                // 특정 설정이 바뀔 때 Set
                _bool isChanged = false;

                // bool Enable(체크박스)
                if (ImGui::Checkbox("Enable Motion Trail", &m_isEnableMotionTrail))
                {
                    CE_Khazan_Spear* pKhazan = dynamic_cast<CE_Khazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
                        TEXT("Layer_Player"), 0));
                    CE_Body_Khazan_Spear* pKhazanBody = dynamic_cast<CE_Body_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                    pKhazanBody->Set_EnableMotionTrail(m_isEnableMotionTrail);
                }

                // float2 vLifeTime
                // float3 Start / Target Color
                // float Rim Power
                // float Rim Intensity
                // float Emissive Intensity
                // bool Individual(컬러 보간을 개별적으로 적용할 건지 체크 박스)
                // float Color Update Speed 위에 체크 되었을 때, 색 바뀌는 속도
                // float Interval(스폰 주기)
                // uint MaxFrames

                isChanged |= ImGui::SliderFloat("Motion Trail LifeTime", &m_MotionTrailConfig.vLifeTime.y, 0.1f, 10.f, "%.1f");

                isChanged |= ImGui::ColorEdit3("Motion Trail Start Color", reinterpret_cast<_float*>(&m_MotionTrailConfig.vStartColor));
                isChanged |= ImGui::ColorEdit3("Motion Trail Target Color", reinterpret_cast<_float*>(&m_MotionTrailConfig.vTargetColor));

                isChanged |= ImGui::SliderFloat("Motion Trail Rim Power", &m_MotionTrailConfig.fRimPower, 0.f, 10.f, "%.2f");
                isChanged |= ImGui::SliderFloat("Motion Trail Rim Intensity", &m_MotionTrailConfig.fRimIntensity, 0.f, 10.f, "%.2f");
                isChanged |= ImGui::SliderFloat("Motion Trail Emissive Intensity", &m_MotionTrailConfig.fEmissiveIntensity, 0.f, 5.f, "%.2f");

                isChanged |= ImGui::Checkbox("Individual Color", &m_MotionTrailConfig.isIndividualColor);

                if (false == m_MotionTrailConfig.isIndividualColor)
                    isChanged |= ImGui::SliderFloat("Motion Trail Color Update Speed", &m_MotionTrailConfig.fColorUpdateSpeed, 1.f, 1000.f, "%.1f");

                isChanged |= ImGui::SliderFloat("Motion Trail Interval", &m_MotionTrailConfig.fInterval, 0.1f, 5.f, "%.2f");
                isChanged |= ImGui::SliderInt("Motion Trail Max Frame Snapshot", reinterpret_cast<_int*>(&m_MotionTrailConfig.iMaxFrames), 1, 10);

                if (true == isChanged)
                {
                    CE_Khazan_Spear* pKhazan = dynamic_cast<CE_Khazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
                        TEXT("Layer_Player"), 0));
                    CE_Body_Khazan_Spear* pKhazanBody = dynamic_cast<CE_Body_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                    pKhazanBody->Set_MotionTrailConfig(m_MotionTrailConfig);
                }

                ImGui::SliderFloat("Motion Trail Duration", &m_fMotionTrailDuration, 0.f, 10.f, "%.1f");

                if (ImGui::Button("Start Motion Trail"))
                {
                    CE_Khazan_Spear* pKhazan = dynamic_cast<CE_Khazan_Spear*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::SHADER),
                        TEXT("Layer_Player"), 0));
                    CE_Body_Khazan_Spear* pKhazanBody = dynamic_cast<CE_Body_Khazan_Spear*>(pKhazan->Find_PartObject(TEXT("Part_Body")));
                    pKhazanBody->Start_MotionTrail(m_fMotionTrailDuration);
                }
            }

            ImGui::Separator();
        }

        if (ImGui::CollapsingHeader("Light Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _uint iCurrentLevelIndex = ENUM_CLASS(LEVEL::SHADER);

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

	return S_OK;
}

void CLevel_Shader::Update(_float fTimeDelta)
{
	// Picking Test
	if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::WB))
	{
		_float3 vPos, vNorm;
		m_pGameInstance->isPicked(&vPos, &vNorm);
		m_DecalDesc.vPosition = vPos;

		if (FAILED(m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_Decal"), m_DecalDesc)))
			MSG_BOX(TEXT("Failed to Spawn : Decal"));
	}

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif
}

HRESULT CLevel_Shader::Render()
{
	SetWindowText(g_hWnd, TEXT("쉐이더툴"));

#ifdef _DEBUG
	++m_iRenderCount;

	if (m_fTimeAcc >= 1.f)
	{
		sprintf_s(m_szFPS, "FPS:%d", m_iRenderCount);
		m_fTimeAcc = 0.f;
		m_iRenderCount = 0;
	}
#endif

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Lights()
{
	// Directional
	//  LIGHT_DESC LightDesc = {};
	//  LightDesc.eType = LIGHT_DESC::DIRECTIONAL;
	//  LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	//  LightDesc.vDiffuse = _float4(0.2f, 0.2f, 0.2f, 0.2f);
	//  LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 0.2f);
	//  LightDesc.vSpecular = LightDesc.vDiffuse;
	//  if (FAILED(m_pGameInstance->Add_Light(TEXT("Directional"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
	//  	return E_FAIL;

    LIGHT_DESC LightDesc = {};
    LightDesc.eType = LIGHT_DESC::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Directional"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
        return E_FAIL;

    // Point_Red
    LightDesc = {};
    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(0.f, 10.f, -10.f, 1.f);
    LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.6f, 0.f, 0.f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 10.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Point_Red"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
        return E_FAIL;

    // Point_HDR
    LightDesc = {};
    LightDesc.eType = LIGHT_DESC::POINT;
    LightDesc.vPosition = _float4(0.f, 10.f, -60.f, 1.f);
    LightDesc.vDiffuse = _float4(0.9f, 0.8f, 0.7f, 1.f);
    LightDesc.vAmbient = _float4(0.8f, 0.6f, 0.4f, 1.f);    
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.fRange = 60.f;
    if (FAILED(m_pGameInstance->Add_Light(TEXT("Point_HDR"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
        return E_FAIL;

    // Point_HDR_Blink
    //  LightDesc = {};
    //  LightDesc.eType = LIGHT_DESC::POINT;
    //  LightDesc.vPosition = _float4(0.f, 10.f, -60.f, 1.f);
    //  LightDesc.vDiffuse = _float4(10.f, 9.f, 8.f, 1.f);
    //  LightDesc.vAmbient = _float4(1.f, 0.9f, 0.8f, 1.f);
    //  LightDesc.vSpecular = LightDesc.vDiffuse;
    //  LightDesc.fRange = 60.f;
    //  if (FAILED(m_pGameInstance->Add_Light(TEXT("Point_HDR"), ENUM_CLASS(LEVEL::SHADER), LightDesc)))
    //      return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_Camera()
{
	CCamera_Shader::CAMERA_EFFECT_DESC Desc{};

	Desc.vEye = _float4(0.f, 5.f, -30.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 20.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 25.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fMouseSensor = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_Camera"),
		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Camera_Shader"), TIME_CHANNEL::WORLD, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_BackGround()
{
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_BackGround"),
	//		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Terrain_Shader"))))
	//		return E_FAIL;

	CProp_Object::PROP_OBJECT_DESC Desc = {};
	Desc.eLevel = LEVEL::SHADER;
	XMStoreFloat4x4(&Desc.WorldMatrix, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.f, 0.f, 200.f));
	_tchar szPrototypeModelTag[MAX_PATH] = TEXT("Prototype_Component_Model_Rock");
	memcpy(Desc.szModelName, szPrototypeModelTag, MAX_PATH);
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_BackGround"),
		ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Prop_Object"), TIME_CHANNEL::WORLD, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Layer_Player()
{
	//  if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_Player"),
	//  	ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Player_Shader"))))
	//  	return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::SHADER), TEXT("Layer_Player"),
        ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Khazan_Spear"))))
        return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Shader::Ready_Light_Window()
{
    return E_NOTIMPL;
}

_bool CLevel_Shader::Lights_Load_Binary()
{
    return _bool();
}

HRESULT CLevel_Shader::Ready_Layer_MapObject(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_Shader::Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_Shader::Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
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

HRESULT CLevel_Shader::Ready_Layer_MapObject_Interactive(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap)
{
    _wstring strDataFilePath = { TEXT("../../Client/Bin/Data/Map/MapData/") };

    switch (eMap)
    {
    case KHAZAN_MAP::HEINMACH:
        strDataFilePath += TEXT("Embars/");
        break;
    case KHAZAN_MAP::CREVICE:
        strDataFilePath += TEXT("Crevice/");
        break;
    case KHAZAN_MAP::EMBARS:
        strDataFilePath += TEXT("Embars/");
        break;
    case KHAZAN_MAP::VIPER:
        strDataFilePath += TEXT("Viper/");
        break;
    default:
        break;
    }

    strDataFilePath += pDataFileName;

    // 동일한 파일명의 _objects.dat 불러오기
    strDataFilePath += TEXT("_interactive.dat");

    DWORD dwByte = {};

    HANDLE hFile = CreateFile(strDataFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return E_FAIL;
    }
    CHECK_EQUAL_MSG(INVALID_HANDLE_VALUE, hFile, TEXT("데이터 파일이 없거나 박준영 문제"), E_FAIL);

    _uint iStatueIndex0 = {};
    _uint iStatueIndex1 = {};

    // 1. 오브젝트의 총 개수
    _uint iObjectCnt = {};
    CHECK_FALSE(ReadFile(hFile, &iObjectCnt, sizeof(_uint), &dwByte, nullptr), E_FAIL);

    // 오브젝트 총 개수만큼 순회
    for (_uint i = 0; i < iObjectCnt; ++i)
    {
        CProp_Interactive::PROP_INTERACTIVE_DESC ObjectDesc = {};

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

        // 5. 상호 작용 타입 불러오기
        INTERACTIVE_TYPE eType = {};
        CHECK_FALSE(ReadFile(hFile, &eType, sizeof(INTERACTIVE_TYPE), &dwByte, nullptr), E_FAIL);
        CHECK_EQUAL_MSG(INTERACTIVE_TYPE::END, eType, TEXT("맵 에디터에서 상호 작용 타입 미지정"), false);


        switch (eType)
        {
        case INTERACTIVE_TYPE::CHECKPOINT:
        {
            _int iBladeNexusID = {};
            CHECK_FALSE(ReadFile(hFile, &iBladeNexusID, sizeof(_int), &dwByte, nullptr), E_FAIL);
            CHECK_FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(ObjectDesc.eLevel), TEXT("Layer_MapObject_Interact"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Prop_BladeNexus"), TIME_CHANNEL::MAP, &ObjectDesc), E_FAIL);
            break;
        }
        default:
            MSG_BOX(TEXT("잉 있으면 안되는디"));
            break;
        }
    }

    CloseHandle(hFile);

    return S_OK;
}

CLevel_Shader* CLevel_Shader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Shader* pInstance = new CLevel_Shader(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Shader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Shader::Free()
{
	__super::Free();



}
