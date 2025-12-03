#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	const _byte* GetKeyCurr() const { return m_byKeyStateCurr; }
	const _byte* GetKeyPrev() const { return m_byKeyStatePrev; }

	const DIMOUSESTATE& GetMouseCurr() const { return m_tMouseStateCurr; }
	const DIMOUSESTATE& GetMousePrev() const { return m_tMouseStatePrev; }

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte    m_byKeyStateCurr[256]{};
	_byte    m_byKeyStatePrev[256]{};

	DIMOUSESTATE m_tMouseStateCurr{};
	DIMOUSESTATE m_tMouseStatePrev{};

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
NS_END

