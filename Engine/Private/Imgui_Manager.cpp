#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

CImgui_Manager::CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_Manager::Initialize(list<wstring> Menu, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    m_Menu = Menu;
    m_hWnd = hWnd;
    m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

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
    m_io = &(ImGui::GetIO());
    m_io->Fonts->AddFontFromFileTTF("../../Client/Bin/Resources/Font/DNFForgedBlade-Medium.ttf", 18.f, NULL, m_io->Fonts->GetGlyphRangesKorean());
    ImGuiStyle& style = ImGui::GetStyle();
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    m_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);


    IM_ASSERT(m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable);
    IM_ASSERT(m_io->BackendFlags & ImGuiBackendFlags_PlatformHasViewports);
    IM_ASSERT(m_io->BackendFlags & ImGuiBackendFlags_RendererHasViewports);

    return S_OK;
}

void CImgui_Manager::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    Render_Docking();
    Render_Gizmo();
    Render_Widet();
}

void CImgui_Manager::Render()
{
    // Rendering
    if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
        m_isShowTopBar = !m_isShowTopBar;
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	m_io = &(ImGui::GetIO());
    if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        m_pImgRTV = nullptr;
        m_pImgDSV = nullptr;
        m_pContext->OMGetRenderTargets(1, &m_pImgRTV, &m_pImgDSV);

        ImGui::UpdatePlatformWindows();         
        ImGui::RenderPlatformWindowsDefault(); 

        m_pContext->OMSetRenderTargets(1, &m_pImgRTV, m_pImgDSV);
        if (m_pImgRTV) m_pImgRTV->Release();
        if (m_pImgDSV) m_pImgDSV->Release();
    }

    //m_pGameInstance->Present_SwapChain(1, 0);
}

void CImgui_Manager::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

     if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
     ImGui::DestroyPlatformWindows();

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

void CImgui_Manager::Render_Widet()
{
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

void CImgui_Manager::Render_Docking()
{
    // --- DockSpace Host ---
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowViewport(vp->ID);

    ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        //ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoBackground;

    if (m_isShowTopBar)
        hostFlags |= ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

    if (ImGui::Begin("###DockSpaceHost", nullptr, hostFlags))
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockFlags);

        if (m_isShowTopBar && ImGui::BeginMenuBar())
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
            ImGui::EndMenuBar();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar(3);

    ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
}

void CImgui_Manager::Render_Gizmo()
{
    if (m_pGizmoObject == nullptr)
        return;

    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList(vp));
    ImGuizmo::SetRect(vp->Pos.x, vp->Pos.y, vp->Size.x, vp->Size.y);

    float viewM[16];
    XMStoreFloat4x4((XMFLOAT4X4*)viewM, m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
    float projM[16];
    XMStoreFloat4x4((XMFLOAT4X4*)projM, m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

    CTransform* pTransform = dynamic_cast<CTransform*>(m_pGizmoObject->Get_Component(TEXT("Com_Transform")));
    if (!pTransform)
        return;

    XMFLOAT4X4 worldFloat;
    XMStoreFloat4x4(&worldFloat, pTransform->Get_WorldMatrix());
    float objM[16];
    memcpy(objM, &worldFloat, sizeof(float) * 16);

    // 조작 모드 전환
    if (m_pGameInstance->Key_Down(DIK_F1))
        m_GizmoOp = ImGuizmo::TRANSLATE;
    if (m_pGameInstance->Key_Down(DIK_F2))
        m_GizmoOp = ImGuizmo::ROTATE;
    if (m_pGameInstance->Key_Down(DIK_F3))
        m_GizmoOp = ImGuizmo::SCALE;

    // F4/F5 로 모드 전환
    if (m_pGameInstance->Key_Down(DIK_F4))
    {
        if (ImGuizmo::WORLD == m_GizmoMode)
            m_GizmoMode = ImGuizmo::LOCAL;
        else if (ImGuizmo::LOCAL == m_GizmoMode)
            m_GizmoMode = ImGuizmo::WORLD;
    }

    // 스냅 설정
    bool useSnap = false;
    float snap[3] = { 0.25f, 15.0f, 0.1f };
    float* pSnap = nullptr;

    // Shift / Ctrl / Alt 키로 단위 스냅 활성화
    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, 0.0001f))
    {
        useSnap = true;
        snap[0] = 5.0f;     // 이동 5
        snap[1] = 15.0f;    // 회전 15도
        snap[2] = 0.002f;   // 스케일 0.002
    }
    else if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, 0.0001f))
    {
        useSnap = true;
        snap[0] = 1.0f;     // 이동 1
        snap[1] = 5.0f;     // 회전 5도
        snap[2] = 0.001f;   // 스케일 0.001
    }
    else if (m_pGameInstance->Key_Pressing(DIK_LALT, 0.0001f))
    {
        useSnap = true;
        snap[0] = 0.5f;     // 이동 0.5
        snap[1] = 1.0f;     // 회전 1도
        snap[2] = 0.0005f;  // 스케일 0.0005
    }

    if (useSnap)
    {
        if (m_GizmoOp == ImGuizmo::TRANSLATE) pSnap = &snap[0];
        else if (m_GizmoOp == ImGuizmo::ROTATE) pSnap = &snap[1];
        else if (m_GizmoOp == ImGuizmo::SCALE) pSnap = &snap[2];
    }

    ImGuizmo::SetGizmoSizeClipSpace(0.15f * vp->DpiScale);

    bool changed = ImGuizmo::Manipulate(
        viewM,
        projM,
        m_GizmoOp,
        m_GizmoMode,  // F4/F5 로 수동 전환된 모드 사용
        objM,
        nullptr,
        pSnap
    );

    if (changed)
    {
        XMMATRIX newWorld = XMLoadFloat4x4((XMFLOAT4X4*)objM);
        pTransform->Set_WorldMatrix(newWorld);
    }
}

void CImgui_Manager::All_Clean()
{
    for (auto Menu : m_Widgets)
    {
        Menu.second.clear();
    }
}

CImgui_Manager* CImgui_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, list<wstring> Menu, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    CImgui_Manager* pInstance = new CImgui_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize(Menu, hWnd, iWinSizeX, iWinSizeY)))
    {
        MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CImgui_Manager::Free()
{
    __super::Free();

    Shutdown();

    for (auto Menu : m_Widgets)
    {
		Menu.second.clear();
    }

    /*if (m_pImgRTV) m_pImgRTV->Release();
    if (m_pImgDSV) m_pImgDSV->Release();*/
	if (m_io) { m_io = nullptr; }

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
