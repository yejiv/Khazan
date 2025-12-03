#include "BladeNexus_Map_List.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "UI_Default_Tex.h"

#include "UI_BladeNexus_Map.h"

CUI_BladeNexus_Map_List::CUI_BladeNexus_Map_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Tap{ pDevice, pContext }
{
}

CUI_BladeNexus_Map_List::CUI_BladeNexus_Map_List(const CUI_BladeNexus_Map_List& Prototype)
    : CUI_Tap(Prototype)
{
}

void CUI_BladeNexus_Map_List::Update_Pos(_int iIndex, _float2 vPos, _float fOffSetY)
{
    m_iIndex = iIndex;

    m_vLocalPos.x = vPos.x;
    m_vLocalPos.y = vPos.y + iIndex * fOffSetY;

    __super::Update_Transform(nullptr, m_vWorldPos);
}

void CUI_BladeNexus_Map_List::Setting_List(_int iType)
{
    if (iType == 0)
    {
        if (m_iIndex == 0)
        {
            m_pListText->Set_Text(TEXT("눈보라 협곡"));
            m_bIsOn = CClientInstance::GetInstance()->Find_BladeNexus(0)->isUnLock;
        }
        else if (m_iIndex == 1)
        {
            m_pListText->Set_Text(TEXT("냉기 서린 동굴"));
            m_bIsOn = CClientInstance::GetInstance()->Find_BladeNexus(1)->isUnLock;
        }
        else if (m_iIndex == 2)
        {
            m_pListText->Set_Text(TEXT("설인의 대지"));
            m_bIsOn = CClientInstance::GetInstance()->Find_BladeNexus(2)->isUnLock;
        }
    }
    else
    {
        if (m_iIndex == 0)
        {
            m_pListText->Set_Text(TEXT("잊혀진 사원의 지하"));
            m_bIsOn = CClientInstance::GetInstance()->Find_BladeNexus(4)->isUnLock;
        }
        else if (m_iIndex == 1)
        {
            m_pListText->Set_Text(TEXT("잊혀진 사원의 깊은 곳"));
            m_bIsOn = CClientInstance::GetInstance()->Find_BladeNexus(5)->isUnLock;
        }
        else if (m_iIndex == 2)
        {
            m_pListText->Set_Text(TEXT("잊혀진 사원의 심장"));
            m_bIsOn = CClientInstance::GetInstance()->Find_BladeNexus(6)->isUnLock;
        }
    }

    
    if (m_bIsOn)
        m_pListText->Set_Color({ 1.f, 1.f, 1.f, 1.f });
    else
        m_pListText->Set_Color({ 1.f, 1.f, 1.f, 0.6f });

}

void CUI_BladeNexus_Map_List::Set_Selete(_bool isSelete)
{
    if (m_bIsSelete == isSelete)
        return;

    m_bIsSelete = isSelete;

    if (m_bIsSelete)
    {
        _wstring wstrSound = TEXT("UI_common_mouse_over_0") + to_wstring(m_pGameInstance->Rand(1, 4)) + TEXT(" (SFX).wav");
        m_pGameInstance->PlaySoundOnce(wstrSound.c_str());
    }
}

HRESULT CUI_BladeNexus_Map_List::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    return S_OK;
}

HRESULT CUI_BladeNexus_Map_List::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Componet(), E_FAIL);

    return S_OK;
}

void CUI_BladeNexus_Map_List::Priority_Update(_float fTimeDelta)
{
}

void CUI_BladeNexus_Map_List::Update(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    if (!m_bIsOn)
    {
        return;
    }
    if (ButtonOver(g_hWnd))
    {
        CUI_BladeNexus_Map::BUBBLE_MAP_DESC Desc = {};
        Desc.iIndex = m_iIndex;
        Desc.isClick = false;
        Bubble_EventCall(&Desc);
    }
    if (ButtonClick(g_hWnd, false, true, INPUT_TYPE::UI) || m_bIsSelete && m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::UI))
    {
        CUI_BladeNexus_Map::BUBBLE_MAP_DESC Desc = {};
        Desc.iIndex = m_iIndex;
        Desc.isClick = true;
        Bubble_EventCall(&Desc);
    }
}

void CUI_BladeNexus_Map_List::Late_Update(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    if (!m_bIsSelete || !m_bIsOn)
        m_pOverTex->Update_Visible(false);
    else
        m_pOverTex->Update_Visible(true);

    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_BladeNexus_Map_List::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(m_iShaderPass);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_BladeNexus_Map_List::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);
    m_iState = ENUM_CLASS(UISTATE::ENABLE);

    for (auto pChild : m_Children)
    {
        string strName = pChild->Get_Name();

        if ("List_Text" == strName)
        {
            m_pListText = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }
        else if ("List_Hover" == strName)
        {
            m_pOverTex = static_cast<CUI_Default_Tex*>(pChild);
            Safe_AddRef(pChild);
        }
    }

    return S_OK;
}

HRESULT CUI_BladeNexus_Map_List::Ready_Componet()
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

CUI_BladeNexus_Map_List* CUI_BladeNexus_Map_List::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CUI_BladeNexus_Map_List* pInstance = new CUI_BladeNexus_Map_List(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_BladeNexus_Map_List"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BladeNexus_Map_List::Clone(void* pArg)
{
    CUI_BladeNexus_Map_List* pInstance = new CUI_BladeNexus_Map_List(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_BladeNexus_Map_List"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_BladeNexus_Map_List::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pListText);
    Safe_Release(m_pOverTex);
}
