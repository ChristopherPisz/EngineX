

#ifndef REFRESHRATE_H
#define REFRESHRATE_H

// Standard Includes
#include <string>


//------------------------------------------------------------------------------------------
/**
* RefreshRate
**/
struct RefreshRate
{
   /**
   * Constructor
   **/
   RefreshRate(const unsigned numerator, const unsigned denominator);

   /**
   * Copy Constructor
   **/
   RefreshRate(const RefreshRate & rhs);
   
   /**
   * Deconstructor
   **/
   ~RefreshRate();


   RefreshRate & operator = (const RefreshRate & rhs);
   bool operator == (const RefreshRate & rhs) const;
   bool operator != (const RefreshRate & rhs) const;
   bool operator < (const RefreshRate & rhs) const;
   bool operator > (const RefreshRate & rhs) const;
   bool operator <= (const RefreshRate & rhs) const;
   bool operator >= (const RefreshRate & rhs) const;


   /**
   * Forms a string of form "##.## Hz"
   **/
   const std::string AsString() const;

   /**
   * Get as a Hz value
   **/
   const double AsHertz() const;


   unsigned m_numerator;
   unsigned m_denominator;
};



#endif
