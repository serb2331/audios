#pragma once

class IGeometryElement
{

};

class ISpace
{
public:
    virtual IGeometryElement getOrigin() = 0;
    virtual IGeometryElement getAxes() = 0;
};