#ifndef IMAGE2D_H
#define IMAGE2D_H

// EngineX Includes
#include "Graphics/3D/Transform.h"
#include "Graphics/3D/Buffers.h"
#include "Graphics/3D/InputLayoutManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Effects/EffectManager.h"
#include "Graphics/Effects/Material.h"

// Common Lib Includes
#include "Timer.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <vector>
#include <string>

//----------------------------------------------------------------------------
/**
*
**/
class Image2D : protected Transform
{
public:

   /**
   * Defines the current frame index used each Render call
   **/
   enum AnimationMode
   {
      ANIMATION_MODE_NONE = 0,   // No animation is in progress. User may set current frame index themselves
      ANIMATION_MODE_ONCE,       // Animation counts up from start index to end index and stops
      ANIMATION_MODE_LOOP,       // Animation counts up from start index to end index, then starts again
      ANIMATION_MODE_BOUNCE,     // Animation counts up from start index to end index, then counts back down
   };
   

   /**
   * Constructor
   *
   * Creates an image that is composed of a texture rendered on top of a quad
   * The quad will have a default width and height, in pixels, matching the dimensions of the image file.
   *
   * If the image contains multiple frames, it can be divided into its frames using the rows and columns
   * parameters. In this case, the image will have a default width and height, in pixels, matching the
   * dimensions of a single frame.
   *
   * @param device             -
   * @param inputLayoutManager -
   * @param textureManager     -
   * @param effectManager      -
   * @param textureFilePath    - File path to the image, relative to the directory path the textureManager has been created with
   * @param rows               - For images that contain more than 1 frame, how many rows of frames the image contains
   * @param cols               - For images that contain more than 1 frame, how many columns of frames the image contains
   *
   * @throws BaseException - If image creation fails
   **/
   Image2D(ID3D10Device & device, 
           InputLayoutManager & inputLayoutManager,
           TextureManager & textureManager, 
           EffectManager & effectManager,
           const std::string & textureFilePath,
           unsigned rows,
           unsigned cols);
   
   /**
   * Deconstructor
   **/
   virtual ~Image2D();

   /**
   * Gets the number of frames the current image contains
   *
   * @return unsigned - Number of frames the current image contains
   **/
   virtual unsigned GetNumFrames() const;

   /**
   * Gets which frame of the image will be rendered
   *
   * @return unsigned - Frame index (zero based) that will be rendered
   **/
   virtual unsigned GetCurrentFrameIndex() const;

   /**
   * Sets which frame of the image will be rendered
   *
   * Note - If an animation is currently in progress, calling this method will stop
   *        the animation and set the current frame to the index argument
   *
   * @param index - Frame index (zero based) to render
   * 
   * @throws BaseException - If the frame does not exist
   **/
   virtual void SetCurrentFrameIndex(unsigned index);


   /**
   * Starts an animation
   *
   * @param startFrameIndex - Start frame index of the animation
   * @param endFrameIndex   - End frame index of the animation
   * @param animationMode   - See Image2D::AnimationMode enumeration
   * @param fps             - How many frame per second to advance the animation
   *
   * @throws BaseException - If the passed indices are invalid
   **/
   virtual void StartAnimation(unsigned startFrameIndex, unsigned endFrameIndex, AnimationMode animationMode, double fps);

   /**
   * Stops an animation
   *
   * NOTE - The current frame to be rendered will continue to be the last frame rendered
   **/
   virtual void StopAnimation();


   /**
   * Renders the Image
   *
   * @throws BaseException - If the image fails to render. Exception message will contain a reason. 
   **/
   virtual void Render();


   /**
   * Gets the position of the image
   *
   * @return D3DXVECTOR2 - Position of the image, in in pixels, where (0,0) is the center of the screen
   **/
   const D3DXVECTOR2 GetPosition2D() const;

   /**
   * Gets the depth of the image
   *
   * return float - Depth between 0.0 and 1.0, to use when rendering the image
   **/
   const float GetDepth2D() const;

   /**
   * Gets the scale of the image
   *
   * @return D3DXVECTOR2 - Scale of the image in the x and y direction
   **/
   const D3DXVECTOR2 GetScale2D() const;

   /**
   * Sets the position of the image
   *
   * @param position - Position of the image, in in pixels, where (0,0) is the center of the screen
   **/
   virtual void SetPosition2D(const D3DXVECTOR2 & position);

   /**
   * Sets the depth of the image
   *
   * The image is drawn in 2D having a depth range of 0.0 to 1.0
   * A depth of 0 will render the image closest to the viewer.
   * A depth of 0.99999999 will render the image farthest from the viewer.
   * Anything outside that range, will be clipped.
   *
   * @param depth - Depth between 0.0 and 1.0, to use when rendering the image
   **/
   virtual void SetDepth2D(const float depth);

   /**
   * Sets the scale of the image
   *
   * @param scale - Scale of the image in the x and y direction
   **/
   virtual void SetScale2D(const D3DXVECTOR2 & scale);

protected:

   /**
   * Calculates which frame of an animation is the current frame to render
   *
   * NOTE - If no animation is in progress, this method does nothing
   **/
   virtual void Image2D::CalculateAnimationFrame();


   ID3D10Device &                 m_device;              // DirectX device
   InputLayoutManager &           m_inputLayoutManager;  // Contains and manages the lifetime of input layouts
   TextureManager &               m_textureManager;      // Contains and manages the lifetime of textures
   EffectManager &                m_effectManager;       // Contains and manages the lifetime of effects
   
   std::vector<Buffer::SharedPtr> m_quadBuffers;         // Vertex buffers containing quads to render image frame to
   ID3D10InputLayout *            m_inputLayout;         // DirectX description of the vertex buffers
   std::vector<unsigned>          m_strides;             // Number of bytes of an element of a vertex buffer. 1 per buffer
   std::string                    m_textureName;         // Name of the texture that contains the image frames
   unsigned                       m_numFrames;           // Number of frames in the image

   std::auto_ptr<Material>        m_material;            // Effect variable states to use when rendering
   float                          m_width;               // Width of each frame, in pixels
   float                          m_height;              // Height of each frame, in pixels
   float                          m_depth;               // 0 to 1, depth to render the frame
   
   common_lib_cpisz::Timer        m_animationTimer;      // Keeps track of time passed between frames
   AnimationMode                  m_animationMode;       // If and how to set the frame index every Render call
   unsigned                       m_startFrameIndex;     // Start frame of an animation
   unsigned                       m_endFrameIndex;       // End frame of an animation
   unsigned                       m_currentFrameIndex;   // Current frame to be rendered
   double                         m_fps;                 // Frames per second of the animation
};

#endif // IMAGE2D
