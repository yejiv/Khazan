#include "EnginePch.h"
#include "Imgui_Manager.h"

extern LRESULT WINAPI ImGui_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CImgui_Manager::CImgui_Manager()
{
}

HRESULT CImgui_Manager::Initialize(_uint iWinSizeX, _uint iWinSizeY, list<wstring> Menu)
{

#if  defined(_CLIENT) && !defined(_EDITOR)
    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
#endif // !_CLIENT

    m_Menu = Menu;

	for (auto Menu : m_Menu)
	{
		m_Widgets[Menu] = vector<function<void()>>();
	}

	_uint MenuIndex = 0;
	for (auto Menu : m_Menu)
	{
        if (MenuIndex == 0)
		    m_MenuOpen[Menu] = true;
        else
            m_MenuOpen[Menu] = false;

		++MenuIndex;
	}

    // Create application window
    wc = { sizeof(wc), CS_CLASSDC, ImGui_WndProcHandler, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, iWinSizeX, iWinSizeY, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

#if  defined(_CLIENT) && !defined(_EDITOR)
   // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
#endif // !_CLIENT


    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    return S_OK;
}

void CImgui_Manager::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    //ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    if (ImGui::BeginMainMenuBar())
    {
        for (auto& widget : m_Widgets)
        {
            _wstring OpenMenu = Get_OpenMenu_Name();
            if (ImGui::BeginMenu(WStringToAnsi(OpenMenu).c_str()))
            {
                ImGui::MenuItem(WStringToAnsi(widget.first).c_str(), nullptr, &m_MenuOpen[widget.first]);
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }


	for (auto& Menu : m_Widgets)
	{
		if (m_MenuOpen[Menu.first])
		{
            for (auto& Widget : Menu.second)
            {
                Widget();
            }
		}
	}
}

void CImgui_Manager::Render()
{
    // Rendering
    ImGui::Render();
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present
    HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
    g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
}

void CImgui_Manager::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

void CImgui_Manager::AddWidget(const _wstring Menu, const function<void()>& widget)
{
	m_Widgets[Menu].push_back(widget);
}

_wstring CImgui_Manager::Get_OpenMenu_Name()
{
    for (auto Menu : m_MenuOpen)
    {
		if (Menu.second)
			return Menu.first;
    }

    return TEXT("Menu");
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI ImGui_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;

        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool CImgui_Manager::CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CImgui_Manager::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CImgui_Manager::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CImgui_Manager::CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

CImgui_Manager* CImgui_Manager::Create(_uint iWinSizeX, _uint iWinSizeY, list<wstring> Menu)
{
    CImgui_Manager* pInstance = new CImgui_Manager();

    if (FAILED(pInstance->Initialize(iWinSizeX, iWinSizeY, Menu)))
    {
        MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CImgui_Manager::Free()
{
    __super::Free();

    for (auto Menu : m_Widgets)
    {
		Menu.second.clear();
    }
}
