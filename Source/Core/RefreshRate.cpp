

#include "RefreshRate.h"

// Common Lib Includes
#include "Exception.h"

// Standard Includes
#include <sstream>

//------------------------------------------------------------------------------------------
RefreshRate::RefreshRate(const unsigned numerator, const unsigned denominator)
    :
    m_numerator(numerator),
    m_denominator(denominator)
{
}

//------------------------------------------------------------------------------------------
RefreshRate::RefreshRate(const RefreshRate & rhs)
    :
    m_numerator(rhs.m_numerator),
    m_denominator(rhs.m_denominator)
{
}

//------------------------------------------------------------------------------------------
RefreshRate::~RefreshRate()
{
}

//------------------------------------------------------------------------------------------
RefreshRate & RefreshRate::operator = (const RefreshRate & rhs)
{
    m_numerator   = rhs.m_numerator;
    m_denominator = rhs.m_denominator;

    return *this;
}

//------------------------------------------------------------------------------------------
bool RefreshRate::operator == (const RefreshRate & rhs) const
{
    if( this == &rhs )
    {
        return true;
    }

    if( m_numerator != rhs.m_numerator )
    {
        return false;
    }

    if( m_denominator != rhs.m_denominator )
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
bool RefreshRate::operator != (const RefreshRate & rhs) const
{
    return !(*this == rhs);
}

//------------------------------------------------------------------------------------------
bool RefreshRate::operator < (const RefreshRate & rhs) const
{
    if( AsHertz() < rhs.AsHertz() )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------
bool RefreshRate::operator > (const RefreshRate & rhs) const
{
    if( AsHertz() > rhs.AsHertz() )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------
bool RefreshRate::operator <= (const RefreshRate & rhs) const
{
    if( *this > rhs )
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
bool RefreshRate::operator >= (const RefreshRate & rhs) const
{
    if( *this < rhs )
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
const std::string RefreshRate::AsString() const
{
    std::stringstream s;
    s << AsHertz() << " Hz";

    return s.str();
}

//------------------------------------------------------------------------------------------
const double RefreshRate::AsHertz() const
{
    return (static_cast<double>(m_numerator) / static_cast<double>(m_denominator));
}

