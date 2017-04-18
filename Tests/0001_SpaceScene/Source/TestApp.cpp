

#include "TestApp.h"

// EngineX Includes
#include "Graphics\3D\PolygonSetParser.h"
#include "Graphics\3D\Shapes.h"

// Common Lib Includes
#include "Exception"

// OS Includes
#include <windows.h>

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <cmath>

//---------------------------------------------------------------------------
TestApp::TestApp()
   :
   GFXApplication("Test Application"),
   m_modelDirectory("Not Set"),
   m_camera(NULL),
   m_sectorBackground(NULL),
   m_ship(NULL),
   m_asteroid(NULL),
   m_ui_crosshair(NULL)
{
}

//---------------------------------------------------------------------------
TestApp::~TestApp()
{
   FreeResources();
}

//---------------------------------------------------------------------------
void TestApp::Init(HINSTANCE instance, const DisplayMode & displayMode)
{
   // Get the paths to the resource directories
   char currentDir[MAX_PATH];
   GetCurrentDirectory(sizeof(currentDir), currentDir);

   std::string textureDirectory(currentDir);
   textureDirectory += "\\Textures";

   std::string effectDirectory(currentDir);
   effectDirectory += "\\HLSL";

   m_modelDirectory = currentDir;
   m_modelDirectory += "\\Models";
   
   // Initialize the base class
   GFXApplication::Init(instance, displayMode, textureDirectory, effectDirectory, "effectPool.fxh");
}

//----------------------------------------------------------------------------
void TestApp::InitResources()
{
   // Create a camera
   RECT rect;
   GetClientRect(m_hwnd, &rect);

   LONG client_width  = rect.right  - rect.left;
   LONG client_height = rect.bottom - rect.top; 
   
   m_camera = new FlightCamera(client_width, 
                               client_height,
                               0.1f,
                               1000.0f,
                               D3DXVECTOR3(  0.0f,  0.0f,    0.0f ),
                               D3DXVECTOR3(  0.0f,  0.0f,    1.0f ),
                               D3DXVECTOR3(  0.0f,  1.0f,    0.0f ));

   //-----
   // Create geometry to render
   std::string modelFilePath;

   // Create the sector background
   try
   {
      m_sectorBackground = new SectorBackground(*m_device, 
                                                *m_inputLayoutManager, 
                                                *m_textureManager, 
                                                *m_effectManager,
                                                *m_renderQueue,
                                                "nebula_bluedistance.dds",
                                                "nebula_bluedistance_stars_blue_diff_alpha.dds",
                                                "glow.png",
                                                "Not Implemented Yet",
                                                "flare1.png",
                                                "flare2.png",
                                                "flare3.png");
   }
   catch(BaseException & e)
   {
      throw e;
   }

   // Ship
   modelFilePath = m_modelDirectory + "\\Argon_M3.dat";

   try
   {
      PolygonSetParser parser(*m_device, *m_inputLayoutManager, *m_textureManager, *m_effectManager);
      
      parser.ParseFile(modelFilePath, false);
      m_ship = parser.GetPolygonSet(0).release();
      
      // I think it looked better without the specular map
      Material material = m_ship->GetMaterial();
      material.SetBool("specularMapped", false);
      material.SetFloat("specularExponent", 15.0f);
      m_ship->SetMaterial(material);
      
      m_ship->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 100.0f));
   }
   catch(BaseException e)
   {
      throw e;
   }

   // Asteroid
   modelFilePath = m_modelDirectory + "\\asteroid_A_ClassMine.dat";

   try
   {
      PolygonSetParser parser(*m_device, *m_inputLayoutManager, *m_textureManager, *m_effectManager);
      
      parser.ParseFile(modelFilePath, false);
      m_asteroid = parser.GetPolygonSet(0).release();
      
      // I think it looked better without the specular map
//    Material material = m_asteroid->GetMaterial();
//    material.SetBool("specularMapped", false);
//    material.SetFloat("specularExponent", 15.0f);
//    m_asteroid->SetMaterial(material);
      
      m_asteroid->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 600.0f));
   }
   catch(BaseException e)
   {
      throw e;
   }

   

   

   // Create the UI Crosshair
   try
   {
      m_ui_crosshair = new Image2D(*m_device,
                                   *m_inputLayoutManager, 
                                   *m_textureManager, 
                                   *m_effectManager,
                                   "\\UI\\crosshair.dds",
                                   1,
                                   1);

      m_ui_crosshair->SetDepth2D(0.1f);
   }
   catch(BaseException & e)
   {
      throw e;
   }
}

//----------------------------------------------------------------------------
void TestApp::PreRender()
{
   //-----
   // Handle any AI actions here
   
   const double shipRotationSpeed =   0.25;  // 360 degree rotations per second
   const double shipStrafeSpeed   =  50.00;  // Units per second
   const double shipFowardSpeed   = 100.00;  // units per second

   float angle = static_cast<float>(2.0 * D3DX_PI * shipRotationSpeed * GetElapsedTime());
   m_ship->ApplyRotation(angle, D3DXVECTOR3(1.0f, 0.0f, 0.0f));
   m_ship->ApplyRotation(angle, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
   m_ship->ApplyRotation(angle, D3DXVECTOR3(0.0f, 0.0f, 1.0f));

   // Handle user actions here
   HandleKeyboardInput();
}

//----------------------------------------------------------------------------
void TestApp::Render()
{
   //--------
   // TODO - Alot of the following should belong in a renderqueue class

   // Bind the view and projection matrices from the camera
   m_effectManager->BindCamera(m_camera);
      
   m_renderQueue->Render();

   m_ship->Render();
   m_asteroid->Render();
   m_ui_crosshair->Render();

   // DEBUG
   D3DXMATRIX viewMatrix = m_camera->GetViewMatrix();
   
   // ENDDEBUG

   // TODO - It is advisable to render all UI or 2D into a seperate render target
   //        and then render the result at a minimum depth, to keep UI from blending
   //        with the scene, while still supporting layering of UI elements
}

//----------------------------------------------------------------------------
void TestApp::FreeResources()
{
   // Release the UI crosshair
   if( m_ui_crosshair )
   {
      delete m_ui_crosshair;
      m_ui_crosshair = NULL;
   }

   // Release the Asteroid
   if( m_asteroid )
   {
      delete m_asteroid;
      m_asteroid = NULL;
   }

   // Release the Polygon Sets
   if( m_ship )
   {
      delete m_ship;
      m_ship = NULL;
   }

   if( m_sectorBackground )
   {
      delete m_sectorBackground;
      m_sectorBackground = NULL;
   }

   // Release the Camera
   if( m_camera )
   {
      delete m_camera;
      m_camera = NULL;
   }
}

//----------------------------------------------------------------------------
void TestApp::HandleKeyboardInput()
{
   const double cameraRotationSpeed =   0.25;  // 360 degree rotations per second
   const double cameraStrafeSpeed   =  50.00;  // Units per second
   const double cameraFowardSpeed   = 100.00;  // units per second

   float cameraStrafeDistance = static_cast<float>(cameraStrafeSpeed * GetLast5AvgTime());
   float cameraRotationAngle  = static_cast<float>(2.0 * D3DX_PI * cameraRotationSpeed * GetLast5AvgTime());

   // ESC
   if( m_keystates[VK_ESCAPE] )
   {
      PostQuitMessage(0);
   }

   // W
   if( m_keystates[0x57] )
   {
      m_camera->ApplyTranslation( cameraStrafeDistance, D3DXVECTOR3( 0, 1, 0), true);
   }

   // S
   if( m_keystates[0x53] )
   {
      m_camera->ApplyTranslation( cameraStrafeDistance, D3DXVECTOR3( 0, -1, 0), true);
   }

   // A
   if( m_keystates[0x41] )
   {
      m_camera->ApplyTranslation( cameraStrafeDistance, D3DXVECTOR3( -1, 0, 0), true);
   }

   // D
   if( m_keystates[0x44] )
   {
      m_camera->ApplyTranslation( cameraStrafeDistance, D3DXVECTOR3( 1, 0, 0), true);
   }
   
   // Q
   if( m_keystates[0x51] )
   {
      m_camera->ApplyRotation(cameraRotationAngle, D3DXVECTOR3( 0, 0, 1));
   }

   // E
   if( m_keystates[0x45] )
   {
      m_camera->ApplyRotation(cameraRotationAngle, D3DXVECTOR3( 0, 0, -1));
   }

   // Left Arrow
   if( m_keystates[VK_LEFT] )
   {
      m_camera->ApplyRotation(cameraRotationAngle, D3DXVECTOR3( 0, -1, 0));
   }

   // Right Arrow
   if( m_keystates[VK_RIGHT] )
   {
      m_camera->ApplyRotation(cameraRotationAngle, D3DXVECTOR3( 0, 1, 0));
   }
 
   // Up Arrow
   if( m_keystates[VK_UP] )
   {
      m_camera->ApplyRotation(cameraRotationAngle, D3DXVECTOR3( 1, 0, 0));
   }

   // Down Arrow
   if( m_keystates[VK_DOWN] )
   {
      m_camera->ApplyRotation(cameraRotationAngle, D3DXVECTOR3( -1, 0, 0));
   }
}

//----------------------------------------------------------------------------
void TestApp::OnSizeChange(const unsigned clientWidth, const unsigned clientHeight)
{
   if( m_camera )
   {
      m_camera->SetPerspectiveMatrix(clientWidth, clientHeight, 0.1f, 1000.0f);
   }
}
