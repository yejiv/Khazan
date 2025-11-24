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
    m_MotionBlurDesc = m_pGameInstance->Get_MotionBlurDesc();
    m_RimLightDesc = m_pGameInstance->Get_RimLightDesc();

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

					if (ImGui::SliderFloat("Fog Base Height", &m_FogConfig.fBaseHeight, -1000.f, 3000.f))
						m_pGameInstance->Set_FogConfig(m_FogConfig);

					if (ImGui::SliderFloat("Fog Height Density", &m_FogConfig.fHeightDensity, 0.001f, 1.f))
						m_pGameInstance->Set_FogConfig(m_FogConfig);

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
                    if (ImGui::SliderFloat("LUT Intensity", &m_fLUTIntensity, 0.f, 1.f, "%.2f"));
                    m_pGameInstance->Set_LUTIntensity(m_fLUTIntensity);
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
                const _char* ObjectTags[] = { "Khazan_Spear", "Spear", "Viper" };
                ImGui::Combo("GameObject List", &m_iCurrentGameObjectIndex, ObjectTags, IM_ARRAYSIZE(ObjectTags));

                // 고르면 해당 객체의 모션 트레일 정보 Get해서 띄우기
                CKhazan_Spear* pKhazan = {};
                CBody_Khazan_Spear* pBody = {};
                CSpear_Khazan_Spear* pWeapon = {};

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
                        }
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
