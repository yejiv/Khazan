#include "UI_Talk_Danjinjar.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_WorldTextBox.h"
#include "UI_WorldTex.h"
#include "UI_WorldList.h"


CUI_Talk_Danjinjar::CUI_Talk_Danjinjar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_Talk_Danjinjar::CUI_Talk_Danjinjar(const CUI_Talk_Danjinjar& Prototype)
    : CUI_Panel(Prototype)
{
}

_bool CUI_Talk_Danjinjar::isTalking()
{
    return m_eTaking == TALKSTATE::END ? false : true;
}

HRESULT CUI_Talk_Danjinjar::On_Panel(_int iTalkIndex)
{
    const DANJINJAR_DB* pData = CClientInstance::GetInstance()->Get_Data<DANJINJAR_DB>(iTalkIndex);
    
    CHECK_NULLPTR(pData, E_FAIL);

    m_eTaking = TALKSTATE::TALKING;
    m_iNextEvent = pData->iNextIndex;
    m_wstrFullText = pData->wstrTalk;
    m_fTextSpeed = pData->fTextSpeed;
    m_fDeleyTime = pData->fDeleyTime;

    m_IsUpdate = true;
    m_eAnimState = UIANIMSTATE::ON;
    m_fAccTime = 0.f;
    m_fTalktime = 0.f;
    m_wstrCulText = {};
    m_pText1->Set_Text(TEXT(""));
}

void CUI_Talk_Danjinjar::Off_Panel()
{
    m_eTaking = TALKSTATE::END;
    m_eAnimState = UIANIMSTATE::OFF;
}

void CUI_Talk_Danjinjar::Update_UITransform(_vector vPos)
{
    m_fSpeedWeight = 10.f;
    m_vUV[0] = { 56.f / 168.f, 0.f, 112.f / 168.f, 1.f };

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetZ(XMVectorSetX(XMVectorSetY(vPos, XMVectorGetY(vPos) + 2.5f), XMVectorGetX(vPos)), XMVectorGetZ(vPos)));

    _float offsetY = sin(m_fSpeedWeight * 2.f) * 1.f * 0.5f
        + sin(m_fSpeedWeight * 2.f * 0.5f) * 1.f * 0.5f;
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetY(m_pTransformCom->Get_State(STATE::POSITION), XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)) + offsetY * 0.02f));

    m_vColor = { 1.f,1.f, 1.f, 1.f };
    m_pTransformCom->LookAt_Revers(XMLoadFloat4(m_pGameInstance->Get_CamPosition()));

    for(auto pBg : m_BG)
        pBg->Update_UITransform(m_pTransformCom->Get_WorldMatrix());

    m_pText1->Update_UITransform(m_pTransformCom->Get_WorldMatrix());

//    _float fSide_ScaleX = (36.f / 168.f) * 2.625f;
    

}

HRESULT CUI_Talk_Danjinjar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Talk_Danjinjar::Initialize_Clone(void* pArg)
{
    m_vColor = { 1.f,1.f, 1.f, 0.9f };
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Component(), E_FAIL);
    CHECK_FAILED(Ready_Children(), E_FAIL);

    m_vUV.resize(1);
    return S_OK;
}
void CUI_Talk_Danjinjar::Priority_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

}

void CUI_Talk_Danjinjar::Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    UI_Animation(fTimeDelta);
    Update_Font(fTimeDelta);
}

void CUI_Talk_Danjinjar::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;

    for (auto pBG : m_BG)
        pBG->Late_Update(fTimeDelta);

    m_pText1->Late_Update(fTimeDelta);
}

HRESULT CUI_Talk_Danjinjar::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vUV", &m_vUV[0], sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(18);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();
    return S_OK;
}

HRESULT CUI_Talk_Danjinjar::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_World_Talk"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Talk_Danjinjar::Ready_Children()
{
    m_IsUpdate = false;
     
    CUIObject::UIOBJECT_DESC Desc = {};
    Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
    Desc.vLocalPos = { 0.f, 0.f };
    Desc.vLocalSize = { 1.f, 1.f };
    Desc.szName = "TalkUI";
    m_pText1 = static_cast<CUI_WorldTextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldText"), &Desc));
    CHECK_NULLPTR(m_pText1, E_FAIL);
    Add_Child(m_pText1);
    m_pText1->Set_TextTag(TEXT("Blade_Medium_20"));
    m_pText1->Set_TextAlign(TEXT_ALIGN::CENTER);

    for (_int i = 0; i < 4; ++i)
    {
        CUIObject::UIOBJECT_DESC Desc = {};
        Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
        Desc.vLocalPos = { 0.f, 0.f };
        Desc.vLocalSize = { 1.f, 1.f };
        Desc.szName = "TalkUI";
        CUI_WorldTex* pBG = static_cast<CUI_WorldTex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldTex"), &Desc));
        CHECK_NULLPTR(pBG, E_FAIL);

        if(i == 0)
            pBG->Setting_Texture(0, TEXT("Prototype_Component_UI_World_Talk"), {0, 0.f, 20.f / 168.f, 1.f });        
        else if (i == 1)
            pBG->Setting_Texture(0, TEXT("Prototype_Component_UI_World_Talk"), { 21.f / 168.f, 0.f, 55.f / 168.f, 1.f });
        else if (i == 2)
            pBG->Setting_Texture(0, TEXT("Prototype_Component_UI_World_Talk"), { 113.f / 168.f, 0.f, 147.f / 168.f, 1.f });
        else if (i == 3)
            pBG->Setting_Texture(0, TEXT("Prototype_Component_UI_World_Talk"), { 148.f / 168.f, 0.f, 1.f, 1.f });

        m_BG.push_back(pBG);
        Add_Child(pBG);
    }
    return S_OK;
}

void CUI_Talk_Danjinjar::UI_Animation(_float fTimeDelta)
{
    if (m_eAnimState == UIANIMSTATE::ON)
    {
        m_fAccTime += fTimeDelta * 3.f;
        __super::Update_Alpha(m_fAccTime);

        if (m_fAccTime >= 1.f)
        {
            m_fAccTime = 1.f;
            m_eAnimState = UIANIMSTATE::END;
        }
    }
    else if (m_eAnimState == UIANIMSTATE::OFF)
    {
        m_fAccTime -= fTimeDelta * 3.f;
        __super::Update_Alpha(m_fAccTime);

        if (m_fAccTime <= 0.f)
        {
            m_fAccTime = 0.f;
            m_eAnimState = UIANIMSTATE::END;
            m_IsUpdate = false;
        }
    }
}

void CUI_Talk_Danjinjar::Update_Font(_float fTimeDelta)
{
    m_fTalktime += fTimeDelta * m_fTextSpeed;

    if (m_fTalktime >= 1.f && !m_wstrFullText.empty())
    {
        m_fTalktime = 0.f;

        m_wstrCulText += m_wstrFullText.front();
        m_wstrFullText.erase(0, 1);

        m_pText1->Set_Text(m_wstrCulText);

        _float fCenter_ScaleX = (56.f / 168.f) * 2.625f;
        m_pTransformCom->Scale({ (56.f / 168.f) * 2.625f, 1.f, 1.f });

        _float fSide_ScaleX = {};
        if (m_wstrCulText.size() <= 5)
            fSide_ScaleX = 0.01f;
        else
            fSide_ScaleX = (m_wstrCulText.size() - 5) * 0.08f;

        m_BG[1]->Set_LocalPos({ -fCenter_ScaleX * 0.5f - fSide_ScaleX * 0.5f , 0.f, 0.f, 1.f });
        m_BG[1]->Set_LocalSize({ fSide_ScaleX, 1.f,1.f });
        m_BG[2]->Set_LocalPos({ fCenter_ScaleX * 0.5f + fSide_ScaleX * 0.5f ,0.f, 0.f, 1.f });
        m_BG[2]->Set_LocalSize({ fSide_ScaleX, 1.f,1.f });

        _float fLastSide_ScaleX = (20.f / 168.f) * 2.625f;
        m_BG[0]->Set_LocalPos({ -fCenter_ScaleX * 0.5f - fSide_ScaleX - fLastSide_ScaleX * 0.5f, 0.f, 0.f, 1.f });
        m_BG[0]->Set_LocalSize({ fLastSide_ScaleX, 1.f,1.f });
        m_BG[3]->Set_LocalPos({ fCenter_ScaleX * 0.5f + fSide_ScaleX + fLastSide_ScaleX * 0.5f, 0.f, 0.f, 1.f });
        m_BG[3]->Set_LocalSize({ fLastSide_ScaleX, 1.f,1.f });


        m_pText1->Set_LocalPos({ 0.f, 0.25f, -0.001f, 1.f });
        m_pText1->Set_LocalSize({ 2.f, 2.f, 1.f });
        m_pText1->Set_Color({ 1.f,1.f,1.f,1.f });
    }
    if (m_wstrFullText.empty())
    {
        if (m_fDeleyTime <= 0.f && m_eTaking == TALKSTATE::NEXT)
        {
            if (m_iNextEvent == 0)
                Off_Panel();
            else
            {
                const DANJINJAR_DB* pData = CClientInstance::GetInstance()->Get_Data<DANJINJAR_DB>(m_iNextEvent);

                if (pData == nullptr)
                    Off_Panel();

                m_iNextEvent = pData->iNextIndex;
                m_wstrFullText = pData->wstrTalk;
                m_fTextSpeed = pData->fTextSpeed;
                m_fDeleyTime = pData->fDeleyTime;
                m_fTalktime = 0.f;
                m_wstrCulText = {};
                m_pText1->Set_Text(TEXT(""));
            }
        }
        else if (m_fDeleyTime > 0.f && m_fTalktime >= m_fDeleyTime)
        {
            if (m_iNextEvent == 0)
                Off_Panel();
            else
            {
                const DANJINJAR_DB* pData = CClientInstance::GetInstance()->Get_Data<DANJINJAR_DB>(m_iNextEvent);

                if (pData == nullptr)
                    Off_Panel();

                m_iNextEvent = pData->iNextIndex;
                m_wstrFullText = pData->wstrTalk;
                m_fTextSpeed = pData->fTextSpeed;
                m_fDeleyTime = pData->fDeleyTime;
                m_fTalktime = 0.f;
                m_wstrCulText = {};
                m_pText1->Set_Text(TEXT(""));
            }
        }
    }
}

CUI_Talk_Danjinjar* CUI_Talk_Danjinjar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Talk_Danjinjar* pInstance = new CUI_Talk_Danjinjar(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Talk_Danjinjar"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Talk_Danjinjar::Clone(void* pArg)
{
    CUI_Talk_Danjinjar* pInstance = new CUI_Talk_Danjinjar(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Talk_Danjinjar"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Talk_Danjinjar::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pText1);

    for (auto pBG : m_BG)
        Safe_Release(pBG);
    m_BG.clear();
}
