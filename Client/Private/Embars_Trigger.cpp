#include "Embars_Trigger.h"
#include "GameInstance.h"
#include "ClientInstance.h"
//#include "Sequence_HeinMach_Field.h"
//#include "Sequence_HeinMach_Yetuga.h"
//#include "Sequence_HeinMach_Start_Chat.h"
#include "Camera_Compre.h"
#include "Transform.h"
#include "Creature.h"

#include "MapObject_Header.h"

CEmbars_Trigger::CEmbars_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTrigger { pDevice, pContext }
    , m_pClientInstance { CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

CEmbars_Trigger::CEmbars_Trigger(const CEmbars_Trigger& Prototype)
    : CTrigger { Prototype }
    , m_pClientInstance { Prototype.m_pClientInstance }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CEmbars_Trigger::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CEmbars_Trigger::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_TriggerType(pArg), E_FAIL);

    return S_OK;
}

void CEmbars_Trigger::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CEmbars_Trigger::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    /*if (m_EventVTGate.isUnLockGate(m_iEventID))
    {
        if (m_iEventID == 0)
        {
            if (m_strTriggerKey == "Puzzle_1")
            {
                m_pClientInstance->Camera_Set_FixEnd();
                m_isDead = true;
            }
        }

        else if (m_iEventID == 1)
        {
            if (m_strTriggerKey == "Puzzle_2")
            {
                m_pClientInstance->Camera_Set_FixEnd();
                m_isDead = true;
            }
        }
    }*/

}

void CEmbars_Trigger::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    
    
    
}

HRESULT CEmbars_Trigger::Render()
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

HRESULT CEmbars_Trigger::Ready_Components(void* pArg)
{
    return S_OK;
}

HRESULT CEmbars_Trigger::Ready_Collision(void* pArg)
{
    return S_OK;
}

HRESULT CEmbars_Trigger::Ready_TriggerType(void* pArg)
{
    if (m_strTriggerKey == "Puzzle_1")
    {
        /*m_iEventID = 0;
        string filePath = "../../Client/Bin/Data/Camera/Animation/Statue1";
        m_pClientInstance->Camera_Set_Animation_Json(filePath);
        m_iEventID = m_pGameInstance->Subscribe_Event<EventVerticalGate>(ENUM_CLASS(EVENT_TYPE::STATUE_PUZZLE0), [&](const EventVerticalGate& e) { m_EventVTGate = e; });*/        
    }
    else if (m_strTriggerKey == "Puzzle_2")
    {
        /*m_iEventID = 1;
        string filePath = "../../Client/Bin/Data/Camera/Animation/Statue2";
        m_pClientInstance->Camera_Set_Animation_Json(filePath);
        m_iEventID = m_pGameInstance->Subscribe_Event<EventVerticalGate>(ENUM_CLASS(EVENT_TYPE::STATUE_PUZZLE1), [&](const EventVerticalGate& e) { m_EventVTGate = e; });*/
    }

    /*
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
    */

    /*
    if (m_strTriggerKey == "Talk_03")
    {
        m_pHeinMach_Start_Chat = CSequence_HeinMach_Start_Chat::Create();
    }
    */

    return S_OK;
}

void CEmbars_Trigger::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER))
    {
        if (m_strTriggerKey == "Puzzle_1")
        {
            m_pClientInstance->Camera_Set_Animation(TEXT("Statue1"));
        }
        else if (m_strTriggerKey == "Puzzle_2")
        {
            m_pClientInstance->Camera_Set_Animation(TEXT("Statue2"));
        }
        else if (m_strTriggerKey == "포그 바뀌는")
        {
            FOG_TRANSITION_DESC Desc{};
            Desc.fDensity = 0.035f;
            Desc.fBias = 1.f;
            Desc.vColor = _float4(0.031f, 0.137f, 0.200f, 1.f);
            Desc.isUseHeight = false;
            Desc.isUseNoise = false;
            m_pGameInstance->Start_FogTransition(5.f, Desc);

            // 그림자 보간 추가
            m_pGameInstance->Start_ShadowTransition(5.f, 1.f);
        }
        /*
        else if (m_strTriggerKey == "Talk_03")
        {
            SEQ_REQ_PLAY_DESC tPlayDesc{};
            tPlayDesc.tId.iSeq = 1100;
            tPlayDesc.pAsset = L"Start_Chat";
            tPlayDesc.fStartTime = 0.f;
            m_pGameInstance->SEQ_AdoptAndPlay(m_pHeinMach_Start_Chat, tPlayDesc);

            m_isDead = true;
        }
        */
    }
}

void CEmbars_Trigger::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CEmbars_Trigger::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER))
    {
        if (m_strTriggerKey == "Puzzle_1" || m_strTriggerKey == "Puzzle_2")
        {
            m_pClientInstance->Camera_Set_FixEnd();
        }
    }

}

void CEmbars_Trigger::Set_FogConfig(FOG_CONFIG FogConfig)
{
    m_pGameInstance->Set_FogConfig(FogConfig);
}

CEmbars_Trigger* CEmbars_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEmbars_Trigger* pInstance = new CEmbars_Trigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEmbars_Trigger"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEmbars_Trigger::Clone(void* pArg)
{
    CEmbars_Trigger* pInstance = new CEmbars_Trigger(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CEmbars_Trigger"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEmbars_Trigger::Free()
{
    if (m_strTriggerKey == "Puzzle_1")
    {
        //m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::STATUE_PUZZLE0), m_iEventID);
    }
    else if (m_strTriggerKey == "Puzzle_2")
    {
        //m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::STATUE_PUZZLE1), m_iEventID);
    }
    

    __super::Free();

    Safe_Release(m_pClientInstance);
    //Safe_Release(m_pHeinMach_Field);
    //Safe_Release(m_pHeinMach_Yetuga);
    //Safe_Release(m_pHeinMach_Start_Chat);
}
