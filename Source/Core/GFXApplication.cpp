
// Project Includes
#include "GFXApplication.h"

// Common Lib Includes
#include "Exception.h"
//#include "Timer.h"

// OS Includes
#include <windows.h>

//-----------------------------------------------------------------------------
// GFXApplication
//-----------------------------------------------------------------------------
GFXApplication::GFXApplication(const std::string & title)
    :
    BaseWindow          (),
    m_dxgiFactory       (nullptr),
    m_device            (nullptr),
    m_swapChain         (nullptr),
    m_backBuffer        (nullptr),
    m_renderTargetView  (nullptr),
    m_depthStencil      (nullptr),
    m_depthStencilState (nullptr),
    m_depthStencilView  (nullptr),
    m_rasterizerState   (nullptr),
    m_ignoreSizeChange  (false),
    m_textureManager    (nullptr),
    m_effectManager     (nullptr),
    m_inputLayoutManager(nullptr),
    m_renderQueue       (nullptr)
{
    m_className = L"GFXApplication";
    m_title     = L"D3D Application";

    // Initialize keyboard input states
    std::fill(m_keystates, m_keystates + 256, false);
}

//-----------------------------------------------------------------------------
GFXApplication::~GFXApplication()
{
    // Release resources
    FreeResources();

    // Set the device to the state is was in when it was created
    if( m_device )
    {
        m_device->ClearState();
    }

    // Release the Render Queue
    if( m_renderQueue )
    {
        delete m_renderQueue;
    }

    // Release the Input Layout Manager
    if( m_inputLayoutManager )
    {
        delete m_inputLayoutManager;
    }

    // Release the Effect Manager
    if( m_effectManager )
    {
        delete m_effectManager;
        m_effectManager = NULL;
    }

    // Release the Texture Manager
    if( m_textureManager )
    {
        delete m_textureManager;
        m_textureManager = NULL;
    }

    // Release the rasterizer state
    if( m_rasterizerState )
    {
        m_rasterizerState->Release();
        m_rasterizerState = NULL;
    }

    // Release the depth stencil view
    if( m_depthStencilView )
    {
        m_depthStencilView->Release();
        m_depthStencilView = NULL;
    }

    // Release the depth stencil state
    if( m_depthStencilState )
    {
        m_depthStencilState->Release();
        m_depthStencilState = NULL;
    }

    // Release the depth stencil texture
    if( m_depthStencil )
    {
        m_depthStencil->Release();
        m_depthStencil = NULL;
    }

    // Release the render target view
    if( m_renderTargetView )
    {
        m_renderTargetView->Release();
        m_renderTargetView = NULL;
    }

    // Release the back buffer
    if( m_backBuffer )
    {
        m_backBuffer->Release();
        m_backBuffer = NULL;
    }

    // Release the swap chain
    if( m_swapChain )
    {
        m_swapChain->Release();
        m_swapChain = NULL;
    }
   
    // Release the D3D device
    // boost::unique_ptr releases the device for us

    // Release the DXGI factory
    if( m_dxgiFactory )
    {
        m_dxgiFactory->Release();
        m_dxgiFactory = NULL;
    }
}

//-----------------------------------------------------------------------------
void GFXApplication::Init(HINSTANCE instance, 
                          const DisplayMode & displayMode,
                          const std::string & textureDirectory,
                          const std::string & effectDirectory,
                          const std::string & effectPoolFileName)
{
    // Validate the display mode
    if( !m_displayModeEnumerator.IsValidDisplayMode(displayMode) )
    {
        const std::string msg("Invalid display mode");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    m_displayMode = displayMode;

    // Set the window's style, initial position, width and height
    m_style = WS_OVERLAPPEDWINDOW;
   
    RECT desktop = m_displayModeEnumerator.GetDesktopCoords(m_displayMode.m_adapterIndex, m_displayMode.m_monitorIndex);

    RECT windowBounds;
    windowBounds.left   = desktop.left;
    windowBounds.top    = desktop.top;
    windowBounds.right  = windowBounds.left + displayMode.m_resolutionWindowed.m_width;
    windowBounds.bottom = windowBounds.top  + displayMode.m_resolutionWindowed.m_height;

    // Adjust the window rectangle such that the client area will be the desired width and height
    AdjustWindowRectEx(&windowBounds, m_style, false, m_extendedStyle);

    // Center the window rectangle in the desktop it will be displayed on
    unsigned desktopWidth  = desktop.right  - desktop.left;
    unsigned desktopHeight = desktop.bottom - desktop.top;
    unsigned windowWidth   = windowBounds.right  - windowBounds.left;
    unsigned windowHeight  = windowBounds.bottom - windowBounds.top;
   
    windowBounds.left   = desktopWidth  / 2 - windowWidth  / 2 + desktop.left;
    windowBounds.top    = desktopHeight / 2 - windowHeight / 2 + desktop.top;
    windowBounds.right  = windowBounds.left + windowWidth;
    windowBounds.bottom = windowBounds.top  + windowHeight;

    // Ignore buffer resizing from WM_SIZE messages until done
    m_ignoreSizeChange = true;

    // Create the window
    try
    {
        BaseWindow::Init(instance,
                        NULL,
                        NULL,
                        windowBounds.left,
                        windowBounds.top,
                        windowBounds.right  - windowBounds.left,
                        windowBounds.bottom - windowBounds.top);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    ShowWindow(m_hwnd, SW_SHOW);

    // Create a DXGI Factory
    if( FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)(&m_dxgiFactory))) )
    {
        const std::string msg("Failed to create DXGI factory");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Let DXGI manage alt-enter transitions from full screen to windowed and back
    if( FAILED(m_dxgiFactory->MakeWindowAssociation(m_hwnd, 0)) )
    {
        const std::string msg("Failed to make window association with DXGI");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Create a D3D device
    try
    {
        m_displayModeEnumerator.CreateDevice(m_displayMode.m_adapterIndex, m_device);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Create the swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
   
    swapChainDesc.BufferCount                         = 2;
    swapChainDesc.BufferDesc.Format                   = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator    = m_displayMode.m_refreshRate.m_numerator;
    swapChainDesc.BufferDesc.RefreshRate.Denominator  = m_displayMode.m_refreshRate.m_denominator;
    swapChainDesc.BufferUsage                         = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow                        = m_hwnd;
    swapChainDesc.SwapEffect                          = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.SampleDesc.Count                    = m_displayMode.m_multisamplingCount;
    swapChainDesc.SampleDesc.Quality                  = m_displayMode.m_multisamplingQuality;

    if( m_displayMode.m_fullscreen )
    {
        swapChainDesc.BufferDesc.Width   = m_displayMode.m_resolutionFullscreen.m_width;
        swapChainDesc.BufferDesc.Height  = m_displayMode.m_resolutionFullscreen.m_height;
        swapChainDesc.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swapChainDesc.Windowed           = false;
    }
    else
    {
        swapChainDesc.BufferDesc.Width   = m_displayMode.m_resolutionWindowed.m_width;
        swapChainDesc.BufferDesc.Height  = m_displayMode.m_resolutionWindowed.m_height;
        swapChainDesc.Flags              = 0;
        swapChainDesc.Windowed           = true;
    }

    if( FAILED(m_dxgiFactory->CreateSwapChain(m_device.get(), &swapChainDesc, &m_swapChain)) )
    {
        const std::string msg("Failed to create swapchain");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Setup the viewport
    D3D10_VIEWPORT viewport;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    if( m_displayMode.m_fullscreen )
    {
        viewport.Width    = m_displayMode.m_resolutionFullscreen.m_width;
        viewport.Height   = m_displayMode.m_resolutionFullscreen.m_height;
    }
    else
    {
        viewport.Width    = m_displayMode.m_resolutionWindowed.m_width;
        viewport.Height   = m_displayMode.m_resolutionWindowed.m_height;
    }

    m_device->RSSetViewports(1, &viewport);

    // Setup a scissor rectangle
    D3D10_RECT scissor;
    scissor.left   = 0;
    scissor.top    = 0;

    if( m_displayMode.m_fullscreen )
    {
        scissor.right    = m_displayMode.m_resolutionFullscreen.m_width;
        scissor.bottom   = m_displayMode.m_resolutionFullscreen.m_height;
    }
    else
    {
        scissor.right    = m_displayMode.m_resolutionWindowed.m_width;
        scissor.bottom   = m_displayMode.m_resolutionWindowed.m_height;
    }

    m_device->RSSetScissorRects(1, &scissor);

    // Create the depth stencil state
    D3D10_DEPTH_STENCILOP_DESC depthStencilOpDesc;
   
    depthStencilOpDesc.StencilFailOp      = D3D10_STENCIL_OP_KEEP;
    depthStencilOpDesc.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
    depthStencilOpDesc.StencilPassOp      = D3D10_STENCIL_OP_KEEP;
    depthStencilOpDesc.StencilFunc        = D3D10_COMPARISON_ALWAYS;

    D3D10_DEPTH_STENCIL_DESC   depthStencilDesc;

    depthStencilDesc.DepthEnable      = TRUE;
    depthStencilDesc.DepthWriteMask   = D3D10_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc        = D3D10_COMPARISON_LESS;
    depthStencilDesc.StencilEnable    = FALSE;
    depthStencilDesc.StencilReadMask  = D3D10_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D10_DEFAULT_STENCIL_WRITE_MASK;
    depthStencilDesc.FrontFace        = depthStencilOpDesc;
    depthStencilDesc.BackFace         = depthStencilOpDesc;

    if( FAILED(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)) )
    {
        const std::string msg("Failed to create D3D depth stencil state");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    m_device->OMSetDepthStencilState(m_depthStencilState, 0);

    // Set rasterizer state
    D3D10_RASTERIZER_DESC rasterizerDesc;
    rasterizerDesc.FillMode              = D3D10_FILL_SOLID;
    rasterizerDesc.CullMode              = D3D10_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias             = 0;
    rasterizerDesc.DepthBiasClamp        = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    rasterizerDesc.DepthClipEnable       = TRUE;
    rasterizerDesc.ScissorEnable         = TRUE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;

    if( m_displayMode.m_multisamplingCount > 1 )
    {
        rasterizerDesc.MultisampleEnable = TRUE;
    }
    else
    {
        rasterizerDesc.MultisampleEnable = FALSE;
    }

    if( FAILED(m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState)) )
    {
        const std::string msg("Failed to create rasterizer state");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    m_device->RSSetState(m_rasterizerState);

    // Let the rest of the DX initialization occur in HandleSizeChanges()
    m_ignoreSizeChange = false;
   
    GetClientRect(m_hwnd, &windowBounds);

    try
    {
        HandleSizeChange(windowBounds.right - windowBounds.left, windowBounds.bottom - windowBounds.top);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Create a texture manager
    m_textureManager = new TextureManager(*m_device, textureDirectory);

    // Create the default effect pool
    try
    {
        m_effectManager = new EffectManager(*m_device, *m_textureManager, effectDirectory, effectPoolFileName);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }
   
    // Create an input layout manager
    m_inputLayoutManager = new InputLayoutManager(*m_device);

    // Create the render queue
    m_renderQueue = new RenderQueue(*m_effectManager);
}

//-----------------------------------------------------------------------------
int GFXApplication::Run()
{
    // Initialize graphic data
    try
    {
        InitResources();
    }
    catch(Common::Exception & e)
    {
        // We must be in windowed mode before exiting
        m_swapChain->SetFullscreenState(FALSE, NULL);

        throw e;
    }
    
    // Start the GFXApp timer
    m_timer.Start();

    // Begin the main loop
    MSG msg;                // Windows message

    while(true)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                break;
            }

            TranslateMessage(&msg);		// Translate any accelerator keys
            DispatchMessage(&msg);		// Send the message to event handler
        }

        try
        {
            // Any processing that must take place previous to render this frame
            PreRender();
         
            // Main processing loop
            if(m_active)
            {
                // Render
                RenderLoop();
            }

            // Any processing that must take place after rendering this frame
            PostRender();

            // Update the timer
            m_timer.SetUpdateTime();
        }
        catch(Common::Exception & e)
        {
            // We must be in windowed mode before exiting
            m_swapChain->SetFullscreenState(FALSE, NULL);

            throw e;
        }
    }

    // We must be in windowed mode before exiting
    m_swapChain->SetFullscreenState(FALSE, NULL);

    // Release Resources
    FreeResources();

    // Return to windows
    return(static_cast<int>(msg.wParam));
}


//-------------------------------------------------------------------------------
double GFXApplication::GetTotalTime() const
{
    return m_timer.GetTotalTime();
}

//-------------------------------------------------------------------------------
double GFXApplication::GetLast5AvgTime() const
{
    return m_timer.GetLast5AvgTime();
}

//-------------------------------------------------------------------------------
double GFXApplication::GetElapsedTime() const
{
    return m_timer.GetElapsedTime();
}

//-------------------------------------------------------------------------------
LRESULT GFXApplication::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool & callDefWndProc)
{
    // Flag that signifies whether or not message needs to be handed back to the default windows
    // procedure when this method returns
    callDefWndProc = true;

    switch(message)
    {
        case WM_SYSCOMMAND:
        {
            switch(wparam)
            {
                // Ignore stand-by messages
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                {
                    callDefWndProc = false;
                    return 0;
                }
            }

            break;
        }

        case WM_ACTIVATE:
        {
            // If he window is not active stop rendering
            if(WA_INACTIVE == wparam)
            {
                m_active = false;
            }
            
            // If the window is being activated start rendering again
            else
            {
                m_active = true;
            }

            callDefWndProc = false;
            return 0;
        }

        case WM_ENTERSIZEMOVE:
        {
            // Stop rendering while the window is being sized or moved
            m_active = false;
            m_sizing = true;

            callDefWndProc = false;
            return 0;
        }
        
        case WM_SIZE:
        {
            unsigned clientWidth  = LOWORD(lparam);
            unsigned clientHeight = HIWORD(lparam);

            // I know this seems silly after doing the above, but the actual top and botton are needed
            // for the first "else-if" case
            RECT clientRect;
            GetClientRect(m_hwnd, &clientRect);

            // Check if the window has become minimized
            if( wparam == SIZE_MINIMIZED )
            {
                // Stop rendering
                m_active    = false;
                m_minimized = true;
                m_maximized = false;
            }
         
            // Rapid clicking the taskbar to minimize and restore can cause a WM_SIZE message with SIZE_RESTORED
            // when the window actually has become minimized due to rapid changing. Ignore this case
            else if( clientRect.top == 0 && clientRect.bottom == 0 )
            {
            }

            // Check if the window has become maximized 
            else if( wparam == SIZE_MAXIMIZED )
            {
                m_active    = true;
                m_minimized = false;
                m_maximized = true;

                HandleSizeChange(clientWidth, clientHeight);
            }

            // Check if the window has become restored
            else if( wparam == SIZE_RESTORED )
            {
                // If coming from a maximized state
                if( m_maximized )
                {
                    m_maximized = false;

                    HandleSizeChange(clientWidth, clientHeight);
                }

                // If coming from a minimized state
                else if( m_minimized )
                {
                    m_active    = true;
                    m_minimized = false;

                    HandleSizeChange(clientWidth, clientHeight);
                }

                // Check if the window is being sized via a mouse drag
                else if( m_sizing )
                {
                    // Don't do anything. Instead, wait until a WM_EXITSIZEMOVE message comes.
                }

                // Otherwise, the window is being sized via an API call such as SetWindowPos
                else
                {
                    HandleSizeChange(clientWidth, clientHeight);
                }
            }

            callDefWndProc = false;
            return 0;
        }

        case WM_EXITSIZEMOVE:
        {	
            // Handle any possible size changes
            m_active = true;
            m_sizing = false;

            RECT clientRect;
            GetClientRect(m_hwnd, &clientRect);

            HandleSizeChange(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
            
            callDefWndProc = false;
            return 0;
        }

        case WM_KEYDOWN:
        {
            UINT_PTR key = wparam; 
            m_keystates[key] = true;

            break;
        }
        case WM_KEYUP:
        {
            UINT_PTR key = wparam; 
            m_keystates[key] = false;

            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);

            callDefWndProc = false;
            return 0;
        }

        default:
        {
            break;
        }
    }

    return 0;
}

//--------------------------------------------------------------------------------
void GFXApplication::HandleSizeChange(const unsigned clientWidth, const unsigned clientHeight)
{
    // Check if we should skip buffer resizing
    if( m_ignoreSizeChange )
    {
        return;
    }

    // Check whether we are transitioning to fullscreen or to windowed mode
    BOOL          fullscreen;
    IDXGIOutput * monitor;

    m_swapChain->GetFullscreenState(&fullscreen, &monitor);

    if( m_displayMode.m_fullscreen != (fullscreen != 0) )
    {
        m_displayMode.m_fullscreen = (fullscreen != 0);
    }

    // Check if a resize is needed
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    m_swapChain->GetDesc(&swapChainDesc);

    if( swapChainDesc.BufferDesc.Width  != clientWidth ||
        swapChainDesc.BufferDesc.Height != clientHeight )
    {
        // Release the backbuffer and its dependant interfaces
        if( m_depthStencilView )
        {
            m_depthStencilView->Release();
            m_depthStencilView = NULL;
        }

        if( m_depthStencil )
        {
            m_depthStencil->Release();
            m_depthStencil = NULL;
        }

        if( m_renderTargetView )
        {
            m_renderTargetView->Release();
            m_renderTargetView = NULL;
        }

        if( m_backBuffer )
        {
            m_backBuffer->Release();
            m_backBuffer = NULL;
        }

        if( m_displayMode.m_fullscreen )
        {
            swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        }
        else
        {
            swapChainDesc.Flags = 0;
        }

        // Resize the swap chain buffers to match the new window client area
        if( FAILED(m_swapChain->ResizeBuffers(swapChainDesc.BufferCount,
                                            clientWidth,
                                            clientHeight,
                                            swapChainDesc.BufferDesc.Format,
                                            swapChainDesc.Flags)) )
        {
            const std::string msg("Failed to resize swap chain buffers");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        // Get the new swap chain description
        m_swapChain->GetDesc(&swapChainDesc);

        // Resize the viewport
        D3D10_VIEWPORT viewport;
        viewport.Width    = clientWidth;
        viewport.Height   = clientHeight;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;

        m_device->RSSetViewports(1, &viewport);

        // Resize the scissor rectangle
        D3D10_RECT scissor;
        scissor.left   = 0;
        scissor.top    = 0;
        scissor.right  = clientWidth;
        scissor.bottom = clientHeight;

        m_device->RSSetScissorRects(1, &scissor);
    }

    // If a back buffer is needed, then get it
    if( !m_backBuffer )
    {
        if( FAILED(m_swapChain->GetBuffer(0,__uuidof(ID3D10Texture2D),(LPVOID *)&m_backBuffer)) )
        {
            const std::string msg("Failed to get back buffer for creation of D3D render target view");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }

    // If a render target view is needed, then create it
    if( !m_renderTargetView )
    {
        if( FAILED(m_device->CreateRenderTargetView(m_backBuffer, NULL, &m_renderTargetView)) )
        {
            const std::string msg("Failed to create D3D render target view");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }
   
        // Create a depth stencil texture
        D3D10_TEXTURE2D_DESC descDepth;
        descDepth.Width              = clientWidth;
        descDepth.Height             = clientHeight;
        descDepth.MipLevels          = 1;
        descDepth.ArraySize          = 1;
        descDepth.Format             = DXGI_FORMAT_D32_FLOAT;
        descDepth.SampleDesc.Count   = swapChainDesc.SampleDesc.Count;
        descDepth.SampleDesc.Quality = swapChainDesc.SampleDesc.Quality;
        descDepth.Usage              = D3D10_USAGE_DEFAULT;
        descDepth.BindFlags          = D3D10_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags     = 0;
        descDepth.MiscFlags          = 0;

        if( FAILED(m_device->CreateTexture2D(&descDepth, NULL, &m_depthStencil)) )
        {
            const std::string msg("Failed to create D3D depth stencil texture");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        // Create a depth stencil view
        D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
        descDSV.Format             = descDepth.Format;
        descDSV.Texture2D.MipSlice = 0;

        if( descDepth.SampleDesc.Count > 1 )
        {
            descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DMS;
        }
        else
        {
            descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
        }

        if( FAILED(m_device->CreateDepthStencilView(m_depthStencil, &descDSV, &m_depthStencilView)) )
        {
            const std::string msg("Failed to create D3D depth stencil view");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
    }

    // Notify derived classes of the size change
    OnSizeChange(clientWidth, clientHeight);
}

//--------------------------------------------------------------------------------
void GFXApplication::RenderLoop()
{      
    // Clear the backbuffer
    float ClearColor[4] = { 0.0f, 0.125f, 0.6f, 1.0f }; // RGBA
    m_device->ClearRenderTargetView(m_renderTargetView, ClearColor);

    // Clear the depth buffer to 1.0 (max depth)
    m_device->ClearDepthStencilView(m_depthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

    // Render
    Render();

    // Show the frame
    m_swapChain->Present(1, 0);
}

