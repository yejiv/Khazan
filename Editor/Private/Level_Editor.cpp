#include "EditorPch.h"
#include "Level_Editor.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"

CLevel_Editor::CLevel_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Editor::Initialize()
{
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

    // Test Code
    
    //  m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
    //      {
    //          ImGui::Begin("Emitter Settings");
    //  
    //          ImGui::Text("Base Parameters");
    //          ImGui::InputFloat3("Center", reinterpret_cast<_float*>(&m_vCenter));
    //          ImGui::InputFloat3("Direction", reinterpret_cast<_float*>(&m_vDir));
    //          ImGui::InputFloat("Speed", &m_fSpeed);
    //          ImGui::InputFloat("Scale", &m_fScale);
    //          ImGui::InputFloat("LifeTime", &m_fLifeTime);
    //          ImGui::Checkbox("Loop", &m_bLoop);
    //          ImGui::InputInt("Num Instances", &m_iNumInstance);
    //  
    //          ImGui::Separator();
    //  
    //          ImGui::Text("Advanced Options");
    //          ImGui::Checkbox("Use Random Direction", &m_bUseRandomDir);
    //  
    //          if (m_bUseRandomDir)
    //          {
    //              ImGui::InputFloat("Angle Min", &m_fAngleMin);
    //              ImGui::InputFloat("Angle Max", &m_fAngleMax);
    //          }
    //          else
    //          {
    //              ImGui::TextDisabled("RandomDir is off");
    //          }
    //  
    //          ImGui::Checkbox("Use Gravity", &m_bUseGravity);
    //  
    //          if (m_bUseGravity)
    //          {
    //              ImGui::InputFloat3("Gravity", reinterpret_cast<_float*>(&m_vGravity));
    //          }
    //          else
    //          {
    //              ImGui::TextDisabled("Gravity is off");
    //          }
    //  
    //          // ===== 적용 버튼 =====
    //          if (ImGui::Button("Apply"))
    //          {
    //              //  ParticleDesc OR EmitterDesc Desc{};
    //              //  Desc.Center = vCenter;
    //              //  Desc.Dir = vDir;
    //              //  Desc.Speed = fSpeed;
    //              //  Desc.Scale = fScale;
    //              //  Desc.LifeTime = fLifeTime;
    //              //  Desc.bLoop = bLoop;
    //              //  Desc.NumInstance = iNumInstance;
    //              //  Desc.bUseRandomDir = bUseRandomDir;
    //              //  Desc.AngleMin = fAngleMin;
    //              //  Desc.AngleMax = fAngleMax;
    //              //  Desc.Gravity = vGravity;
    //  
    //              // Create Buffer
    //          }
    //  
    //          ImGui::End();
    //      });

    m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
        {
            ImGui::Begin("Particle System Editor");

            // ==============================
            // 좌측 상단: System 생성
            // ==============================

            ImGui::SetNextItemWidth(200.f);
            ImGui::InputText("System Name", m_szSystemName, sizeof(m_szSystemName));
            
            if (ImGui::Button("Create System"))
            {
                if (strlen(m_szSystemName) > 0)
                {
                    // 클론한 파티클 시스템 
                    // 1. 파티클 시스템 컨테이너에 넣기
                    // 2. 게임오브젝트 인자로 넣어서 Layer에 추가해주는 함수 호출하기
                    // 
                    // if (nullptr !=pParticleSystem)
                    // {
                    //      m_ParticleSystems.push_back();
                    //      memset(newSystemName, 0, sizeof(newSystemName));
                    // }
                }
                else
                {
                    MSG_BOX(TEXT("Please enter a System name!"));
                }
            }

            ImGui::Separator();

            // ==============================
            // 좌측: Particle System List
            // ==============================
            ImGui::Text("Particle Systems");
            if (ImGui::BeginListBox("SystemList", ImVec2(200, 150)))
            {
                for (_int i = 0; i < static_cast<_int>(m_ParticleSystems.size()); ++i)
                {
                    _bool isSelected = (m_iSelectedSystem == i);
                    
                    _int iLength = WideCharToMultiByte(CP_ACP, 0, m_ParticleSystems[i]->Get_Name().c_str(), -1, nullptr, 0, nullptr, nullptr);
                    string strSystemName(iLength, 0);
                    WideCharToMultiByte(CP_ACP, 0, m_ParticleSystems[i]->Get_Name().c_str(), -1, &strSystemName[0], iLength, nullptr, nullptr);

                    if (ImGui::Selectable(strSystemName.c_str(), isSelected))
                        m_iSelectedSystem = i;
                }
                ImGui::EndListBox();
            }

            // 선택된 System이 있을 때 Delete 버튼
            if (m_iSelectedSystem >= 0)
            {
                if (ImGui::Button("Delete System"))
                {
                    Safe_Release(m_ParticleSystems[m_iSelectedSystem]);
                    m_ParticleSystems.erase(m_ParticleSystems.begin() + m_iSelectedSystem);
                    m_iSelectedSystem = -1;
                    m_iSelectedEmitter = -1;
                }
            }

            ImGui::Separator();

            // ==============================
            // 좌측 하단: Particle Emitter List
            // ==============================
            if (m_iSelectedSystem >= 0)
            {
                CParticleSystem* pSystem = m_ParticleSystems[m_iSelectedSystem];
                
                ImGui::Text("Emitters");
                
                if (ImGui::BeginListBox("Emitter List", ImVec2(200, 150)))
                {
                    for (_uint i = 0; i < pSystem->Get_NumEmitters(); ++i)
                    {
                        _bool isSelected = (m_iSelectedEmitter == i);
                        
                        _int iLength = WideCharToMultiByte(CP_ACP, 0, pSystem->Get_Emitter(i)->Get_Name().c_str(), -1, nullptr, 0, nullptr, nullptr);
                        string strEmitterName(iLength, 0);
                        WideCharToMultiByte(CP_ACP, 0, pSystem->Get_Emitter(i)->Get_Name().c_str(), -1, &strEmitterName[0], iLength, nullptr, nullptr);

                        if (ImGui::Selectable(strEmitterName.c_str(), isSelected))
                            m_iSelectedEmitter = i;
                    }
                    ImGui::EndListBox();
                }

                ImGui::SameLine();
            }

            ImGui::End();
        });

        m_pGameInstance->AddWidget(TEXT("Effect"), [&]()
            {
                // ==============================
                // 우측: Emitter Settings
                // ==============================
                ImGui::Begin("EmitterSettings");

                if (m_iSelectedSystem >= 0 && m_iSelectedEmitter >= 0)
                {
                    //  CParticleEmitter* pEmitter = m_ParticleSystems[m_iSelectedSystem]->Get_Emitter(m_iSelectedEmitter);
                    //  auto& Desc = pEmitter->Get_Desc();

                    //  ImGui::InputText("Emitter Name", &Desc.strName[0], sizeof(Desc.strName));
                    //  ImGui::InputFloat3("Center", (float*)&Desc.vCenter);
                    //  ImGui::InputFloat3("Direction", (float*)&Desc.vDir);
                    //  ImGui::InputFloat("Speed", &Desc.fSpeed);
                    //  ImGui::InputFloat("Scale", &Desc.fScale);
                    //  ImGui::InputFloat("LifeTime", &Desc.fLifeTime);
                    //  
                    //  ImGui::Separator();
                    //  ImGui::Checkbox("Loop", &Desc.isLoop);
                    //  ImGui::Checkbox("Spread", &Desc.isSpread);
                    //  ImGui::Checkbox("Drop", &Desc.isDrop);
                    //  ImGui::Checkbox("Gravity", &Desc.isGravity);
                    //  
                    //  ImGui::Separator();

                    if (ImGui::Button("Apply Changes"))
                    {
                    }
                }
                else
                {
                    ImGui::Text("NULL");
                }

                if (ImGui::Button("Add Emitter"))
                {
                    // System Add Emitter
                }
                ImGui::SameLine();
                if (m_iSelectedEmitter >= 0 && ImGui::Button("Delete Emitter"))
                {
                    // System Delete Emitter
                    m_iSelectedEmitter = -1;
                }

                ImGui::End();
            });

	return S_OK;
}

void CLevel_Editor::Update(_float fTimeDelta)
{
	return;
}

HRESULT CLevel_Editor::Render()
{
	SetWindowText(g_hWnd, TEXT("에디터 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}


CLevel_Editor* CLevel_Editor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Editor* pInstance = new CLevel_Editor(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Editor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Editor::Free()
{
	__super::Free();

    for (auto& pParticleSystem : m_ParticleSystems)
        Safe_Release(pParticleSystem);
    m_ParticleSystems.clear();
}
