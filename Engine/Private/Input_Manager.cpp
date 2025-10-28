#include "Input_Manager.h"
#include "Input_Device.h"

inline bool IsDownEdge(const _byte* prev, const _byte* curr, _ubyte k) {
    const bool p = (prev[k] & 0x80) != 0;
    const bool c = (curr[k] & 0x80) != 0;
    return (!p && c);
}

inline bool IsUpEdge(const _byte* prev, const _byte* curr, _ubyte k) {
    const bool p = (prev[k] & 0x80) != 0;
    const bool c = (curr[k] & 0x80) != 0;
    return (p && !c);
}

inline bool IsPressing(const _byte* curr, _ubyte k) {
    return (curr[k] & 0x80) != 0;
}

inline bool MouseIsDownEdge(const DIMOUSESTATE& prev, const DIMOUSESTATE& curr, int b) {
    const bool p = (prev.rgbButtons[b] & 0x80) != 0;
    const bool c = (curr.rgbButtons[b] & 0x80) != 0;
    return (!p && c);
}
inline bool MouseIsUpEdge(const DIMOUSESTATE& prev, const DIMOUSESTATE& curr, int b) {
    const bool p = (prev.rgbButtons[b] & 0x80) != 0;
    const bool c = (curr.rgbButtons[b] & 0x80) != 0;
    return (p && !c);
}
inline bool MouseIsPressing(const DIMOUSESTATE& curr, int b) {
    return (curr.rgbButtons[b] & 0x80) != 0;
}

CInput_Manager::CInput_Manager()
{
}

HRESULT CInput_Manager::Initialize(HINSTANCE hInst, HWND hWnd)
{
    m_pInput_Device = CInput_Device::Create(hInst, hWnd);
    if (nullptr == m_pInput_Device)
        return E_FAIL;

    return S_OK;
}

void CInput_Manager::Update()
{
    m_pInput_Device->Update();
}

_bool CInput_Manager::Key_Pressing(_ubyte byKeyID, _float fTimeDelta, INPUT_TYPE eType, _float* pPressingTime)
{
    if (m_eInputType != eType) return false;

    const _byte* curr = m_pInput_Device->GetKeyCurr();
    if (IsPressing(curr, byKeyID)) {
        m_fPressingTime[byKeyID] += fTimeDelta;
        if (pPressingTime) *pPressingTime = m_fPressingTime[byKeyID];
        return true;
    }
    else {
        // 누르지 않으면 시간 초기화(원하면 유지해도 됨)
        m_fPressingTime[byKeyID] = 0.f;
        if (pPressingTime) *pPressingTime = 0.f;
        return false;
    }
}

_bool CInput_Manager::Key_Down(_ubyte byKeyID, INPUT_TYPE eType)
{
    if (m_eInputType != eType) return false;
    return IsDownEdge(m_pInput_Device->GetKeyPrev(), m_pInput_Device->GetKeyCurr(), byKeyID);
}

_bool CInput_Manager::Key_Up(_ubyte byKeyID, INPUT_TYPE eType)
{
    if (m_eInputType != eType) return false;
    return IsUpEdge(m_pInput_Device->GetKeyPrev(), m_pInput_Device->GetKeyCurr(), byKeyID);

}

_bool CInput_Manager::Mouse_Pressing(MOUSEKEYSTATE eMouse, INPUT_TYPE eType)
{
    if (m_eInputType != eType) return false;
    return MouseIsPressing(m_pInput_Device->GetMouseCurr(), ENUM_CLASS(eMouse));

    return false;
}

_bool CInput_Manager::Mouse_Down(MOUSEKEYSTATE eMouse, INPUT_TYPE eType)
{
    if (m_eInputType != eType) return false;
    return MouseIsDownEdge(m_pInput_Device->GetMousePrev(), m_pInput_Device->GetMouseCurr(), ENUM_CLASS(eMouse));
}

_bool CInput_Manager::Mouse_Up(MOUSEKEYSTATE eMouse, INPUT_TYPE eType)
{
    if (m_eInputType != eType) return false;
    return MouseIsUpEdge(m_pInput_Device->GetMousePrev(), m_pInput_Device->GetMouseCurr(), ENUM_CLASS(eMouse));
}

_long CInput_Manager::Mouse_Move(MOUSEMOVESTATE eMouseState, INPUT_TYPE eType)
{
    if (m_eInputType != eType) return 0;
    const auto& ms = m_pInput_Device->GetMouseCurr();
    switch (eMouseState) {
    case MOUSEMOVESTATE::X: return ms.lX;
    case MOUSEMOVESTATE::Y: return ms.lY;
    case MOUSEMOVESTATE::WHEEL: return ms.lZ; // 휠
    }
    return 0;
}

void CInput_Manager::Change_InputType(INPUT_TYPE eType)
{
    m_eInputType = eType;
}

CInput_Manager* CInput_Manager::Create(HINSTANCE hInst, HWND hWnd)
{
    CInput_Manager* pInstance = new CInput_Manager();

    if (FAILED(pInstance->Initialize(hInst, hWnd)))
    {
        MSG_BOX(TEXT("Failed to Created : CInput_Device"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInput_Manager::Free()
{
    Safe_Release(m_pInput_Device);
}
