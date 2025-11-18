#include "UI_WorldTextBox.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_WorldTextBox::CUI_WorldTextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Text(pDevice, pContext)
{
}

CUI_WorldTextBox::CUI_WorldTextBox(const CUI_WorldTextBox& Prototype)
    : CUI_Text(Prototype)
{
}

void CUI_WorldTextBox::Update_UITransform(_matrix vParentMat)
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

HRESULT CUI_WorldTextBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_WorldTextBox::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);
    CHECK_FAILED(Ready_Component(), E_FAIL);

    m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
    return S_OK;
}

void CUI_WorldTextBox::Priority_Update(_float fTimeDelta)
{
}

void CUI_WorldTextBox::Update(_float fTimeDelta)
{
}

void CUI_WorldTextBox::Late_Update(_float fTimeDelta)
{
    m_pTransformCom->Set_State(STATE::POSITION, { -0.4f, 0.2f, -0.01f, 1.f });
    m_pTransformCom->Scale({ 1.f, 1.f, 1.f });

    if (m_isVisible)
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
            return;
}

HRESULT CUI_WorldTextBox::Render()
{  
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    m_pShaderCom->Begin(3);

    m_pGameInstance->DrawTextWorld(TEXT("Blade_Medium_20"), TEXT("다프로네"), m_vWorldPos.x + m_iPivotX, m_vWorldPos.y + m_iPivotY, {0.6f,0.6f,0.f,1.f}, TEXT_ALIGN::LEFT_CENTER,XMLoadFloat4x4(&m_CombinedWorldMatrix));

    return S_OK;
}

HRESULT CUI_WorldTextBox::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Font"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CUI_WorldTextBox* CUI_WorldTextBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_WorldTextBox* pInstance = new CUI_WorldTextBox(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_WorldTextBox"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_WorldTextBox::Clone(void* pArg)
{
    CUI_WorldTextBox* pInstance = new CUI_WorldTextBox(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_WorldTextBox"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_WorldTextBox::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
}
