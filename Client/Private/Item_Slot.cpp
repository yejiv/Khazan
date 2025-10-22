#include "Item_Slot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"

CItem_Slot::CItem_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Slot{ pDevice, pContext }
{
}

CItem_Slot::CItem_Slot(const CItem_Slot& Prototype)
    : CUI_Slot(Prototype)
{
}

void CItem_Slot::Input_Slot()
{

}

void CItem_Slot::Update_Pos(_int iIndex, _float2 vPos, _float fOffSet, _int iMaxIndexX, _int iMaxIndexY)
{
    _int iCol = iIndex % iMaxIndexX;  // 열 인덱스 (x방향)
    _int iRow = iIndex / iMaxIndexX;

    m_vWorldPos.x = vPos.x + iCol * fOffSet - (iMaxIndexX - 1) * fOffSet / 2;
    m_vWorldPos.y = vPos.y + iRow * fOffSet - (iMaxIndexY - 1) * fOffSet / 2;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_vWorldPos.x - m_iWinSizeX * 0.5f, -m_vWorldPos.y + m_iWinSizeY * 0.5f, 0.f, 1.f));
    __super::Update_Transform(nullptr, m_vWorldPos);
}

HRESULT CItem_Slot::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    return S_OK;
}

HRESULT CItem_Slot::Initialize_Clone(void* pArg)
{
    m_iTexPass = 1;
    _float4 vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_01_Empty.png", m_iTexPass);
    m_vUV.push_back(vUV);

    for (_int i = 0; i < 3; i++)
    {
        _float4 vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_03_UnCommon_Set.png", m_iTexPass);
        m_vUV.push_back(vUV);
    }
    m_iShaderPass = 0;
    m_vColor = { 1.f,1.f,1.f,1.f };

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Childer(), E_FAIL);

    __super::Update_Transform(this, m_vLocalPos);
    return S_OK;
}

void CItem_Slot::Priority_Update(_float fTimeDelta)
{
}

void CItem_Slot::Update(_float fTimeDelta)
{
    
}

void CItem_Slot::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
    
    if (ButtonClick(g_hWnd, false, true))
    {
        m_iState == ENUM_CLASS(UISTATE::ENABLE) ? m_iState = ENUM_CLASS(UISTATE::DISABLE) : m_iState = ENUM_CLASS(UISTATE::ENABLE);
    }

    if (ButtonClick(g_hWnd, true, true))
    {
        m_bIsSelete ? m_bIsSelete = false : m_bIsSelete = true;
    }
    if (m_iState == ENUM_CLASS(UISTATE::DISABLE))
    {
        m_vColor.w = 0.8f;
    }
    if (m_iState == ENUM_CLASS(UISTATE::ENABLE))
    {
        m_vColor.w = 1.f;
        if (ButtonOver(g_hWnd))
        {
            m_pOverFx->Late_Update(fTimeDelta);
        }
        if (m_bIsSelete)
        {
            m_pSeleteFx->Late_Update(fTimeDelta);
        }
        m_pIcon->Late_Update(fTimeDelta);
    }


}

HRESULT CItem_Slot::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Item_Icon"),
        CItem_Slot::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

    return S_OK;
}

HRESULT CItem_Slot::Ready_Childer()
{
    CUI_Atlas_Icon::UIATLASICON_DESC AtlasDesc;

    AtlasDesc.fDepth = m_fDepth;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Over";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    AtlasDesc.vLocalSize = { 130.f, 130.f };

    AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_Hover.png", 1);
    AtlasDesc.iShaderPass = 1;
    AtlasDesc.iTexPass = 1;
    AtlasDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pOverFx = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pOverFx == nullptr)
        return E_FAIL; 
    m_Children.push_back(m_pOverFx);
    Safe_AddRef(m_pOverFx);

    AtlasDesc.fDepth = m_fDepth;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Selet";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    AtlasDesc.vLocalSize = { 131.f, 131.f };

    AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_Select.png", 1);
    AtlasDesc.iShaderPass = 1;
    AtlasDesc.iTexPass = 1;
    AtlasDesc.vColor = { 1.f, 0.7f, 0.6f, 1.f };
    m_pSeleteFx = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pSeleteFx == nullptr)
        return E_FAIL;

    m_Children.push_back(m_pSeleteFx);
    Safe_AddRef(m_pSeleteFx);

    AtlasDesc.fDepth = m_fDepth;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Icon";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    AtlasDesc.vLocalSize = { 93.f, 93.f };

    AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Item_GSword_FallenGeneral_UI.png", 2);
    AtlasDesc.iShaderPass = 0;
    AtlasDesc.iTexPass = 2;
    AtlasDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pIcon = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pIcon == nullptr)
        return E_FAIL;

    m_Children.push_back(m_pIcon);
    Safe_AddRef(m_pIcon);

    return S_OK;
}

CItem_Slot* CItem_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CItem_Slot* pInstance = new CItem_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CItem_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CItem_Slot::Clone(void* pArg)
{
    CItem_Slot* pInstance = new CItem_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CItem_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CItem_Slot::Free()
{
    __super::Free();
    Safe_Release(m_pOverFx);
    Safe_Release(m_pSeleteFx);
    Safe_Release(m_pIcon);
}
