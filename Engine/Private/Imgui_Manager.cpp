#include "Imgui_Manager.h"
#include "GameInstance.h"
CImgui_Manager::CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_Manager::Initialize(list<wstring> Menu, HWND hWnd)
{
    m_Menu = Menu;
    m_hWnd = hWnd;


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

    // Show the window
    //::ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    //::UpdateWindow(m_hWnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);
    

    IM_ASSERT(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable);
    IM_ASSERT(io.BackendFlags & ImGuiBackendFlags_PlatformHasViewports);
    IM_ASSERT(io.BackendFlags & ImGuiBackendFlags_RendererHasViewports);

    return S_OK;
}

void CImgui_Manager::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // --- DockSpace Host ---
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowViewport(vp->ID);

    ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::Begin("###DockSpaceHost", nullptr, hostFlags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode |
        ImGuiDockNodeFlags_NoDockingInCentralNode;

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

    ImGui::End();

    ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags testFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking;

    

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
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ID3D11RenderTargetView* backup_rtv = nullptr;
        ID3D11DepthStencilView* backup_dsv = nullptr;
        m_pContext->OMGetRenderTargets(1, &backup_rtv, &backup_dsv);

        ImGui::UpdatePlatformWindows();         
        ImGui::RenderPlatformWindowsDefault(); 

        m_pContext->OMSetRenderTargets(1, &backup_rtv, backup_dsv);
        if (backup_rtv) backup_rtv->Release();
        if (backup_dsv) backup_dsv->Release();
    }

    m_pGameInstance->Present_SwapChain(1, 0);
}

void CImgui_Manager::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

HRESULT CImgui_Manager::CleanMenu(_wstring strMenu)
{
    auto iter = m_Widgets.find(strMenu);

    if (iter == m_Widgets.end())
        return E_FAIL;

    iter->second.clear();

    return true;
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParm, LPARAM lParm);

_bool CImgui_Manager::HandleWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

CImgui_Manager* CImgui_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, list<wstring> Menu, HWND hWnd)
{
    CImgui_Manager* pInstance = new CImgui_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize(Menu, hWnd)))
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

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
