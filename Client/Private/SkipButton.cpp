#include "SkipButton.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "SkipButton_Progress.h"
#include "UI_TextBox.h"

CSkipButton::CSkipButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel(pDevice, pContext)
{
}

CSkipButton::CSkipButton(const CSkipButton& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CSkipButton::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    CHECK_FAILED(Ready_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CSkipButton::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);
    m_vUV.resize(1);
    m_iTexPass = 3;
    m_iShaderPass = 0;
    m_vColor = { 1.f, 1.f, 1.f, 1.f };
    m_isPool = true;

    m_fAlpha = 0.f;
    m_vUV[0] = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_G.png", 3);

    m_fAccTime = 0.f;
    m_fDelayTime = 1.5f;
    CHECK_FAILED(Ready_Children(), E_FAIL);

    CClientInstance::GetInstance()->Add_RootUI(TEXT("SkipButton"), this);
    return S_OK;
}

void CSkipButton::Priority_Update(_float fTimeDelta)
{
}

void CSkipButton::Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    if (m_pGameInstance->Key_Pressing(DIK_G, 0, INPUT_TYPE::FORCE))
        m_isPressing = true;
    
    if (m_bPrePressingState != m_isPressing)
    {
        m_fAccTime = 0.f;
        m_bPrePressingState = m_isPressing;
    }

    if (m_isPressing)
    {
        m_fAccTime += fTimeDelta;
        m_fAlpha += fTimeDelta;
        if (m_fAlpha >= 0.8f)
        {
            m_fAlpha = 0.8f;
        }
    }
    else
    {
        m_fAlpha -= fTimeDelta;
        if (m_fAlpha <= 0.f)
        {
            m_fAlpha = 0.f;
        }
    }
    
    if (m_fAccTime >= m_fDelayTime)
    {
        m_Event();
        m_IsUpdate = false;
    }
    Update_Alpha(m_fAlpha);
    m_isPressing = false;

    __super::Update(fTimeDelta);
}

void CSkipButton::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
    __super::Late_Update(fTimeDelta);

}

HRESULT CSkipButton::Render()
{
    return S_OK;
}

HRESULT CSkipButton::Update_Switch(void* pArg)
{
    SKIPBUTTON_DESC* pDesc = static_cast<SKIPBUTTON_DESC*>(pArg);

    m_IsUpdate = pDesc->isOpen;
    if (m_IsUpdate)
        m_Event = pDesc->Event;
    else
        m_Event = nullptr;

    return S_OK;
}

HRESULT CSkipButton::Ready_Prototype()
{
    if (FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_SkipButton_Progress"),
        CSkipButton_Progress::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkipButton::Ready_Children()
{
    CUIObject::UIOBJECT_DESC TextDesc = {};
    TextDesc.fDepth = m_fDepth;
    TextDesc.iUIType = ENUM_CLASS(UITYPE::TEXT);
    TextDesc.szName = "Text";
    TextDesc.vLocalPos = _float2{ 31.f, 20.f };
    TextDesc.vLocalSize = { m_vLocalSize.x, m_vLocalSize.y };
    TextDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pTextBox = static_cast<CUI_TextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevel, TEXT("Prototype_GameObject_UI_TextBox"), &TextDesc));

    if (m_pTextBox == nullptr)
        return E_FAIL;
    m_Children.push_back(m_pTextBox);
    Safe_AddRef(m_pTextBox);

    CUI_TextBox::TEXTBOX_DESC TextSet = {};
    TextSet.bIsTextBox = false;
    TextSet.eTextAlign = TEXT_ALIGN::LEFT_CENTER;
    TextSet.fMaxWidth = 0;
    TextSet.fOffsetHeight = 0;
    TextSet.iPivotX = 0;
    TextSet.iPivotY = 0;
    TextSet.wstrTexttag = TEXT("Blade_Medium_22");
    TextSet.wstrText = TEXT("");
    TextSet.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pTextBox->Setting_Text(TextSet);

    //가이드 게이지
    CSkipButton_Progress::GUIDEGAUGE_DESC GuideDesc = {};
    GuideDesc.fDepth = m_fDepth;
    GuideDesc.iUIType = ENUM_CLASS(UITYPE::PROGRESSBAR);
    GuideDesc.szName = "Gauge";
    GuideDesc.vLocalPos = _float2{ 0.f, 0.f };
    GuideDesc.vLocalSize = { m_vLocalSize.x + 4.f, m_vLocalSize.y + 4.f };
    GuideDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    GuideDesc.pCulValue = &m_fAccTime;
    GuideDesc.pMaxValue = &m_fDelayTime;

    m_pGauge = static_cast<CSkipButton_Progress*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevel, TEXT("Prototype_GameObject_UI_SkipButton_Progress"), &GuideDesc));

    if (m_pGauge == nullptr)
        return E_FAIL;
    m_Children.push_back(m_pGauge);
    Safe_AddRef(m_pGauge);

    m_pTextBox->Set_Text(TEXT("건너뛰기"));
    __super::Update_Transform(nullptr,m_vLocalPos);

    return S_OK;
}

CSkipButton* CSkipButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CSkipButton* pInstance = new CSkipButton(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CSkipButton"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkipButton::Clone(void* pArg)
{
    CSkipButton* pInstance = new CSkipButton(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkipButton"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkipButton::Free()
{
    __super::Free();
    Safe_Release(m_pGauge);
    Safe_Release(m_pTextBox);
}
