#ifndef GFXAPPLICATION_H
#define GFXAPPLICATION_H

// EngineX Includes
#include "GFXAppTimer.h"
#include "DisplayModeEnumerator.h"
#include "DisplayMode.h"
#include "Graphics\Textures\TextureManager.h"
#include "Graphics\Effects\EffectManager.h"
#include "Graphics\3D\InputLayoutManager.h"
#include "Graphics\3D\RenderQueue.h"

// Common Lib Includes
#include "BaseWindow.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>


// Standard Includes
#include <fstream>

//------------------------------------------------------------------------------------------
class GFXApplication : public Common::BaseWindow
{
public:
   
   /**
   * Constructor
   *
   * @param title - Title to be given to the window that will contain the application
   **/
   GFXApplication(const std::string & title);
	
   /**
   * Deconstructor
   **/
   virtual ~GFXApplication();
	

   /**
   * Initializes the application window and D3D interfaces
   *
   * @param instance           - Instanced assigned to the process
   * @param displayMode        - Display settings to initially use
   * @param textureDirectory   - Path to the directory that contains texture files
   * @param effectDirectory    - Path to the directory that contains D3D effect files
   * @param effectPoolFileName - Name of the D3D effect pool .fxh file
   *
   * @throws BaseException - If initialization failed
   **/
   void Init(HINSTANCE instance, 
             const DisplayMode & displayMode,
             const std::string & textureDirectory,
             const std::string & effectDirectory,
             const std::string & effectPoolFileName);

   /**
   * Starts the application run loop
   */
   virtual int Run();

protected:
	
   /**
   *
   */
   virtual void InitResources(){}

   /**
   *
   */
   virtual void PreRender(){};
	
   /**
   *
   */
   virtual void Render() = 0;
	
   /**
   *
   */
   virtual void PostRender(){};
	
   /**
   *
   */
   virtual void FreeResources(){};


   /**
   * The client area of the window being rendered to has changed
   * 
   * Handle any appropriate changes. For example the perspective matrices of all cameras need to change.
   **/
   virtual void OnSizeChange(const unsigned clientWidth, const unsigned clientHeight) = 0;


   /**
   * Gets the time, in seconds, since the application start rendering
   **/
   double GetTotalTime() const;

   /**
   * Get the average time, in seconds, of the last 5 intervals between updates
   **/
   double GetLast5AvgTime() const;

   /**
   * Gets the time, in seconds, between the last two updates
   **/
   double GetElapsedTime() const;


   DisplayModeEnumerator      m_displayModeEnumerator;
   DisplayMode                m_displayMode;
   IDXGIFactory *             m_dxgiFactory;
   ID3D10Device_UniquePtr     m_device;             // D3D device
   IDXGISwapChain *           m_swapChain;
   ID3D10Texture2D *          m_backBuffer;
   ID3D10RenderTargetView *   m_renderTargetView;
   ID3D10Texture2D *          m_depthStencil;
   ID3D10DepthStencilState *  m_depthStencilState;
   ID3D10DepthStencilView *   m_depthStencilView;
   ID3D10RasterizerState *    m_rasterizerState;

   bool                       m_ignoreSizeChange;   // Whether to temporarily ignore buffer resizing from WM_SIZE messages

   TextureManager *           m_textureManager;     // Contains and manages D3D Textures
   EffectManager *            m_effectManager;      // Contains and manages D3D Effects along with variables shared amongst them
   InputLayoutManager *       m_inputLayoutManager; // Contains and manages D3D Input Layouts
   RenderQueue *              m_renderQueue;        // Contains objects to be rendered and handles sorting them

   bool                       m_keystates[256];     // True if a key is down, false if up. Indices correspond to windows virtual keycodes.

private:

   /**
   *
   */
   LRESULT WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool & callDefWndProc);

   /**
   *
   */
	void HandleSizeChange(const unsigned clientWidth, const unsigned clientHeight);
	
   /**
   *
   */
   void RenderLoop();


   GFXAppTimer m_timer;
};

#endif