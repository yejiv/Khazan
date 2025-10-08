#include "EnginePch.h"
#include "Input_Manager.h"
#include "Input_Device.h"

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

_bool CInput_Manager::Key_Pressing(_ubyte byKeyID, _float fTimeDelta, _float* pPressingTime)
{
    if ((m_pInput_Device->Get_DIKeyState(byKeyID)) & 0x8000)
    {
        m_fPressingTime[byKeyID] += fTimeDelta;
        if (pPressingTime != nullptr)
            *pPressingTime = m_fPressingTime[byKeyID];
        return true;
    }
    return false;
}

_bool CInput_Manager::Key_Down(_ubyte byKeyID)
{
    if (!(m_bKeyState_Down[byKeyID]) && ((m_pInput_Device->Get_DIKeyState(byKeyID)) & 0x8000))
    {
        m_bKeyState_Down[byKeyID] = !m_bKeyState_Down[byKeyID];
        m_fPressingTime[byKeyID] = 0.f;
        return true;
    }

    if ((m_bKeyState_Down[byKeyID]) && !((m_pInput_Device->Get_DIKeyState(byKeyID)) & 0x8000))
    {
        m_bKeyState_Down[byKeyID] = !m_bKeyState_Down[byKeyID];
        return false;
    }

    return false;
}

_bool CInput_Manager::Key_Up(_ubyte byKeyID)
{

    if (!m_bKeyState_Up[byKeyID] && ((m_pInput_Device->Get_DIKeyState(byKeyID)) & 0x8000))
    {
        m_bKeyState_Up[byKeyID] = !m_bKeyState_Up[byKeyID];
        return false;
    }
    if (m_bKeyState_Up[byKeyID] && !((m_pInput_Device->Get_DIKeyState(byKeyID)) & 0x8000))
    {
        m_bKeyState_Up[byKeyID] = !m_bKeyState_Up[byKeyID];
        return true;
    }
    return false;

}

_bool CInput_Manager::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
    if ((m_pInput_Device->Get_DIMouseState(eMouse)) & 0x8000)
        return true;

    return false;
}

_bool CInput_Manager::Mouse_Down(MOUSEKEYSTATE eMouse)
{
    if (!(m_bMouseState_Down[ENUM_CLASS(eMouse)]) && ((m_pInput_Device->Get_DIMouseState(eMouse)) & 0x8000))
    {
        m_bMouseState_Down[ENUM_CLASS(eMouse)] = !m_bMouseState_Down[ENUM_CLASS(eMouse)];
        return true;
    }

    if ((m_bMouseState_Down[ENUM_CLASS(eMouse)]) && !((m_pInput_Device->Get_DIMouseState(eMouse)) & 0x8000))
    {
        m_bMouseState_Down[ENUM_CLASS(eMouse)] = !m_bMouseState_Down[ENUM_CLASS(eMouse)];
        return false;
    }

    return false;
}

_bool CInput_Manager::Mouse_Up(MOUSEKEYSTATE eMouse)
{
    if (!(m_bMouseState_Up[ENUM_CLASS(eMouse)]) && ((m_pInput_Device->Get_DIMouseState(eMouse)) & 0x8000))
    {
        m_bMouseState_Up[ENUM_CLASS(eMouse)] = !m_bMouseState_Up[ENUM_CLASS(eMouse)];
        return false;
    }

    if ((m_bMouseState_Up[ENUM_CLASS(eMouse)]) && !((m_pInput_Device->Get_DIMouseState(eMouse)) & 0x8000))
    {
        m_bMouseState_Up[ENUM_CLASS(eMouse)] = !m_bMouseState_Up[ENUM_CLASS(eMouse)];
        return true;
    }

    return false;
}

_long CInput_Manager::Mouse_Move(MOUSEMOVESTATE eMouseState)
{
    return m_pInput_Device->Get_DIMouseMove(eMouseState);
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
