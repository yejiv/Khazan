#include "DanjinJar.h"

#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Talk_Danjinjar.h"

CDanjinJar::CDanjinJar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CDanjinJar::CDanjinJar(const CDanjinJar& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CDanjinJar::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CDanjinJar::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(CDanjinJar::Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_Collision(pArg), E_FAIL);

    DANJINJAR_DESC* pDesc = static_cast<DANJINJAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    DANJINJAR_INFO* pInfo = static_cast<DANJINJAR_INFO*>(pDesc->pOtherDesc);
    CHECK_NULLPTR(pInfo, E_FAIL);

    m_eJarType = pInfo->eJarType;
    m_DanjinJarStep = pInfo->StepPosition;

    m_fDefaultLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_DanjinJarStep.vStep1) - XMLoadFloat4(&m_DanjinJarStep.vStep2)));

    return S_OK;
}

void CDanjinJar::Priority_Update(_float fTimeDelta)
{
    m_pTalk->Priority_Update(fTimeDelta);
}

void CDanjinJar::Update(_float fTimeDelta)
{
    m_pTalk->Update_UITransform(m_pTransformCom->Get_State(STATE::POSITION));
    m_pTalk->Update(fTimeDelta);
}

void CDanjinJar::Late_Update(_float fTimeDelta)
{
    m_pTalk->Late_Update(fTimeDelta);
}

HRESULT CDanjinJar::Render()
{
    return S_OK;
}

HRESULT CDanjinJar::Ready_Components(void* pArg)
{
    DANJINJAR_DESC* pDesc = static_cast<DANJINJAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CDanjinJar::Ready_Collision(void* pArg)
{
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
    m_tCollisionDesc.pGameObject = this;
    m_tCollisionDesc.isForceVaildation = true;
    //pCollDesc.pInfo = ?? // 작성하기
    TriggerDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Trigger"), reinterpret_cast<CComponent**>(&m_pTriggerCom), &TriggerDesc)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CDanjinJar::Bind_Materials(_uint iMeshIndex)
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

    m_iMtrlFlags &= ~M_EMISSIVE;

    //m_iMtrlFlags &= ~M_EMISSIVE;
    //m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

HRESULT CDanjinJar::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    // 카메라 위치 바인딩 (자체 림 라이트)
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3)), E_FAIL);

    _float fEdgeIntensity = 0.5f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float)), E_FAIL);

    _float fShadeIntensity = 0.5f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float)), E_FAIL);

    _float fDiffusePower = 2.f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fDiffusePower", &fDiffusePower, sizeof(_float)), E_FAIL);

    _float fRimPower = 1.f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &fRimPower, sizeof(_float)), E_FAIL);

    _float fRimIntensity = 0.5f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fRimLightIntensity", &fRimIntensity, sizeof(_float)), E_FAIL);

    _float3 vRimColor = _float3(0.9f, 1.f, 0.8f);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &vRimColor, sizeof(_float3)), E_FAIL);

    _float fRimEmissive = 2.f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fRimEmissive", &fRimEmissive, sizeof(_float)), E_FAIL);

    return S_OK;
}

void CDanjinJar::AnimChange(ANIM_STATE eAnimState, _bool isLoop, _bool isCheck)
{
    if (true == isCheck)
    {
        if (eAnimState == m_eAnimState || static_cast<ANIM_STATE>(ENUM_CLASS(eAnimState) + 1) == m_eAnimState)
            return;
    }

    m_eAnimState = eAnimState;
    m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
    m_pModelCom->AnimationLoop(isLoop);
}

bool CDanjinJar::Skip_Mesh(_uint iMeshIndex)
{
    switch (m_eAnimState)
    {
    case ANIM_STATE::DANCE1_ACTIVE:
    case ANIM_STATE::DANCE1_LOOP:
        if (MESH_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::FLIPPING_ACTIVE:
    case ANIM_STATE::FLIPPING_LOOP:
        if (MESH_RIGHT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::LIE_ACTIVE:
    case ANIM_STATE::LIE_LOOP:
        if (MESH_RIGHT == iMeshIndex || MESH_LEFT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::WALK_ACTIVE:
    case ANIM_STATE::WALK_LOOP:
        if (MESH_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::LEAN_ACTIVE:
    case ANIM_STATE::LEAN_LOOP:
        if (MESH_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::SHADOWBOXING_ACTIVE:
    case ANIM_STATE::SHADOWBOXING_LOOP:
        if (MESH_RIGHT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DANCE2_ACTIVE:
    case ANIM_STATE::DANCE2_LOOP:
        if (MESH_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DANCE3_ACTIVE:
    case ANIM_STATE::DANCE3_LOOP:
        if (MESH_RIGHT == iMeshIndex || MESH_LEFT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DRSTRANGE_ACTIVE:
    case ANIM_STATE::DRSTRANGE_LOOP:
        break;
    case ANIM_STATE::DEACTIVE:
        if (MESH_BODY == iMeshIndex || MESH_CENTER == iMeshIndex || MESH_RIGHT == iMeshIndex || MESH_LEFT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DEACTIVE_IDLE:
        return true;
        break;
    }

    return false;
}

void CDanjinJar::Find_Target()
{
    CHECK_TRUE(m_isFindTarget, );

    LEVEL eLevel = CClientInstance::GetInstance()->Get_CurrLevel();
    m_pTargetCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(ENUM_CLASS(eLevel), TEXT("Layer_Creature_Player"), TEXT("Com_Transform")));
    CHECK_NULLPTR(m_pTargetCom, );

    m_isFindTarget = true;
}

void CDanjinJar::Look_Target(_float fTimeDelta)
{
    CHECK_NULLPTR(m_pTargetCom, );

    _vector vPos = {};

    vPos = m_pTargetCom->Get_State(STATE::POSITION);
    vPos.m128_f32[1] = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];

    m_pTransformCom->LookAt_Lerp(vPos, fTimeDelta, 4.f);
}

void CDanjinJar::MoveToNextStep(_float4 vTargetPos, _float4 vStartPos, _float fTimeDelta, _uint& iStep)
{
    _float4 vPos = vTargetPos;

    m_fLerpTime += fTimeDelta;

    _float fLerpTime = m_fLerpTime / m_fDuration;

    _float4 vLerpPos = Lerp(vStartPos, vPos, fLerpTime);

    _float fDistance = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vPos) - XMLoadFloat4(&vLerpPos)));

    if (0.05f > fDistance)
    {
        m_eMoveState = MOVE_STATE::IDLE;
        ++iStep;
        m_isMoveFlag = false;
        m_fLerpTime = 0.f;
    }
    else
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vLerpPos));
        vPos.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&vPos));
    }
}

void CDanjinJar::Set_Duration()
{
    _float fDistance = Calculate_StepDistance(m_vStartPos, m_vEndPos);

    m_fDuration = (fDistance / m_fDefaultLength) / m_fMoveSpeed;
}

_float CDanjinJar::Calculate_StepDistance(_float4 vPosition1, _float4 vPosition2)
{
    return XMVectorGetX(XMVector4Length(XMLoadFloat4(&vPosition1) - XMLoadFloat4(&vPosition2)));
}

_float CDanjinJar::Calculate_Distance(_vector vPosition1, _vector vPosition2)
{
    return XMVectorGetX(XMVector4Length(vPosition1 - vPosition2));
}

void CDanjinJar::Check_OnPanel_TalkUI(_uint iTalkIndex, _float fLimitDistance)
{
    CHECK_NULLPTR(m_pTargetCom, );

    if (0.f != fLimitDistance)
    {
        if (fLimitDistance > Calculate_Distance(m_pTransformCom->Get_State(STATE::POSITION), m_pTargetCom->Get_State(STATE::POSITION)))
        {
            if (false == m_pTalk->isTalking())
                m_pTalk->On_Panel(iTalkIndex);
        }
        else
        {
            if (false == m_pTalk->isTalking())
                m_pTalk->Off_Panel();
        }
    }
    else
    {
        if (false == m_pTalk->isTalking())
            m_pTalk->On_Panel(iTalkIndex);
    }
}

void CDanjinJar::Free()
{
    __super::Free();

    Safe_Release(m_pTriggerCom);

    Safe_Release(m_pTalk);
}
