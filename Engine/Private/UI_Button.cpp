#include "UI_Button.h"
#include "GameInstance.h"

CUI_Button::CUI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject{ pDevice,pContext }
{
}

CUI_Button::CUI_Button(const CUI_Button& Prototype)
    :CUIObject{ Prototype }
{
}

HRESULT CUI_Button::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Button::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUI_Button::Priority_Update(_float fTimeDelta)
{
}

void CUI_Button::Update(_float fTimeDelta)
{

}

void CUI_Button::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_Button::Render()
{
    return S_OK;
}

_bool CUI_Button::IsPicked(HWND hWnd)
{
    RECT rc =
    {
        LONG(m_vWorldPos.x - m_vLocalSize.x * 0.5f),
        LONG(m_vWorldPos.y - m_vLocalSize.y * 0.5f),
        LONG(m_vWorldPos.x + m_vLocalSize.x * 0.5f),
        LONG(m_vWorldPos.y + m_vLocalSize.y * 0.5f)
    };

    POINT ptMouse = {};
    GetCursorPos(&ptMouse); // 마우스 커서의 현재 스크린 좌표를 가져온다.
    ScreenToClient(hWnd, &ptMouse); // 윈도우 좌표로 변환한다.

    return PtInRect(&rc, ptMouse); // 마우스가 UI의 Rect 영역 안에 있는지 확인한다.
}

_bool CUI_Button::Update_Picking(HWND hWnd)
{
    if (__super::Update_Picking(hWnd))
    {
        POINT ptMouse{};
        GetCursorPos(&ptMouse);
        ScreenToClient(hWnd, &ptMouse);

        _bool bisPicked = IsPicked(hWnd);

        if (bisPicked)
        {
            m_eState = BUTTON_STATE::HOVERED;
            Broadcast_Hover(static_cast<_float>(ptMouse.x), static_cast<_float>(ptMouse.y));

            _bool isMouseDown = m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB);
            if (isMouseDown && !m_isPrevMouseDown)
            {
                m_eState = BUTTON_STATE::PRESSED;
                Broadcast_Click();
            }

            m_isPrevMouseDown = isMouseDown;
        }
        else
        {
            m_eState = BUTTON_STATE::NORMAL;
            m_isPrevMouseDown = false;
        }

        /*if (bisPicked && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
        {
            m_eState = BUTTON_STATE::PRESSED;
            Broadcast_Click();
        }
        else if (!bisPicked)
        {
            m_eState = BUTTON_STATE::NORMAL;
        }*/

        return bisPicked;
    }
    return false;
}


void CUI_Button::Free()
{
    __super::Free();
}
