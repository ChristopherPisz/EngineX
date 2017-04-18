

#include "DisplayModeEnumerator.h"

// Common Lib Includes
#include "Exception.h"
#include "StringUtility.h"

// Standard Includes
#include <algorithm>
#include <string>

using namespace Common;

//------------------------------------------------------------------------------------------
DisplayModeEnumerator::DisplayModeEnumerator()
{
    try
    {
        Enumerate();
    }
    catch(Common::Exception & e)
    {
        // Nothing to deallocate
        throw e;
    }
}

//------------------------------------------------------------------------------------------
DisplayModeEnumerator::~DisplayModeEnumerator()
{
}

//------------------------------------------------------------------------------------------
const unsigned DisplayModeEnumerator::GetNumAdapters() const
{
    return static_cast<unsigned>(m_adapterNames.size());
}

//------------------------------------------------------------------------------------------
const std::string DisplayModeEnumerator::GetAdapterName(const unsigned adapterIndex) const
{
    // Check for valid index
    if( adapterIndex < 0 || adapterIndex >= m_adapterNames.size() )
    {
        const std::string msg("Requested adapter index does not exist");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
   
    // Return the adapter name
    return m_adapterNames[adapterIndex];
}

//------------------------------------------------------------------------------------------
void DisplayModeEnumerator::GetMultisamplingOptions(const unsigned adapterIndex, 
                                                    std::vector<std::pair<unsigned, unsigned> > & multisamplingOptions) const
{
    // Start with an empty vector
    multisamplingOptions.clear();

    // Check for valid index
    if( adapterIndex < 0 || adapterIndex >= m_adapterNames.size() )
    {
        const std::string msg("Requested adapter index does not exist");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Copy the multisampling options to the provided vector
    multisamplingOptions = m_multisamplingCapsByAdapter[adapterIndex];
}

//------------------------------------------------------------------------------------------
const unsigned DisplayModeEnumerator::GetNumMonitors(const unsigned adapterIndex) const
{
    // Check for valid index
    if( adapterIndex < 0 || adapterIndex >= m_numMonitors.size() )
    {
        const std::string msg("Requested adapter index does not exist");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    return m_numMonitors[adapterIndex];
}

//------------------------------------------------------------------------------------------
void DisplayModeEnumerator::GetResolutions(const unsigned adapterIndex, 
                                           const unsigned monitorIndex,
                                           std::vector<Resolution> & resolutions) const
{
    // Start with an empty vector
    resolutions.clear();

    // Check for a valid key
    Resolutions::const_iterator it = m_resolutions.end();

    if( !m_resolutions.empty() )
    {
        // Get the resolutions
        AdapterMonitorCombo adapterMonitorCombo(adapterIndex, monitorIndex);

        it = m_resolutions.find(adapterMonitorCombo);
    }

    if( it == m_resolutions.end() )
    {
        const std::string msg("The requested adapter and monitor does not support any resolution with the required R8G8B8A8_UNORM format");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Copy the resolutions to the provided vector
    resolutions = it->second;
}

//------------------------------------------------------------------------------------------
void DisplayModeEnumerator::GetRefreshRates(const unsigned adapterIndex, 
                                            const unsigned monitorIndex, 
                                            const Resolution & resolution,
                                            std::vector<RefreshRate> & refreshRates) const
{
    // Start with an empty vector
    refreshRates.clear();

    // Check for a valid key
    RefreshRates::const_iterator it = m_refreshRates.end();

    if( !m_refreshRates.empty() )
    {
        // Get the refresh rates
        AdapterMonitorCombo adapterMonitorCombo(adapterIndex, monitorIndex);
        AdapterMonitorResolutionCombo adapterMonitorResolutionCombo(adapterMonitorCombo, resolution);

        it = m_refreshRates.find(adapterMonitorResolutionCombo);
    }

    if( it == m_refreshRates.end() )
    {
        const std::string msg("Could not find any valid refresh rates for the requested adapter, monitor and resolution");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Copy the refresh rates to the provided vector
    refreshRates = it->second;
}

//------------------------------------------------------------------------------------------
const RECT DisplayModeEnumerator::GetDesktopCoords(const unsigned adapterIndex, const unsigned monitorIndex) const
{
    DesktopCoords::const_iterator it = m_desktopCoords.end();

    if( !m_desktopCoords.empty() )
    {
        AdapterMonitorCombo adapterMonitorCombo(adapterIndex, monitorIndex);
        it = m_desktopCoords.find(adapterMonitorCombo);
    }

    if( it == m_desktopCoords.end() )
    {
        const std::string msg("The requested adapter and monitor combination does was not reported to have a desktop display");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    return (it->second);
}

//------------------------------------------------------------------------------------------
const Resolution DisplayModeEnumerator::GetMaxWindowedResolution(const unsigned adapterIndex, const unsigned monitorIndex) const
{
    // Get the desktop resolution
    RECT rc;

    try
    {
        rc = GetDesktopCoords(adapterIndex, monitorIndex);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    Resolution desktop(rc.right - rc.left, rc.bottom - rc.top);

    // Get the non client area metrics
    unsigned extraWidth  = GetSystemMetrics(SM_CXSIZEFRAME) * 2;
    unsigned extraHeight = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME) * 2;  

    Resolution windowed(desktop.m_width - extraWidth, desktop.m_height - extraHeight);

    return windowed;
}

//------------------------------------------------------------------------------------------
const bool DisplayModeEnumerator::IsValidDisplayMode(const DisplayMode & displayMode) const
{
    // Check for valid adapter
    if( displayMode.m_adapterIndex < 0 ||
        displayMode.m_adapterIndex >= m_adapterNames.size() )
    {
        return false;
    }

    // Check for valid monitor
    if( displayMode.m_monitorIndex < 0 ||
        displayMode.m_monitorIndex >= m_numMonitors[displayMode.m_adapterIndex] )
    {
        return false;
    }

    // Check for a valid fullscreen resolution
    AdapterMonitorCombo adapterMonitorCombo(displayMode.m_adapterIndex, displayMode.m_monitorIndex);
    Resolutions::const_iterator itResolutionVecs = m_resolutions.end();
   
    if( !m_resolutions.empty() )
    {
        itResolutionVecs = m_resolutions.find(adapterMonitorCombo);
    }

    if( itResolutionVecs == m_resolutions.end() )
    {
        return false;
    }

    std::vector<Resolution>::const_iterator itResolutions = itResolutionVecs->second.end();

    if( !itResolutionVecs->second.empty() )
    {
        itResolutions = std::find(itResolutionVecs->second.begin(), itResolutionVecs->second.end(), displayMode.m_resolutionFullscreen);
    }

    if( itResolutions == itResolutionVecs->second.end() )
    {
        return false;
    }

    // Check for a valid windowed resolution
    try
    {
        const Resolution & windowed = GetMaxWindowedResolution(displayMode.m_adapterIndex, displayMode.m_monitorIndex);
      
        if( displayMode.m_resolutionWindowed > windowed )
        {
            return false;
        }
    }
    catch(Common::Exception &)
    {
        return false;
    }

    // Check for a valid refresh rate
    AdapterMonitorResolutionCombo adapterMonitorResolutionCombo(adapterMonitorCombo, displayMode.m_resolutionFullscreen);
    RefreshRates::const_iterator itRefreshRateVecs = m_refreshRates.end();
   
    if( !m_refreshRates.empty() )
    {
        itRefreshRateVecs = m_refreshRates.find(adapterMonitorResolutionCombo);
    }

    if( itRefreshRateVecs == m_refreshRates.end() )
    {
        return false;
    }

    std::vector<RefreshRate>::const_iterator itRefreshRate = itRefreshRateVecs->second.end();

    if( !itRefreshRateVecs->second.empty() )
    {
        itRefreshRate = std::find(itRefreshRateVecs->second.begin(), itRefreshRateVecs->second.end(), displayMode.m_refreshRate);
    }

    if( itRefreshRate == itRefreshRateVecs->second.end() )
    {
        return false;
    }

    // Check for a valid multisampling option
    MultisampleOption multisampleSetting(displayMode.m_multisamplingCount, displayMode.m_multisamplingQuality);

    MultisamplingCaps::const_iterator itMultisample = m_multisamplingCapsByAdapter[displayMode.m_adapterIndex].end();
   
    if( !m_multisamplingCapsByAdapter[displayMode.m_adapterIndex].empty() )
    {
        itMultisample = std::find_if(m_multisamplingCapsByAdapter[displayMode.m_adapterIndex].begin(),
                                    m_multisamplingCapsByAdapter[displayMode.m_adapterIndex].end(),
                                    IsMultisampleSettingEqual(multisampleSetting));
    }

    if( itMultisample == m_multisamplingCapsByAdapter[displayMode.m_adapterIndex].end() )
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
void DisplayModeEnumerator::CreateDevice(const unsigned adapterIndex, ID3D10Device_UniquePtr & ptr)
{
    // Create a DXGI Factory
    IDXGIFactory * dxgi = NULL;
   
    if( FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)(&dxgi))) )
    {
        const std::string msg("Failed to create DXGI factory");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Get the adapter
    IDXGIAdapter * adapter = NULL;
   
    if( DXGI_ERROR_NOT_FOUND == dxgi->EnumAdapters(adapterIndex, &adapter) )
    {
        const std::string msg("Failed to get adapter from DXGI factory");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Create the D3D device
    try
    {
        CreateDevice(adapter, ptr);
    }
    catch(Common::Exception & e)
    {
        adapter->Release();
        dxgi->Release();
      
        throw e;
    }

    // Release the factory and adapter
    adapter->Release();
    dxgi->Release();
}

//------------------------------------------------------------------------------------------
void DisplayModeEnumerator::Enumerate()
{
    // Create a DXGI Factory
    IDXGIFactory * dxgi;
   
    if( FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory),(void**)(&dxgi))) )
    {
        dxgi->Release();

        const std::string msg("Failed to create DXGI factory");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // For each adapter, get all the display modes
    IDXGIAdapter * adapter       = NULL;
    unsigned       adapterIndex  = 0;
   
    while(dxgi->EnumAdapters(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND) 
    {
        // Get a description of this adapter
        DXGI_ADAPTER_DESC adapterDesc;
        adapter->GetDesc(&adapterDesc);

        std::string adapterName = Common::WideToMultibyte(std::wstring(adapterDesc.Description));
        m_adapterNames.push_back(adapterName);
      
        // Only interested in display modes that meet following criteria
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
        unsigned    flags  = DXGI_ENUM_MODES_INTERLACED;

        // Get the multisampling capabilities
        MultisamplingCaps      multisamplingCaps;
        unsigned               maxSampleCount     = D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT;
      
        ID3D10Device_UniquePtr device;
      
        try
        {
            CreateDevice(adapter, device);
        }
        catch(Common::Exception & e)
        {
            adapter->Release();
            dxgi->Release();

            throw e;
        }

        for(unsigned sampleCount = 0; sampleCount < maxSampleCount; ++sampleCount)
        {
            unsigned numQualityLevels = 0;
            device->CheckMultisampleQualityLevels(format, sampleCount, &numQualityLevels);

            // Quality level of 0 means the sample count is not a valid option
            if( numQualityLevels )
            {
            MultisampleOption multisampleOption(sampleCount, numQualityLevels);
            multisamplingCaps.push_back(multisampleOption);
            }
        }

        m_multisamplingCapsByAdapter.push_back(multisamplingCaps);

        // For each monitor, get all the display modes that match the desired format
        IDXGIOutput *            monitor                   = NULL;
        unsigned                 monitorIndex              = 0;
        unsigned                 numMonitors               = 0;

        while(adapter->EnumOutputs(monitorIndex, &monitor) != DXGI_ERROR_NOT_FOUND)
        {  
            // Store which adapter and monitor is being dealt with this iteration
            AdapterMonitorCombo adapterMonitorCombo(adapterIndex, monitorIndex);

            // Get a description of this monitor
            DXGI_OUTPUT_DESC monitorDesc;
            monitor->GetDesc(&monitorDesc);
         
            // Check if the monitor is attached to the desktop
            if( !monitorDesc.AttachedToDesktop )
            {
            // Next monitor
            monitor->Release();
            monitor = NULL;
            ++monitorIndex;

            continue;
            }
         
            // Increment the number of monitors for this adapter
            ++numMonitors;

            // Get the desktop resolution for the monitor
            m_desktopCoords[adapterMonitorCombo] = monitorDesc.DesktopCoordinates;

            // Get the number of display modes with the requested format, that this monitor supports
            unsigned numModes = 0;
            monitor->GetDisplayModeList(format, flags, &numModes, NULL);

            // Get the actual display mode descriptions with the requested format, that this monitor supports
            DXGI_MODE_DESC * dxgiModeDescs = new DXGI_MODE_DESC[numModes];
            monitor->GetDisplayModeList(format, flags, &numModes, dxgiModeDescs);

            std::vector<DXGI_MODE_DESC> modeDescs(dxgiModeDescs, dxgiModeDescs + numModes);
            delete [] dxgiModeDescs;

            // Sort the mode descriptions
            std::sort(modeDescs.begin(), modeDescs.end(), IsModeDescLessThan());
         
            // Remove duplicates (differ by scaling or scanline ordering only)
            std::vector<DXGI_MODE_DESC>::iterator newModeDescsEnd = std::unique(modeDescs.begin(), modeDescs.end(), IsModeDescEqual());
            modeDescs.erase(newModeDescsEnd, modeDescs.end());

            numModes = static_cast<unsigned>(modeDescs.size());

            // Get the supported resolutions
            std::vector<Resolution>  resolutions;

            for(unsigned modeIndex = 0; modeIndex < numModes; ++modeIndex)
            {
            Resolution resolution(modeDescs[modeIndex].Width,
                                    modeDescs[modeIndex].Height);
            resolutions.push_back(resolution);

            // Store what adapter, monitor and resolution combination is being dealt with in this iteration
            AdapterMonitorResolutionCombo adapterMonitorResolutionCombo(adapterMonitorCombo, resolution);

            // Get the supported refresh rate
            RefreshRate refreshRate(modeDescs[modeIndex].RefreshRate.Numerator,
                                    modeDescs[modeIndex].RefreshRate.Denominator);

            
            // Store the supported refresh rate
            RefreshRates::iterator it = m_refreshRates.end();
            
            if( !m_refreshRates.empty() )
            {
                it = m_refreshRates.find(adapterMonitorResolutionCombo);
            }

            if( it == m_refreshRates.end() )
            {
                std::vector<RefreshRate> refreshRates;
                refreshRates.push_back(refreshRate);
                m_refreshRates[adapterMonitorResolutionCombo] = refreshRates;
            }
            else
            {
                it->second.push_back(refreshRate);
            }
            }
         
            // Remove duplicate resolutions
            std::vector<Resolution>::iterator newResolutionsEnd = std::unique(resolutions.begin(), resolutions.end());
            resolutions.erase(newResolutionsEnd, resolutions.end());

            // Store the supported resolutions
            m_resolutions[adapterMonitorCombo] = resolutions;

            // Next monitor
            monitor->Release();
            monitor = NULL;

            ++monitorIndex;
        }

        // Store the number of monitors for this adapter
        m_numMonitors.push_back(numMonitors);

        // Next Adapter
        adapter->Release();
        adapter = NULL;

	    ++adapterIndex;
    }

    // Release the DXGI Factory
    dxgi->Release();
}

//------------------------------------------------------------------------------------------
void DisplayModeEnumerator::CreateDevice(IDXGIAdapter * adapter, ID3D10Device_UniquePtr & ptr)
{
    // Create the device
    ID3D10Device * device = NULL;

    D3D10_DRIVER_TYPE driverTypes[] =
    {
        D3D10_DRIVER_TYPE_HARDWARE,
        D3D10_DRIVER_TYPE_REFERENCE,
    };

    unsigned numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

    // Set device creation flags
    #ifdef _DEBUG
        UINT flags = D3D10_CREATE_DEVICE_DEBUG;
    #else
        UINT flags = 0;
    #endif

    HRESULT result = 0;

    for(unsigned driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
    {
        result = D3D10CreateDevice(adapter,
                                    driverTypes[driverTypeIndex],
                                    NULL,
                                    flags,
                                    D3D10_SDK_VERSION,
                                    &device);

        if( SUCCEEDED(result) )
        {
            break;
        }
    }

    if( FAILED(result) )
    {
        const std::string msg("Failed to create D3D device");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    ptr.reset(device);
}


//------------------------------------------------------------------------------------------
bool DisplayModeEnumerator::IsModeDescLessThan::operator() (const DXGI_MODE_DESC & lhs, const DXGI_MODE_DESC & rhs) const
{
    if( lhs.Format < rhs.Format )
    {
        return true;
    }
   
    if( lhs.Format > rhs.Format )
    {
        return false;
    }

    Resolution lhsResolution(lhs.Width, lhs.Height);
    Resolution rhsResolution(rhs.Width, rhs.Height);

    if( lhsResolution < rhsResolution )
    {
        return true;
    }
   
    if( lhsResolution > rhsResolution )
    {
        return false;
    }
   
    RefreshRate lhsRefreshRate(lhs.RefreshRate.Numerator, lhs.RefreshRate.Denominator);
    RefreshRate rhsRefreshRate(rhs.RefreshRate.Numerator, rhs.RefreshRate.Denominator);

    if( lhsRefreshRate < rhsRefreshRate )
    {
        return true;
    }
   
    if( lhsRefreshRate > rhsRefreshRate )
    {
        return false;
    }

    if( lhs.Scaling < rhs.Scaling )
    {
        return true;
    }
   
    if( lhs.Scaling > rhs.Scaling )
    {
        return false;
    }

    if( lhs.ScanlineOrdering < rhs.ScanlineOrdering )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------
bool DisplayModeEnumerator::IsModeDescEqual::operator() (const DXGI_MODE_DESC & lhs, const DXGI_MODE_DESC & rhs) const
{
    if( lhs.Format != rhs.Format )
    {
        return false;
    }

    if( lhs.Width != rhs.Width )
    {
        return false;
    }

    if( lhs.Height != rhs.Height )
    {
        return false;
    }

    if( lhs.RefreshRate.Numerator != rhs.RefreshRate.Numerator )
    {
        return false;
    }

    if( lhs.RefreshRate.Denominator != rhs.RefreshRate.Denominator )
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
DisplayModeEnumerator::IsLowerResolution::IsLowerResolution(const Resolution & desktopResolution)
    :
    m_desktopResolution(desktopResolution)
{
}

//------------------------------------------------------------------------------------------
bool DisplayModeEnumerator::IsLowerResolution::operator() (const Resolution & resolution) const
{
    if( resolution.m_width  < m_desktopResolution.m_width && 
        resolution.m_height < m_desktopResolution.m_height )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------
DisplayModeEnumerator::IsMultisampleSettingEqual::IsMultisampleSettingEqual(const MultisampleOption & setting)
    :
    m_setting(setting)
{
}

//------------------------------------------------------------------------------------------
bool DisplayModeEnumerator::IsMultisampleSettingEqual::operator() (const MultisampleOption & option) const
{
    // Compare multisample count
    if( m_setting.first != option.first )
    {
        return false;
    }

    // The multisample quality setting must be less than the number of multisample quality levels
    if( m_setting.second >= 0 && 
        m_setting.second < option.second )
    {
        return true;
    }

    return false;
}

