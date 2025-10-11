#include "EditorPch.h"
#include "Level_Effect.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "Camera_Effect.h"

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
                    Create_ParticleSystem();   
                else
                    MSG_BOX(TEXT("Please enter a System name!"));
            }

            ImGui::SameLine();

            // 선택된 System이 있을 때 Delete 버튼
            if (m_iSelectedSystem >= 0)
            {
                if (ImGui::Button("Delete System"))
                {
                    m_ParticleSystems[m_iSelectedSystem]->Set_IsDead(true);
                    Safe_Release(m_ParticleSystems[m_iSelectedSystem]);
                    m_ParticleSystems.erase(m_ParticleSystems.begin() + m_iSelectedSystem);

                    m_iSelectedSystem = -1;
                    m_iSelectedEmitter = -1;
                }
            }

            ImGui::Separator();

            // ==============================
            // 좌측: Particle System List
            // ==============================
            ImGui::Text("Particle Systems");
            if (ImGui::BeginListBox("System List", ImVec2(200, 150)))
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

            if (m_iPrevSelectedSystem != m_iSelectedSystem)
            {
                m_iSelectedEmitter = -1;
                m_iPrevSelectedSystem = m_iSelectedSystem;
            }

            if ((m_iSelectedSystem >= 0 && m_iSelectedEmitter >= 0) && (m_iPrevSelectedSystem == m_iSelectedSystem))
            {
                CParticleEmitter* pEmitter = m_ParticleSystems[m_iSelectedSystem]->Get_Emitter(m_iSelectedEmitter);

                if (m_iPrevSelectedEmitter != m_iSelectedEmitter)
                {
                    m_strEmitterName = pEmitter->Get_Name();
                    m_PointInfo = pEmitter->Get_ParticleInfo();
                    m_isSpread = pEmitter->Get_Spread();
                    m_isDrop = pEmitter->Get_Drop();

                    m_iPrevSelectedEmitter = m_iSelectedEmitter;
                    // wstring -> char 변환 후 버퍼에 저장
                    WideCharToMultiByte(CP_ACP, 0, m_strEmitterName.c_str(), -1, m_szEmitterName, sizeof(m_szEmitterName), nullptr, nullptr);
                }

                ImGui::InputText("Emitter Name", m_szEmitterName, sizeof(m_szEmitterName));

                ImGui::Separator();
                ImGui::Text("Emitter Parameters");
                ImGui::Separator();

                // 파티클 개수
                ImGui::InputInt("Num Instance", reinterpret_cast<_int*>(&m_PointInfo.iNumInstance));

                // 중심 위치
                ImGui::InputFloat3("Center", reinterpret_cast<_float*>(&m_PointInfo.vCenter));

                // 퍼지는 범위
                ImGui::InputFloat3("Range", reinterpret_cast<_float*>(&m_PointInfo.vRange));

                // 크기 (최소, 최대)
                ImGui::InputFloat2("Size (Min/Max)", reinterpret_cast<_float*>(&m_PointInfo.vSize));

                // 수명 (최소, 최대)
                ImGui::InputFloat2("LifeTime (Min/Max)", reinterpret_cast<_float*>(&m_PointInfo.vLifeTime));

                // 회전 중심 (Pivot)
                ImGui::InputFloat3("Pivot", reinterpret_cast<_float*>(&m_PointInfo.vPivot));

                // 속도 (최소, 최대)
                ImGui::InputFloat2("Speed (Min/Max)", reinterpret_cast<_float*>(&m_PointInfo.vSpeed));

                ImGui::Separator();
                ImGui::Text("Emitter Behavior");
                ImGui::Separator();

                // 반복 여부 및 물리 효과
                ImGui::Checkbox("Loop", &m_PointInfo.isLoop);
                ImGui::Checkbox("Spread", &m_isSpread);
                ImGui::Checkbox("Drop", &m_isDrop);
                //  ImGui::Checkbox("Gravity", &m_isGravity);
                
                ImGui::Separator();

                if (ImGui::Button("Apply Changes"))
                {
                    // char -> wstring 변환
                    _int iLength = MultiByteToWideChar(CP_ACP, 0, m_szEmitterName, -1, nullptr, 0);
                    _wstring strNewEmitterName(iLength, 0);
                    MultiByteToWideChar(CP_ACP, 0, m_szEmitterName, -1, &strNewEmitterName[0], iLength);

                    if (m_strEmitterName != strNewEmitterName)
                        pEmitter->Set_Name(strNewEmitterName);

                    pEmitter->Recreate_Particle(m_PointInfo);
                    pEmitter->Set_Spread(m_isSpread);
                    pEmitter->Set_Drop(m_isDrop);
                }
            }

            if (ImGui::Button("Add Emitter"))
            {
                Create_ParticleEmitter();
            }

            ImGui::SameLine();

            if (m_iSelectedEmitter >= 0 && ImGui::Button("Delete Emitter"))
            {
                Delete_ParticleEmitter();

                m_iSelectedEmitter = -1;
            }

            ImGui::End();
        });

	return S_OK;
}

void CLevel_Effect::Update(_float fTimeDelta)
{
	return;
}

HRESULT CLevel_Effect::Render()
{
	SetWindowText(g_hWnd, TEXT("이펙트툴"));

	return S_OK;
}

HRESULT CLevel_Effect::Create_ParticleSystem()
{
    _tchar szWideSystemName[64] = {};
    MultiByteToWideChar(CP_ACP, 0, m_szSystemName, -1, szWideSystemName, 128);
    _wstring strSystemName = szWideSystemName;

    CParticleSystem::PARTICLE_SYSTEM_DESC Desc{};
    Desc.strName = strSystemName;

    CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::EFFECT),
        TEXT("Prototype_GameObject_ParticleSystem"), &Desc));
    if (nullptr == pGameObject)
        return E_FAIL;

    CParticleSystem* pSystem = dynamic_cast<CParticleSystem*>(pGameObject);
    if (nullptr == pSystem)
        return E_FAIL;

    if (FAILED(m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Effect"), pSystem)))
        return E_FAIL;

    m_ParticleSystems.push_back(dynamic_cast<CParticleSystem*>(pSystem));
    Safe_AddRef(pSystem);
    
    memset(m_szSystemName, 0, sizeof(m_szSystemName));

    return S_OK;
}

HRESULT CLevel_Effect::Create_ParticleEmitter()
{
    if (m_iSelectedSystem < 0)
        return E_FAIL;

    CParticleSystem* pSystem = m_ParticleSystems[m_iSelectedSystem];
    if (nullptr == pSystem)
        return E_FAIL;

    CParticleEmitter::PARTICLE_EMITTER_DESC Desc{};
    Desc.strName = TEXT("Emitter") + to_wstring(pSystem->Get_NumEmitters());

    // Desc Editor에서 수치 조정한 멤버 변수 추가 작성 필요

    if (FAILED(pSystem->Add_Emitter(Desc)))
        MSG_BOX(TEXT("Failed to Add Emitter!"));

    return S_OK;
}

HRESULT CLevel_Effect::Delete_ParticleEmitter()
{
    // 시스템의 Remove Emitter 호출
    if (m_iSelectedSystem < 0 || m_iSelectedEmitter < 0)
        return E_FAIL;

    CParticleSystem* pSystem = m_ParticleSystems[m_iSelectedSystem];
    if (nullptr == pSystem)
        return E_FAIL;

    if (FAILED(pSystem->Remove_Emitter(static_cast<_uint>(m_iSelectedEmitter))))
        MSG_BOX(TEXT("Failed to Remove Emitter!"));

    return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_BackGround()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_BackGround"),
		ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Terrain_Grid"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Camera()
{
    CCamera_Effect::CAMERA_EFFECT_DESC Desc{};

    Desc.vEye = _float4(0.f, 5.f, -5.f, 1.f);
    Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.fFovy = XMConvertToRadians(60.0f);
    Desc.fNear = 0.1f;
    Desc.fFar = 1000.f;
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
    Desc.fMouseSensor = 0.1f;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EFFECT), TEXT("Layer_Camera"),
        ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_GameObject_Camera_Effect"), &Desc)))
        return E_FAIL;

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

    for (auto& pParticleSystem : m_ParticleSystems)
        Safe_Release(pParticleSystem);
    m_ParticleSystems.clear();
}
