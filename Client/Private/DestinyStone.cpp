#include "DestinyStone.h"

#include "GameInstance.h"

#include "Interaction_Guide.h"

#include "DestinyGem.h"

CDestinyStone::CDestinyStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CDestinyStone::CDestinyStone(const CDestinyStone& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CDestinyStone::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CDestinyStone::Initialize_Clone(void* pArg)
{
    DESTINYSTONE_DESC* pDesc = static_cast<DESTINYSTONE_DESC*>(pArg);
    CHECK_NULLPTR(pArg, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    CHECK_FAILED(Ready_Interaction_Guide(pArg), E_FAIL);

    _uint* pInt = static_cast<_uint*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pInt, E_FAIL);

    _tchar m_szLightTag[MAX_PATH] = {  };

    wsprintf(m_szLightTag, TEXT("DestinyStoneLight%d"), *pInt);

    m_wstrLightTag = m_szLightTag;

    m_pTransformCom->Scale(_float3(0.01f, 0.01f, 0.01f));

    LIGHT_DESC LightDesc = {};

    LightDesc.eType = LIGHT_DESC::TYPE::POINT;

    LightDesc.vDiffuse = _float4(0.9f, 0.05f, 0.05f, 1.f);
    LightDesc.vAmbient = _float4(0.28f, 0.18f, 0.18f, 1.f);
    LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 1.f);
    XMStoreFloat4(&LightDesc.vPosition, m_pTransformCom->Get_State(STATE::POSITION));
    LightDesc.vPosition.y += 1.2f;

    LightDesc.fRange = 2.1f;

    m_pGameInstance->Add_Light(m_wstrLightTag, ENUM_CLASS(LEVEL::HEINMACH), LightDesc, true);

    m_iSubscribeEventID = m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e)
        {
            m_Event = e;
        });

    return S_OK;
}

void CDestinyStone::Priority_Update(_float fTimeDelta)
{
    if (false == m_isCollision)
    {
        m_Event.None();
    }

    if (true == m_isDissolved)
    {
        m_isDissolved = false;

        m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), { EventObject::OffEvent() });

        m_pGameInstance->Set_LightEnable(m_wstrLightTag, ENUM_CLASS(LEVEL::HEINMACH), false);

        LIGHT_TRANSITION_DESC LightDesc{};
        LightDesc.fDuration = 1.f;
        LightDesc.vFadeTime = _float2(1.f, 0.f);
        LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
        LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
        LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
        LightDesc.isReturnToStart = false;
        LightDesc.Callback = [&]() { m_pGameInstance->Set_LightEnable(m_wstrLightTag, ENUM_CLASS(LEVEL::HEINMACH), false); };
        m_pGameInstance->Start_LightTransition(m_wstrLightTag, ENUM_CLASS(LEVEL::HEINMACH), LightDesc);

        //  m_pGameInstance->Set_LightEnable(m_wstrLightTag, ENUM_CLASS(LEVEL::HEINMACH), false);
    }

    __super::Priority_Update(fTimeDelta);
}

void CDestinyStone::Update(_float fTimeDelta)
{
    Event_Update(fTimeDelta);

    __super::Update(fTimeDelta);

    m_fBlinkTimeAcc += fTimeDelta;
}

void CDestinyStone::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);

    __super::Late_Update(fTimeDelta);
}

HRESULT CDestinyStone::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CLever : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    _float fDiffuseRedPower = 1.25f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseRedPower", &fDiffuseRedPower, sizeof(_float))))
        return E_FAIL;

    _float fEmissiveIntensity = 5.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    _float4 vGemColor = _float4(0.5f, 0.5f, 0.5f, 1.f);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vGemColor", &vGemColor, sizeof(_float4))))
        return E_FAIL;

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        if (false == m_isInteracted)
        {
            if (FAILED(Bind_Blink_ShaderResources()))
                return E_FAIL;

            CHECK_FAILED_ASSERT(m_pShaderCom->Begin(16), E_FAIL);
        }
        else
            CHECK_FAILED_ASSERT(m_pShaderCom->Begin(12), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CDestinyStone::Ready_Components(void* pArg)
{
    DESTINYSTONE_DESC* pDesc = static_cast<DESTINYSTONE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CDestinyStone::Ready_Collision(void* pArg)
{
#pragma region 스태틱 몸체
    CBody::BODY_BOXSHAPE_DESC StaticBodyDesc{};
    StaticBodyDesc.vExtent = _float3(0.3f, 1.f, 0.3f);
    StaticBodyDesc.bIsTrigger = false;
    StaticBodyDesc.bStartActive = true;
    StaticBodyDesc.eMotion = EMotionType::Static;
    StaticBodyDesc.eQuality = EMotionQuality::LinearCast;
    StaticBodyDesc.eShapeType = SHAPE::BOX;
    StaticBodyDesc.fFriction = 0.8f;
    StaticBodyDesc.fMass = 1.0f;
    StaticBodyDesc.fRestitution = 0.0f;
    StaticBodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_STATIC_TRIGGER);
    _float3 vPos{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    vPos.y += StaticBodyDesc.vExtent.y;
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    StaticBodyDesc.vPos = vPos;
    StaticBodyDesc.vQuat = vQuat;
    StaticBodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    StaticBodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Static"), reinterpret_cast<CComponent**>(&m_pStaticCom), &StaticBodyDesc)))
        return E_FAIL;
#pragma endregion

#pragma region 트리거 영역
    CBody::BODY_BOXSHAPE_DESC TriggerDesc{};
    TriggerDesc.vExtent = _float3(2.f, 1.f, 2.f);
    TriggerDesc.bIsTrigger = true;
    TriggerDesc.bStartActive = true;
    TriggerDesc.eMotion = EMotionType::Kinematic;
    TriggerDesc.eQuality = EMotionQuality::LinearCast;
    TriggerDesc.eShapeType = SHAPE::BOX;
    TriggerDesc.fFriction = 0.8f;
    TriggerDesc.fMass = 1.0f;
    TriggerDesc.fRestitution = 0.0f;
    TriggerDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT);

    XMStoreFloat3(&TriggerDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    TriggerDesc.vPos.y += TriggerDesc.vExtent.y;
    XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
    TriggerDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_TriggerCollisionDesc.pGameObject = this;
    m_TriggerCollisionDesc.isForceVaildation = true;
    TriggerDesc.pCollisionDesc = &m_TriggerCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger"), reinterpret_cast<CComponent**>(&m_pTriggerCom), &TriggerDesc)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CDestinyStone::Ready_Interaction_Guide(void* pArg)
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1] + 1.f), TEXT("흡수"), 0.75f);

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

HRESULT CDestinyStone::Ready_PartObjects(void* pArg)
{
    DESTINYSTONE_DESC* pDesc = static_cast<DESTINYSTONE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CDestinyGem::DESTINYGEM_DESC DestinyGemDesc = {};

    DestinyGemDesc.eLevel = eLevel;
    DestinyGemDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    DestinyGemDesc.pConsumed = &m_isInteracted;
    DestinyGemDesc.pDissolved = &m_isDissolved;

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Gem"), ENUM_CLASS(eLevel),
        TEXT("Prototype_GameObject_Prop_DestinyStone_Gem"), &DestinyGemDesc), E_FAIL);

    return S_OK;
}

void CDestinyStone::Event_Update(_float fTimeDelta)
{
    if (false == m_isCollision || true == m_isInteracted)
        return;

    Input_Interact_Event(fTimeDelta);

    if (m_Event.isOn())               // 켠다는 신호
    {
        SoundOnce(TEXT("IP_DestinyStone_Disappear"), m_fInteract_Volume);

        m_pGuide->Update_Visible(false);

        m_isInteracted = true;

        // 귀석 상호작용 시
        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::DESTINYSTONE;
        InteractType.isEvent = true;

        EventDestinyStone DSEvent = {};

        XMStoreFloat4(&DSEvent.vPosition, Get_Position());

        InteractType.DSEvent = DSEvent;

        // OPENING 중에는 UI, Player 용 Active 변수는 false, 상자 앞 위치랑 상자 위치 던지기
        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);

        m_Event.None();
    }
}

void CDestinyStone::Input_Interact_Event(_float fTimeDelta)
{
    CHECK_TRUE(m_isInteracted, );

    _bool isPressing = { false };

    if (m_pGameInstance->Key_Pressing(DIK_F, fTimeDelta))
    {
        isPressing = m_pGuide->IsPressing();
    }

    if (true == isPressing)
    {
        m_pGuide->Update_Visible(false);

        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::DESTINYSTONE;

        InteractType.eState = EventInteractType::BEGIN;

        EventDestinyStone DSEvent = {};

        XMStoreFloat4(&DSEvent.vPosition, Get_Position());

        InteractType.DSEvent = DSEvent;

        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
    }
}

HRESULT CDestinyStone::Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        m_iMtrlFlags |= M_DIFFUSE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;

    //  m_iMtrlFlags &= ~M_EMISSIVE;
    //  m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

HRESULT CDestinyStone::Bind_Blink_ShaderResources()
{
    _float fRimPower = 3.f;
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

void CDestinyStone::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (false == m_isInteracted)
        m_pGuide->Update_Visible(true);

    m_isCollision = true;
}

void CDestinyStone::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

    if (false == m_isInteracted)
        m_pGuide->Update_Visible(true);

    m_isCollision = true;
}

void CDestinyStone::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    m_pGuide->Update_Visible(false);

    m_isCollision = false;
}

CDestinyStone* CDestinyStone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDestinyStone* pInstance = new CDestinyStone(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDestinyStone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDestinyStone::Clone(void* pArg)
{
    CDestinyStone* pInstance = new CDestinyStone(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDestinyStone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDestinyStone::Free()
{
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), m_iSubscribeEventID);

    __super::Free();

    Safe_Release(m_pStaticCom);
    Safe_Release(m_pTriggerCom);

    if (nullptr != m_pGuide)
    {
        m_pGuide->Set_IsDead(true);
        m_pGuide = nullptr;
    }
}
