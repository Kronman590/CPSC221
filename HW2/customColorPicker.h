/**
 * @file customColorPicker.h
 * Definition of a custom color picker.
 *
 */
#ifndef CUSTOMCOLORPICKER_H
#define CUSTOMCOLORPICKER_H

#include "colorPicker.h"

/**
 * customColorPicker: a functor that determines the color that should be used
 * given an x and a y coordinate using a custom pattern.
 *
 */
class customColorPicker : public colorPicker
{
  public:
    customColorPicker(HSLAPixel fillColor, PNG & img);
    virtual HSLAPixel operator()(int x, int y);

  private:
    HSLAPixel color; /**< Color used for the custom. */
    int middle_x;
    int middle_y;
};

#endif
