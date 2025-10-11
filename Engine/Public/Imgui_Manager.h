#pragma once
#include "Base.h"
#include "GameInstance.h"

NS_BEGIN(Engine)

class CImgui_Manager final : public CBase
{
private:
    CImgui_Manager();
    virtual ~CImgui_Manager() = default;

public:
	// Initialize 시에 윈도우 크기와 메뉴 리스트를 받아온다.
    HRESULT             Initialize(_uint iWinSizeX, _uint iWinSizeY, list<_wstring> Menu);
    void                BeginFrame();
    void                Render();
    void                Shutdown();

public:
	// Widget 추가시에 메뉴 이름과 위젯 함수를 받아온다.
    void                AddWidget(const _wstring Menu, const function<void()>& widget);
public:
    _wstring            Get_OpenMenu_Name();

private:
    //ID3D11Device* m_pDevice = nullptr;
    //ID3D11DeviceContext* m_pContext = nullptr;
    //HWND  m_hWnd = nullptr;

    ID3D11Device* g_pd3dDevice = { nullptr };
    ID3D11DeviceContext* g_pd3dDeviceContext = { nullptr };
    IDXGISwapChain* g_pSwapChain = { nullptr };
    bool                     g_SwapChainOccluded = { false };
    UINT                     g_ResizeWidth = { 0 };
    UINT                     g_ResizeHeight = { 0 };
    ID3D11RenderTargetView* g_mainRenderTargetView = { nullptr };
    WNDCLASSEXW wc = {};
    HWND hwnd = {};

    ImVec4 clear_color = ImVec4{ 0.45f, 0.55f, 0.60f, 1.00f };

    map<_wstring, vector<std::function<void()>>> m_Widgets;
	map<_wstring, _bool> m_MenuOpen;
	list<wstring> m_Menu;

private:
    bool                 CreateDeviceD3D(HWND hWnd);
    void                 CleanupDeviceD3D();
    void                 CreateRenderTarget();
    void                 CleanupRenderTarget();


public:
    static CImgui_Manager* Create(_uint iWinSizeX, _uint iWinSizeY, list<wstring> Menu);
    virtual void Free() override;
};

NS_END