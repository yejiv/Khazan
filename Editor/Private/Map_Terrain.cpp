#include "Map_Terrain.h"
#include "GameInstance.h"

CMap_Terrain::CMap_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CMap_Terrain::CMap_Terrain(const CMap_Terrain& Prototype)
    : CGameObject (Prototype)
{
}

HRESULT CMap_Terrain::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMap_Terrain::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-500.f, 0.f, -500.f, 1.f));

    //m_pTransformCom->Scale(_float3(100.f, 0.f, 100.f));

    return S_OK;
}

void CMap_Terrain::Priority_Update(_float fTimeDelta)
{

}

void CMap_Terrain::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_F5))
    {
        _float4 vCamPos = *m_pGameInstance->Get_CamPosition();
        vCamPos.x -= 500.f;
        vCamPos.y = 0.f;
        vCamPos.z -= 500.f;
        vCamPos.w = 1.f;
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vCamPos));
    }
    if (m_pGameInstance->Key_Down(DIK_F6)) m_isRender = !m_isRender;

    if (m_pGameInstance->Key_Down(DIK_F7)) m_isWireFrame = !m_isWireFrame;

    if (true == m_isWireFrame)
        m_iShaderPass = 1;
    else
        m_iShaderPass = 0;
}

void CMap_Terrain::Late_Update(_float fTimeDelta)
{
    if (true == m_isRender)
        CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this), );
}

HRESULT CMap_Terrain::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(m_iShaderPass);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CMap_Terrain::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Texture_Terrain_Grid"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMap_Terrain::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resources(m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;

    return S_OK;
}

CMap_Terrain* CMap_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_Terrain* pInstance = new CMap_Terrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CMap_Terrain"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_Terrain::Clone(void* pArg)
{
    CMap_Terrain* pInstance = new CMap_Terrain(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CMap_Terrain"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_Terrain::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
