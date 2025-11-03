#include "ScreenTrail.h"
#include "GameInstance.h"

CScreenTrail::CScreenTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CLineTrail{pDevice, pDeviceContext}
{
}

CScreenTrail::CScreenTrail(const CScreenTrail& Prototype)
    : CLineTrail(Prototype)
    , m_fViewportSize{ Prototype.m_fViewportSize }
{
}

HRESULT CScreenTrail::Initialize_Prototype()
{
    D3D11_VIEWPORT viewport = {};
    UINT numViewports = 1;
    m_pContext->RSGetViewports(&numViewports, &viewport);

    m_fViewportSize.x = viewport.Width;
    m_fViewportSize.y = viewport.Height;

    m_iTextureIdx = 0;
    m_fLifeTime = 0.4f;
    m_iDivisionCount = 5;

    return S_OK;
}

HRESULT CScreenTrail::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);

    return S_OK;
}

void CScreenTrail::Late_Update(_float fTimeDelta)
{
    m_pVIBufferCom->Update(m_TrailPoints);

    if (m_ControlPoints.size() > 1)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
}

HRESULT CScreenTrail::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CScreenTrail::Add_ControlPoint(POINT pos)
{
    if (m_ControlPoints.size() && XMVector4Equal(XMLoadFloat4(&m_ControlPoints.back().vPos), XMVectorSet(pos.x, pos.y, 1.f, 1.f)))
        return;

    if (m_ControlPoints.size() > 2)
        int a = 0;

    CVIBuffer_LineTrail::LINE_TRAIL_POINT newNode;

    newNode.vPos = _float4(pos.x, pos.y, 1.f, 1.f);
    newNode.fLifeTime = 0.f;

    m_ControlPoints.push_back(newNode);
}

HRESULT CScreenTrail::Ready_Component(void* pArg)
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;
    
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Slash"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    LINE_TRAIL_DESC dsc{};
    if (pArg == nullptr)
    {
        dsc.fOffset = 8.f;
        pArg = &dsc;
    }

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_VIBuffer_LineTrail"),
        TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CScreenTrail::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_ViewportSize", &m_fViewportSize, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
        return E_FAIL;

    return S_OK;
}

CScreenTrail* CScreenTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CScreenTrail* pInstance = new CScreenTrail(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CScreenTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CScreenTrail::Clone(void* pArg)
{
    CScreenTrail* pInstance = new CScreenTrail(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CScreenTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CScreenTrail::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
}
