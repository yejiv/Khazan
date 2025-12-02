#include "Jar_1st.h"

#include "GameInstance.h"

CJar_1st::CJar_1st(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDanjinJar { pDevice, pContext }
{
}

CJar_1st::CJar_1st(const CJar_1st& Prototype)
    : CDanjinJar { Prototype }
{
}

HRESULT CJar_1st::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CJar_1st::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_iStepState = STEP_STATE::STEP1;
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep1));

    m_eMoveState = MOVE_STATE::IDLE;

    m_eAnimState = ANIM_STATE::DANCE1_ACTIVE;

    AnimChange(m_eAnimState);

    return S_OK;
}

void CJar_1st::Priority_Update(_float fTimeDelta)
{
    Find_Target();
}

void CJar_1st::Update(_float fTimeDelta)
{
    Update_Step(fTimeDelta);

    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);

    m_pTriggerCom->Sync_Update(m_pTransformCom);
    m_pTriggerCom->Update(fTimeDelta, m_pTransformCom);
}

void CJar_1st::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
}

HRESULT CJar_1st::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CJar_1st : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (2 <= i)
        {
            if (true == Skip_Mesh(i))
                continue;
            else
                _int a = 10;
        }

        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CJar_1st::Ready_Components(void* pArg)
{
    DANJINJAR_DESC* pDesc = static_cast<DANJINJAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

     CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_NPC_DanjinJar_A"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CJar_1st::Bind_Materials(_uint iMeshIndex)
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

    //m_iMtrlFlags &= ~M_EMISSIVE;
    //m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

void CJar_1st::Animation_Update(_float fTimeDelta)
{
#pragma region KEEP

    if (MOVE_STATE::MOVE == m_eMoveState)
        return;

    switch (m_iStepState)
    {
    case STEP1:
        AnimChange(ANIM_STATE::DANCE1_ACTIVE, false, true);
        break;
    case STEP2:
        AnimChange(ANIM_STATE::DANCE2_ACTIVE, false, true);
        break;
    case STEP3:
        AnimChange(ANIM_STATE::DANCE3_ACTIVE, false, true);
        break;
    case STEP4:
        AnimChange(ANIM_STATE::DRSTRANGE_ACTIVE, false, true);
        break;
    case STEP5:
        AnimChange(ANIM_STATE::FLIPPING_ACTIVE, false, true);
        break;
    case STEP6:
        AnimChange(ANIM_STATE::LIE_ACTIVE, false, true);
        break;
    case STEP7:
        AnimChange(ANIM_STATE::SHADOWBOXING_ACTIVE, false, true);
        break;
    case STEP8:
        AnimChange(ANIM_STATE::DRSTRANGE_ACTIVE, false, true);
        break;
    case STEP9:
        AnimChange(ANIM_STATE::DANCE1_ACTIVE, false, true);
        break;
    case STEP10:
        AnimChange(ANIM_STATE::DANCE2_ACTIVE, false, true);
        break;
    case STEP11:
        AnimChange(ANIM_STATE::DANCE3_ACTIVE, false, true);
        break;
    case STEP12:
        AnimChange(ANIM_STATE::DRSTRANGE_ACTIVE, false, true);
        break;
    case STEP13:
        AnimChange(ANIM_STATE::FLIPPING_ACTIVE, false, true);
        break;
    case STEP14:
        AnimChange(ANIM_STATE::LIE_ACTIVE, false, true);
        break;
    case STEP15:
        AnimChange(ANIM_STATE::SHADOWBOXING_ACTIVE, false, true);
        break;
    case STEP16:
        AnimChange(ANIM_STATE::DEACTIVE, false, true);
        break;
    }

#pragma endregion

}

void CJar_1st::Animation_Change(_float fTimeDelta)
{
    switch (m_eAnimState)
    {
    case ANIM_STATE::DANCE1_ACTIVE:
        AnimChange(ANIM_STATE::DANCE1_LOOP, true);
        break;
    case ANIM_STATE::FLIPPING_ACTIVE:
        AnimChange(ANIM_STATE::FLIPPING_LOOP, true);
        break;
    case ANIM_STATE::LIE_ACTIVE:
        AnimChange(ANIM_STATE::LIE_LOOP, true);
        break;
    case ANIM_STATE::WALK_ACTIVE:
        AnimChange(ANIM_STATE::WALK_LOOP, true);
        break;
    case ANIM_STATE::LEAN_ACTIVE:
        AnimChange(ANIM_STATE::LEAN_LOOP, true);
        break;
    case ANIM_STATE::SHADOWBOXING_ACTIVE:
        AnimChange(ANIM_STATE::SHADOWBOXING_LOOP, true);
        break;
    case ANIM_STATE::DANCE2_ACTIVE:
        AnimChange(ANIM_STATE::DANCE2_LOOP, true);
        break;
    case ANIM_STATE::DANCE3_ACTIVE:
        AnimChange(ANIM_STATE::DANCE3_LOOP, true);
        break;
    case ANIM_STATE::DRSTRANGE_ACTIVE:
        AnimChange(ANIM_STATE::DRSTRANGE_LOOP, true);
        break;
    case ANIM_STATE::DEACTIVE:
        AnimChange(ANIM_STATE::DEACTIVE_IDLE, true);
        m_pModelCom->Set_AnimationBlend(false);
        break;
    }
}

void CJar_1st::Update_Step(_float fTimeDelta)
{
    if (false == m_isMoveFlag)
    {
        Look_Target(fTimeDelta);
    }
    else if (true == m_isMoveFlag)
    {
        MoveToNextStep(m_vEndPos, m_vStartPos, fTimeDelta, m_iStepState);
    }

    Check_Step();
}

void CJar_1st::Check_Step()
{
    _bool isSkip = false;

    switch (m_iStepState)
    {
    case STEP3:
        isSkip = true;
        break;
    }

    if (false == isSkip)
        return;

    if (false == m_isMoveFlag)
    {
        XMStoreFloat4(&m_vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
        m_vEndPos = Get_NextStepPos();
        if (0.f != m_vEndPos.w)
        {
            AnimChange(ANIM_STATE::WALK_LOOP, true);
            m_eMoveState = MOVE_STATE::MOVE;
            m_isMoveFlag = true;
        }
    }
}

_float4 CJar_1st::Get_NextStepPos()
{
    _float4 vTargetPos = { _float4(0.f, 0.f, 0.f, 0.f) };

    switch (m_iStepState)
    {
    case STEP1:
        vTargetPos = m_DanjinJarStep.vStep2;
        m_fDuration = 1.f;
        break;
    case STEP2:
        vTargetPos = m_DanjinJarStep.vStep3;
        m_fDuration = 1.f;
        break;
    case STEP3:
        vTargetPos = m_DanjinJarStep.vStep4;
        m_fDuration = 1.f;
        break;
    case STEP4:
        vTargetPos = m_DanjinJarStep.vStep5;
        m_fDuration = 1.f;
        break;
    case STEP5:
        vTargetPos = m_DanjinJarStep.vStep6;
        m_fDuration = 1.f;
        break;
    case STEP6:
        vTargetPos = m_DanjinJarStep.vStep7;
        m_fDuration = 1.f;
        break;
    case STEP7:
        vTargetPos = m_DanjinJarStep.vStep8;
        m_fDuration = 1.f;
        break;
    case STEP8:
        vTargetPos = m_DanjinJarStep.vStep9;
        m_fDuration = 1.f;
        break;
    case STEP9:
        vTargetPos = m_DanjinJarStep.vStep10;
        m_fDuration = 1.f;
        break;
    case STEP10:
        vTargetPos = m_DanjinJarStep.vStep11;
        m_fDuration = 1.f;
        break;
    case STEP11:
        vTargetPos = m_DanjinJarStep.vStep12;
        m_fDuration = 1.f;
        break;
    case STEP12:
        vTargetPos = m_DanjinJarStep.vStep13;
        m_fDuration = 1.f;
        break;
    case STEP13:
        vTargetPos = m_DanjinJarStep.vStep14;
        m_fDuration = 1.f;
        break;
    case STEP14:
        vTargetPos = m_DanjinJarStep.vStep15;
        m_fDuration = 1.f;
        break;
    case STEP15:
        vTargetPos = m_DanjinJarStep.vStep16;
        m_fDuration = 1.f;
        break;
    case STEP16:
        break;
    }

    return vTargetPos;
}

void CJar_1st::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (false == m_isMoveFlag)
    {
        XMStoreFloat4(&m_vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
        m_vEndPos = Get_NextStepPos();
        if (0.f != m_vEndPos.w)
        {
            AnimChange(ANIM_STATE::WALK_LOOP, true);
            m_eMoveState = MOVE_STATE::MOVE;
            m_isMoveFlag = true;
        }
    }
}

void CJar_1st::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;


}

void CJar_1st::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;


}

CJar_1st* CJar_1st::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CJar_1st* pInstance = new CJar_1st(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CJar_1st"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CJar_1st::Clone(void* pArg)
{
    CJar_1st* pInstance = new CJar_1st(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CJar_1st"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CJar_1st::Free()
{
    __super::Free();
}
