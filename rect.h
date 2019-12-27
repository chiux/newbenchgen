#ifndef NTUCOD_RECT_H
#define NTUCOD_RECT_H
#include "point2d.h"

class Rect {
  public:
    Rect() {}
    // Construct from specified values
    Rect (double left_x, double bottom_y, double right_x, double top_y)
         : _left_x(left_x), _bottom_y(bottom_y), _right_x(right_x), _top_y(top_y)
         { orderCorrect(); }
    // Construct from an existing Rect object
    Rect (const Rect& r)
         : _left_x(r._left_x), _bottom_y(r._bottom_y), _right_x(r._right_x), _top_y(r._top_y)
         { orderCorrect(); }
    // Set Functions
    void   setLx (double lx) { _left_x = lx; }                          // Set the x position of left boundary
    void   setBy (double by) { _bottom_y = by; }                        // Set the y position of bottom boundary
    void   setRx (double rx) { _right_x = rx; }                         // Set the x position of right boundary
    void   setTy (double ty) { _top_y = ty; }                           // Set the y position of top boundary
    void   setRect (const Rect& r);                                     // Set all positions from a Rect object
    // Get Functions
    double getLx () const { return _left_x; }                           // Get the x position of left boundary
    double getBy () const { return _bottom_y; }                         // Get the y position of bottom boundary
    double getRx () const { return _right_x; }                          // Get the x position of right boundary
    double getTy () const { return _top_y; }                            // Get the y position of top boundary
    Point2D getLBPoint() const { return Point2D(_left_x, _bottom_y); }  // Get a Point2D of the left-bottom point
    Point2D getLTPoint() const { return Point2D(_left_x, _top_y); }     // Get a Point2D of the left-top point
    Point2D getRBPoint() const { return Point2D(_right_x, _bottom_y); } // Get a Point2D of the right-bottom point
    Point2D getRTPoint() const { return Point2D(_right_x, _top_y); }    // Get a Point2D of the right-top point
    double getCx () const { return (_left_x + _right_x)/2; }            // Get the center x position
    double getCy () const { return (_bottom_y + _top_y)/2; }            // Get the center y position
    double getWidth() const { return _right_x - _left_x; }              // Get the x range
    double getHeight() const { return _top_y - _bottom_y; }             // Get the y range
    Rect   getRect() const { return *this; }                            // Get this Rect object
    Rect   getOverlapRectLoose(const Rect& r);
    // Application Set Functions
    void   shift (double offset_x, double offset_y); // Move the positions of this Rect by an x and a y offset value (negative value is okay)
    void   absorb (const Rect& r);                   // Absorb another Rect r and change the area of this Rect to their bounding box
    void   orderCorrect();                           // [CHECK FUNCTION] check and correct if (ty < by) or (rx < lx)
    // Application Judge(bool) Functions
    //   - Check if a Point2D is in the interier
    bool   isInterierPoint (const Point2D& p2d) const {
        return ((p2d.getx() > _left_x) && (p2d.getx() < _right_x) && (p2d.gety() > _bottom_y) && (p2d.gety() < _top_y));
    }
    //   - Check if a Point2D is in the interier, or on the boundary
    bool   isCoveredPoint (const Point2D& p2d) const {
        return ((p2d.getx() >= _left_x) && (p2d.getx() <= _right_x) && (p2d.gety() >= _bottom_y) && (p2d.gety() <= _top_y));
    }
    //   - (TODO improve) Check if a vertical/horizonatal segment (in Rect form) is fully located on boundary
    bool isCoveredInBoundary (const Rect& r) const;
    //   - Check if a Rect is overlap (edge or point touching is also overlap)
    bool isOverlapRect (const Rect& r) const;
    //   - Check if a Rect is strictly overlap (overlap area > 0)
    bool isStrictOverlapRect (const Rect& r) const;
    
    double _left_x;
    double _bottom_y;
    double _right_x;
    double _top_y;
};

#endif