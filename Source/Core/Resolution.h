

#ifndef RESOLUTION_H
#define RESOLUTION_H

// Standard Includes
#include <string>


//------------------------------------------------------------------------------------------
/**
* Resolution
**/
struct Resolution
{
   /**
   * Constructor
   **/
   Resolution(const unsigned width, const unsigned height);
   
   /**
   * Constructor
   *
   * @param resolution - string of form "WidthXHeight"
   *
   * @throws BaseException - If the string was not valid
   **/
   Resolution(const std::string & resolution);

   /**
   *
   **/
   Resolution(const Resolution & rhs);
   
   /**
   *
   **/
   ~Resolution();


   Resolution & operator = (const Resolution & rhs);
   bool operator == (const Resolution & rhs) const;
   bool operator != (const Resolution & rhs) const;
   bool operator < (const Resolution & rhs) const;
   bool operator > (const Resolution & rhs) const;
   bool operator <= (const Resolution & rhs) const;
   bool operator >= (const Resolution & rhs) const;


   /**
   * Forms a string of form "WidthXHeight"
   **/
   const std::string AsString() const;

   /**
   * Sets the resolution from a string of form "WidthXHeight"
   **/
   void FromString(const std::string & resolution);


   unsigned m_width;
   unsigned m_height;
};



#endif
