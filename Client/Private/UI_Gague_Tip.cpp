#include "UI_Gague_Tip.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Gague_Tip::CUI_Gague_Tip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Texture{ pDevice, pContext }
{
}

CUI_Gague_Tip::CUI_Gague_Tip(const CUI_Gague_Tip& Prototype)
    : CUI_Texture( Prototype )
{
}

void CUI_Gague_Tip::Update_Pos(_float2 vPos)
{
    m_vWorldPos.x = vPos.x - (m_vLocalSize.x * 0.5f);
    m_vWorldPos.y = vPos.y;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_vWorldPos.x - m_iWinSizeX * 0.5f, -m_vWorldPos.y + m_iWinSizeY * 0.5f, 0.f, 1.f));
}

HRESULT CUI_Gague_Tip::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Gague_Tip::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CUI_Gague_Tip::Priority_Update(_float fTimeDelta)
{
}

void CUI_Gague_Tip::Update(_float fTimeDelta)
{
}

void CUI_Gague_Tip::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_Gague_Tip::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 5)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(1);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_Gague_Tip::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hud_HPGauge"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CUI_Gague_Tip* CUI_Gague_Tip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Gague_Tip* pInstance = new CUI_Gague_Tip(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Gague_Tip"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Gague_Tip::Clone(void* pArg)
{
    CUI_Gague_Tip* pInstance = new CUI_Gague_Tip(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Gague_Tip"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Gague_Tip::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
