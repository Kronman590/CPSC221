#include "borderColorPicker.h"

borderColorPicker::borderColorPicker(HSLAPixel fillColor, PNG & img, double tolerance,HSLAPixel center)
{
    color = fillColor;
    im = img;
    tol = tolerance;
    ctr = center;
}

HSLAPixel borderColorPicker::operator()(int x, int y)
{
    for(int i = x - 3; i <= x + 3; i++) {
        for(int j = y - 3; j <= y + 3; j++) {
            // See if it is within the circle of radius 3
            if((x-i)*(x-i)+(y-j)*(y-j) <= 9) {
                // See if it is within the image
                if(i < 0 || i >= (int)im.width() || j < 0 || j >= (int)im.height())
                    return color;
                // See if it is within the tolerance
                HSLAPixel *pixel = im.getPixel(i, j);
                if(pixel->dist(ctr) > tol)
                    return color;
            }
        }
    }

    return *im.getPixel(x, y);
}
