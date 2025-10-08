#pragma once
#include "Base.h"
#include "GameInstance.h"
#define IMGUI_DEFINE_MATH_OPERATORS

NS_BEGIN(Engine)

class CImgui_Manager final : public CBase
{
private:
    CImgui_Manager();
    virtual ~CImgui_Manager() = default;

public:
    // LRESULT WINAPI      WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HRESULT             Initialize(_uint iWinSizeX, _uint iWinSizeY);
    void                BeginFrame();
    void                Render();
    void                Shutdown();

public:
    void                AddWidget(const function<void()>& widget);
    void                AllClearWidget() { m_vecWidgets.clear(); }

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

    vector<function<void()>> m_vecWidgets;

private:
    bool                 CreateDeviceD3D(HWND hWnd);
    void                 CleanupDeviceD3D();
    void                 CreateRenderTarget();
    void                 CleanupRenderTarget();


public:
    static CImgui_Manager* Create(_uint iWinSizeX, _uint iWinSizeY);
    virtual void Free() override;
};

NS_END