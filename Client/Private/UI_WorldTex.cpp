#include "UI_WorldTex.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_WorldTextBox.h"
CUI_WorldTex::CUI_WorldTex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_WorldTex::CUI_WorldTex(const CUI_WorldTex& Prototype)
    : CUI_Panel(Prototype)
{
}


void CUI_WorldTex::Update_UITransform(_matrix vParentMat)
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

void CUI_WorldTex::Set_LocalPos(_vector vPos)
{
    m_pTransformCom->Set_State(STATE::POSITION, vPos);
}

void CUI_WorldTex::Set_LocalSize(_float3 vSize)
{
    m_pTransformCom->Scale(vSize);
}

HRESULT CUI_WorldTex::Setting_Texture(_int iTexPass, _float4 vUV)
{
    m_IsAtlas = true;
    m_vUV[0] = vUV;
    m_iTexPass = iTexPass;

    return S_OK;
}

HRESULT CUI_WorldTex::Setting_Texture(_int iTexPass, const _wstring& strComponentTag)
{
    m_IsAtlas = false;
    m_vUV[0] = { 0.f, 0.f, 1.f, 1.f };
    m_iTexPass = iTexPass;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), strComponentTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CUI_WorldTex::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_WorldTex::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Component(), E_FAIL);
    m_iShaderPass = 18;
    m_vColor = { 1.f,1.f, 1.f, 1.f };
    m_vUV.resize(1);
    return S_OK;
}
void CUI_WorldTex::Priority_Update(_float fTimeDelta)
{
}

void CUI_WorldTex::Update(_float fTimeDelta)
{

}

void CUI_WorldTex::Late_Update(_float fTimeDelta)
{

    if (m_isVisible)
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
            return;
}

HRESULT CUI_WorldTex::Render()
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)),E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix),E_FAIL);

    if(m_IsAtlas)
        CHECK_FAILED(m_pAtlasTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);
    else
        CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vUV", &m_vUV[0], sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(m_iShaderPass);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();
    return S_OK;
}

HRESULT CUI_WorldTex::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UI_Atlas"),
        TEXT("Com_AtlasTexture"), reinterpret_cast<CComponent**>(&m_pAtlasTextureCom), nullptr)))
        return E_FAIL;
    return S_OK;
}

CUI_WorldTex* CUI_WorldTex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_WorldTex* pInstance = new CUI_WorldTex(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_WorldTex"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_WorldTex::Clone(void* pArg)
{
    CUI_WorldTex* pInstance = new CUI_WorldTex(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_WorldTex"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_WorldTex::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pAtlasTextureCom);
}
