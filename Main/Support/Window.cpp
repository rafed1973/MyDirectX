#include "Window.h"

bool DXWindow::Init()
{
    //Window Class
    WNDCLASSEXW wcex{}; //decalring an instance of WNDCLASSEXW
    wcex.cbSize = sizeof(wcex); //the size in byte of this structure
    wcex.style =CS_OWNDC; //specifies the class style(s). Here we picked own device content
    wcex.lpfnWndProc = &DXWindow::OnWindowMessage; //lpfnWndProc is a pointer to the window procedure function. This function processes messages sent to windows of this class.
    wcex.cbClsExtra = 0; //specifies the number of extra bytes to allocate following the window-class structure. Usually set to zero.
    wcex.cbWndExtra = 0; //specifies the number of extra bytes to allocate following the window instance. Usually set to zero.
    wcex.hInstance = GetModuleHandleW(nullptr); //hInstance is a handle to the instance that contains the window procedure for the class.
    wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION); //hIcon specifies a handle to the class icon. This icon appears in the taskbar when the window is minimized.
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW); //hCursor specifies a handle to the class cursor. This cursor appears when the mouse pointer is over the window.
    wcex.hbrBackground = nullptr; //hbrBackground is a handle to the class background brush. It's used to paint the background of the window.
    wcex.lpszMenuName = nullptr; //lpszMenuName is a pointer to a null-terminated string that specifies the resource name of the class menu, if any.
    wcex.lpszClassName = L"D3D12ExWndCls"; //lpszClassName is a pointer to a null-terminated string or is an atom that specifies the window class name.
    wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION); //hIconSm specifies a handle to a small icon associated with the window class. This icon is used in places where a smaller icon is needed.


    //register the window
    m_wndClass = RegisterClassExW(&wcex);
    if (m_wndClass == 0) 
    { return false; }

    //the actual window
    m_window = CreateWindowExW(
        WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW, 
        LPCWSTR(m_wndClass), L"MyDirectX", 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
        100, 100, 1280, 720, nullptr, nullptr, wcex.hInstance, nullptr);

    if (m_window == nullptr)
    { return false; }

    
    //Describe swap chain
    DXGI_SWAP_CHAIN_DESC1 swd{};
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC sfd{};

    swd.Width = 1280;
    swd.Height = 720;
    swd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swd.Stereo = false;
    swd.SampleDesc.Count = 1;
    swd.SampleDesc.Quality = 0;
    swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swd.BufferCount = GetFrameCount();
    swd.Scaling = DXGI_SCALING_STRETCH;
    swd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    swd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    //When setting windowed to true there is no need to set the sfd descriptors since this is considered always on windowed mode.
    sfd.Windowed = true;

    //Swap chain creation
    auto& factory = DXContext::Get().GetFactory();
    ComPointer<IDXGISwapChain1> sc1;
    
    if(FAILED(factory->CreateSwapChainForHwnd(DXContext::Get().GetCommandQueue(), m_window, &swd, &sfd, nullptr, &sc1)))
    { return false; }
    
    if (!sc1.QueryInterface(m_swapChain))
    { return false; }

    //Get swap chain buffers
    if (!GetBuffers())
    {
        return false;
    }

    return true;

}

void DXWindow::Update()
{
    MSG msg;

    //Peeking on windows message
    while (PeekMessageW(&msg, m_window, 0, 0, PM_REMOVE))
    {
        //std::cout << "updating window \n";
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void DXWindow::Present()
{
    m_swapChain->Present(1, 0);
}

void DXWindow::Shutdown()
{
    ReleaseBuffers();
    
    m_swapChain.Release();

    if (m_window) {
        DestroyWindow(m_window);
    }
    if (m_wndClass) {
        UnregisterClassW(LPCWSTR(m_wndClass), GetModuleHandleW(nullptr));
    }
}

void DXWindow::Resize()
{
    ReleaseBuffers();
    RECT cr;
    if (GetClientRect(m_window, &cr))
    {
        m_width = cr.right - cr.left;
        m_height = cr.bottom - cr.top;

        m_swapChain->ResizeBuffers(GetFrameCount(), m_width, m_height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
        m_shouldResize = false;
    }
    GetBuffers();
}

void DXWindow::SetFullscreen(bool enabled)
{
    //Update the window style
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    DWORD exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
    if (enabled)
    {
        style = WS_POPUP | WS_VISIBLE;
        exStyle = WS_EX_APPWINDOW;
    }
    SetWindowLongW(m_window, GWL_STYLE, style);
    SetWindowLongW(m_window, GWL_EXSTYLE, exStyle);

    //Adjust window size
    if (enabled) {
        HMONITOR monitor = MonitorFromWindow(m_window, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (GetMonitorInfoW(monitor, &monitorInfo))
        {
            SetWindowPos(
                m_window, nullptr,
                monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_NOZORDER
            );
        }
        else 
        {
            ShowWindow(m_window, SW_MAXIMIZE);
        }
    }
    

    m_isFullscreen = enabled;
}

void DXWindow::BeginFrame(ID3D12GraphicsCommandList7* cmdlist)
{
    m_currentBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    
    D3D12_RESOURCE_BARRIER barr;
    cmdlist->ResourceBarrier(1, &barr);
    barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barr.Transition.pResource = m_buffers[m_currentBufferIndex];
    barr.Transition.Subresource = 0;
    barr.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barr.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

}

void DXWindow::EndFrame(ID3D12GraphicsCommandList7* cmdlist)
{
    D3D12_RESOURCE_BARRIER barr;
    cmdlist->ResourceBarrier(1, &barr);
    barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barr.Transition.pResource = m_buffers[m_currentBufferIndex];
    barr.Transition.Subresource = 0; 
    barr.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barr.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
}

bool DXWindow::GetBuffers()
{
    for (size_t i = 0; i < FrameCount; ++i)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i]))))
        {
            return false;
        }
    }
    return true;
}

void DXWindow::ReleaseBuffers()
{
    for (size_t i = 0; i < FrameCount; ++i)
    {
        m_buffers[i].Release();
    }
}

LRESULT DXWindow::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_KEYDOWN:
        if (wParam == VK_F11) {
            Get().SetFullscreen(!Get().IsFullscreen());
        }
    case WM_SIZE:
        //lParam is passed by the wimdow message and it has the height and width encoded
        //we are not doing the resizing if the window is minimized or maximaized
        if (lParam && (HIWORD(lParam) != Get().m_height || LOWORD(lParam) != Get().m_width))
        { 
            Get().m_shouldResize = true; 
        }
        break;
    case WM_CLOSE:
        Get().m_shouldClose = true;
        return 0;
    }
    //this returns the default behavior for any window
    return DefWindowProcW(wnd, msg, wParam, lParam);
}
