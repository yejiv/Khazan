#include "Edit_UIBackGround.h"
#include "GameInstance.h"

CEdit_UIBackGround::CEdit_UIBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CUIObject{ pDevice, pDeviceContext }
{
}

CEdit_UIBackGround::CEdit_UIBackGround(const CUIObject& Prototype)
    : CUIObject(Prototype)
{
}

HRESULT CEdit_UIBackGround::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEdit_UIBackGround::Initialize_Clone(void* pArg)
{
    m_vBackColor = { 0.f, 0.2f, 0.7f };
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CEdit_UIBackGround::Priority_Update(_float fTimeDelta)
{
}

void CEdit_UIBackGround::Update(_float fTimeDelta)
{
}

void CEdit_UIBackGround::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CEdit_UIBackGround::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4 vBackColor = { m_vBackColor.x, m_vBackColor.y, m_vBackColor.z, 1.f};
 
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vBackColor, sizeof(_float4))))
        return E_FAIL;

    m_pShaderCom->Begin(0);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

_float3& CEdit_UIBackGround::Get_BackColor()
{
    return m_vBackColor;
}

HRESULT CEdit_UIBackGround::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CEdit_UIBackGround* CEdit_UIBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CEdit_UIBackGround* pInstance = new CEdit_UIBackGround(pDevice, pDeviceContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEdit_UIBackGround"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEdit_UIBackGround::Clone(void* pArg)
{
    CEdit_UIBackGround* pInstance = new CEdit_UIBackGround(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CGameObject"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEdit_UIBackGround::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}





