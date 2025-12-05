#include "Jar_8th.h"

#include "GameInstance.h"

#include "UI_Talk_Danjinjar.h"

CJar_8th::CJar_8th(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDanjinJar { pDevice, pContext }
{
}

CJar_8th::CJar_8th(const CJar_8th& Prototype)
    : CDanjinJar { Prototype }
{
}

HRESULT CJar_8th::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CJar_8th::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_TalkUI(pArg), E_FAIL);

    m_iStepState = STEP_STATE::STEP1;
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep1));

    m_eMoveState = MOVE_STATE::IDLE;

    m_eAnimState = ANIM_STATE::DANCE1_ACTIVE;

    AnimChange(m_eAnimState);

    m_fMoveSpeed = 0.35f;

    return S_OK;
}

void CJar_8th::Priority_Update(_float fTimeDelta)
{
    Find_Target();

    __super::Priority_Update(fTimeDelta);
}

void CJar_8th::Update(_float fTimeDelta)
{
    Update_Step(fTimeDelta);

    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);

    __super::Update(fTimeDelta);

    m_pTriggerCom->Sync_Update(m_pTransformCom);
    m_pTriggerCom->Update(fTimeDelta, m_pTransformCom);
}

void CJar_8th::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);

    __super::Late_Update(fTimeDelta);
}

HRESULT CJar_8th::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CJar_8th : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (true == Skip_Mesh(i))
            continue;

        _float fShadeIntensity = 3.f;
        CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float)), E_FAIL);

        _float fEdgeIntensity = 1.f;

        switch (i)
        {
        case MESH_BODY:
            fEdgeIntensity = 1.2f;
            break;
        case MESH_HEAD:
            break;
        case MESH_CENTER:
        case MESH_LEFT:
        case MESH_RIGHT:
            fEdgeIntensity = 2.6f;
            break;
        }

        CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float)), E_FAIL);

        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(20), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CJar_8th::Ready_Components(void* pArg)
{
    DANJINJAR_DESC* pDesc = static_cast<DANJINJAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

     CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_NPC_DanjinJar_B"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CJar_8th::Ready_TalkUI(void* pArg)
{
    CUIObject::UIOBJECT_DESC Desc;

    Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
    Desc.vLocalPos = { 0.f, 0.f };
    Desc.vLocalSize = { 3.625f, 1.f };
    Desc.szName = "DanjinJar_8th_TalkUI";
    m_pTalk = static_cast<CUI_Talk_Danjinjar*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_TalkDanjinjar"), &Desc));
    CHECK_NULLPTR(m_pTalk, E_FAIL);

    return S_OK;
}

HRESULT CJar_8th::Bind_Materials(_uint iMeshIndex)
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

void CJar_8th::Animation_Update(_float fTimeDelta)
{
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
}

void CJar_8th::Animation_Change(_float fTimeDelta)
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

void CJar_8th::Update_Step(_float fTimeDelta)
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

void CJar_8th::Check_Step()
{
    if (MOVE_STATE::MOVE == m_eMoveState)
        return;

    _bool isSkip = false;

    switch (m_iStepState)
    {
    case STEP1:
        Check_OnPanel_TalkUI(101, 5.5f);
        break;
    case STEP2:
        Check_OnPanel_TalkUI(102);
        break;
    case STEP3:
        isSkip = true;
        break;
    case STEP4:
        Check_OnPanel_TalkUI(103);
        break;
    case STEP5:
        isSkip = true;
        break;
    case STEP6:
        isSkip = true;
        break;
    case STEP7:
        isSkip = true;
        break;
    case STEP8:
        Check_OnPanel_TalkUI(104);
        break;
    case STEP9:
    case STEP10:
    case STEP11:
    case STEP12:
    case STEP13:
    case STEP14:
        isSkip = true;
        break;
    case STEP15:
        Check_OnPanel_TalkUI(113);
        break;
    case STEP16:
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
            Set_Duration();
            AnimChange(ANIM_STATE::WALK_LOOP, true);
            m_eMoveState = MOVE_STATE::MOVE;
            m_isMoveFlag = true;
        }
    }
}

_float4 CJar_8th::Get_NextStepPos()
{
    _float4 vTargetPos = { _float4(0.f, 0.f, 0.f, 0.f) };

    switch (m_iStepState)
    {
    case STEP1:
        vTargetPos = m_DanjinJarStep.vStep2;
        break;
    case STEP2:
        vTargetPos = m_DanjinJarStep.vStep3;
        break;
    case STEP3:
        vTargetPos = m_DanjinJarStep.vStep4;
        break;
    case STEP4:
        vTargetPos = m_DanjinJarStep.vStep5;
        break;
    case STEP5:
        vTargetPos = m_DanjinJarStep.vStep6;
        break;
    case STEP6:
        vTargetPos = m_DanjinJarStep.vStep7;
        break;
    case STEP7:
        vTargetPos = m_DanjinJarStep.vStep8;
        break;
    case STEP8:
        vTargetPos = m_DanjinJarStep.vStep9;
        break;
    case STEP9:
        vTargetPos = m_DanjinJarStep.vStep10;
        break;
    case STEP10:
        vTargetPos = m_DanjinJarStep.vStep11;
        break;
    case STEP11:
        vTargetPos = m_DanjinJarStep.vStep12;
        break;
    case STEP12:
        vTargetPos = m_DanjinJarStep.vStep13;
        break;
    case STEP13:
        vTargetPos = m_DanjinJarStep.vStep14;
        break;
    case STEP14:
        vTargetPos = m_DanjinJarStep.vStep15;
        break;
    case STEP15:
        vTargetPos = m_DanjinJarStep.vStep16;
        break;
    case STEP16:
        break;
    }

    return vTargetPos;
}

void CJar_8th::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (false == m_isMoveFlag && /*m_pTalk->isEmptyNextEvent() && */m_pTalk->isTalkingEnd())
    {
        XMStoreFloat4(&m_vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
        m_vEndPos = Get_NextStepPos();
        if (0.f != m_vEndPos.w)
        {
            if (false == m_pTalk->isExistNextTalk())
                m_pTalk->Off_Panel();
            Set_Duration();
            AnimChange(ANIM_STATE::WALK_LOOP, true);
            m_eMoveState = MOVE_STATE::MOVE;
            m_isMoveFlag = true;
        }
    }
}

void CJar_8th::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;

    if (false == m_isMoveFlag && /*m_pTalk->isEmptyNextEvent() && */m_pTalk->isTalkingEnd())
    {
        XMStoreFloat4(&m_vStartPos, m_pTransformCom->Get_State(STATE::POSITION));
        m_vEndPos = Get_NextStepPos();
        if (0.f != m_vEndPos.w)
        {
            if (false == m_pTalk->isExistNextTalk())
                m_pTalk->Off_Panel();
            Set_Duration();
            AnimChange(ANIM_STATE::WALK_LOOP, true);
            m_eMoveState = MOVE_STATE::MOVE;
            m_isMoveFlag = true;
        }
    }
}

void CJar_8th::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::CAMERA) || iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        return;


}

CJar_8th* CJar_8th::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CJar_8th* pInstance = new CJar_8th(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CJar_8th"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CJar_8th::Clone(void* pArg)
{
    CJar_8th* pInstance = new CJar_8th(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CJar_8th"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CJar_8th::Free()
{
    __super::Free();
}
