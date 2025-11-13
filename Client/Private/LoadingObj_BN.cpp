#include "LoadingObj_BN.h"

#include "GameInstance.h"

#include "Interaction_Guide.h"

#include "ClientInstance.h"
#include "UI_BladeNexus.h"

CLoadingObj_BN::CLoadingObj_BN(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CLoadingObj_BN::CLoadingObj_BN(const CLoadingObj_BN& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CLoadingObj_BN::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CLoadingObj_BN::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::AFTER_IDLE;
    m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
    m_pModelCom->Set_AnimationLoop(true);

    m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e) { m_Event = e; });

    return S_OK;
}

void CLoadingObj_BN::Priority_Update(_float fTimeDelta)
{
}

void CLoadingObj_BN::Update(_float fTimeDelta)
{
    Animation_Update(fTimeDelta);

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        Animation_Change(fTimeDelta);
}

void CLoadingObj_BN::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );
}

HRESULT CLoadingObj_BN::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    // 0 칼손잡이 | 1 손 잘림 보호대 | 2 뭐 존나 작은 눈 | 3 밑에 작은 날카로운 | 4 밑에 큰 날카로운 | 5 눈
    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        _bool isBNEye = { 5 == i };
        m_pShaderCom->Bind_RawValue("g_isBNEye", &isBNEye, sizeof(_bool));

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(8), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CLoadingObj_BN::Ready_Components(void* pArg)
{
    LOADINGOBJ_BLADENEXUS_DESC* pDesc = static_cast<LOADINGOBJ_BLADENEXUS_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LoadingObj"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CLoadingObj_BN::Bind_Materials(_uint iMeshIndex)
{
    _bool isDiffuse = { false };
    _bool isNormal = { false };
    _bool isEmissive = { false };
    _bool isSpecular = { false };
    _bool isMetalic = { false };
    _bool isRoughness = { false };

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        isDiffuse = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        isNormal = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        isEmissive = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        isSpecular = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", iMeshIndex, aiTextureType_METALNESS, 0)))
        isMetalic = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", iMeshIndex, aiTextureType_SHININESS, 0)))
        isRoughness = true;

    m_pShaderCom->Bind_RawValue("g_isDiffuse", &isDiffuse, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isSpecular", &isSpecular, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isMetalic", &isMetalic, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isRoughness", &isRoughness, sizeof(_bool));

    return S_OK;
}

void CLoadingObj_BN::Animation_Update(_float fTimeDelta)
{
    if (m_Event.isOn())               // 켠다는 신호
    {
        // 해금 전 IDLE 상태
        if (ANIM_STATE::BEFORE_IDLE == m_eAnimState)
        {
            // 처음 상호 작용 시
            m_eAnimState = ANIM_STATE::BEFORE_START;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
        }
        // 해금 후 IDLE 상태
        else if (ANIM_STATE::AFTER_IDLE == m_eAnimState)
        {
            // 2번 이상의 상호 작용 시
            m_eAnimState = ANIM_STATE::AFTER_START;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
        }
    }
    else if (m_Event.isOff())         // 끈다는 신호 ( 내가 받기만 하면 됨
    {
        if (ANIM_STATE::BEFORE_LOOP == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::BEFORE_END;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
        }
        if (ANIM_STATE::AFTER_LOOP == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::AFTER_END;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
            m_pModelCom->Set_AnimationLoop(false);
        }
    }
}

void CLoadingObj_BN::Animation_Change(_float fTimeDelta)
{
    // 귀검 가동 끝나면 ( 첫 해금 O )
    if (ANIM_STATE::BEFORE_START == m_eAnimState)       // BEFORE_START 가 끝나면 BEFORE_LOOP ( 플레이어가 UI랑 상호 작용 )
    {
        // 처음 상호 작용 후 애니메이션 루프로 전환 및 이벤트 발생
        m_eAnimState = ANIM_STATE::BEFORE_LOOP;
        m_pModelCom->Set_Animation(ANIM_STATE::BEFORE_LOOP);
        m_pModelCom->Set_AnimationLoop(true);
    }
    // 귀검 상호 작용 종료 후 ( 첫 해금 O )
    if (ANIM_STATE::BEFORE_END == m_eAnimState)
    {
        // 처음 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::AFTER_IDLE;
        m_pModelCom->Set_Animation(ANIM_STATE::AFTER_IDLE);
        m_pModelCom->Set_AnimationLoop(true);
    }
    // 귀검 가동 끝나면 ( 첫 해금 X )
    if (ANIM_STATE::AFTER_START == m_eAnimState)
    {
        // 다회 상호 작용 후 애니메이션 루프로 전환
        m_eAnimState = ANIM_STATE::AFTER_LOOP;
        m_pModelCom->Set_Animation(ANIM_STATE::AFTER_LOOP);
        m_pModelCom->Set_AnimationLoop(true);
    }
    // 귀검 상호 작용 종료 후 ( 첫 해금 X )
    if (ANIM_STATE::AFTER_END == m_eAnimState)
    {
        // 다회 상호 작용이 끝난 후 After Idle 상태로 전환
        m_eAnimState = ANIM_STATE::AFTER_IDLE;
        m_pModelCom->Set_Animation(ANIM_STATE::AFTER_IDLE);
        m_pModelCom->Set_AnimationLoop(true);
    }
}

CLoadingObj_BN* CLoadingObj_BN::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLoadingObj_BN* pInstance = new CLoadingObj_BN(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLoadingObj_BN"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoadingObj_BN::Clone(void* pArg)
{
    CLoadingObj_BN* pInstance = new CLoadingObj_BN(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLoadingObj_BN"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoadingObj_BN::Free()
{
    __super::Free();
}
