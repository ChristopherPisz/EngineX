
#ifndef DISPLAYMODE_H
#define DISPLAYMODE_H

// EngineX Includes
#include "Resolution.h"
#include "RefreshRate.h"

// Windows Includes
#include <dxgi.h>

//------------------------------------------------------------------------------------------
/**
* Display mode settings
**/
struct DisplayMode
{
   DisplayMode();

   DisplayMode(const unsigned adapterIndex,
               const unsigned monitorIndex,
               const bool fullscreen,
               const Resolution & resolutionFullscreen,
               const Resolution & resolutionWindowed,
               const RefreshRate & refreshRate,
               const unsigned multisamplingCount,
               const unsigned multisamplingQuality,
               const DXGI_FORMAT backbufferFormat);

   DisplayMode(const DisplayMode & rhs);

   ~DisplayMode();

   DisplayMode & operator = (const DisplayMode & rhs);

   bool operator == (const DisplayMode & rhs) const;

   bool operator != (const DisplayMode & rhs) const;


   unsigned    m_adapterIndex;
   unsigned    m_monitorIndex;
   bool        m_fullscreen;
   Resolution  m_resolutionFullscreen;
   Resolution  m_resolutionWindowed;

   RefreshRate m_refreshRate;
   unsigned    m_multisamplingCount;
   unsigned    m_multisamplingQuality;
   DXGI_FORMAT m_backbufferFormat;
};

#endif