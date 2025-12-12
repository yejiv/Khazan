#include "HeinMach_Trigger.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Sequence_HeinMach_Field.h"
#include "Sequence_HeinMach_Yetuga.h"
#include "Sequence_HeinMach_Start_Chat.h"
#include "Camera_Compre.h"
#include "Transform.h"
#include "Creature.h"
#include "UI_Tutorial.h"

#include "MapObject_Header.h"

CHeinMach_Trigger::CHeinMach_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTrigger { pDevice, pContext }
    , m_pClientInstance { CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

CHeinMach_Trigger::CHeinMach_Trigger(const CHeinMach_Trigger& Prototype)
    : CTrigger { Prototype }
    , m_pClientInstance { Prototype.m_pClientInstance }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CHeinMach_Trigger::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CHeinMach_Trigger::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_TriggerType(pArg), E_FAIL);

    return S_OK;
}

void CHeinMach_Trigger::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CHeinMach_Trigger::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CHeinMach_Trigger::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CHeinMach_Trigger::Render()
{
    //CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    //_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    //for (_uint i = 0; i < iNumMeshes; ++i)
    //{
    //    CHECK_FAILED_ASSERT(m_pShaderCom->Begin(1), E_FAIL);

    //    CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    //}

    return S_OK;
}

HRESULT CHeinMach_Trigger::Ready_Components(void* pArg)
{
    return S_OK;
}

HRESULT CHeinMach_Trigger::Ready_Collision(void* pArg)
{
    return S_OK;
}

HRESULT CHeinMach_Trigger::Ready_TriggerType(void* pArg)
{
    if (m_strTriggerKey == "CutScene")
    {
        m_pHeinMach_Field = CSequence_HeinMach_Field::Create(
            dynamic_cast<CCamera_Compre*>(m_pClientInstance->Find_Camera(
                ENUM_CLASS(LEVEL::HEINMACH),
                CAMERATYPE::PLAYER))
        );
    }
    else if (m_strTriggerKey == "Yetuga")
    {
        m_pHeinMach_Yetuga = CSequence_HeinMach_Yetuga::Create(
            dynamic_cast<CCamera_Compre*>(m_pClientInstance->Find_Camera(ENUM_CLASS(LEVEL::HEINMACH), CAMERATYPE::PLAYER)),
            dynamic_cast<CCreature*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Creature_Player")))
        );

        Load_SkyBoxBinaryFile(TEXT("HeinMach_Yetuga"));
    }
#pragma region 동굴 정방향
    else if (m_strTriggerKey == "CaveEntry")
    {
#pragma region FOG ( 동굴 진입 -> 동굴 중간까지 )

#pragma endregion

        Load_SkyBoxBinaryFile(TEXT("HeinMach_Dawn"));
    }
    else if (m_strTriggerKey == "CaveMidEntry")
    {
#pragma region FOG ( 동굴 중간 -> 동굴 출구까지 )

#pragma endregion
    }
    else if (m_strTriggerKey == "CaveExit")
    {
#pragma region FOG ( 동굴 출구 -> 동굴 정방향 출구 후 지역 )

#pragma endregion

        Load_SkyBoxBinaryFile(TEXT("HeinMach_Day"));
    }
#pragma endregion

#pragma region 동굴 역방향
    else if (m_strTriggerKey == "CaveEntry_Rev")
    {
#pragma region FOG ( 동굴 정방향 입구 -> 동굴 정방향 입구 전지역 )

#pragma endregion

        Load_SkyBoxBinaryFile(TEXT("HeinMach_Day"));
    }
    else if (m_strTriggerKey == "CaveMidEntry_Rev")
    {
#pragma region FOG ( 동굴 정방향 중간 -> 동굴 정방향 입구까지 )

#pragma endregion
    }
    else if (m_strTriggerKey == "CaveExit_Rev")
    {
#pragma region FOG ( 동굴 정방향 출구 -> 동굴 정방향 중간까지 )

#pragma endregion

        Load_SkyBoxBinaryFile(TEXT("HeinMach_Dawn"));
    }
#pragma endregion
    else if (m_strTriggerKey == "Guide_LockOn")
    {
        m_eGuideType = GUIDE_TYPE::LOCKON;
    }
    else if (m_strTriggerKey == "Guide_Guard")
    {
        m_eGuideType = GUIDE_TYPE::GUARD;
    }
    else if (m_strTriggerKey == "Guide_UnderWorld")
    {
        m_eGuideType = GUIDE_TYPE::UNDERWORLD;
    }
    else if (m_strTriggerKey == "Guide_Dodge")
    {
        m_eGuideType = GUIDE_TYPE::DODGE;
    }
    else if (m_strTriggerKey == "Guide_Brutal")
    {
        m_eGuideType = GUIDE_TYPE::BURTALATTACK;
    }
    else if (m_strTriggerKey == "Guide_FallAttack")
    {
        m_eGuideType = GUIDE_TYPE::FALLATTACK;
    }
    else if (m_strTriggerKey == "Guide_Impulse")
    {
        m_eGuideType = GUIDE_TYPE::IMPULSE;
    }
    else if (m_strTriggerKey == "Talk_03")
    {
        m_pHeinMach_Start_Chat = CSequence_HeinMach_Start_Chat::Create();
    }

    return S_OK;
}

void CHeinMach_Trigger::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER))
    {
        if (m_strTriggerKey == "CutScene")
        {
            SEQ_REQ_PLAY_DESC tPlayDesc{};
            tPlayDesc.tId.iSeq = 1000;
            tPlayDesc.pAsset = L"Field_Cut";
            tPlayDesc.fStartTime = 0.f;
            m_pGameInstance->SEQ_AdoptAndPlay(m_pHeinMach_Field, tPlayDesc);

            FOG_TRANSITION_DESC Desc{};
            Desc.fDensity = 0.03f;
            Desc.fBias = 0.6f;
            Desc.vColor = _float4(0.f, 0.106f, 0.137f, 1.f);
            Desc.isUseHeight = true;
            Desc.fBaseHeight = 1120.f;
            Desc.isUseNoise = false;
            m_pGameInstance->Start_FogTransition(3.f, Desc);

            m_pClientInstance->BGM_HeinMach_CutScene();

            m_isDead = true;
        }
        else if (m_strTriggerKey == "Yetuga")
        {
            SEQ_REQ_PLAY_DESC tPlayDesc{};
            tPlayDesc.tId.iSeq = 1001;
            tPlayDesc.pAsset = L"Yetuga_Cut";
            tPlayDesc.fStartTime = 0.f;
            m_pGameInstance->SEQ_AdoptAndPlay(m_pHeinMach_Yetuga, tPlayDesc);
            m_isDead = true;

            // 예투가 Fog
            //  Set_FogConfig(m_FogConfig);
            FOG_TRANSITION_DESC Desc{};
            Desc.fDensity = 0.03f;
            Desc.fBias = 0.6f;
            Desc.vColor = _float4(0.338f, 0.545f, 0.749f, 1.f);
            Desc.isUseHeight = false;
            Desc.isUseNoise = false;
            m_pGameInstance->Start_FogTransition(7.f, Desc);

            // 예투가 스카이 박스
            Start_SkyTransition(m_Sky_Desc, m_Cloud_Desc, 5.f);
        }
        // UI GUIDE 타입일 때
        else if (GUIDE_TYPE::END != m_eGuideType)
        {
            CUI_Tutorial* pUI_Tutorial = static_cast<CUI_Tutorial*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Tutorial")));
            CHECK_NULLPTR_MSG(pUI_Tutorial, TEXT("pUI_Tutorial == nullptr"), );

            switch (m_eGuideType)
            {
            case GUIDE_TYPE::LOCKON:
            case GUIDE_TYPE::GUARD:
            case GUIDE_TYPE::UNDERWORLD:
            case GUIDE_TYPE::DODGE:
            case GUIDE_TYPE::BURTALATTACK:
            case GUIDE_TYPE::FALLATTACK:
            case GUIDE_TYPE::IMPULSE:
                pUI_Tutorial->On_Panel(m_eGuideType);
                break;
            default:
                MSG_BOX(TEXT("트리거 - 가이드 타입 default 뜸 엥"));
                break;
            }

            m_isDead = true;
        }
#pragma region 동굴 정방향
        else if (m_strTriggerKey == "CaveEntry")
        {
            m_pClientInstance->BGM_HeinMach_Cave();

            FOG_TRANSITION_DESC Desc{};
            Desc.fDensity = 0.035f;
            Desc.fBias = 1.f;
            Desc.vColor = _float4(0.031f, 0.137f, 0.200f, 1.f);
            Desc.isUseHeight = false;
            Desc.isUseNoise = false;
            m_pGameInstance->Start_FogTransition(5.f, Desc);

            // 그림자 보간 추가
            m_pGameInstance->Start_ShadowTransition(5.f, 1.f);

            Start_SkyTransition(m_Sky_Desc, m_Cloud_Desc, 5.f);

#pragma region 동굴 들어갈때 플레이어 isInCave = true;

            EventInteractType InteractType = {};

            InteractType.eState = EventInteractType::NONE;
            InteractType.CaveIn();

            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

#pragma endregion
        }
        else if (m_strTriggerKey == "CaveMidEntry")
        {
            FOG_TRANSITION_DESC Desc{};
            Desc.fDensity = 0.015f;
            Desc.fBias = 0.8f;
            Desc.vColor = _float4(0.031f, 0.137f, 0.200f, 1.f);
            Desc.isUseHeight = false;
            Desc.isUseNoise = false;
            m_pGameInstance->Start_FogTransition(2.f, Desc);
        }
        else if (m_strTriggerKey == "CaveExit")
        {
            m_pClientInstance->BGM_HeinMach_Day();

            Start_SkyTransition(m_Sky_Desc, m_Cloud_Desc, 7.f);

            FOG_TRANSITION_DESC Desc{};
            Desc.fDensity = 0.025f;
            Desc.fBias = 0.7f;
            Desc.vColor = _float4(0.631f, 0.522f, 0.471f, 1.f);
            Desc.isUseHeight = false;
            Desc.isUseNoise = false;
            //  Desc.isUseHeight = true;
            //  Desc.fBaseHeight = 3000.f;
            //  Desc.isUseNoise = true;
            //  Desc.vNoiseSpeed = _float2(0.01f, 0.f);
            //  Desc.vNoiseScale = _float2(1.f, 1.f);
            //  Desc.fNoiseStrength = 0.5f;
            //  Desc.fNoiseContrast = 1.f;
            //  Desc.iNoiseIndex = 8;
            m_pGameInstance->Start_FogTransition(15.f, Desc);

            // 그림자 보간 추가
            m_pGameInstance->Start_ShadowTransition(15.f, 0.6f);

#pragma region 동굴 나갈때 플레이어 isInCave = false;

            EventInteractType InteractType = {};

            InteractType.eState = EventInteractType::NONE;
            InteractType.CaveOut();

            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

#pragma endregion
        }
#pragma endregion
#pragma region 동굴 역방향
        else if (m_strTriggerKey == "CaveEntry_Rev")
        {
            m_pClientInstance->BGM_HeinMach_Dawn();

            // 동굴 전 포그
            FOG_TRANSITION_DESC Desc{};
            Desc.fDensity = 0.05f;
            Desc.fBias = 0.6f;
            Desc.vColor = _float4(0.f, 0.106f, 0.137f, 1.f);
            Desc.isUseHeight = true;
            Desc.fBaseHeight = 1120.f;
            Desc.isUseNoise = false;
            m_pGameInstance->Start_FogTransition(5.f, Desc);

            // 그림자 보간 추가
            m_pGameInstance->Start_ShadowTransition(5.f, 0.6f);

#pragma region 동굴 나갈때 ( 역방향 ) 플레이어 isInCave = false;

            EventInteractType InteractType = {};

            InteractType.eState = EventInteractType::NONE;
            InteractType.CaveOut();

            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

#pragma endregion
        }
        else if (m_strTriggerKey == "CaveMidEntry_Rev")
        {
            // 동굴 입구 ~ 중간 포그
            FOG_TRANSITION_DESC Desc{};
            Desc.fDensity = 0.035f;
            Desc.fBias = 1.f;
            Desc.vColor = _float4(0.031f, 0.137f, 0.200f, 1.f);
            Desc.isUseHeight = false;
            Desc.isUseNoise = false;
            m_pGameInstance->Start_FogTransition(2.f, Desc);
        }
        else if (m_strTriggerKey == "CaveExit_Rev")
        {
            m_pClientInstance->BGM_HeinMach_Cave();

            Start_SkyTransition(m_Sky_Desc, m_Cloud_Desc, 2.f);
            
            // 동굴 중간 ~ 출구 포그
            FOG_TRANSITION_DESC Desc{};
            Desc.fDensity = 0.015f;
            Desc.fBias = 0.8f;
            Desc.vColor = _float4(0.031f, 0.137f, 0.200f, 1.f);
            Desc.isUseHeight = false;
            Desc.isUseNoise = false;
            m_pGameInstance->Start_FogTransition(2.f, Desc);

            // 그림자 보간 추가
            m_pGameInstance->Start_ShadowTransition(7.f, 1.f);

#pragma region 동굴 들어갈때 ( 역방향 ) 플레이어 isInCave = true;

            EventInteractType InteractType = {};

            InteractType.eState = EventInteractType::NONE;
            InteractType.CaveIn();

            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

#pragma endregion
        }
#pragma endregion
        else if (m_strTriggerKey == "Shadow_Restore")
        {
            // 그림자 보간 추가
            m_pGameInstance->Start_ShadowTransition(3.f, 0.6f);

            m_isDead = true;
        }
        else if (m_strTriggerKey == "Talk_03")
        {
            m_pClientInstance->BGM_HeinMach_Entry();

            SEQ_REQ_PLAY_DESC tPlayDesc{};
            tPlayDesc.tId.iSeq = 1100;
            tPlayDesc.pAsset = L"Start_Chat";
            tPlayDesc.fStartTime = 0.f;
            m_pGameInstance->SEQ_AdoptAndPlay(m_pHeinMach_Start_Chat, tPlayDesc);

            m_isDead = true;
        }
    }
    
}

void CHeinMach_Trigger::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CHeinMach_Trigger::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

HRESULT CHeinMach_Trigger::Load_SkyBoxBinaryFile(const _tchar* pDataFile)
{
    CHECK_NULLPTR(pDataFile, E_FAIL);

    _wstring strFilePath = TEXT("../../Client/Bin/Data/Map/MapData/HeinMach/");
    strFilePath += pDataFile;

    _wstring strSkyFilePath = strFilePath + TEXT("_sky.dat");
    _wstring strCloudFilePath = strFilePath + TEXT("_cloud.dat");

    /* 파일 입출력으로 이니셜라이즈에서 구조체 채우기 */
    DWORD dwByte = {};

    // 하늘
    HANDLE hSkyFile = CreateFile(strSkyFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    CHECK_EQUAL(INVALID_HANDLE_VALUE, hSkyFile, E_FAIL);
    CHECK_FALSE(ReadFile(hSkyFile, &m_Sky_Desc, sizeof(SKY_DESC), &dwByte, nullptr), E_FAIL);

    CloseHandle(hSkyFile);

    // 구름
    HANDLE hCloudFile = CreateFile(strCloudFilePath.c_str(), GENERIC_READ, NULL, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    CHECK_EQUAL(INVALID_HANDLE_VALUE, hCloudFile, E_FAIL);
    CHECK_FALSE(ReadFile(hCloudFile, &m_Cloud_Desc, sizeof(CLOUD_DESC), &dwByte, nullptr), E_FAIL);

    CloseHandle(hCloudFile);

    return S_OK;
}

void CHeinMach_Trigger::Set_FogConfig(FOG_CONFIG FogConfig)
{
    m_pGameInstance->Set_FogConfig(FogConfig);
}

void CHeinMach_Trigger::Start_SkyTransition(SKY_DESC SkyDesc, CLOUD_DESC CloudDesc, _float fDuration)
{
    static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Sky"), 0))->Start_LerpSky(SkyDesc, fDuration);
    static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Sky"), 1))->Start_LerpCloud(CloudDesc, fDuration);
}

CHeinMach_Trigger* CHeinMach_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHeinMach_Trigger* pInstance = new CHeinMach_Trigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CHeinMach_Trigger"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHeinMach_Trigger::Clone(void* pArg)
{
    CHeinMach_Trigger* pInstance = new CHeinMach_Trigger(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CHeinMach_Trigger"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHeinMach_Trigger::Free()
{
    __super::Free();

    Safe_Release(m_pClientInstance);
    Safe_Release(m_pHeinMach_Field);
    Safe_Release(m_pHeinMach_Yetuga);
    Safe_Release(m_pHeinMach_Start_Chat);
}
