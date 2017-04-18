

// Project Includes
#include "Resolution.h"

// Common Lib Includes
#include "Exception.h"

// Standard Includes
#include <sstream>

//------------------------------------------------------------------------------------------
Resolution::Resolution(const unsigned width, const unsigned height)
    :
    m_width(width),
    m_height(height)
{
}

//------------------------------------------------------------------------------------------
Resolution::Resolution(const std::string & resolution)
{
    FromString(resolution);
}

//------------------------------------------------------------------------------------------
Resolution::Resolution(const Resolution & rhs)
    :
    m_width(rhs.m_width),
    m_height(rhs.m_height)
{
}

//------------------------------------------------------------------------------------------
Resolution::~Resolution()
{
}

//------------------------------------------------------------------------------------------
Resolution & Resolution::operator = (const Resolution & rhs)
{
    m_width  = rhs.m_width;
    m_height = rhs.m_height;

    return *this;
}

//------------------------------------------------------------------------------------------
bool Resolution::operator == (const Resolution & rhs) const
{
    if( m_width != rhs.m_width )
    {
        return false;
    }

    if( m_height != rhs.m_height )
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
bool Resolution::operator != (const Resolution & rhs) const
{
    return !(*this == rhs);
}

//------------------------------------------------------------------------------------------
bool Resolution::operator < (const Resolution & rhs) const
{
    if( m_width < rhs.m_width )
    {
        return true;
    }

    if( m_width > rhs.m_width )
    {
        return false;
    }


    if( m_height < rhs.m_height )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------
bool Resolution::operator > (const Resolution & rhs) const
{
    if( m_width > rhs.m_width )
    {
        return true;
    }

    if( m_width < rhs.m_width )
    {
        return false;
    }


    if( m_height > rhs.m_height )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------
bool Resolution::operator <= (const Resolution & rhs) const
{
    if( *this > rhs )
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
bool Resolution::operator >= (const Resolution & rhs) const
{
    if( *this < rhs )
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------
const std::string Resolution::AsString() const
{
    std::stringstream s;
    s << m_width << "X" << m_height;

    return s.str();
}

//------------------------------------------------------------------------------------------
void Resolution::FromString(const std::string & resolution)
{
    std::string temp(resolution);

    std::string::size_type index = temp.find('X');
    if( index == std::string::npos )
    {
        index = temp.find('x');
        if( index == std::string::npos )
        {
            std::ostringstream msg;
            msg << "String is of invalid form: '" << resolution << "'";
            throw Common::Exception(__FILE__, __LINE__, msg.str());
        }
    }
   
    temp.replace(index, 1, 1, ' ');

    std::stringstream converter(temp);
    if( !(converter >> m_width) )
    {
        std::ostringstream msg;
        msg << "Failed to extract width from string '" << resolution << "'";
        throw Common::Exception(__FILE__, __LINE__, msg.str());
    }

    if( !(converter >> m_height) )
    {
        std::ostringstream msg;
        msg << "Failed to extract height from string '" << resolution << "'";
        throw Common::Exception(__FILE__, __LINE__, msg.str());
    }
}

