#include "Input_Device.h"

CInput_Device::CInput_Device(void)
{
	ZeroMemory(m_byKeyStateCurr, sizeof(m_byKeyStateCurr));
	ZeroMemory(m_byKeyStatePrev, sizeof(m_byKeyStatePrev));
	ZeroMemory(&m_tMouseStateCurr, sizeof(m_tMouseStateCurr));
	ZeroMemory(&m_tMouseStatePrev, sizeof(m_tMouseStatePrev));
}

HRESULT CInput_Device::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		nullptr)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	m_pKeyBoard->Acquire();

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	m_pMouse->SetDataFormat(&c_dfDIMouse);

	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	m_pMouse->Acquire();


	return S_OK;
}

void CInput_Device::Update(void)
{
	memcpy(m_byKeyStatePrev, m_byKeyStateCurr, sizeof(m_byKeyStateCurr));
	m_tMouseStatePrev = m_tMouseStateCurr;

	HRESULT hrKey = m_pKeyBoard->GetDeviceState(256, m_byKeyStateCurr);
	if (FAILED(hrKey)) {
		m_pKeyBoard->Acquire();
		ZeroMemory(m_byKeyStateCurr, sizeof(m_byKeyStateCurr));
	}

	HRESULT hrMouse = m_pMouse->GetDeviceState(sizeof(m_tMouseStateCurr), &m_tMouseStateCurr);
	if (FAILED(hrMouse)) {
		m_pMouse->Acquire();
		ZeroMemory(&m_tMouseStateCurr, sizeof(m_tMouseStateCurr));
	}
}

CInput_Device* CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Ready_InputDev(hInst, hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CInput_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

