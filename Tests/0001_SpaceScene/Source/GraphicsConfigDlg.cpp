

#include "GraphicsConfigDlg.h"

#include "resource.h"

// Common Lib Includes
#include "Exception.h"

// Windows Includes
#include <commctrl.h>

#include "windows.h"
#include "windef.h"
#include "winbase.h"
#include "shlwapi.h"

// Standard Includes
#include <sstream>
#include <cassert>


#define PACKVERSION(major,minor) MAKELONG(minor,major)



//-----------------------------------------------------------------------------
GraphicsConfigDlg::GraphicsConfigDlg(DisplayMode & result)
    :
    BaseModalDialog(),
    m_displayMode(result)
{
    // Init Windows Common Controls
    INITCOMMONCONTROLSEX desc;
    desc.dwICC  = ICC_STANDARD_CLASSES | ICC_USEREX_CLASSES;
    desc.dwSize = sizeof(INITCOMMONCONTROLSEX);

    if( !InitCommonControlsEx(&desc) )
    {
        const std::string msg("Unable to initialize windows common controls 6 or later.");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//-----------------------------------------------------------------------------
GraphicsConfigDlg::~GraphicsConfigDlg()
{
}

//-----------------------------------------------------------------------------
INT_PTR GraphicsConfigDlg::Run(HINSTANCE instance)
{
    return BaseModalDialog::Init(instance, MAKEINTRESOURCEW(IDD_GRAPHICS_CONFIG), NULL);
}

//----------------------------------------------------------------------
void GraphicsConfigDlg::InitComponents()
{
    // Get handles to the controls
    m_cmbAdapters              = GetDlgItem(m_hwnd, IDC_CMB_ADAPTER);
    m_cmbMonitors              = GetDlgItem(m_hwnd, IDC_CMB_MONITOR);
    m_chkWindowed              = GetDlgItem(m_hwnd, IDC_CHK_WINDOWED);
    m_cmbResolutionsWindowed   = GetDlgItem(m_hwnd, IDC_CMB_RESOLUTION_WINDOWED);
    m_cmbResolutionsFullscreen = GetDlgItem(m_hwnd, IDC_CMB_RESOLUTION_FULLSCREEN);
    m_cmbRefreshRates          = GetDlgItem(m_hwnd, IDC_CMB_REFRESHRATE);  
    m_cmbMultisampleCount      = GetDlgItem(m_hwnd, IDC_CMB_MULTISAMPLE_COUNT);  
    m_cmbMultisampleQuality    = GetDlgItem(m_hwnd, IDC_CMB_MULTISAMPLE_QUALITY);
   
    if( !m_cmbAdapters              ||
        !m_cmbMonitors              ||
        !m_chkWindowed              ||
        !m_cmbResolutionsWindowed   ||
        !m_cmbResolutionsFullscreen ||
        !m_cmbRefreshRates          ||
        !m_cmbMultisampleCount      ||
        !m_cmbMultisampleQuality )
    {
        const std::string msg("Unable to get handles to one or more dialog controls");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Uncheck the windowed mode box
    SendMessage(m_chkWindowed, BM_SETCHECK, BST_UNCHECKED, NULL);
   
    // Validate or initialize dependant controls (which will also validate or initialize thier dependants)
    OnAdapterChanged();
}

//----------------------------------------------------------------------
void GraphicsConfigDlg::OnAdapterChanged()
{
    // Get the relevant current selections
    unsigned adapterIndex = static_cast<unsigned>(SendMessage(m_cmbAdapters, CB_GETCURSEL, NULL, NULL));
   
    // Check if the adapter selection list is invalid or uninitialized
    unsigned numAdapters = m_displayModeEnumerator.GetNumAdapters();
    bool     listChanged = false;

    if( m_numAdapters != numAdapters )
    {
        // Initialize the adapters combo box
        SendMessage(m_cmbAdapters, CB_RESETCONTENT, NULL, NULL);

        for( unsigned i = 0; i < numAdapters; ++i)
        {
            SendMessage(m_cmbAdapters, CB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(m_displayModeEnumerator.GetAdapterName(i).c_str()));
        }

        if( numAdapters <= 5 )
        {
            SendMessage(m_cmbAdapters, CB_SETMINVISIBLE, numAdapters, NULL);
        }
        else
        {
            SendMessage(m_cmbAdapters, CB_SETMINVISIBLE, 5, NULL);
        }

        m_numAdapters = numAdapters;
        listChanged = true;
    }

    // Check if the selection was uninitialized or invalid
    if( CB_ERR == adapterIndex )
    {
        SendMessage(m_cmbAdapters, CB_SETCURSEL, 0, NULL);
    }
    else if( listChanged )
    {
        if( adapterIndex < 0 || adapterIndex >= numAdapters )
        {
            SendMessage(m_cmbAdapters, CB_SETCURSEL, 0, NULL);
        }
    }

    // Validate or initialize dependant controls (which will also validate or initialize thier dependants)
    OnMonitorChanged();
    OnMultisampleCountChanged();
}

//----------------------------------------------------------------------
void GraphicsConfigDlg::OnMonitorChanged()
{
    // Get the relevant current selections
    unsigned adapterIndex = static_cast<unsigned>(SendMessage(m_cmbAdapters, CB_GETCURSEL, NULL, NULL));
    unsigned monitorIndex = static_cast<unsigned>(SendMessage(m_cmbMonitors, CB_GETCURSEL, NULL, NULL));   
   
    // Check if the monitor selection list is invalid or uninitialized
    unsigned numMonitors = m_displayModeEnumerator.GetNumMonitors(adapterIndex);
    bool     listChanged = false;

    if( m_numMonitors != numMonitors )
    {
        // Initialize the monitors combo box
        SendMessage(m_cmbMonitors, CB_RESETCONTENT, NULL, NULL);

        for( unsigned i = 0; i < numMonitors; ++i)
        {
            std::ostringstream converter;
            converter << i;

            SendMessage(m_cmbMonitors, CB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(converter.str().c_str()));
        }

        if( numMonitors <= 5 )
        {
            SendMessage(m_cmbMonitors, CB_SETMINVISIBLE, numMonitors, NULL);
        }
        else
        {
            SendMessage(m_cmbMonitors, CB_SETMINVISIBLE, 5, NULL);
        }

        m_numMonitors = numMonitors;
        listChanged = true;
    }

    // Check if the selection is uninitialized or invalid
    if( CB_ERR == monitorIndex )
    {
        SendMessage(m_cmbMonitors, CB_SETCURSEL, 0, NULL);
    }
    else if( listChanged )
    {
        if( monitorIndex < 0 || monitorIndex >= numMonitors )
        {
            SendMessage(m_cmbMonitors, CB_SETCURSEL, 0, NULL);
        }
    }

    // Validate or initialize dependant controls (which will also validate or initialize thier dependants)
    OnResolutionFullscreenChanged();
    OnResolutionWindowedChanged();
}

//----------------------------------------------------------------------
void GraphicsConfigDlg::OnResolutionFullscreenChanged()
{
    // Get the relevant current selections
    unsigned adapterIndex    = static_cast<unsigned>(SendMessage(m_cmbAdapters, CB_GETCURSEL, NULL, NULL));
    unsigned monitorIndex    = static_cast<unsigned>(SendMessage(m_cmbMonitors, CB_GETCURSEL, NULL, NULL));
    unsigned resolutionIndex = static_cast<unsigned>(SendMessage(m_cmbResolutionsFullscreen, CB_GETCURSEL, NULL, NULL));
    Resolution resolution(0,0);

    if( CB_ERR != resolutionIndex )
    {
        resolution = m_resolutionsFullscreen[resolutionIndex];
    }

    // Get the resolutions
    std::vector<Resolution> resolutions;
    m_displayModeEnumerator.GetResolutions(adapterIndex, monitorIndex, resolutions);

    // Check if the resolution selection list is invalid or uninitialized
    bool listChanged = false;

    if( m_resolutionsFullscreen != resolutions )
    {
        // Initialize the resolutions combo box
        SendMessage(m_cmbResolutionsFullscreen, CB_RESETCONTENT, NULL, NULL);

        for( std::vector<Resolution>::iterator it = resolutions.begin(); it != resolutions.end(); ++it)
        {
            SendMessage(m_cmbResolutionsFullscreen, CB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(it->AsString().c_str()));
        }

        unsigned numResolutions = static_cast<unsigned>(resolutions.size());

        if( numResolutions <= 5 )
        {
            SendMessage(m_cmbResolutionsFullscreen, CB_SETMINVISIBLE, numResolutions, NULL);
        }
        else
        {
            SendMessage(m_cmbResolutionsFullscreen, CB_SETMINVISIBLE, 5, NULL);
        }

        m_resolutionsFullscreen = resolutions;
        listChanged = true;
    }

    // Check if the selection is uninitialized or invalid
    if( CB_ERR == resolutionIndex )
    {
        SendMessage(m_cmbResolutionsFullscreen, CB_SETCURSEL, 0, NULL);
    }
    else if( listChanged )
    {
        std::vector<Resolution>::const_iterator it = resolutions.end();
   
        if( !resolutions.empty() )
        {
            it = std::find(resolutions.begin(), resolutions.end(), resolution);
        }

        if( it == resolutions.end() )
        {
            SendMessage(m_cmbResolutionsFullscreen, CB_SETCURSEL, 0, NULL);
        }
        else
        {
            resolutionIndex = static_cast<unsigned>(it - resolutions.begin());
            SendMessage(m_cmbResolutionsFullscreen, CB_SETCURSEL, resolutionIndex, NULL);
        } 
    }

    // Validate or initialize dependant controls (which will also validate or initialize thier dependants)
    OnRefreshRateChanged();
}

//----------------------------------------------------------------------
void GraphicsConfigDlg::OnResolutionWindowedChanged()
{
    // Get the relevant current selections
    unsigned adapterIndex    = static_cast<unsigned>(SendMessage(m_cmbAdapters, CB_GETCURSEL, NULL, NULL));
    unsigned monitorIndex    = static_cast<unsigned>(SendMessage(m_cmbMonitors, CB_GETCURSEL, NULL, NULL));
    unsigned resolutionIndex = static_cast<unsigned>(SendMessage(m_cmbResolutionsWindowed, CB_GETCURSEL, NULL, NULL));
    Resolution resolution(0,0);

    if( CB_ERR != resolutionIndex )
    {
        resolution = m_resolutionsWindowed[resolutionIndex];
    }

    // Get the resolutions
    std::vector<Resolution> resolutions;
    m_displayModeEnumerator.GetResolutions(adapterIndex, monitorIndex, resolutions);

    const Resolution & windowed = m_displayModeEnumerator.GetMaxWindowedResolution(adapterIndex, monitorIndex);
   
    std::vector<Resolution>::iterator it = resolutions.begin();
    while( it != resolutions.end() )
    {
        if( *it > windowed )
        {
            // Since the resolutions are ordered smallest to largest, we can just remove the resolutions greater than the desktop
            // from the fullscreen resolution list and indexing will still be OK.
            it = resolutions.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Check if the resolution selection list is invalid or uninitialized
    bool listChanged = false;

    if( m_resolutionsWindowed != resolutions )
    {
        // Initialize the resolutions combo box
        SendMessage(m_cmbResolutionsWindowed, CB_RESETCONTENT, NULL, NULL);

        for( std::vector<Resolution>::iterator it = resolutions.begin(); it != resolutions.end(); ++it)
        {
            SendMessage(m_cmbResolutionsWindowed, CB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(it->AsString().c_str()));
        }

        unsigned numResolutions = static_cast<unsigned>(resolutions.size());

        if( numResolutions <= 5 )
        {
            SendMessage(m_cmbResolutionsWindowed, CB_SETMINVISIBLE, numResolutions, NULL);
        }
        else
        {
            SendMessage(m_cmbResolutionsWindowed, CB_SETMINVISIBLE, 5, NULL);
        }

        m_resolutionsWindowed = resolutions;
        listChanged = true;
    }

    // Check if the selection is uninitialized or invalid
    if( CB_ERR == resolutionIndex )
    {
        SendMessage(m_cmbResolutionsWindowed, CB_SETCURSEL, 0, NULL);
    }
    else if( listChanged )
    {
        std::vector<Resolution>::const_iterator it = resolutions.end();
   
        if( !resolutions.empty() )
        {
            it = std::find(resolutions.begin(), resolutions.end(), resolution);
        }

        if( it == resolutions.end() )
        {
            SendMessage(m_cmbResolutionsWindowed, CB_SETCURSEL, 0, NULL);
        }
        else
        {
            resolutionIndex = static_cast<unsigned>(it - resolutions.begin());
            SendMessage(m_cmbResolutionsWindowed, CB_SETCURSEL, resolutionIndex, NULL);
        } 
    }

    // No dependant controls to validate
}

//----------------------------------------------------------------------
void GraphicsConfigDlg::OnRefreshRateChanged()
{
    // Get the relevant current selections
    unsigned    adapterIndex     = static_cast<unsigned>(SendMessage(m_cmbAdapters, CB_GETCURSEL, NULL, NULL));
    unsigned    monitorIndex     = static_cast<unsigned>(SendMessage(m_cmbMonitors, CB_GETCURSEL, NULL, NULL));
    unsigned    resolutionIndex  = static_cast<unsigned>(SendMessage(m_cmbResolutionsFullscreen, CB_GETCURSEL, NULL, NULL));
    Resolution  resolution       = m_resolutionsFullscreen[resolutionIndex];
    unsigned    refreshRateIndex = static_cast<unsigned>(SendMessage(m_cmbRefreshRates, CB_GETCURSEL, NULL, NULL)); 
    RefreshRate refreshRate(0,0);
   
    if( CB_ERR != refreshRateIndex )
    {
        refreshRate = m_refreshRates[refreshRateIndex];
    }

    // Get the refresh rates
    std::vector<RefreshRate> refreshRates;
    m_displayModeEnumerator.GetRefreshRates(adapterIndex, monitorIndex, resolution, refreshRates);

    // Check if the resolution selection list is invalid or uninitialized
    bool listChanged = false;

    if( m_refreshRates != refreshRates )
    {
        // Initialize the Refresh Rate combo box
        SendMessage(m_cmbRefreshRates, CB_RESETCONTENT, NULL, NULL);

        for( std::vector<RefreshRate>::iterator it = refreshRates.begin(); it != refreshRates.end(); ++it)
        {
            SendMessage(m_cmbRefreshRates, CB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(it->AsString().c_str()));
        }

        unsigned numRefreshRates = static_cast<unsigned>(refreshRates.size());

        if( numRefreshRates <= 5 )
        {
            SendMessage(m_cmbRefreshRates, CB_SETMINVISIBLE, numRefreshRates, NULL);
        }
        else
        {
            SendMessage(m_cmbRefreshRates, CB_SETMINVISIBLE, 5, NULL);
        }

        m_refreshRates = refreshRates;
        listChanged = true;
    }

    // Check if the selection is invalid or uninitialized
    if( CB_ERR == refreshRateIndex )
    {
        SendMessage(m_cmbRefreshRates, CB_SETCURSEL, 0, NULL);
    }
    else if( listChanged )
    {
        std::vector<RefreshRate>::const_iterator it = refreshRates.end();
   
        if( !refreshRates.empty() )
        {
            it = std::find(refreshRates.begin(), refreshRates.end(), refreshRate);
        }

        if( it == refreshRates.end() )
        {
            SendMessage(m_cmbRefreshRates, CB_SETCURSEL, 0, NULL);
        }
        else
        {
            refreshRateIndex = static_cast<unsigned>(it - refreshRates.begin());
            SendMessage(m_cmbRefreshRates, CB_SETCURSEL, refreshRateIndex, NULL);
        } 
    }
}

//----------------------------------------------------------------------
void GraphicsConfigDlg::OnMultisampleCountChanged()
{
    // Get the relevant current selections
    unsigned adapterIndex          = static_cast<unsigned>(SendMessage(m_cmbAdapters, CB_GETCURSEL, NULL, NULL));
    unsigned multisampleCountIndex = static_cast<unsigned>(SendMessage(m_cmbMultisampleCount, CB_GETCURSEL, NULL, NULL));
    unsigned multisampleCount      = 0;

    if( CB_ERR != multisampleCountIndex )
    {
        multisampleCount = m_multisamplingOptions[multisampleCountIndex].first;
    }

    // Get the multisampling options
    std::vector<std::pair<unsigned, unsigned> > multisamplingOptions;
    m_displayModeEnumerator.GetMultisamplingOptions(adapterIndex, multisamplingOptions);

    // Check if the multisample count selection list is invalid or uninitialized
    bool listChanged = false;

    if( m_multisamplingOptions != multisamplingOptions )
    {
        // Initialize the multisampling count combo box
        SendMessage(m_cmbMultisampleCount, CB_RESETCONTENT, NULL, NULL);

        unsigned numMultisampleCounts = static_cast<unsigned>(multisamplingOptions.size());

        for( unsigned i = 0; i < numMultisampleCounts; ++i)
        {
            std::ostringstream converter;
            converter << multisamplingOptions[i].first;

            SendMessage(m_cmbMultisampleCount, CB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(converter.str().c_str()));
        }

        if( numMultisampleCounts <= 5 )
        {
            SendMessage(m_cmbMultisampleCount, CB_SETMINVISIBLE, numMultisampleCounts, NULL);
        }
        else
        {
            SendMessage(m_cmbMultisampleCount, CB_SETMINVISIBLE, 5, NULL);
        }

        m_multisamplingOptions = multisamplingOptions;
        listChanged = true;
    }

    // Check if the selection is invalid or uninitialized
    if( CB_ERR == multisampleCountIndex )
    {
        SendMessage(m_cmbMultisampleCount, CB_SETCURSEL, 0, NULL);
    }
    else if( listChanged )
    {
        std::vector<std::pair<unsigned, unsigned> >::const_iterator it = multisamplingOptions.end();
   
        if( !multisamplingOptions.empty() )
        {
            it = std::find_if(multisamplingOptions.begin(), multisamplingOptions.end(), HasMultisampleCount(multisampleCount));
        }

        if( it == multisamplingOptions.end() )
        {
            SendMessage(m_cmbMultisampleCount, CB_SETCURSEL, 0, NULL);
        }
        else
        {
            multisampleCountIndex = static_cast<unsigned>(it - multisamplingOptions.begin());
            SendMessage(m_cmbMultisampleCount, CB_SETCURSEL, multisampleCountIndex, NULL);
        } 
    }

    // Validate or initialize dependant controls (which will also validate or initialize thier dependants)
    OnMultisampleQualityChanged();
}

//----------------------------------------------------------------------
void GraphicsConfigDlg::OnMultisampleQualityChanged()
{
    // Get the relevant current selections
    unsigned multisampleCountIndex       = static_cast<unsigned>(SendMessage(m_cmbMultisampleCount, CB_GETCURSEL, NULL, NULL));
    unsigned multisampleQualityIndex     = static_cast<unsigned>(SendMessage(m_cmbMultisampleQuality, CB_GETCURSEL, NULL, NULL));
    unsigned numMultisampleQualityLevels = static_cast<unsigned>(SendMessage(m_cmbMultisampleQuality, CB_GETCOUNT, NULL, NULL));

    // Check if the multisample quality selection list is invalid or uninitialized
    bool listChanged = false;

    if( CB_ERR == numMultisampleQualityLevels || 
        numMultisampleQualityLevels != m_multisamplingOptions[multisampleCountIndex].second )
    {
        // Initialize the multisampling quality level combo box
        SendMessage(m_cmbMultisampleQuality, CB_RESETCONTENT, NULL, NULL);

        numMultisampleQualityLevels = m_multisamplingOptions[multisampleCountIndex].second;

        for( unsigned i = 0; i < numMultisampleQualityLevels; ++i)
        {
            std::ostringstream converter;
            converter << i;

            SendMessage(m_cmbMultisampleQuality, CB_ADDSTRING, NULL, reinterpret_cast<LPARAM>(converter.str().c_str()));
        }

        if( numMultisampleQualityLevels <= 5 )
        {
            SendMessage(m_cmbMultisampleQuality, CB_SETMINVISIBLE, numMultisampleQualityLevels, NULL);
        }
        else
        {
            SendMessage(m_cmbMultisampleQuality, CB_SETMINVISIBLE, 5, NULL);
        }

        listChanged = true;
    }

    // Check if the selection is invalid or uninitialized
    if( CB_ERR == multisampleQualityIndex )
    {
        SendMessage(m_cmbMultisampleQuality, CB_SETCURSEL, 0, NULL);
    }
    else if( listChanged )
    {
        if( multisampleQualityIndex < numMultisampleQualityLevels )
        {
            SendMessage(m_cmbMultisampleQuality, CB_SETCURSEL, multisampleQualityIndex, NULL);
        }
        else
        {
            SendMessage(m_cmbMultisampleQuality, CB_SETCURSEL, 0, NULL);
        } 
    }
}

//----------------------------------------------------------------------
void GraphicsConfigDlg::FinalizeDisplayMode()
{
    // Get the relevant current selections
    m_displayMode.m_adapterIndex           = static_cast<unsigned>(SendMessage(m_cmbAdapters, CB_GETCURSEL, NULL, NULL));
   
    m_displayMode.m_monitorIndex           = static_cast<unsigned>(SendMessage(m_cmbMonitors, CB_GETCURSEL, NULL, NULL));

    m_displayMode.m_fullscreen             = !(BST_CHECKED == IsDlgButtonChecked(m_hwnd, IDC_CHK_WINDOWED));

    unsigned resolutionIndex               = static_cast<unsigned>(SendMessage(m_cmbResolutionsFullscreen, CB_GETCURSEL, NULL, NULL));
    m_displayMode.m_resolutionFullscreen   = m_resolutionsFullscreen[resolutionIndex];

    resolutionIndex                        = static_cast<unsigned>(SendMessage(m_cmbResolutionsWindowed, CB_GETCURSEL, NULL, NULL));
    m_displayMode.m_resolutionWindowed     = m_resolutionsWindowed[resolutionIndex];

    unsigned refreshRateIndex              = static_cast<unsigned>(SendMessage(m_cmbRefreshRates, CB_GETCURSEL, NULL, NULL));                          
    m_displayMode.m_refreshRate            = m_refreshRates[refreshRateIndex];

    unsigned multisampleCountIndex         = static_cast<unsigned>(SendMessage(m_cmbMultisampleCount, CB_GETCURSEL, NULL, NULL));
    m_displayMode.m_multisamplingCount     = m_multisamplingOptions[multisampleCountIndex].first;

    m_displayMode.m_multisamplingQuality = static_cast<unsigned>(SendMessage(m_cmbMultisampleQuality, CB_GETCURSEL, NULL, NULL));
}

//----------------------------------------------------------------------
INT_PTR CALLBACK GraphicsConfigDlg::ModalDlgProc(HWND dlg, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch(message)
    {
        case WM_COMMAND:
        {
            switch(LOWORD(wparam))
            {
            // Combo Box - Adapters
            case IDC_CMB_ADAPTER:
            {
                if( HIWORD(wparam) == CBN_SELCHANGE )
                {
                    OnAdapterChanged();
                    return true;
                }

                break;
            }

            // Combo Box - Monitors
            case IDC_CMB_MONITOR:
            {
                if( HIWORD(wparam) == CBN_SELCHANGE )
                {
                    OnMonitorChanged();
                    return true;
                }

                break;
            }

            // Combo Box - Fullscreen Resolutions
            case IDC_CMB_RESOLUTION_FULLSCREEN:
            {
                if( HIWORD(wparam) == CBN_SELCHANGE )
                {
                    OnResolutionFullscreenChanged();
                    return true;
                }

                break;
            }

            // Combo Box - Multisample Count
            case IDC_CMB_MULTISAMPLE_COUNT:
            {
                if( HIWORD(wparam) == CBN_SELCHANGE )
                {
                    OnMultisampleCountChanged();
                    return true;
                }

                break;
            }

            // Button - OK
            case IDOK:
            {
                FinalizeDisplayMode();
                EndDialog(dlg, 0);
                return true;
            }

            default:
            {
                break;
            }
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------
GraphicsConfigDlg::HasMultisampleCount::HasMultisampleCount(const unsigned multisampleCount)
    :
    m_multisampleCount(multisampleCount)
{
}

//------------------------------------------------------------------------------------------
bool GraphicsConfigDlg::HasMultisampleCount::operator() (const std::pair<unsigned, unsigned> & multisampleOption) const
{
    if( m_multisampleCount == multisampleOption.first )
    {
        return true;
    }

    return false;
}
