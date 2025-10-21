#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CInput_Manager : public CBase
{
private:
	CInput_Manager();
	virtual ~CInput_Manager() = default;

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);

	void Update();

public:
	// 키보드 입력값
	// 지속적으로 키를 누를경우 사용될 함수 (byKeyID : 키값,  fTimeDelta : 시간값, pPressingTime : 누를 시간 알수있도록 포인터 값)
	_bool		Key_Pressing(_ubyte byKeyID, _float fTimeDelta, INPUT_TYPE eType, _float* pPressingTime);
	// 해당 키값이 눌릴경우 사용될 함수 (byKeyID : 키값)
	_bool		Key_Down(_ubyte byKeyID, INPUT_TYPE eType);
	// 해당 키값이 떼어질경우 사용될 함수 (byKeyID : 키값)
	_bool		Key_Up(_ubyte byKeyID, INPUT_TYPE eType);

	// 마우스 입력값
	// 지속적으로	마우스 버튼을 누를경우 사용될 함수 (eMouse : 마우스 버튼 enum)
	_bool		Mouse_Pressing(MOUSEKEYSTATE eMouse, INPUT_TYPE eType);
	// 해당 마우스 버튼이 눌릴경우 사용될 함수 (eMouse : 마우스 버튼 enum)
	_bool		Mouse_Down(MOUSEKEYSTATE eMouse, INPUT_TYPE eType);
	// 해당 마우스 버튼이 떼어질경우 사용될 함수 (eMouse : 마우스 버튼 enum)
	_bool		Mouse_Up(MOUSEKEYSTATE eMouse, INPUT_TYPE eType);

	// 마우스 이동값
	_long		Mouse_Move(MOUSEMOVESTATE eMouseState, INPUT_TYPE eType);

	void		Change_InputType(INPUT_TYPE eType);

private:
	class CInput_Device* m_pInput_Device = { nullptr };

private:
	_bool			m_bKeyState_Down[0xff] = { false, };
	_float			m_fPressingTime[0xff] = { 0.f, };
	_bool			m_bKeyState_Up[0xff] = { false, };


	_bool			m_bMouseState_Down[ENUM_CLASS(MOUSEKEYSTATE::END)] = { false, };
	_bool			m_bMouseState_Up[ENUM_CLASS(MOUSEKEYSTATE::END)] = { false, };

	INPUT_TYPE		m_eInputType = { INPUT_TYPE::GAMEPLAY };

public:
	static CInput_Manager* Create(HINSTANCE hInst, HWND hWnd);
	virtual void Free() override;

};

NS_END