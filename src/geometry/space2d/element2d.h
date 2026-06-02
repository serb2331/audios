#pragma once

#include "core/interfaces/geometry.h"

#include <utility>

class Element2D : public IGeometryElement
{

};

class Point2D : public Element2D
{
private:
    std::pair<double, double> coords;

public:
    Point2D();
    Point2D(double x, double y);

    static Point2D ZERO();
};

class Line2D : public Element2D
{
private:
    std::pair<Point2D, Point2D> points;

public:
    Line2D();
    Line2D(std::pair<Point2D, Point2D> points);
    
};