#include "block.h"

/*From im, grabs the vertical strip of pixels whose upper left corner is at position (column,0), 
  and whose width is width.*/
void Block::build(PNG & im, int column, int width) {
	data.resize(im.height());
	for (unsigned i = 0; i < im.height(); i++) {
		vector<HSLAPixel> xdata; 
		xdata.resize(width);
		for (int j = column; j < column+width; j++) {
			xdata[j-column] = *(im.getPixel(j,i));
		}
		data[i] = xdata;
	}
}
/*Draws the current block at position (column,0) in im*/
void Block::render(PNG & im, int column) const {
	for (unsigned i = 0; i < im.height(); i++) {
		vector<HSLAPixel> xdata = data[i]; 
		for (unsigned long j = column; j < column+xdata.size(); j++) {
			*(im.getPixel(j,i)) = xdata[j-column]; 
		}
	}
}
/*This function changes the saturation of every pixel in the block to 0, 
  which removes the color, leaving grey.*/
void Block::greyscale() {
	for (unsigned i = 0; i < data.size(); i++) {
		vector<HSLAPixel> xdata = data[i]; 
		for (unsigned long j = 0; j < xdata.size(); j++) {
			xdata[j].s = 0;
		}
		data[i] = xdata;
	}	
}
/*Returns the width of the current block.*/
int Block::width() const {
	return (data.front()).size();
}
/*Returns the height of the current block.*/
int Block::height() const {
	return data.size();
}

