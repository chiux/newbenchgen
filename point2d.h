#ifndef NTUCOD_POINT2D_H
#define NTUCOD_POINT2D_H
#include <cmath>

class Point2D {
  public:
    // Constructors
    Point2D ()
      {}
    Point2D (double x, double y)
      : _x(x), _y(y)
      {}
    Point2D (const Point2D& p)
      : _x(p.getx()), _y(p.gety())
      {}
    // Compare Operators
    bool p2d_eq (double a, double b) const { return std::abs(a-b) < 0.000000001; }
    bool operator< (const Point2D& p) const {
        return (this->_x != p._x) ? (this->_x < p._x) : (this->_y < p._y);
    }
    bool operator== (const Point2D& p) const {
        return ( p2d_eq(this->_x, p._x) && p2d_eq(this->_y, p._y) );
    }
    // Set Functions
    void   setx (double x) { _x = x; }
    void   sety (double y) { _y = y; }
    // Get Functions
    double getx () const { return _x; }
    double gety () const { return _y; }
    double x () const { return _x; }
    double y () const { return _y; }
    
    // Data Members
  private:
    double _x;
    double _y;
};

#endif