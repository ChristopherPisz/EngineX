#ifndef TESTAPP_H
#define TESTAPP_H

#include "SectorBackground.h"

// Engine X Includes
#include "Core\GFXApplication.h"
#include "Graphics\Cameras\FlightCamera.h"
#include "Graphics\2D\Image2D.h"
#include "Graphics\3D\SkyBox.h"
#include "Graphics\3D\PolygonSet.h"
#include "Graphics\Lights\AmbientLight.h"

//-----------------------------------------------------------------------
class TestApp : public GFXApplication
{
public:
   
   TestApp();
   ~TestApp();

   /**
   *
   **/
   void Init(HINSTANCE instance, const DisplayMode & displayMode);

protected:

   void InitResources();
	void PreRender();
   void Render();
   void PostRender(){}
   void FreeResources();

   /**
   * Handle keyboard input
   **/
   void HandleKeyboardInput();

   /**
   * Handle screen resizing
   **/
   void OnSizeChange(const unsigned clientWidth, const unsigned clientHeight);

private:

   std::string         m_modelDirectory;    // Directory that contains all model files
   FlightCamera *      m_camera;            // Camera to look through
   PolygonSet   *      m_ship;              // The model to be rendered
   PolygonSet   *      m_asteroid;          // A large asteroid
   SectorBackground *  m_sectorBackground;  
   Image2D      *      m_ui_crosshair;      // UI crosshair
};


#endif
