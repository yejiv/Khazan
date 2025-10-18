#include "JOH_EditorModelTest.h"
#include "GameInstance.h"
#include "Editor_Model.h"

CJOH_EditorModelTest::CJOH_EditorModelTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CJOH_EditorModelTest::CJOH_EditorModelTest(const CJOH_EditorModelTest& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CJOH_EditorModelTest::Initialize_Prototype()
{
    return S_OK;

}

HRESULT CJOH_EditorModelTest::Initialize_Clone(void* pArg)
{

    EDITORTESTMODEL_DESC* pDesc = static_cast<EDITORTESTMODEL_DESC*>(pArg);
    m_isAnim = pDesc->isAnim;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc->strPrototypeTag)))
        return E_FAIL;

    if(m_isAnim)  m_pModelCom->Set_Animation(3, true);

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(1.f, 0.f, 0.f, 1.f));
    //m_pTransformCom->Scale(_float3(0.01f, 0.01f, 0.01f));

    return S_OK;
}

void CJOH_EditorModelTest::Priority_Update(_float fTimeDelta)
{
}

void CJOH_EditorModelTest::Update(_float fTimeDelta)
{
    if (!m_isEnble) return;

    if (m_isAnim && true == m_pModelCom->Play_Animation(fTimeDelta))
        int a = 10;

    if (m_isAnim && m_pGameInstance->Key_Down(DIK_1))
    {
        m_pModelCom->Set_Animation(++m_iCurrentAnimIndex, true);
    }
    if (m_isAnim && m_pGameInstance->Key_Down(DIK_2))
    {
        m_iCurrentAnimIndex = 1;
        m_pModelCom->Set_Animation(m_iCurrentAnimIndex, true);
    }

}

void CJOH_EditorModelTest::Late_Update(_float fTimeDelta)
{
    if (!m_isEnble) return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;
}

HRESULT CJOH_EditorModelTest::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (m_isAnim)
        {
            m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
            // m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
            m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

            m_pShaderCom->Begin(0);
        }
        else
        {
            m_pModelCom->Bind_Materials(m_pShaderCom_NonAnim, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
            m_pShaderCom_NonAnim->Begin(0);
        }

        m_pModelCom->Render(i);

    }

    return S_OK;
}

HRESULT CJOH_EditorModelTest::Render_Shadow()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(2);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CJOH_EditorModelTest::Ready_Components(const _wstring& strModelTag)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;


    // // 테스트로 셰이더 레벨로 바꿨습니다 오현 할아버지 수정하면 고쳐주세요..!
    // if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), strModelTag,

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader1"), reinterpret_cast<CComponent**>(&m_pShaderCom_NonAnim), nullptr)))
        return E_FAIL;
    
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), strModelTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CJOH_EditorModelTest::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

CJOH_EditorModelTest* CJOH_EditorModelTest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CJOH_EditorModelTest* pInstance = new CJOH_EditorModelTest(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CJOH_EditorModelTest"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CJOH_EditorModelTest::Clone(void* pArg)
{
    CJOH_EditorModelTest* pInstance = new CJOH_EditorModelTest(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CJOH_EditorModelTest"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CJOH_EditorModelTest::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pShaderCom_NonAnim);
}
