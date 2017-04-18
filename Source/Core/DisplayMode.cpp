

#include "DisplayMode.h"

//------------------------------------------------------------------------------------------
DisplayMode::DisplayMode()
    :
    m_adapterIndex(0),
    m_monitorIndex(0),
    m_fullscreen(false),
    m_resolutionFullscreen(Resolution(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN))),
    m_resolutionWindowed(Resolution(200, 200)),
    m_refreshRate(RefreshRate(60000, 1000)),
    m_multisamplingCount(1),
    m_multisamplingQuality(0),
    m_backbufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
{
}

//------------------------------------------------------------------------------------------
DisplayMode::DisplayMode(const unsigned adapterIndex,
                         const unsigned monitorIndex,
                         const bool fullscreen,
                         const Resolution & resolutionFullscreen,
                         const Resolution & resolutionWindowed,
                         const RefreshRate & refreshRate,
                         const unsigned multisamplingCount,
                         const unsigned multisamplingQuality,
                         const DXGI_FORMAT backbufferFormat)
    :
    m_adapterIndex(adapterIndex),
    m_monitorIndex(monitorIndex),
    m_fullscreen(fullscreen),
    m_resolutionFullscreen(resolutionFullscreen),
    m_resolutionWindowed(resolutionWindowed),
    m_refreshRate(refreshRate),
    m_multisamplingCount(multisamplingCount),
    m_multisamplingQuality(multisamplingQuality),
    m_backbufferFormat(backbufferFormat)
{
}

//------------------------------------------------------------------------------------------
DisplayMode::DisplayMode(const DisplayMode & rhs)
    :
    m_adapterIndex(rhs.m_adapterIndex),
    m_monitorIndex(rhs.m_monitorIndex),
    m_fullscreen(rhs.m_fullscreen),
    m_resolutionFullscreen(rhs.m_resolutionFullscreen),
    m_resolutionWindowed(rhs.m_resolutionWindowed),
    m_refreshRate(rhs.m_refreshRate),
    m_multisamplingCount(rhs.m_multisamplingCount),
    m_multisamplingQuality(rhs.m_multisamplingQuality),
    m_backbufferFormat(rhs.m_backbufferFormat)
{
}

//------------------------------------------------------------------------------------------
DisplayMode::~DisplayMode()
{
}

//------------------------------------------------------------------------------------------
DisplayMode & DisplayMode::operator = (const DisplayMode & rhs)
{
    m_adapterIndex         = rhs.m_adapterIndex;
    m_monitorIndex         = rhs.m_monitorIndex;
    m_fullscreen           = rhs.m_fullscreen;
    m_resolutionFullscreen = rhs.m_resolutionFullscreen;
    m_resolutionWindowed   = rhs.m_resolutionWindowed;
    m_refreshRate          = rhs.m_refreshRate;
    m_multisamplingCount   = rhs.m_multisamplingCount;
    m_multisamplingQuality = rhs.m_multisamplingQuality;
    m_backbufferFormat     = rhs.m_backbufferFormat;

    return *this;
}

//------------------------------------------------------------------------------------------
bool DisplayMode::operator == (const DisplayMode & rhs) const
{
    if( this == &rhs )
    {
        return true;
    }

    if( m_adapterIndex != rhs.m_adapterIndex )
    {
        return false;
    }

    if( m_monitorIndex != rhs.m_monitorIndex )
    {
        return false;
    }
   
    if( m_fullscreen != rhs.m_fullscreen )
    {
        return false;
    }
   
    if( m_resolutionFullscreen != rhs.m_resolutionFullscreen )
    {
        return false;
    }

    if( m_resolutionWindowed != rhs.m_resolutionWindowed )
    {
        return false;
    }

    if( m_refreshRate != rhs.m_refreshRate )
    {
        return false;
    }

    if( m_multisamplingCount != rhs.m_multisamplingCount )
    {
        return false;
    }

    if( m_multisamplingQuality != rhs.m_multisamplingQuality )
    {
        return false;
    }

    if( m_backbufferFormat != rhs.m_backbufferFormat )
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
bool DisplayMode::operator != (const DisplayMode & rhs) const
{
    return !(*this == rhs);
}

