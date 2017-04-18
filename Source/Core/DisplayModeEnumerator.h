
#ifndef DISPLAYMODEENUMERATOR_H
#define DISPLAYMODEENUMERATOR_H

// EngineX Includes
#include "DisplayMode.h"
#include "Resolution.h"
#include "RefreshRate.h"

// DirectX Includes
#include <d3d10.h>

// Windows Includes
#include <dxgi.h>

// Standard Includes
#include <string>
#include <vector>
#include <map>
#include <memory>

//------------------------------------------------------------------------------------------
/**
* Unique pointer to a D3D10 device
*
* This pointer type will Release the D3D device it points to upon its destruction
*/
struct ID3D10Device_Deletor
{
   void operator() (ID3D10Device * p) {p->Release();}
};

// typedef boost::interprocess::unique_ptr<ID3D10Device, ID3D10Device_Deletor> ID3D10Device_UniquePtr;
typedef std::unique_ptr<ID3D10Device, ID3D10Device_Deletor> ID3D10Device_UniquePtr;


//------------------------------------------------------------------------------------------
/**
* Display Mode Enumerator
*
* Queries the system for its display capabilites and provides an interface to retrieve those capabilities
**/
class DisplayModeEnumerator
{
public:
   
   /**
   * Constructor
   **/
   DisplayModeEnumerator();

   /**
   * Deconstructor
   **/
   ~DisplayModeEnumerator();


   /**
   * Gets the number of adapters
   *
   * @return unsigned - Number of supported adapters
   **/
   const unsigned GetNumAdapters() const;

   /**
   * Gets the name of the adapter
   *
   * @param adapterIndex - Index of the adapter (number of adapters can be obtained from the GetNumAdapters method )
   * @return std::string - Name of the adapter
   *
   * @throws BaseException - If the adapter index param is invalid
   **/
   const std::string GetAdapterName(const unsigned adapterIndex) const;

   /**
   * Gets the multisampling count and quality levels supported by the adapter
   *
   * @param adapterIndex             - Index of the adapter (number of adapters can be obtained from the GetNumAdapters method )
   * @param multisamplingOptions OUT - std::vector containing multisampling options that are supported for the adapter.
   *    The vector contains pairs where
   *       first  - multisample count
   *       second - maximum multisample quality level available for that count
   *    Disabled multisampling should be represented by a count of 1 and a quality level of 0
   *
   * @throws BaseException - If the adapter index param is invalid
   **/
   void GetMultisamplingOptions(const unsigned adapterIndex, 
                                std::vector<std::pair<unsigned, unsigned> > & multisamplingOptions) const;

   /**
   * Gets the number of monitors connected to the adapter and attached to the desktop
   * 
   * @param adapterIndex - Index of the adapter (number of adapters can be obtained from the GetNumAdapters method )
   * @return unsigned    - Number of monitors connected to the adapter (The monitor must be powered on to be reported)
   *
   * @throws BaseException - If the adapter index param is invalid
   **/
   const unsigned GetNumMonitors(const unsigned adapterIndex) const;

   /**
   * Gets the supported resolutions for a adapter and monitor combination
   *
   * @param adapterIndex    - Index of the adapter (number of adapters can be obtained from the GetNumAdapters method )
   * @param monitorIndex    - Index of the monitor (number of monitors can be obtained from the GetNumMonitors method )
   * @param resolutions OUT - Supported resolutions for the requested adapter and monitor combination
   *
   * @throws BaseException - If the adapter or monitor index params are invalid
   **/
   void GetResolutions(const unsigned adapterIndex, const unsigned monitorIndex, std::vector<Resolution> & resolutions) const;
   

   /**
   * Gets the supported resolutions for a adapter, monitor and resolution combination
   *
   * @param adapterIndex     - Index of the adapter (number of adapters can be obtained from the GetNumAdapters method )
   * @param monitorIndex     - Index of the monitor (number of monitors can be obtained from the GetNumMonitors method )
   * @param resolution       - Resolution (obtained from the GetResolutions method )
   * @param refreshRates OUT - Supported refresh rates for the requested adapter, monitor and resolution combination
   *
   * @throws BaseException - If the adapter, monitor and resolution params are invalid
   **/
   void GetRefreshRates(const unsigned adapterIndex, 
                        const unsigned monitorIndex, 
                        const Resolution & resolution, 
                        std::vector<RefreshRate> & refreshRates) const;

   /**
   * Gets the screen coordinates of a desktop
   *
   * @param adapterIndex             - Index of the adapter (number of adapters can be obtained from the GetNumAdapters method )
   * @param monitorIndex             - Index of the monitor (number of monitors can be obtained from the GetNumMonitors method )
   * @return std::vector<Resolution> - Supported resolutions for the requested adapter and monitor combination
   *
   * @throws BaseException - If the adapter or monitor index params are invalid
   **/
   const RECT GetDesktopCoords(const unsigned adapterIndex, const unsigned monitorIndex) const;

   /**
   * Gets the maximum resolution that will fit on the desktop
   *
   * NOTE - This method assumes a WS_OVERLAPPEDWINDOW style with no menu
   *
   * @param adapterIndex - Index of the adapter (number of adapters can be obtained from the GetNumAdapters method )
   * @param monitorIndex - Index of the monitor (number of monitors can be obtained from the GetNumMonitors method )
   * @return Resolution  - maximum resolution that will fit on the desktop
   *
   * @throws BaseException - If the adapter or monitor index params are invalid, or if a resolution could not be found
   **/
   const Resolution GetMaxWindowedResolution(const unsigned adapterIndex, const unsigned monitorIndex) const;

   /**
   * Validates settings in a DisplayMode object
   *
   * @param displayMode - DisplayMode to be validated
   * @return bool       - true if the DisplayMode is valid, otherwise false.
   **/
   const bool IsValidDisplayMode(const DisplayMode & displayMode) const;

   /**
   * Creates a D3D device
   *
   * @param adapterIndex               - Adapter with which to create the device
   * @param ID3D10Device_UniquePtr OUT - boost::unique_ptr to a ID3D10Device interface
   *
   * @throws BaseException - If creation of the device failed
   **/
   void CreateDevice(const unsigned adapterIndex, ID3D10Device_UniquePtr & ptr);

private:

   //-----
   // Adapter Names
   //

   // Index - Adapter index
   // Value - Name of the adapter
   typedef std::vector<std::string> AdapterNames;
   AdapterNames m_adapterNames;
   
   //
   //-----
   // Multisampling Capabilities
   //
   // Multisampling capabilities are dependant on:
   // 1) Adapter index 
   // 2) Backbuffer format
   //
   // NOTE - Currently, a format of DXGI_R8G8B8A8_UNORM is always assumed
   //
   
   // first  - Multisample count
   // second - Number of available multisample quality levels for that count
   //
   // NOTE - An std::pair supports a less than comparison and performs it lexagraphically, 
   //        which is suitable for comparison
   typedef std::pair<unsigned, unsigned> MultisampleOption;

   // Index - No meaning
   // Value - Supported multisampling counts and number of quality levels for each count
   typedef std::vector<MultisampleOption> MultisamplingCaps;

   // Index - Adapter Index
   // Value - Supported multisampling counts and number of quality levels for each count
   typedef std::vector<MultisamplingCaps> MultisamplingCapsByAdapter;
   MultisamplingCapsByAdapter m_multisamplingCapsByAdapter;

   //
   //-----
   // Number of Monitors

   // Index - Adapter Index
   // Value - Number of monitors connected to the adapter (monitor must be power on to be reported)
   typedef std::vector<unsigned> NumMonitors;
   NumMonitors m_numMonitors;

   //
   //-----
   // Adapter and Monitor index combination
   //
   // Provides a means for look up of data dependant on an adapter and monitor combination
   // 
   // NOTE - An std::pair supports a less than comparison and performs it lexagraphically, 
   //        which is suitable for comparison
   
   // first  - Adapter index
   // second - Monitor index
   typedef std::pair<unsigned, unsigned> AdapterMonitorCombo;

   //
   //-----
   // Desktop coordinates
   //
   // Desktop coordinates are dependant on:
   // 1) Adapter index
   // 2) Monitor index
   //

   // Key   - Adapter and Monitor combination
   // Value - Desktop Coordinates
   typedef std::map<AdapterMonitorCombo, RECT> DesktopCoords;
   DesktopCoords m_desktopCoords;

   //
   //-----
   // Resolutions
   //
   // Resolutions are dependant on:
   // 1) Adapter index
   // 2) Monitor index
   // 3) Backbuffer format
   //
   // NOTE - Currently, a format of DXGI_R8G8B8A8_UNORM is always assumed
   //

   // Key   - Adapter and Monitor combination
   // Value - Resolutions that are supported
   typedef std::map<AdapterMonitorCombo, std::vector<Resolution> > Resolutions;
   Resolutions m_resolutions;

   //
   //-----
   // Adapter index, Monitor index and Resolution combination
   //
   // Provides a means for look up of data dependant on an adapter, monitor and resolution combination
   // 
   // NOTE - An std::pair supports a less than comparison and performs it lexagraphically, 
   //        which is suitable for comparison
   
   // first  - Pair<Adapter index, Monitor index>
   // second - Resolution
   typedef std::pair<AdapterMonitorCombo, Resolution> AdapterMonitorResolutionCombo;

   //
   //-----
   // Refresh Rates
   //
   // Refresh Rates are dependant on:
   // 1) Adapter index
   // 2) Monitor index
   // 3) Backbuffer format
   // 4) Resolution
   //
   // NOTE - Currently, a format of DXGI_R8G8B8A8_UNORM is always assumed
   //

   // Key   - Adapter, Monitor and Resolution combination
   // Value - Refresh rates that are supported
   typedef std::map<AdapterMonitorResolutionCombo, std::vector<RefreshRate> > RefreshRates;
   RefreshRates m_refreshRates;


   //-----
   // Methods

   /**
   * Queries the system for all supported display modes
   *
   * @throws BaseException - If enumeration fails
   **/
   void Enumerate();

   /**
   * Creates a D3D device
   *
   * @param adapter                    - Adapter with which to create the device
   * @param ID3D10Device_UniquePtr OUT - boost::unique_ptr to a ID3D10Device interface
   *
   * @throws BaseException - If creation of the device failed
   **/
   void CreateDevice(IDXGIAdapter * adapter, ID3D10Device_UniquePtr & ptr);

   /**
   * Comparator to determine of a DXGI_MODE_DESC element less than another in a collection, for sorting purposes
   **/
   struct IsModeDescLessThan
   {
      bool operator() (const DXGI_MODE_DESC & lhs, const DXGI_MODE_DESC & rhs) const;
   };

   /**
   * Comparator to remove all DXGI_MODE_DESC elements from a collection, which do not have a unique:
   * 1) Format
   * 2) Resolution
   * 3) RefreshRate
   *
   * In other words, scanline ordering and scaling differences are disgarded 
   **/
   struct IsModeDescEqual
   {
      bool operator() (const DXGI_MODE_DESC & lhs, const DXGI_MODE_DESC & rhs) const;
   };

   /**
   * Comparator to find a resolution with lower width and height than the desktop resolution
   **/
   struct IsLowerResolution
   {
      IsLowerResolution(const Resolution & desktopResolution);
      bool operator() (const Resolution & resolution) const;

      Resolution m_desktopResolution;
   };

   /**
   * Comparator to find a valid multisampling setting in a collection of MultisampleOptions
   *
   * The difference between the two being that a setting has a multisample quality value,
   * while the other has the number of available multisample quality levels
   **/
   struct IsMultisampleSettingEqual
   {
      IsMultisampleSettingEqual(const MultisampleOption & setting);
      bool operator() (const MultisampleOption & option) const;

      MultisampleOption m_setting;
   };
};

#endif //  DISPLAYMODEENUMERATOR_H
