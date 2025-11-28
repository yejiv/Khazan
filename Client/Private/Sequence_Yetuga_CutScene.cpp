#include "Sequence_Yetuga_CutScene.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "GameObject.h"
#include "Yetuga.h"
#include "Camera_Compre.h"
#include "AS_CutScene_Yetuga.h"
#include "CharacterVirtual.h"
#include "AI_Controller_Yetuga.h"

CSequence_Yetuga_CutScene::CSequence_Yetuga_CutScene(CYetuga* pYetuga)
    : m_pClientInstance{ CClientInstance::GetInstance() }
    , m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pYetuga{ pYetuga }
{
    Safe_AddRef(m_pClientInstance);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSequence_Yetuga_CutScene::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
    m_Id = tDesc.tId;
    m_fTime = tDesc.fStartTime;
    m_pCamera = dynamic_cast<CCamera_Compre*>(m_pClientInstance->Get_ActiveCamera());
    CTransform* pTransform = dynamic_cast<CTransform*>(m_pCamera->Get_Component(TEXT("Com_Transform")));

    //1. 카메라 예투가 맵 가운데로 이동
    pTransform->Set_State(STATE::POSITION, XMVectorSet(513.9f, -10.25, 240.47, 1.f));

    CTransform* pYetugaTransform = static_cast<CTransform*>(m_pYetuga->Get_Component(TEXT("Com_Transform")));
    pYetugaTransform->Set_State(STATE::POSITION, XMVectorSet(537.354f, 18.684f, 221.961f, 1.f));
    CCharacterVirtual* pCharVir = dynamic_cast<CCharacterVirtual*>(m_pYetuga->Get_Component(TEXT("Com_CharacterVirtual")));
    pCharVir->Set_Position(XMVectorSet(537.354f, 18.684f, 221.961f, 1.f));
    pCharVir->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
    
    
    return S_OK;
}

void CSequence_Yetuga_CutScene::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;
    m_fSnowTime += fTimeDelta;
    
    CTransform* pYetugaTransform = static_cast<CTransform*>(m_pYetuga->Get_Component(TEXT("Com_Transform")));
    pYetugaTransform->LookAt(XMVectorSet(507.5f, -9.15f, 260.09f, 1.f));
    //2. 작은 눈안개 깔기 시작
    if (m_fTime < 4.f)
    {
        if (m_fSnowTime > 0.4f)
        {        
            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Smoke"), XMVectorSet(
                513.90f,
                -7.44f,
                240.47,
                1.f));
            for (size_t i = 0; i < 1; ++i)
            {
                m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Smoke"), XMVectorSet(
                    m_pGameInstance->Rand(500.f, 530.f),
                    -7.44f,
                    m_pGameInstance->Rand(217.f, 240.f),
                    1.f));
            }
            m_fSnowTime = 0.f;
        }        
    }
    if (m_fTime > 1.5f && !m_isYetugaJump)
    {
        CAS_CutScene_Yetuga* Cut_Yetuga = m_pYetuga->Get_Yetuga_CutSceneState();
        Cut_Yetuga->YetugaScene_Jump(m_pYetuga);
        m_isYetugaJump = true;
    }

    if (m_fTime > 4.5f && !m_isYetugaLand)
    {
        CAS_CutScene_Yetuga* Cut_Yetuga = m_pYetuga->Get_Yetuga_CutSceneState();
        Cut_Yetuga->YetugaScene_Land(m_pYetuga);
        m_pClientInstance->ActiveCamera_Shaking(5.f, 1.f);
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_SnowUp"), XMVectorSet(518.58f, -11.952f, 220.08f, 1.f));

        // Vignette
        VIGNETTE_CONFIG Config{};
        Config.eMode = VIGNETTE_CONFIG::SMOOTH_SMOOTH;
        Config.vColor = _float3(0.f, 0.f, 0.f);
        Config.fPower = 3.5f;
        Config.fIntensity = 1.f;
        Config.fMaxIntensity = 4.f;
        m_pGameInstance->Start_VignetteAnimation(2.f, Config);

        m_isYetugaLand = true;
    }

    if (m_fTime > 5.5f && !m_isYetugaRoar1)
    {
        CAS_CutScene_Yetuga* Cut_Yetuga = m_pYetuga->Get_Yetuga_CutSceneState();
        Cut_Yetuga->YetugaScene_Roar1(m_pYetuga);
        m_isYetugaRoar1 = true;
    }



    if (m_fTime > 9.f && !m_isYetugaRoar2)
    {
        CAS_CutScene_Yetuga* Cut_Yetuga = m_pYetuga->Get_Yetuga_CutSceneState();
        Cut_Yetuga->YetugaScene_Roar2(m_pYetuga);
        m_isYetugaRoar2 = true;
    }

    if (m_fTime > 10.f && !m_isRoar1Effect)
    {
        // Distortion
        DISTORTION_DESC Desc{};
        _vector vCenterPos = XMVectorSet(518.41f, -10.6f, 233.39f, 1.f);
        _float fPosY = XMVectorGetY(vCenterPos);
        _float fOffset = 2.f;
        vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
        XMStoreFloat3(&Desc.vCenter, vCenterPos);
        Desc.fRange = 0.6f;
        Desc.fPower = 0.05f;
        Desc.fDuration = 1.f;
        Desc.vFadeTime = _float2(1.f, 0.2f);
        Desc.fSpeed = 1.f;
        Desc.iNoiseIndex = 17;
        m_pGameInstance->Start_Distortion(Desc);

       

        RADIAL_BLUR_DESC RadialDesc{};
        RadialDesc.vCenterUV = _float2(0.5f, 0.5f);
        RadialDesc.fSampleRadius = 0.05f;
        RadialDesc.vMaskRadius = _float2(0.f, 0.3f);
        RadialDesc.fExponent = 1.f;
        RadialDesc.iNumSamples = 16;
        RadialDesc.fAttenuation = 0.1f;
        RadialDesc.fStrength = 1.f;
        RadialDesc.fDuration = 1.f;
        RadialDesc.vFadeTime = _float2(0.05f, 0.25f);
        m_pGameInstance->Start_RadialBlur(RadialDesc);

        m_isRoar1Effect = true;
    }

    if (m_fTime > 11.2f && !m_isRoar2Effect)
    {
        // Distortion
        DISTORTION_DESC Desc{};
        _vector vCenterPos = XMVectorSet(518.47f, -8.5f, 233.18f, 1.f);
        _float fPosY = XMVectorGetY(vCenterPos);
        _float fOffset = 2.f;
        vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
        XMStoreFloat3(&Desc.vCenter, vCenterPos);
        Desc.fRange = 0.6f;
        Desc.fPower = 0.05f;
        Desc.fDuration = 1.5f;
        Desc.vFadeTime = _float2(1.f, 0.2f);
        Desc.fSpeed = 1.f;
        Desc.iNoiseIndex = 17;
        m_pGameInstance->Start_Distortion(Desc);



        RADIAL_BLUR_DESC RadialDesc{};
        RadialDesc.vCenterUV = _float2(0.5f, 0.5f);
        RadialDesc.fSampleRadius = 0.05f;
        RadialDesc.vMaskRadius = _float2(0.f, 0.3f);
        RadialDesc.fExponent = 1.f;
        RadialDesc.iNumSamples = 16;
        RadialDesc.fAttenuation = 0.1f;
        RadialDesc.fStrength = 1.f;
        RadialDesc.fDuration = 1.5f;
        RadialDesc.vFadeTime = _float2(0.05f, 0.25f);
        m_pGameInstance->Start_RadialBlur(RadialDesc);

        m_isRoar2Effect = true;
    }    

    if (m_fTime > 20.f)
    {
        m_pYetuga->Get_Controller()->Set_ControllerActivate(true);
        m_isEnd = true;
    }

    //3. 페이드 인
    //4. 허공 보여주며 시작
    //5. 1.5초뒤 예투가 점프 시작
    //6. 1초뒤 예투가 랜드
    //7. 카메라 쉐이킹과 큰 눈안개 발생
    //8. 바닥 훑으면서 가슴 두드리기 시작
    //9. 로어 발사
    //10. 디스토션과 레디얼블러 처리
    //11. 플레이어로 카메라 이동
    //12. 게임 시작

    

}

void CSequence_Yetuga_CutScene::Pause()
{

}

void CSequence_Yetuga_CutScene::Resume()
{

}

void CSequence_Yetuga_CutScene::StopImmediate()
{

}

void CSequence_Yetuga_CutScene::Jump(_float fTime)
{
    m_fTime = fTime;
}

_bool CSequence_Yetuga_CutScene::IsEnd() const
{
    return m_isEnd;
}

void CSequence_Yetuga_CutScene::PlayerMove(_float fTimeDelta)
{

}

CSequence_Yetuga_CutScene* CSequence_Yetuga_CutScene::Create(class CYetuga* pYetuga)
{
    return new CSequence_Yetuga_CutScene(pYetuga);
}

void CSequence_Yetuga_CutScene::Free()
{
    __super::Free();

    Safe_Release(m_pClientInstance);
    Safe_Release(m_pGameInstance);    
}
