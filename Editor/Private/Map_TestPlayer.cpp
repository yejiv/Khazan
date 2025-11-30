#include "Map_TestPlayer.h"
#include "GameInstance.h"

CMap_TestPlayer::CMap_TestPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CMap_TestPlayer::CMap_TestPlayer(const CMap_TestPlayer& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CMap_TestPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMap_TestPlayer::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

    return S_OK;
}

void CMap_TestPlayer::Priority_Update(_float fTimeDelta)
{
}

void CMap_TestPlayer::Update(_float fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
        _int a = 10;
    }
}

void CMap_TestPlayer::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
}

HRESULT CMap_TestPlayer::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_iMtrlFlags = 0;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            m_iMtrlFlags |= M_DIFFUSE;

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        m_pShaderCom->Begin(9);

        m_pModelCom->Render(i);
    }

    return S_OK;
}


HRESULT CMap_TestPlayer::Ready_Components()
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_PlayerTest"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMap_TestPlayer::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

CMap_TestPlayer* CMap_TestPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_TestPlayer* pInstance = new CMap_TestPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMap_TestPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_TestPlayer::Clone(void* pArg)
{
    CMap_TestPlayer* pInstance = new CMap_TestPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CMap_TestPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_TestPlayer::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

}
