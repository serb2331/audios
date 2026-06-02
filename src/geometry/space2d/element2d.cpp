#include "element2d.h"
#include <utility>

Point2D::Point2D() {}

Point2D::Point2D(double x, double y)
{
    this->coords = {x, y};
}

Point2D Point2D::ZERO()
{
    Point2D ret_p(0, 0);
    return ret_p;
}

Line2D::Line2D() {}

Line2D::Line2D(std::pair<Point2D, Point2D> p)
{
    this->points = p;
}