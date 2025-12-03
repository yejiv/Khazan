#include "GiantGate_Part_R.h"

#include "GameInstance.h"

CGiantGate_Part_R::CGiantGate_Part_R(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CGiantGate_Part_R::CGiantGate_Part_R(const CGiantGate_Part_R& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CGiantGate_Part_R::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGiantGate_Part_R::Initialize_Clone(void* pArg)
{
    GIANTGATE_RIGHT_DESC* pDesc = static_cast<GIANTGATE_RIGHT_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_NULLPTR(pDesc->pSocketMatrix, E_FAIL);

    m_pSocketMatrix = pDesc->pSocketMatrix;

    m_pTransformCom->Scale(_float3(0.01f, 0.01f, 0.01f));

    _float4 vPos = {};

    XMStoreFloat4(&vPos, m_pTransformCom->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::UP) * 84.5f);

    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vPos));

    m_pTransformCom->Rotation(XMConvertToRadians(270.f), XMConvertToRadians(180.f), XMConvertToRadians(180.f));

    return S_OK;
}

void CGiantGate_Part_R::Priority_Update(_float fTimeDelta)
{
}

void CGiantGate_Part_R::Update(_float fTimeDelta)
{
    _matrix     BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; i++)
    {
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
    }

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CGiantGate_Part_R::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);
}

HRESULT CGiantGate_Part_R::Render()
{
    //if (m_pGameInstance->Key_Pressing(DIK_NUMPAD1, 0.f))
    //    return S_OK;

    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CGiantGate_Part_R : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(4), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CGiantGate_Part_R::Ready_Components(void* pArg)
{
    GIANTGATE_RIGHT_DESC* pDesc = static_cast<GIANTGATE_RIGHT_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_GiantGate_Part"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CGiantGate_Part_R::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

HRESULT CGiantGate_Part_R::Bind_Materials(_uint iMeshIndex)
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

CGiantGate_Part_R* CGiantGate_Part_R::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGiantGate_Part_R* pInstance = new CGiantGate_Part_R(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CGiantGate_Part_R"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGiantGate_Part_R::Clone(void* pArg)
{
    CGiantGate_Part_R* pInstance = new CGiantGate_Part_R(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CGiantGate_Part_R"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGiantGate_Part_R::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
