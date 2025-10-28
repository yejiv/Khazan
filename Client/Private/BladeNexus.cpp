#include "BladeNexus.h"

#include "GameInstance.h"

CBladeNexus::CBladeNexus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CBladeNexus::CBladeNexus(const CBladeNexus& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CBladeNexus::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CBladeNexus::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::BEFORE_IDLE;
    m_pModelCom->Set_Animation(ANIM_STATE::BEFORE_IDLE);
    m_pModelCom->Set_AnimationLoop(true);

    m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e)
        {
            m_isBNOn = e.isObjectOn;
            m_isBNOff = e.isObjectOff;
        });

    return S_OK;
}

void CBladeNexus::Priority_Update(_float fTimeDelta)
{
}

void CBladeNexus::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);
}

void CBladeNexus::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this), );
}

HRESULT CBladeNexus::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(0), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CBladeNexus::Ready_Components(void* pArg)
{
    BLADENEXUS_DESC* pDesc = static_cast<BLADENEXUS_DESC*>(pArg);
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

HRESULT CBladeNexus::Ready_Collision(void* pArg)
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
    StaticBodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_STATIC);
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
    TriggerDesc.vExtent = _float3(0.9f, 1.f, 0.9f);
    TriggerDesc.bIsTrigger = true;
    TriggerDesc.bStartActive = true;
    TriggerDesc.eMotion = EMotionType::Static;
    TriggerDesc.eQuality = EMotionQuality::LinearCast;
    TriggerDesc.eShapeType = SHAPE::BOX;
    TriggerDesc.fFriction = 0.8f;
    TriggerDesc.fMass = 1.0f;
    TriggerDesc.fRestitution = 0.0f;
    TriggerDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_INTERACT);

    XMStoreFloat3(&TriggerDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
    TriggerDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    TriggerDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger"), reinterpret_cast<CComponent**>(&m_pTriggerCom), &TriggerDesc)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

void CBladeNexus::Animation_Update(_float fTimeDelta)
{
    if (false == m_isCollide)
        return;

    if (true == m_isBNOn)               // 켠다는 신호
    {
        m_isBNOff = false;

        if (ANIM_STATE::BEFORE_IDLE == m_eAnimState)
        {
            // 처음 상호 작용 시
            m_eAnimState = ANIM_STATE::BEFORE_START;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));

            EventInteractType InteractType = {};

            InteractType.eInteractType = INTERACTIVE_TYPE::CHECKPOINT;
            InteractType.isEvent = true;

            EventBladeNexus BNEvent = {};

            XMStoreFloat3(&BNEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

            InteractType.BNEvent = BNEvent;

            // 귀검을 바라볼 수 있도록 포지션을 던짐
            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
        }
        else if (ANIM_STATE::AFTER_IDLE == m_eAnimState)
        {
            // 2번 이상의 상호 작용 시
            m_eAnimState = ANIM_STATE::AFTER_START;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));

            EventInteractType InteractType = {};

            InteractType.eInteractType = INTERACTIVE_TYPE::CHECKPOINT;
            InteractType.isEvent = true;

            EventBladeNexus BNEvent = {};

            XMStoreFloat3(&BNEvent.vPosition, m_pTransformCom->Get_State(STATE::POSITION));

            // 귀검을 바라볼 수 있도록 포지션을 던짐
            m_pGameInstance->Emit_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), InteractType);
        }
    }
    else if (true == m_isBNOff)         // 끈다는 신호 ( 내가 받기만 하면 됨
    {
        m_isBNOn = false;

        if (ANIM_STATE::BEFORE_LOOP == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::BEFORE_END;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        }
        if (ANIM_STATE::AFTER_LOOP == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::AFTER_END;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        }
    }
}

void CBladeNexus::Animation_Change(_float fTimeDelta)
{
    if (false == m_isCollide)
        return;

    if (ANIM_STATE::BEFORE_START == m_eAnimState)       // BEFORE_START 가 끝나면 BEFORE_LOOP ( 플레이어가 UI랑 상호 작용 )
    {
        // 처음 상호 작용 후 애니메이션 루프로 전환 및 이벤트 발생
        m_eAnimState = ANIM_STATE::BEFORE_LOOP;
        m_pModelCom->Set_Animation(ANIM_STATE::BEFORE_LOOP);
        m_pModelCom->Set_AnimationLoop(true);

        m_isBNOn = false;
    }
    if (ANIM_STATE::BEFORE_END == m_eAnimState)         // BEFORE_END 가 끝나면 AFTER_IDLE
    {
        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::AFTER_IDLE;
        m_pModelCom->Set_Animation(ANIM_STATE::AFTER_IDLE);
        m_pModelCom->Set_AnimationLoop(true);

        m_isBNOff = false;
    }
    if (ANIM_STATE::AFTER_START == m_eAnimState)
    {
        // 다회 상호 작용 후 애니메이션 루프로 전환
        m_eAnimState = ANIM_STATE::AFTER_LOOP;
        m_pModelCom->Set_Animation(ANIM_STATE::AFTER_LOOP);
        m_pModelCom->Set_AnimationLoop(true);

        m_isBNOn = false;
    }
    if (ANIM_STATE::AFTER_END == m_eAnimState)
    {
        // 다회 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::AFTER_IDLE;
        m_pModelCom->Set_Animation(ANIM_STATE::AFTER_IDLE);
        m_pModelCom->Set_AnimationLoop(true);

        m_isBNOff = false;
    }
}

void CBladeNexus::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    m_isCollide = true;
}

void CBladeNexus::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CBladeNexus::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
    m_isCollide = false;
}

CBladeNexus* CBladeNexus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBladeNexus* pInstance = new CBladeNexus(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBladeNexus::Clone(void* pArg)
{
    CBladeNexus* pInstance = new CBladeNexus(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBladeNexus::Free()
{
    __super::Free();

    Safe_Release(m_pStaticCom);
    Safe_Release(m_pTriggerCom);
}
