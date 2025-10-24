#pragma once
#include "Base.h"
#include "GameInstance.h"

NS_BEGIN(Engine)

class CImgui_Manager final : public CBase
{
private:
    CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CImgui_Manager() = default;

public:
	// Initialize 시에 윈도우 크기와 메뉴 리스트를 받아온다.
    HRESULT             Initialize(list<_wstring> Menu, HWND hWnd);
    void                BeginFrame();
    void                Render();
    void                Shutdown();


    HRESULT             CleanMenu(_wstring strMenu);

public:
	// Widget 추가시에 메뉴 이름과 위젯 함수를 받아온다.
    void                AddWidget(const _wstring Menu, const function<void()>& widget);
public:
    _wstring            Get_OpenMenu_Name();

public:
    _bool HandleWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    //ID3D11Device* m_pDevice = nullptr;
    //ID3D11DeviceContext* m_pContext = nullptr;
    //HWND  m_hWnd = nullptr;

    HWND m_hWnd = {};
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

	class CGameInstance* m_pGameInstance = nullptr;

    ImVec4 clear_color = ImVec4{ 0.45f, 0.55f, 0.60f, 1.00f };

    map<_wstring, vector<std::function<void()>>> m_Widgets;
	map<_wstring, _bool> m_MenuOpen;
	list<wstring> m_Menu;


public:
    static CImgui_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, list<wstring> Menu, HWND hWnd);
    virtual void Free() override;
};

NS_END