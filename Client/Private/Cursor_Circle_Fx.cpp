#include "Cursor_Circle_Fx.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CCursor_Circle_Fx::CCursor_Circle_Fx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CCursor_Circle_Fx::CCursor_Circle_Fx(const CCursor_Circle_Fx& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CCursor_Circle_Fx::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCursor_Circle_Fx::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_vColor = { 0.2f, 0.81f, 1.f,1.f };
    return S_OK;
}
void CCursor_Circle_Fx::Priority_Update(_float fTimeDelta)
{

}

void CCursor_Circle_Fx::Update(_float fTimeDelta)
{
    POINT ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    // 마우스 중심 맞추기
    ptMouse.x -= m_vLocalSize.x * 0.5f;
    ptMouse.y -= m_vLocalSize.y * 0.5f;

    // dt 기반 자연스러운 따라오기
    float fSpeed = 12.f;          // 조절 가능
    float t = 1.f - expf(-fSpeed * fTimeDelta);

    m_CulPoint.x += (ptMouse.x - m_CulPoint.x) * t;
    m_CulPoint.y += (ptMouse.y - m_CulPoint.y) * t;

    if (m_CulPoint.y <= 150)
        m_CulPoint.y = 150;

    if (m_CulPoint.y >= 850)
        m_CulPoint.y = 850;

    if (m_CulPoint.x <= 180)
        m_CulPoint.x = 180;

    if (m_CulPoint.x >= 1650)
        m_CulPoint.x = 1650;
    __super::Update_Transform(nullptr, { m_CulPoint.x + m_vLocalSize.x * 0.5f, m_CulPoint.y + m_vLocalSize.y * 0.5f });

}

void CCursor_Circle_Fx::Late_Update(_float fTimeDelta)
{
    m_fAccTime += fTimeDelta;
    if (m_fAccTime > 1000.f)
        m_fAccTime -= 1000.f;

    if (m_isVisible)
        CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CCursor_Circle_Fx::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fValue", &m_fAccTime, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(7);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CCursor_Circle_Fx::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Effect"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Cursor_FX"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CCursor_Circle_Fx* CCursor_Circle_Fx::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCursor_Circle_Fx* pInstance = new CCursor_Circle_Fx(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CCursor_Circle_Fx"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCursor_Circle_Fx::Clone(void* pArg)
{
    CCursor_Circle_Fx* pInstance = new CCursor_Circle_Fx(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CCursor_Circle_Fx"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCursor_Circle_Fx::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

}
