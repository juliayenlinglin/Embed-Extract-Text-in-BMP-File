/* Julia Lin
 * jylin@scu.edu
 */
#include <iostream>
#include <iomanip>
#include "bmp.h"

using namespace std;
using namespace csenbmp;

constexpr size_t BMPWIDTH = 1024;
constexpr size_t BMPHEIGHT = 600;
constexpr size_t XDIV = 4;
constexpr size_t YDIV = 2;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Need BMP filename." << endl;
        return EXIT_FAILURE;
    }
    
    string bmpfn {argv[1]};
    BMP bmp;
    bmp.info.setWidth(BMPWIDTH);
    bmp.info.setHeight(BMPHEIGHT);
    bmp.bmpHdr.setFSize(bmp.bmpHdr.getOffset()+bmp.info.getImgSize());
    bmp.newPixels();

    int xPart = bmp.info.getWidth() / XDIV;
    int yPart = bmp.info.getHeight() / YDIV;
    // The art!
    for (auto x = 0; x < bmp.info.getWidth(); ++x) {
        int xmod = x % xPart + 1;
        int xIdx = x / xPart;
        if (xIdx % 2 == 1)
            xmod = xPart - xmod;
        for (auto y = 0; y < bmp.info.getHeight(); ++y) {
            int ymod = y % yPart + 1;
            int yIdx = y / yPart;
            if (yIdx % 2 == 1)
                ymod = yPart - ymod;
            Pixel24 p = {0};
            p.r = (pixel_t) ((double) xmod / (double) xPart * (double) 0xff);  // Red
            p.b = 0xff - (p.r + p.g)/2; // Blue
            p.g = (pixel_t) ((double) (ymod+xmod) / (double) (yPart) * (double) 0xff); //Green
           

            bmp.setPixel(x, y, p);
        }
    }
    bmpfn = genNewFname(bmpfn);
    bmp.saveBMP(bmpfn);
	cout << bmpfn << endl << bmp << endl;
    return EXIT_SUCCESS;
}
