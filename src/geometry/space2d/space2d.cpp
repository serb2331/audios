#include "space2d.h"
#include "core/geometry/space2d/element2d.h"

Space2D::Space2D() : origin(Point2D::ZERO())
{
    this->oX = Line2D();
}