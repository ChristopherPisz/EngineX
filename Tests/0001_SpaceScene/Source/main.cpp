

#include "TestApp.h"
#include "GraphicsConfigDlg.h"

// Common Lib Includes
#include "BaseException.h"

// Windows Includes
#include <windows.h>

using namespace common_lib_cpisz;

//----------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR, INT)
{
   // Get display mode
   DisplayMode displayMode;

   try
   {
      GraphicsConfigDlg configDlg(displayMode); // The graphics config dialog
      configDlg.Run(instance);                  // Run the dialog
	}
   catch(BaseException & error)
   {
      error.MsgBox();
      return 1;
   }

   // Run the application
   try
   {
      TestApp app;                              // The application
      app.Init(instance, displayMode);          // Create the window
      app.Run();                                // Enter the main run loop
	}

   // Catch any errors that were thrown and display them
   catch(BaseException & error)
   {
      error.MsgBox();
      return 1;
   }

   return 0;
}