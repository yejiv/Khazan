#include "Slate_Switch.h"

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

    m_pModelCom->Set_AnimationBlend(false);

    m_pActive = pDesc->pActive;

    return S_OK;
}

void CSlate_Switch::Priority_Update(_float fTimeDelta)
{
}

void CSlate_Switch::Update(_float fTimeDelta)
{
    if (true == *m_pActive)
    {
        if (ANIM_STATE::DIE != m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::DIE;
            m_pModelCom->Set_Animation(ENUM_CLASS(ANIM_STATE::DIE));
        }
    }
    else
    {
        if (ANIM_STATE::SPAWN != m_eAnimState)
        {
            m_eAnimState = ANIM_STATE::SPAWN;
            m_pModelCom->Set_Animation(ENUM_CLASS(ANIM_STATE::SPAWN));
        }
    }

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {

    }

    Update_CombinedMatrix();
}

void CSlate_Switch::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this);
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
}
