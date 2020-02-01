#include <cmath>
#include "customColorPicker.h"

static const int fib[] = {0, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418, 317811};

customColorPicker::customColorPicker(HSLAPixel fillColor, PNG & img)
{
    color = fillColor;
    middle_x = img.width()/2;
    middle_y = img.height()/2;
}

HSLAPixel customColorPicker::operator()(int x, int y)
{
    bool even_x = false;
    bool even_y = false;
    int x_dist = abs(x - middle_x);
    int y_dist = abs(y - middle_y);
    if(x == middle_x) {
        even_x = true;
    }
    else {
        for(int n = 0; n < 26; n++) {
            int fib_n = fib[n];
            int fib_n_n = fib[n+1];
            if(x_dist > fib_n && x_dist <= fib_n_n) {
                //if n is even
                if(n % 2 == 0)
                    even_x = true;
                else 
                    even_x = false;
                break;
            }
        }   
    }

    if(y == middle_y) {
        even_y = true;
    }
    else {
        for(int n = 0; n < 26; n++) {
            int fib_n = fib[n];
            int fib_n_n = fib[n+1];
            if(y_dist > fib_n && y_dist <= fib_n_n) {
                //if n is even
                if(n % 2 == 0)
                    even_y = true;
                else 
                    even_y = false;
                break;
            }
        }   
    }
    if(even_x == even_y)
        return color;
    return HSLAPixel();
}
