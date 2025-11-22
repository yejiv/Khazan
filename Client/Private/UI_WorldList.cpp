#include "UI_Talk_Daphrona.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_WorldTextBox.h"
#include "UI_WorldTex.h"
#include "UI_WorldList.h"
#include "UI_WorldFX.h"

CUI_WorldList::CUI_WorldList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_WorldList::CUI_WorldList(const CUI_WorldList& Prototype)
    : CUI_Panel(Prototype)
{
}

void CUI_WorldList::Update_UITransform(_matrix vParentMat)
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

    m_pName->Update_UITransform(finalWorld);
    m_pName->Set_LocalPos({ -0.32f, 0.015f, -0.001f, 1.f });
    m_pName->Set_LocalSize({ 0.35f, 0.35f, 1.f });
    m_pName->Set_Color({ 0.8f, 0.8f, 0.8f,1.f });

    for (auto pLine : m_BG)
        pLine->Update_UITransform(finalWorld);

    m_pIcon->Update_UITransform(finalWorld);
    m_pSelete->Update_UITransform(finalWorld);
    m_pSeleteIcon->Update_UITransform(finalWorld);
    m_pFX->Update_UITransform(finalWorld);

    m_BG[0]->Set_LocalPos({ -0.415f,0.f,0.f,1.f });
    m_BG[0]->Set_LocalSize({ 0.03f, 0.1f,1.f });
    m_BG[0]->Set_Color({ 1.f, 1.f, 1.f, 0.9f });


    m_BG[1]->Set_LocalPos({ 0.415f,0.f,0.f,1.f });
    m_BG[1]->Set_LocalSize({ 0.03f, 0.1f,1.f });
    m_BG[1]->Set_Color({ 1.f, 1.f, 1.f, 0.9f });

    m_pIcon->Set_LocalPos({ -0.37f,0.f,-0.001f,1.f });
    m_pIcon->Set_LocalSize({ 0.05f, 0.05f,1.f });

    m_pSelete->Set_LocalPos({ 0.f,0.f,-0.001f, 1.f });
    m_pSelete->Set_LocalSize({ 0.87f, 0.1f,1.f });

    m_pSeleteIcon->Set_LocalPos({ 0.37f,0.f,-0.002f, 1.f });
    m_pSeleteIcon->Set_LocalSize({ 0.1f, 0.1f,1.f });

    m_pFX->Set_LocalPos({ 0.f,0.f,-0.003f, 1.f });
    m_pFX->Set_LocalSize({ 0.87f, 0.1f,1.f });
}

void CUI_WorldList::Set_LocalPos(_vector vPos)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetZ(vPos,XMVectorGetZ(vPos) + m_fOffsetZ));

}

void CUI_WorldList::Set_LocalSize(_float3 vSize)
{
    m_pTransformCom->Scale(vSize);
}

void CUI_WorldList::Setting_Icon(_int iTexPass, _float4 vUV)
{
    m_pIcon->Setting_Texture(iTexPass, vUV);
}

void CUI_WorldList::Set_Text(_wstring wstrText)
{
    m_pName->Set_Text(wstrText);
}

void CUI_WorldList::Set_Selete(_bool isSelete)
{
    m_pSelete->Update_Visible(isSelete);
    m_pSeleteIcon->Update_Visible(isSelete);
    if (isSelete)
        m_fOffsetZ = -0.05f;
    else
        m_fOffsetZ = 0.f;

}

void CUI_WorldList::ON_FX()
{
    m_pFX->Start_Anim();
}

HRESULT CUI_WorldList::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_WorldList::Initialize_Clone(void* pArg)
{
    m_vColor = { 1.f,1.f, 1.f, 0.9f };
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Component(), E_FAIL);
    CHECK_FAILED(Ready_Children(), E_FAIL);
    m_fAccTime = 0.5f;

    return S_OK;
}
void CUI_WorldList::Priority_Update(_float fTimeDelta)
{
}

void CUI_WorldList::Update(_float fTimeDelta)
{

}

void CUI_WorldList::Late_Update(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    if (m_isUpTime)
        m_fAccTime += fTimeDelta * 0.7f;
    else         
        m_fAccTime -= fTimeDelta * 0.7f;
        
    if (m_fAccTime >= 1.f)
        m_isUpTime = false;
    else if (m_fAccTime <= 0.3f)
        m_isUpTime = true;

    m_pSeleteIcon->Set_Color({ 1.f,1.f,1.f,m_fAccTime });

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_WorldList::Render()
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix), E_FAIL);

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 1)))
        return E_FAIL;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(17);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();
    return S_OK;
}

HRESULT CUI_WorldList::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_State_Button"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_WorldList::Ready_Children()
{
    CUIObject::UIOBJECT_DESC Desc = {};
    Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
    Desc.vLocalPos = { 0.f, 0.f };
    Desc.vLocalSize = { 1.f, 1.f };
    Desc.szName = "Name";

    for (_int i = 0; i < 2; ++i)
    {
        CUI_WorldTex* pBG = static_cast<CUI_WorldTex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldTex"), &Desc));
        CHECK_NULLPTR(pBG, E_FAIL);
        m_BG.push_back(pBG);

        m_Children.push_back(pBG);
        Safe_AddRef(pBG);
    }
    m_BG[0]->Setting_Texture(0, TEXT("Prototype_Component_UI_State_Button"));
    m_BG[1]->Setting_Texture(2, TEXT("Prototype_Component_UI_State_Button"));

    m_pName = static_cast<CUI_WorldTextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldText"), &Desc));
    CHECK_NULLPTR(m_pName, E_FAIL);
    m_pName->Set_TextTag(TEXT("Blade_Medium_20"));
    m_Children.push_back(m_pName);
    Safe_AddRef(m_pName);

    m_pIcon = static_cast<CUI_WorldTex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldTex"), &Desc));
    CHECK_NULLPTR(m_pIcon, E_FAIL);
    m_pIcon->Setting_Texture(2, CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_01_WeaponPower.png", 2));
    m_Children.push_back(m_pIcon);
    Safe_AddRef(m_pIcon);

    m_pSelete = static_cast<CUI_WorldTex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldTex"), &Desc));
    CHECK_NULLPTR(m_pSelete, E_FAIL);
    m_pSelete->Setting_Texture(8, TEXT("Prototype_Component_UI_State_Button"));
    m_Children.push_back(m_pSelete);
    Safe_AddRef(m_pSelete);

    m_pSeleteIcon = static_cast<CUI_WorldTex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldTex"), &Desc));
    CHECK_NULLPTR(m_pSeleteIcon, E_FAIL);
    m_pSeleteIcon->Setting_Texture(4, CClientInstance::GetInstance()->Get_AtlasUV("Loading_1.png", 4));
    m_Children.push_back(m_pSeleteIcon);
    Safe_AddRef(m_pSeleteIcon);

    m_pFX = static_cast<CUI_WorldFX*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldFX"), &Desc));
    CHECK_NULLPTR(m_pFX, E_FAIL);
    m_Children.push_back(m_pFX);
    Safe_AddRef(m_pFX);

    m_pFX->Update_Visible(false);
    return S_OK;
}

CUI_WorldList* CUI_WorldList::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_WorldList* pInstance = new CUI_WorldList(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_WorldList"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_WorldList::Clone(void* pArg)
{
    CUI_WorldList* pInstance = new CUI_WorldList(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_WorldList"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_WorldList::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pName);
    Safe_Release(m_pIcon);
    Safe_Release(m_pSelete);
    Safe_Release(m_pSeleteIcon);
    Safe_Release(m_pFX);

    for (auto pBg : m_BG)
        Safe_Release(pBg);
    m_BG.clear();
}
