#include "HeinMach_Trigger.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Sequence_HeinMach_Field.h"
#include "Sequence_HeinMach_Yetuga.h"
#include "Camera_Compre.h"
#include "Transform.h"
#include "Creature.h"
#include "UI_Tutorial.h"
#include "SkySphere.h"

CHeinMach_Trigger::CHeinMach_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTrigger{ pDevice, pContext }
    , m_pClientInstance { CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

CHeinMach_Trigger::CHeinMach_Trigger(const CHeinMach_Trigger& Prototype)
    : CTrigger{ Prototype }
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
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources ÇÔ¼ö E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(1), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

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

    }
    else if (m_strTriggerKey == "Guide_LockOn")
    {
        m_eGuideType = GUIDE_TYPE::LOCKON;
    }
    else if (m_strTriggerKey == "Guide_Guard")
    {
        m_eGuideType = GUIDE_TYPE::GUARD;
    }
    // else if (m_strTriggerKey == "Guide_LockOn")
    // {
    //     m_eGuideType = GUIDE_TYPE::LOCKON;
    // }
    // else if (m_strTriggerKey == "Guide_LockOn")
    // {
    //     m_eGuideType = GUIDE_TYPE::LOCKON;
    // }
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

void CHeinMach_Trigger::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
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
        }
        else if (GUIDE_TYPE::END != m_eGuideType)
        {
            // BURTALATTACK ¤¤¤¤
            // Guide_LockOn
            // Guide_Guard
            //enum class GUIDE_TYPE { LOCKON, GUARD, UNDERWORLD, DODGE, BURTALATTACK, FALLATTACK, IMPULSE, END };

            CUI_Tutorial* pUI_Tutorial = static_cast<CUI_Tutorial*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Tutorial")));
            CHECK_NULLPTR_MSG(pUI_Tutorial, TEXT("pUI_Tutorial == nullptr"), );

            switch (m_eGuideType)
            {
            case GUIDE_TYPE::LOCKON:
            case GUIDE_TYPE::GUARD:
                pUI_Tutorial->On_Panel(m_eGuideType);
                break;
            }

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
    }
    
}

void CHeinMach_Trigger::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CHeinMach_Trigger::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

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
    m_pHeinMach_Field = nullptr;
    m_pHeinMach_Yetuga = nullptr;
}
