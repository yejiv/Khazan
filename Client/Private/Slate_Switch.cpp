#include "Slate_Switch.h"
#include "Effect_Prefab.h"

#include "GameInstance.h"

CSlate_Switch::CSlate_Switch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject { pDevice, pContext }
{
}

CSlate_Switch::CSlate_Switch(const CSlate_Switch& Prototype)
    : CPartObject { Prototype }
{
}

HRESULT CSlate_Switch::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSlate_Switch::Initialize_Clone(void* pArg)
{
    SLATE_SWITCH_DESC* pDesc = static_cast<SLATE_SWITCH_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);
    CHECK_FAILED(Ready_Effect(), E_FAIL);

    m_pActiveElevator = pDesc->pActiveElevator;
    m_pAvailableSwitch = pDesc->pAvailableSwitch;
    m_pSwitchPressed = pDesc->pSwitchPressed;
    m_eElevatorType = pDesc->eType;

    if (ELEVATOR_TYPE::LARGE == m_eElevatorType)
    {
        m_pSocketMatrix = pDesc->pSocketMatrix;

        m_pTransformCom->Rotation(XMConvertToRadians(270.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f));
    }

    m_eAnimState = ANIM_STATE::IDLE;
    m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
    m_pModelCom->Set_AnimationBlend(false);
    m_pModelCom->Play_Animation(0.f);
    m_pModelCom->Set_AnimationBlend(true);

    return S_OK;
}

void CSlate_Switch::Priority_Update(_float fTimeDelta)
{
    m_pEffect->Priority_Update(fTimeDelta);
}

void CSlate_Switch::Update(_float fTimeDelta)
{
    // IDLE 상태
    // 신호오면 DIE 재생 후 멈춤
    // 도착하면 신호 받고 SPAWN 재생 후 IDLE 변경

    if (true == *m_pSwitchPressed && ANIM_STATE::IDLE == m_eAnimState)
    {
        if (ANIM_STATE::IDLE == m_eAnimState)
        {
            m_pGameInstance->PlaySoundOnce(TEXT("IP_Slate_On.wav"), 0.5f);

            m_eAnimState = ANIM_STATE::DIE;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState)); 

            // 예지 엘리베이터 발판 눌려서 작동 될 때 이펙트
            m_pEffect->SetClose();
        }
    }
    else if (false == *m_pActiveElevator && true == *m_pAvailableSwitch)
    {
        if (ANIM_STATE::DIE == m_eAnimState)
        {
            m_pGameInstance->PlaySoundOnce(TEXT("IP_Slate_Off.wav"), 0.5f);

            m_eAnimState = ANIM_STATE::SPAWN;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));

            // 예지 엘리베이터 발판 눌려서 작동 될 때 이펙트 
            m_pEffect->ResetChildren();

            *m_pAvailableSwitch = false;
        }
    }

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        if (ANIM_STATE::DIE == m_eAnimState)
        {
            *m_pActiveElevator = true;
            *m_pAvailableSwitch = true;
            *m_pSwitchPressed = false;
        }
        if (ANIM_STATE::SPAWN == m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::IDLE;
            m_pModelCom->Set_Animation(ENUM_CLASS(m_eAnimState));
        }
    }

    if (ELEVATOR_TYPE::LARGE == m_eElevatorType)
    {
        _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        for (_uint i = 0; i < 3; ++i)
            BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

        XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));
    }
    else if (ELEVATOR_TYPE::SMALL == m_eElevatorType)
    {
        Update_CombinedMatrix();
    }

    _matrix world = XMLoadFloat4x4(&m_CombinedWorldMatrix);
    m_pEffect->UpdatePosition(world.r[3]);
    m_pEffect->Update(fTimeDelta);

    m_fBlinkTimeAcc += fTimeDelta;
}

void CSlate_Switch::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
    m_pEffect->Late_Update(fTimeDelta);
}

HRESULT CSlate_Switch::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CSlate_Switch : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    // 0 칼손잡이 | 1 손 잘림 보호대 | 2 뭐 존나 작은 눈 | 3 밑에 작은 날카로운 | 4 밑에 큰 날카로운 | 5 눈
    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (ANIM_STATE::IDLE == m_eAnimState)
        {
            if (FAILED(Bind_Blink_ShaderResources()))
                return E_FAIL;

            CHECK_FAILED_ASSERT(m_pShaderCom->Begin(30), E_FAIL);
        }
        else
            CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CSlate_Switch::Ready_Components(void* pArg)
{
    SLATE_SWITCH_DESC* pDesc = static_cast<SLATE_SWITCH_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_Slate_Switch"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CSlate_Switch::Ready_Effect()
{ 
    m_pEffect = dynamic_cast<CEffect_Prefab*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::EMBARS), TEXT("Elevator_Button")));

    if (nullptr == m_pEffect)
        return E_FAIL;

    m_pEffect->ResetChildren();

    return S_OK;
}

HRESULT CSlate_Switch::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

HRESULT CSlate_Switch::Bind_Materials(_uint iMeshIndex)
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

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

HRESULT CSlate_Switch::Bind_Blink_ShaderResources()
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

CSlate_Switch* CSlate_Switch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSlate_Switch* pInstance = new CSlate_Switch(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CSlate_Switch"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSlate_Switch::Clone(void* pArg)
{
    CSlate_Switch* pInstance = new CSlate_Switch(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CSlate_Switch"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSlate_Switch::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pEffect);
}
