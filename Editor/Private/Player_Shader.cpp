#include "Player_Shader.h"
#include "GameInstance.h"

CPlayer_Shader::CPlayer_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CPlayer_Shader::CPlayer_Shader(const CPlayer_Shader& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CPlayer_Shader::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer_Shader::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_Animation(8);

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
    //  m_pTransformCom->Scaling(_float3(5.f, 5.f, 5.f));

    return S_OK;
}

void CPlayer_Shader::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CPlayer_Shader::Update(_float fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        int a = 10;

    if (m_pGameInstance->Key_Down(DIK_MINUS))
    {
        m_pModelCom->Set_AnimationLoop(true);
        m_pModelCom->Set_Animation(--m_iCurrentAnimIndex);
    }
    if (m_pGameInstance->Key_Down(DIK_EQUALS))
    {
        //  m_iCurrentAnimIndex = 2;
        m_pModelCom->Set_AnimationLoop(true);
        m_pModelCom->Set_Animation(++m_iCurrentAnimIndex);
    }
}

void CPlayer_Shader::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;

#ifdef _DEBUG

#endif
}

HRESULT CPlayer_Shader::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    // Color Debug
    if (FAILED(m_pShaderCom->Bind_FloatArray("g_Splits", m_pGameInstance->Get_Splits(), m_pGameInstance->Get_NumCascades())))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        //  if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
        //      return E_FAIL;
        //  if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_DIFFUSE, 0)))
        //      return E_FAIL;
        //  if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
        //      return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CPlayer_Shader::Render_Shadow()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_CurrentLightViewMatrix())))
        return E_FAIL;
    
    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_CurrentLightProjMatrix())))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(3);

        m_pModelCom->Render(i);
    }

    return S_OK;
}


HRESULT CPlayer_Shader::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Editor_Model_Test"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer_Shader::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CPlayer_Shader* CPlayer_Shader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer_Shader* pInstance = new CPlayer_Shader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CPlayer_Shader"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer_Shader::Clone(void* pArg)
{
    CPlayer_Shader* pInstance = new CPlayer_Shader(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CPlayer_Shader"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Shader::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
