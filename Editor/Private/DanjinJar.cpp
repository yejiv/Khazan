#include "DanjinJar.h"

#include "GameInstance.h"

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

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    DANJINJAR_DESC* pDesc = static_cast<DANJINJAR_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_eJarType = pDesc->eJarType;
    m_DanjinJarStep = pDesc->StepPosition;

    Set_DanjinJar_ModelType(m_eJarType);

    AnimChange(ANIM_STATE::DANCE1_LOOP, true);

    return S_OK;
}

void CDanjinJar::Priority_Update(_float fTimeDelta)
{
}

void CDanjinJar::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);
}

void CDanjinJar::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
}

HRESULT CDanjinJar::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CDanjinJar : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (true == Skip_Mesh(i))
            continue;

        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

void CDanjinJar::Set_DanjinJar_ModelType(DANJINJAR_TYPE eModelType)
{
    m_eJarType = eModelType;
    m_pModelCom = m_pModelType[ENUM_CLASS(m_eJarType)];
}

void CDanjinJar::StepPositionSetting(_uint iStep)
{
    _float4 vPosition = {};

    XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(STATE::POSITION));

    switch (iStep)
    {
    case 0:
        m_DanjinJarStep.vStep1 = vPosition;
        break;
    case 1:
        m_DanjinJarStep.vStep2 = vPosition;
        break;
    case 2:
        m_DanjinJarStep.vStep3 = vPosition;
        break;
    case 3:
        m_DanjinJarStep.vStep4 = vPosition;
        break;
    case 4:
        m_DanjinJarStep.vStep5 = vPosition;
        break;
    case 5:
        m_DanjinJarStep.vStep6 = vPosition;
        break;
    case 6:
        m_DanjinJarStep.vStep7 = vPosition;
        break;
    case 7:
        m_DanjinJarStep.vStep8 = vPosition;
        break;
    case 8:
        m_DanjinJarStep.vStep9 = vPosition;
        break;
    case 9:
        m_DanjinJarStep.vStep10 = vPosition;
        break;
    case 10:
        m_DanjinJarStep.vStep11 = vPosition;
        break;
    case 11:
        m_DanjinJarStep.vStep12 = vPosition;
        break;
    case 12:
        m_DanjinJarStep.vStep13 = vPosition;
        break;
    case 13:
        m_DanjinJarStep.vStep14 = vPosition;
        break;
    case 14:
        m_DanjinJarStep.vStep15 = vPosition;
        break;
    case 15:
        m_DanjinJarStep.vStep16 = vPosition;
        break;
    default:
        break;
    }
}

void CDanjinJar::MoveStepPosition(_uint iStep)
{
    switch (iStep)
    {
    case 0:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep1));
        break;
    case 1:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep2));
        break;
    case 2:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep3));
        break;
    case 3:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep4));
        break;
    case 4:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep5));
        break;
    case 5:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep6));
        break;
    case 6:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep7));
        break;
    case 7:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep8));
        break;
    case 8:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep9));
        break;
    case 9:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep10));
        break;
    case 10:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep11));
        break;
    case 11:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep12));
        break;
    case 12:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep13));
        break;
    case 13:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep14));
        break;
    case 14:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep15));
        break;
    case 15:
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_DanjinJarStep.vStep16));
        break;
    default:
        break;
    }
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

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_NPC_DanjinJar_A"),
        TEXT("Com_Model_A"), reinterpret_cast<CComponent**>(&m_pModelType[ENUM_CLASS(DANJINJAR_TYPE::A)]), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_NPC_DanjinJar_B"),
        TEXT("Com_Model_B"), reinterpret_cast<CComponent**>(&m_pModelType[ENUM_CLASS(DANJINJAR_TYPE::B)]), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_NPC_DanjinJar_C"),
        TEXT("Com_Model_C"), reinterpret_cast<CComponent**>(&m_pModelType[ENUM_CLASS(DANJINJAR_TYPE::C)]), nullptr), E_FAIL);

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

void CDanjinJar::Animation_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Pressing(DIK_L, fTimeDelta) && m_pGameInstance->Key_Down(DIK_1))
        AnimChange(ANIM_STATE::DANCE1_ACTIVE);
    if (m_pGameInstance->Key_Pressing(DIK_L, fTimeDelta) && m_pGameInstance->Key_Down(DIK_2))
        AnimChange(ANIM_STATE::FLIPPING_ACTIVE);
    if (m_pGameInstance->Key_Pressing(DIK_L, fTimeDelta) && m_pGameInstance->Key_Down(DIK_3))
        AnimChange(ANIM_STATE::LIE_ACTIVE);
    if (m_pGameInstance->Key_Pressing(DIK_L, fTimeDelta) && m_pGameInstance->Key_Down(DIK_4))
        AnimChange(ANIM_STATE::WALK_ACTIVE);
    if (m_pGameInstance->Key_Pressing(DIK_L, fTimeDelta) && m_pGameInstance->Key_Down(DIK_5))
        AnimChange(ANIM_STATE::LEAN_ACTIVE);
    if (m_pGameInstance->Key_Pressing(DIK_L, fTimeDelta) && m_pGameInstance->Key_Down(DIK_6))
        AnimChange(ANIM_STATE::SHADOWBOXING_ACTIVE);
    if (m_pGameInstance->Key_Pressing(DIK_L, fTimeDelta) && m_pGameInstance->Key_Down(DIK_7))
        AnimChange(ANIM_STATE::DANCE2_ACTIVE);
    if (m_pGameInstance->Key_Pressing(DIK_L, fTimeDelta) && m_pGameInstance->Key_Down(DIK_8))
        AnimChange(ANIM_STATE::DANCE3_ACTIVE);
    if (m_pGameInstance->Key_Pressing(DIK_L, fTimeDelta) && m_pGameInstance->Key_Down(DIK_9))
        AnimChange(ANIM_STATE::DRSTRANGE_ACTIVE);
    if (m_pGameInstance->Key_Pressing(DIK_L, fTimeDelta) && m_pGameInstance->Key_Down(DIK_0))
        AnimChange(ANIM_STATE::DEACTIVE);
}

void CDanjinJar::Animation_Change(_float fTimeDelta)
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
        break;
    }
}

void CDanjinJar::AnimChange(ANIM_STATE eAnimState, _bool isLoop)
{
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
        if (SMT_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::FLIPPING_ACTIVE:
    case ANIM_STATE::FLIPPING_LOOP:
        if (SMT_RIGHT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::LIE_ACTIVE:
    case ANIM_STATE::LIE_LOOP:
        if (SMT_RIGHT == iMeshIndex || SMT_LEFT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::WALK_ACTIVE:
    case ANIM_STATE::WALK_LOOP:
        if (SMT_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::LEAN_ACTIVE:
    case ANIM_STATE::LEAN_LOOP:
        if (SMT_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::SHADOWBOXING_ACTIVE:
    case ANIM_STATE::SHADOWBOXING_LOOP:
        if (SMT_RIGHT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DANCE2_ACTIVE:
    case ANIM_STATE::DANCE2_LOOP:
        if (SMT_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DANCE3_ACTIVE:
    case ANIM_STATE::DANCE3_LOOP:
        if (SMT_RIGHT == iMeshIndex || SMT_LEFT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DRSTRANGE_ACTIVE:
    case ANIM_STATE::DRSTRANGE_LOOP:
        break;
    case ANIM_STATE::DEACTIVE:
    case ANIM_STATE::DEACTIVE_IDLE:
        if (SMT_CENTER == iMeshIndex || SMT_RIGHT == iMeshIndex || SMT_LEFT == iMeshIndex)
            return true;
        break;
    }

    return false;
}

void CDanjinJar::Find_Target()
{
}

void CDanjinJar::Chase_Target(_float fTimeDelta)
{
}

CDanjinJar* CDanjinJar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDanjinJar* pInstance = new CDanjinJar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDanjinJar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDanjinJar::Clone(void* pArg)
{
    CDanjinJar* pInstance = new CDanjinJar(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDanjinJar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDanjinJar::Free()
{
    __super::Free();

    for (auto& pModelType : m_pModelType)
        Safe_Release(pModelType);
}
