#include "stripeColorPicker.h"

stripeColorPicker::stripeColorPicker(HSLAPixel fillColor, int stripeSpacing)
{
    color = fillColor;
    spacing = stripeSpacing;
}

HSLAPixel stripeColorPicker::operator()(int x, int y)
{
    // If x is a multiple of the spacing
    if(x % spacing == 0) {
        return color;
    }
    return HSLAPixel();
}
