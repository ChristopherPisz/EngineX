#ifndef GFXAPPTIMER_H
#define GFXAPPTIMER_H

// OS Includes
#include <windows.h>

// Standard Includes
#include <deque>

//-----------------------------------------------------------------------------
class GFXAppTimer
{
public:
   
   /**
   * Constructor
   **/
   GFXAppTimer();

   /**
   * Deconstructor
   **/
   ~GFXAppTimer();


   /**
   * Start the timer
   **/
   void Start();

   /**
   * Called when an update cycle is completed to set the time to use in calculations
   **/
   void SetUpdateTime();


   /**
   * Get the total time, in seconds, since Start() was called
   **/
   double GetTotalTime() const;

   /**
   * Get the average time, in seconds, of the last 5 intervals between calls to SetUpdateTime()
   **/
   double GetLast5AvgTime() const;

   /**
   * Get the time, in seconds, between the last two calls to SetUpdateTime()
   **/
   double GetElapsedTime() const;

private:

   bool               m_started;             // Whether or not Start() was called
   DWORD              m_startTime;           // Time reading of the Start() call
   DWORD              m_lastUpdateTime;      // Time reading of the last SetUpdateTime() call
   
   double             m_last5AvgTime;        // Averaged time of the last 5 intervals between SetUpdateTime() calls 
   std::deque<double> m_lastUpdateDurations; // Last 5 intervals, in seconds, between SetUpdateTime() calls 
};

#endif
