#include "Viper_Trigger.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Camera_Compre.h"
#include "Transform.h"
#include "Creature.h"
#include "UI_Tutorial.h"
#include "SkySphere.h"
#include "Viper.h"
#include "Khazan_GSword.h"
#include "Sequence_Viper_CutScene.h"
#include "ClientInstance.h"
#include "UI_HUD.h"


CViper_Trigger::CViper_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTrigger { pDevice, pContext }
    , m_pClientInstance { CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

CViper_Trigger::CViper_Trigger(const CViper_Trigger& Prototype)
    : CTrigger { Prototype }
    , m_pClientInstance { Prototype.m_pClientInstance }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CViper_Trigger::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CViper_Trigger::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_TriggerType(pArg), E_FAIL);

    return S_OK;
}

void CViper_Trigger::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CViper_Trigger::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CViper_Trigger::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CViper_Trigger::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(1), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CViper_Trigger::Ready_Components(void* pArg)
{
    return S_OK;
}

HRESULT CViper_Trigger::Ready_Collision(void* pArg)
{
    return S_OK;
}

HRESULT CViper_Trigger::Ready_TriggerType(void* pArg)
{
    if (m_strTriggerKey == "CutScene")
    {
    }
    else if (m_strTriggerKey == "Viper")
    {
        /*
        m_pViper_Boss = CSequence_HeinMach_Yetuga::Create(
            dynamic_cast<CCamera_Compre*>(m_pClientInstance->Find_Camera(ENUM_CLASS(LEVEL::HEINMACH), CAMERATYPE::PLAYER)),
            dynamic_cast<CCreature*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Creature_Player")))
        );
        */
    }
    else if (m_strTriggerKey == "Day")
    {
        m_eDayCircle = DAY_CIRCLE::DAY;
    }
    else if (m_strTriggerKey == "Dawn")
    {
        m_eDayCircle = DAY_CIRCLE::DAWN;
    }


    return S_OK;
}

void CViper_Trigger::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER))
    {

        static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(false);

        if (m_strTriggerKey == "CutScene")
        {
            SEQ_REQ_PLAY_DESC tPlayDesc{};
            tPlayDesc.tId.iSeq = 1000;
            tPlayDesc.pAsset = L"Field_Cut";
            tPlayDesc.fStartTime = 0.f;
            //m_pGameInstance->SEQ_AdoptAndPlay(m_pViper_Field, tPlayDesc);

            m_isDead = true;
        }
        else if (m_strTriggerKey == "Viper")
        {
            CViper* pViper = dynamic_cast<CViper*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Viper")));
            CKhazan_GSword* pKhazan = dynamic_cast<CKhazan_GSword*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Creature_Player")));
            CSequence_Viper_CutScene* pSequence = CSequence_Viper_CutScene::Create(pViper, pKhazan);

            SEQ_REQ_PLAY_DESC tPlayDesc{};
            tPlayDesc.tId.iSeq = 100010;
            tPlayDesc.pAsset = L"Viper_CutScene";
            tPlayDesc.fStartTime = 0.f;

            m_pGameInstance->SEQ_AdoptAndPlay(pSequence, tPlayDesc);
            m_isDead = true;
        }
        else if (DAY_CIRCLE::NONE != m_eDayCircle)
        {
            switch (m_eDayCircle)
            {
            case DAY_CIRCLE::DAWN:
                break;
            case DAY_CIRCLE::DAY:
                break;
            }
        }
        else if (m_strTriggerKey == "Feel_Viper")
        {
            m_pGameInstance->Emit_Event<EVENT_ANNOUNCE_TALK>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_TALK), EVENT_ANNOUNCE_TALK{ 23 });

            m_isDead = true;
        }
    }

}

void CViper_Trigger::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CViper_Trigger::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

CViper_Trigger* CViper_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CViper_Trigger* pInstance = new CViper_Trigger(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CViper_Trigger"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CViper_Trigger::Clone(void* pArg)
{
    CViper_Trigger* pInstance = new CViper_Trigger(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CViper_Trigger"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CViper_Trigger::Free()
{
    __super::Free();

    Safe_Release(m_pClientInstance);
    //m_pViper_Field = nullptr;
    //m_pViper_Boss = nullptr;
}
