#include "Lever_Gear.h"

#include "GameInstance.h"

CLever_Gear::CLever_Gear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CLever_Gear::CLever_Gear(const CLever_Gear& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CLever_Gear::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CLever_Gear::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    LEVER_GEAR_DESC* pDesc = static_cast<LEVER_GEAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    _int* pInt = static_cast<_int*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pInt, E_FAIL);

    m_iEventID = *pInt;

    m_eAnimState = ANIM_STATE::IDLE1;
    m_pModelCom->Set_Animation(ANIM_STATE::IDLE1);
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(false);
    m_pModelCom->Play_Animation(0.f);
    m_pModelCom->Set_AnimationBlend(true);

    switch (m_iEventID)
    {
    case 0:
        m_eEventType = EVENT_TYPE::GATE_GEAR0;
        break;
    case 1:
        m_eEventType = EVENT_TYPE::GATE_GEAR1;
        break;
    default:
        m_eEventType = EVENT_TYPE::END;
        break;
    }

    if (EVENT_TYPE::END != m_eEventType)
        m_iSubscribeEventID = m_pGameInstance->Subscribe_Event<EventGateGear>(ENUM_CLASS(m_eEventType), [&](const EventGateGear& e) { m_EventGate = e; });

    return S_OK;
}

void CLever_Gear::Priority_Update(_float fTimeDelta)
{
}

void CLever_Gear::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);

    m_fBlinkTimeAcc += fTimeDelta;
}

void CLever_Gear::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CLever_Gear::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CLever_Gear : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (GEAR == i)
        {
            if (false == m_EventGate.isFirstStep())
            {
                if (FAILED(Bind_Blink_ShaderResources()))
                    return E_FAIL;

                CHECK_FAILED_ASSERT(m_pShaderCom->Begin(30), E_FAIL);
            }
            else
                CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);
        }
        else if (RUNE == i)
        {
            _float3 vRuneColor = _float3(0.f, 1.f, 1.5f);
            if (FAILED(m_pShaderCom->Bind_RawValue("g_vRuneColor", &vRuneColor, sizeof(_float3))))
                return E_FAIL;

            _float fEmissiveIntensity = 40.f;
            if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fEmissiveIntensity, sizeof(_float))))
                return E_FAIL;

            CHECK_FAILED_ASSERT(m_pShaderCom->Begin(24), E_FAIL);
        }

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CLever_Gear::Ready_Components(void* pArg)
{
    LEVER_GEAR_DESC* pDesc = static_cast<LEVER_GEAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CLever_Gear::Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (GEAR == iMeshIndex)
    {
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
            m_iMtrlFlags |= M_DIFFUSE;
    }
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    if (RUNE == iMeshIndex && true == m_EventGate.isFirstStep())
        m_iMtrlFlags |= M_EMISSIVE;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

void CLever_Gear::Animation_Update(_float fTimeDelta)
{
    if (m_EventGate.isFirstStep())          // 레버 작동 완료 시
    {
        if (ANIM_STATE::IDLE1 == m_eAnimState)
        {
            SoundOnce(TEXT("IP_Lever_Gear_Active1"), Get_Position(), nullptr, m_fInteract_Volume);

            m_eAnimState = ANIM_STATE::ACTIVATION1;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
            _vector Effectpos = m_pTransformCom->Get_State(STATE::POSITION);
            Effectpos += XMVector4Normalize(m_pTransformCom->Get_State(STATE::UP)) * 1.4f;
            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS), TEXT("LeverGear_On"), m_pTransformCom->Get_WorldMatrix(), Effectpos);
            //가동시작!
        }
    }
}

void CLever_Gear::Animation_Change(_float fTimeDelta)
{
    if (ANIM_STATE::ACTIVATION1 == m_eAnimState)
    {
        SoundOnce(TEXT("IP_Lever_Gear_Active2"), Get_Position(), nullptr, m_fInteract_Volume);

        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::ACTIVATION2;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(false);
        //가동끝! 
        _vector Effectpos = m_pTransformCom->Get_State(STATE::POSITION);
        Effectpos += XMVector4Normalize(m_pTransformCom->Get_State(STATE::UP)) * 1.4f;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::EMBARS),TEXT("LeverGear_On_Static"), m_pTransformCom->Get_WorldMatrix(), Effectpos);

        m_EventGate.isActiveGear1 = true;

        // OPENING 중에는 UI, Player 용 Active 변수는 false, 상자 앞 위치랑 상자 위치 던지기
        m_pGameInstance->Emit_Event<EventGateGear>(ENUM_CLASS(m_eEventType), m_EventGate);
    }
    else if (ANIM_STATE::ACTIVATION2 == m_eAnimState)
    {
        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::IDLE2;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->Set_AnimationLoop(false);
        //m_pModelCom->Set_AnimationBlend(false);
    }
}

HRESULT CLever_Gear::Bind_Blink_ShaderResources()
{
    _float fRimPower = 5.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &fRimPower, sizeof(_float))))
        return E_FAIL;

    _float fRimIntensity = 1.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimLightIntensity", &fRimIntensity, sizeof(_float))))
        return E_FAIL;

    // 반짝이는 림라이트 이미시브
    _float fRimEmissive = 5.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimEmissive", &fRimEmissive, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fBlinkTimeAcc, sizeof(_float))))
        return E_FAIL;

    _float fCycleSpeed = 3.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCycleSpeed", &fCycleSpeed, sizeof(_float))))
        return E_FAIL;

    _float3 vRimColor = _float3(1.f, 1.f, 1.f);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &vRimColor, sizeof(_float3))))
        return E_FAIL;

    return S_OK;
}

CLever_Gear* CLever_Gear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLever_Gear* pInstance = new CLever_Gear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLever_Gear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLever_Gear::Clone(void* pArg)
{
    CLever_Gear* pInstance = new CLever_Gear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLever_Gear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLever_Gear::Free()
{
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(m_eEventType), m_iSubscribeEventID);

    __super::Free();
}
