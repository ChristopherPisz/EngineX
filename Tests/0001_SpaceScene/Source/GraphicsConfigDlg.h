
#ifndef GRAPHICSCONFIGDLG_H
#define GRAPHICSCONFIGDLG_H

// EngineX Includes
#include "Core\DisplayModeEnumerator.h"
#include "Core\DisplayMode.h"

// Common Lib Includes
#include "BaseModalDialog.h"


//--------------------------------------------------------------------------------
class GraphicsConfigDlg : public Common::BaseModalDialog
{
public:

   /**
   * Constructor
   */
   GraphicsConfigDlg(DisplayMode & result);
	
   /**
   * Deconstructor
   */
   ~GraphicsConfigDlg();

   /**
   * Creates the dialog
   *
   * @param instance - instance of the application that owns the dialog
   * @param templateName - name of an included dialog template resource
   * @param parent - handle to the parent window of the dialog
   *
   * @return int - any value returned from EndDialog when the dialog exits
   *
   **/
   INT_PTR Run(HINSTANCE instance);

private:

   /**
   * Initializes all the controls in the dialog
   **/
   void InitComponents();

   /**
   * The following methods serve multiple purposes:
   *   Each control gets its current selection and validates it
   *   Each control initializes itself if it hasn't been initialized or if the current selection is invalid
   *   Each control will call the 'OnChanged' method of its dependant controls to perform the same actions
   **/
   void OnAdapterChanged();
   void OnMonitorChanged();
   void OnResolutionFullscreenChanged();
   void OnResolutionWindowedChanged();
   void OnRefreshRateChanged();
   void OnMultisampleCountChanged();
   void OnMultisampleQualityChanged();
   
   /**
   * Creates the display mode to match the user's current selections
   */
   void FinalizeDisplayMode();

   /**
   * Graphics Config Dialog Procedure
   */
   INT_PTR CALLBACK ModalDlgProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
   


   /**
   * Enumerates all display mode capabilities
   **/
   DisplayModeEnumerator m_displayModeEnumerator;
   
   /**
   * The display mode that is a result of the users selections
   **/
   DisplayMode & m_displayMode;

   // Current lists displayed in the dialog combo boxes
   unsigned                                    m_numAdapters;
   unsigned                                    m_numMonitors;
   std::vector<Resolution>                     m_resolutionsFullscreen;
   std::vector<Resolution>                     m_resolutionsWindowed;
   std::vector<RefreshRate>                    m_refreshRates;
   std::vector<std::pair<unsigned, unsigned> > m_multisamplingOptions;

   /**
   * Comparator to determine if a multisample count is in the collection of multisampling options
   **/
   struct HasMultisampleCount
   {
      HasMultisampleCount(const unsigned multisampleCount);
      bool operator() (const std::pair<unsigned, unsigned> & multisampleOption) const;

      unsigned m_multisampleCount;
   };

   //-----
   // Dialog Controls
   //-----
   HWND m_cmbAdapters;              // Combo Box listing available adapters
   HWND m_cmbMonitors;              // Combo Box listing available monitors
   HWND m_chkWindowed;              // Check Box enabling or disabling windowed mode
   HWND m_cmbResolutionsWindowed;   // Combo Box listing available resolutions in windowed mode
   HWND m_cmbResolutionsFullscreen; // Combo Box listing available resolutions in fullscreen mode
   HWND m_cmbRefreshRates;          // Combo Box listing available refresh rates
   HWND m_cmbMultisampleCount;      // Combo Box listing available multisample counts
   HWND m_cmbMultisampleQuality;    // Combo Box listing available multisample quality levels 
};


#endif