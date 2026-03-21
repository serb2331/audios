#pragma once

#include "element2d.h"
#include "core/interfaces/geometry.h"

#include <memory>
#include <vector>

class Space2D : public ISpace
{
private:
    std::vector<Element2D*> elements;
    Point2D origin;
    Line2D oX, oY;

public:
    Space2D();

    Space2D(const Point2D &origin, const Line2D &oX, const Line2D &oY);

    void addElement(Element2D &el);

    static std::shared_ptr<Space2D> CreateCartesian()
    {
        return std::make_shared<Space2D>();
    }


    IGeometryElement getOrigin() override;
    IGeometryElement getAxes() override;
};