#include "UnLockGear.h"

#include "GameInstance.h"

#include "Interaction_Guide.h"
#include "Sequence_Embars_Puzzle_First.h"
#include "Sequence_Embars_Puzzle_Second.h"
#include "Sequence_Embars_Puzzle_Third.h"

CUnLockGear::CUnLockGear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CUnLockGear::CUnLockGear(const CUnLockGear& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CUnLockGear::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CUnLockGear::Initialize_Clone(void* pArg)
{
    UNLOCK_GEAR_DESC* pDesc = static_cast<UNLOCK_GEAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

#pragma region 범수 상호작용 슥슥

    CHECK_FAILED(Ready_Interaction_Guide(pArg), E_FAIL);

#pragma endregion

    _int* pInt = static_cast<_int*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pInt, E_FAIL);

    m_iEventID = *pInt;

    m_eAnimState = ANIM_STATE::IDLE;
    m_pModelCom->Set_Animation(ANIM_STATE::IDLE);
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(true);

    m_iObjectInteractEventID = m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e)
        {
            m_Event = e;
        });
     
    switch (m_iEventID)
    {
    case 0:
        m_eGimmickType = EVENT_TYPE::EMBARS_GIMMICK0;
        break;
    case 1:
        m_eGimmickType = EVENT_TYPE::EMBARS_GIMMICK1;
        break;
    case 2:
        m_eGimmickType = EVENT_TYPE::EMBARS_GIMMICK2;
        break;
    }
    m_iGimmickTypeEventID =  m_pGameInstance->Subscribe_Event<EventGimmick>(ENUM_CLASS(m_eGimmickType), [&](const EventGimmick& e)
        {
            m_EventGimmick = e;
        });

    m_eEventType = EVENT_TYPE::HALL_ELEVATOR_UNLOCK;

    m_iSubscribeEventID = m_pGameInstance->Subscribe_Event<EventHallElevator>(ENUM_CLASS(m_eEventType), [&](const EventHallElevator& e) { m_EventHallElevator = e; });

    return S_OK;
}

void CUnLockGear::Priority_Update(_float fTimeDelta)
{
    if (false == m_isCollision)
    {
        m_Event.None();
    }

    if (true == m_EventGimmick.isUnLockGearAvailable(m_iEventID) && false == m_isGimmickDone)
    {
        m_isGimmickDone = true;

        SoundOnce(TEXT("IP_Statue_Done"), m_fInteract_Volume);
    }
}

void CUnLockGear::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);

    m_fBlinkTimeAcc += fTimeDelta;
}

void CUnLockGear::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CUnLockGear::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CUnLockGear : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    _float fRimPower = 5.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &fRimPower, sizeof(_float))))
        return E_FAIL;

    _float fRimIntensity = 1.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimLightIntensity", &fRimIntensity, sizeof(_float))))
        return E_FAIL;

    // 보석 이미시브
    _float fEmissiveIntensity = 30.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fEmissiveIntensity, sizeof(_float))))
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

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        // CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);
        
        // Unlock Gear 보석 살리기, Blink Rim Light Test

        if (true == m_EventGimmick.isUnLockGearAvailable(m_iEventID) || EVENT_TYPE::EMBARS_GIMMICK2 == m_eGimmickType)
        {
            if (false == m_isForceOff)
                CHECK_FAILED_ASSERT(m_pShaderCom->Begin(12), E_FAIL);
            else
                CHECK_FAILED_ASSERT(m_pShaderCom->Begin(13), E_FAIL);
        }
        else
            CHECK_FAILED_ASSERT(m_pShaderCom->Begin(13), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CUnLockGear::Ready_Components(void* pArg)
{
    UNLOCK_GEAR_DESC* pDesc = static_cast<UNLOCK_GEAR_DESC*>(pArg);
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

HRESULT CUnLockGear::Ready_Collision(void* pArg)
{
#pragma region 스태틱 몸체
    CBody::BODY_BOXSHAPE_DESC StaticBodyDesc{};
    StaticBodyDesc.vExtent = _float3(0.75f, 0.5f, 0.75f);
    StaticBodyDesc.bIsTrigger = false;
    StaticBodyDesc.bStartActive = true;
    StaticBodyDesc.eMotion = EMotionType::Static;
    StaticBodyDesc.eQuality = EMotionQuality::LinearCast;
    StaticBodyDesc.eShapeType = SHAPE::BOX;
    StaticBodyDesc.fFriction = 0.8f;
    StaticBodyDesc.fMass = 1.0f;
    StaticBodyDesc.fRestitution = 0.0f;
    StaticBodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_STATIC_TRIGGER);

    XMStoreFloat3(&StaticBodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * 1.f);
    StaticBodyDesc.vPos.y += StaticBodyDesc.vExtent.y;

    m_vLookPosition = StaticBodyDesc.vPos;

    XMStoreFloat4(&StaticBodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

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
    TriggerDesc.vExtent = _float3(0.5f, 0.5f, 0.5f);
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

    m_vCharacterPosition = TriggerDesc.vPos;

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

HRESULT CUnLockGear::Ready_Interaction_Guide(void* pArg)
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::PROGRESS, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, 10.f), TEXT("해제해잇"), 1.f);

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::EMBARS), TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

void CUnLockGear::Input_Interact_Event(_float fTimeDelta)
{
    if (ANIM_STATE::START == m_eAnimState || ANIM_STATE::STOP == m_eAnimState)
        return;

    _bool isPressing = { false };

    if (m_pGameInstance->Key_Pressing(DIK_F, fTimeDelta))
    {
        isPressing = m_pGuide->IsPressing();
    }

    if (true == isPressing)
    {
        m_pGuide->Update_Visible(false);

        EventInteractType InteractType = {};

        InteractType.eInteractType = INTERACTIVE_TYPE::UNLOCKGEAR;

        InteractType.eState = EventInteractType::BEGIN;

        EventUnLockGear UnLockGearEvent = {};

        UnLockGearEvent.vPosition = _float4(m_vLookPosition.x, m_vLookPosition.y, m_vLookPosition.z, 1.f);
        UnLockGearEvent.vPlayerPosition = _float4(m_vCharacterPosition.x, m_vCharacterPosition.y, m_vCharacterPosition.z, 1.f);

        InteractType.UnLockGearEvent = UnLockGearEvent;

        m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
    }
}

void CUnLockGear::Animation_Update(_float fTimeDelta)
{
    if (0.f != m_fColTimeAcc)
    {
        m_fColTimeAcc += fTimeDelta;

        if (0.931f <= m_fColTimeAcc)
        {
            m_fColTimeAcc = 0.f;
            m_pStaticCom->Collision_Active(true);
        }
    }

    if (false == m_isCollision)
        return;

    Input_Interact_Event(fTimeDelta);

    if (m_Event.isOn())               // 켠다는 신호
    {
        if (ANIM_STATE::IDLE == m_eAnimState)
        {
            m_pStaticCom->Collision_Active(false);

            m_pGuide->Update_Visible(false);

            // 비활성화 레버 상호 작용 시
            m_eAnimState = ANIM_STATE::START;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);

            EventInteractType InteractType = {};

            InteractType.eInteractType = INTERACTIVE_TYPE::UNLOCKGEAR;
            InteractType.isEvent = true;

            EventUnLockGear UnLockGearEvent = {};

            _matrix OffSetMatrix = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Root")) * m_pTransformCom->Get_WorldMatrix();

            UnLockGearEvent.vPosition = _float4(m_vLookPosition.x, m_vLookPosition.y, m_vLookPosition.z, 1.f);
            UnLockGearEvent.vPlayerPosition = _float4(m_vCharacterPosition.x, m_vCharacterPosition.y, m_vCharacterPosition.z, 1.f);

            InteractType.UnLockGearEvent = UnLockGearEvent;            

            if (m_iEventID == 0)
            {
                CSequence_Embars_Puzzle_First* pSequence = CSequence_Embars_Puzzle_First::Create();

                SEQ_REQ_PLAY_DESC tPlayDesc{};
                tPlayDesc.tId.iSeq = 110000;
                tPlayDesc.pAsset = L"Puzzle_First";
                tPlayDesc.fStartTime = 0.f;

                m_pGameInstance->SEQ_AdoptAndPlay(pSequence, tPlayDesc);
            }
            else if (m_iEventID == 1)
            {
                CSequence_Embars_Puzzle_Second* pSequence = CSequence_Embars_Puzzle_Second::Create();

                SEQ_REQ_PLAY_DESC tPlayDesc{};
                tPlayDesc.tId.iSeq = 110001;
                tPlayDesc.pAsset = L"Puzzle_Second";
                tPlayDesc.fStartTime = 0.f;

                m_pGameInstance->SEQ_AdoptAndPlay(pSequence, tPlayDesc);
            }
            else if (m_iEventID == 2)
            {
                CSequence_Embars_Puzzle_Third* pSequence = CSequence_Embars_Puzzle_Third::Create();

                SEQ_REQ_PLAY_DESC tPlayDesc{};
                tPlayDesc.tId.iSeq = 110002;
                tPlayDesc.pAsset = L"Puzzle_Third";
                tPlayDesc.fStartTime = 0.f;

                m_pGameInstance->SEQ_AdoptAndPlay(pSequence, tPlayDesc);
            }

            // OPENING 중에는 UI, Player 용 Active 변수는 false, 상자 앞 위치랑 상자 위치 던지기
            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
        }
    }
}

void CUnLockGear::Animation_Change(_float fTimeDelta)
{
    if (ANIM_STATE::START == m_eAnimState)
    {
        m_fColTimeAcc += fTimeDelta;

        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::STOP;
        m_pModelCom->Set_Animation(m_eAnimState);
        m_pModelCom->AnimationLoop(false);

        m_isForceOff = true;

        m_pTriggerCom->Collision_Active(false);

        m_EventHallElevator.EventOn();
        m_EventHallElevator.Set_UnLockState(true);

        m_pGameInstance->Emit_Event<EventHallElevator>(ENUM_CLASS(m_eEventType), m_EventHallElevator);
    }
}

HRESULT CUnLockGear::Bind_Materials(_uint iMeshIndex)
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
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", iMeshIndex, aiTextureType_METALNESS, 0)))
        m_iMtrlFlags |= M_METALIC;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", iMeshIndex, aiTextureType_SHININESS, 0)))
        m_iMtrlFlags |= M_ROUGHNESS;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

void CUnLockGear::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (m_EventGimmick.isUnLockGearAvailable(m_iEventID) || EVENT_TYPE::EMBARS_GIMMICK2 == m_eGimmickType)
    {
        if (ANIM_STATE::IDLE == m_eAnimState)
        {
            if (EVENT_TYPE::GATE_GEAR0 == m_eEventType || EVENT_TYPE::GATE_GEAR1 == m_eEventType)
                m_pGuide->Update_Visible(false);
            else
                m_pGuide->Update_Visible(true);
        }

        m_isCollision = true;
    }
}

void CUnLockGear::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (m_EventGimmick.isUnLockGearAvailable(m_iEventID) || EVENT_TYPE::EMBARS_GIMMICK2 == m_eGimmickType)
    {
        m_isCollision = true;
    }
}

void CUnLockGear::Collision_Exit(COLLISION_DESC * pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    m_pGuide->Update_Visible(false);

    m_isCollision = false;
}

CUnLockGear* CUnLockGear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUnLockGear* pInstance = new CUnLockGear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUnLockGear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUnLockGear::Clone(void* pArg)
{
    CUnLockGear* pInstance = new CUnLockGear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CUnLockGear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUnLockGear::Free()
{
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), m_iObjectInteractEventID);
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(m_eGimmickType), m_iGimmickTypeEventID);
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(m_eEventType), m_iSubscribeEventID);

    __super::Free();

    Safe_Release(m_pStaticCom);
    Safe_Release(m_pTriggerCom);
    if (m_pGuide)
    {
        m_pGuide->Set_IsDead(true);
        m_pGuide = nullptr;
    }
        
}
