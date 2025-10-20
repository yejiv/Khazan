#include "Prop_Preview.h"

#include "GameInstance.h"

#include "Editor_Model.h"

CProp_Preview::CProp_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp { pDevice, pContext }
{
}

CProp_Preview::CProp_Preview(const CProp_Preview& Prototype)
    : CProp { Prototype }
{
}

HRESULT CProp_Preview::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CProp_Preview::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_eShaderPass = SHADER_PASS::MAP;

    m_pTransformCom->Scale(_float3(0.005f, 0.005f, 0.005f));

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 1000.f, 0.f, 1.f));

    m_ResetMatrix.r[0] = m_pTransformCom->Get_State(STATE::RIGHT);
    m_ResetMatrix.r[1] = m_pTransformCom->Get_State(STATE::UP);
    m_ResetMatrix.r[2] = m_pTransformCom->Get_State(STATE::LOOK);
    m_ResetMatrix.r[3] = m_pTransformCom->Get_State(STATE::POSITION);

    return S_OK;
}

void CProp_Preview::Priority_Update(_float fTimeDelta)
{
}

void CProp_Preview::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Pressing(DIK_NUMPAD4, fTimeDelta))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
    if (m_pGameInstance->Key_Pressing(DIK_NUMPAD6, fTimeDelta))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -fTimeDelta);

    if (m_pGameInstance->Key_Pressing(DIK_NUMPAD8, fTimeDelta))
        m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta);
    if (m_pGameInstance->Key_Pressing(DIK_NUMPAD5, fTimeDelta))
        m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), -fTimeDelta);
}

void CProp_Preview::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this), );
}

HRESULT CProp_Preview::Render()
{
    if (nullptr == m_pModelCom)
        return S_OK;

    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Preview : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(ENUM_CLASS(m_eShaderPass)), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

void CProp_Preview::Reset_Preview()
{
    m_pTransformCom->Set_State(STATE::RIGHT, m_ResetMatrix.r[0]);
    m_pTransformCom->Set_State(STATE::UP, m_ResetMatrix.r[1]);
    m_pTransformCom->Set_State(STATE::LOOK, m_ResetMatrix.r[2]);
    m_pTransformCom->Set_State(STATE::POSITION, m_ResetMatrix.r[3]);
}

HRESULT CProp_Preview::Ready_Components(void* pArg)
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CProp_Preview::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    // 맵 오브젝트 ID 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_iMapObjectID", &m_iMapObjectID, sizeof(_uint)), E_FAIL);

    return S_OK;
}

HRESULT CProp_Preview::Bind_Materials(_uint iMeshIndex)
{
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0);
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0);
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0);
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0);

    return S_OK;
}

CProp_Preview* CProp_Preview::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProp_Preview* pInstance = new CProp_Preview(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Preview"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProp_Preview::Clone(void* pArg)
{
    CProp_Preview* pInstance = new CProp_Preview(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Preview"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProp_Preview::Free()
{
    __super::Free();
}
