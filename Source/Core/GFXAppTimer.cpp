

#include "GFXAppTimer.h"

//----------------------------------------------------------------------------
GFXAppTimer::GFXAppTimer()
    :
    m_started(false),
    m_startTime(0),
    m_lastUpdateTime(0)
{
    m_lastUpdateDurations.push_back(0);
}

//----------------------------------------------------------------------------
GFXAppTimer::~GFXAppTimer()
{
}

//----------------------------------------------------------------------------
void GFXAppTimer::Start()
{
    m_started = true;
    m_startTime = m_lastUpdateTime =  timeGetTime();
}

//----------------------------------------------------------------------------
void GFXAppTimer::SetUpdateTime()
{
    double elapsedTime = static_cast<double>(timeGetTime() - m_lastUpdateTime) / 1000.0; 
    m_lastUpdateTime = timeGetTime();

    m_lastUpdateDurations.push_back(elapsedTime);
   
    if( m_lastUpdateDurations.size() > 5 )
    {
        m_lastUpdateDurations.pop_front();
    }

    double sum = 0;

    for(std::deque<double>::const_iterator it = m_lastUpdateDurations.begin(); it != m_lastUpdateDurations.end(); ++it)
    {
        sum += *it;
    }

    m_last5AvgTime =  sum / static_cast<double>(m_lastUpdateDurations.size());
}

//----------------------------------------------------------------------------
double GFXAppTimer::GetTotalTime() const
{
    if( m_started )
    {
        // If the timer is flagged as started
        return static_cast<double>(timeGetTime() - m_startTime) / 1000.0;
    }
    else
    {
        return 0;
    }
}

//----------------------------------------------------------------------------
double GFXAppTimer::GetLast5AvgTime() const
{  
    return m_last5AvgTime;
}

//----------------------------------------------------------------------------
double GFXAppTimer::GetElapsedTime() const
{
   return m_lastUpdateDurations.back();
}

