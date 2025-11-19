#include "UI_WorldFX.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_WorldTextBox.h"
CUI_WorldFX::CUI_WorldFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_WorldFX::CUI_WorldFX(const CUI_WorldFX& Prototype)
    : CUI_Panel(Prototype)
{
}


void CUI_WorldFX::Update_UITransform(_matrix vParentMat)
{
    XMVECTOR pScale, pRot, pTrans;
    XMMatrixDecompose(&pScale, &pRot, &pTrans, vParentMat);

    // 부모에서 스케일 제거
    _matrix parentNoScale = XMMatrixAffineTransformation(
        XMVectorSet(1.f, 1.f, 1.f, 0.f), // 스케일 무시
        XMVectorZero(),
        pRot,
        pTrans
    );

    // 최종 결합 월드행렬
    _matrix finalWorld = m_pTransformCom->Get_WorldMatrix() * parentNoScale;

    XMStoreFloat4x4(&m_CombinedWorldMatrix, finalWorld);
}

void CUI_WorldFX::Set_LocalPos(_vector vPos)
{
    m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void CUI_WorldFX::Set_LocalSize(_float3 vSize)
{
    m_pTransformCom->Scale(vSize);
}

void CUI_WorldFX::Start_Anim()
{
    m_fAccTime = -1.f;
    m_isVisible = true;
}

HRESULT CUI_WorldFX::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_WorldFX::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Component(), E_FAIL);
    m_iShaderPass = 18;
    m_vColor = { 1.f,1.f, 1.f, 1.f };
    m_vUV.resize(1);
    m_vUV[0] = { 0.f, 0.5f, 1.f,1.f };
    return S_OK;
}
void CUI_WorldFX::Priority_Update(_float fTimeDelta)
{
}

void CUI_WorldFX::Update(_float fTimeDelta)
{

}

void CUI_WorldFX::Late_Update(_float fTimeDelta)
{
    if (!m_isVisible)
        return;
    m_vColor = { 1.f,0.2f, 0.2f, 1.f };
    m_fAccTime += fTimeDelta * 2.f;

    if (m_fAccTime >= 1.f)
        m_isVisible = false;
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CUI_WorldFX::Render()
{
    m_vUV[0] = { 0.f, 0.5f, 1.f,1.f };

    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix), E_FAIL);
    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 10), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vUV", &m_vUV[0], sizeof(_float4)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fValue", &m_fAccTime, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(20);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();
    return S_OK;
}

HRESULT CUI_WorldFX::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_FX_Mask"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);
    return S_OK;
}

CUI_WorldFX* CUI_WorldFX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_WorldFX* pInstance = new CUI_WorldFX(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_WorldFX"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_WorldFX::Clone(void* pArg)
{
    CUI_WorldFX* pInstance = new CUI_WorldFX(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_WorldFX"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_WorldFX::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
