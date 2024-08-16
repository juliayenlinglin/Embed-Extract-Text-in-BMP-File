/* Julia Lin
 * jylin@scu.edu
 * Description: Program embeds text in a bmp (by modifying the pixel's LSBs)
 */
#include <iostream>
#include <fstream>
#include <bitset>
#include "bmp.h"

using namespace std;
using namespace csenbmp;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "To run the program: " << argv[0] << " <input BMP filename> <text filename>" << endl;
        return EXIT_FAILURE;
    }

    string bmpFilename = argv[1];
    string textFilename = argv[2];
 
    // Load BMP file
    BMP bmp;
    try {
        bmp.loadBMP(bmpFilename);
    } catch (const exception &e) {
        cerr << "Error loading BMP file: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    // Open text file
    ifstream textFile(textFilename);
    if (!textFile.is_open()) {
        cerr << "Error opening text file: " << textFilename << endl;
        return EXIT_FAILURE;
    }

    // Embed text into BMP
    int x = 0, y = 0; // Variables to track pixel position
    char c;
    int bitCounter = 0;
    Pixel24 pixel;
    while (textFile.get(c)) {
        for (int i = 0; i < 8; ++i) {       
            //cout<<"Bitcounter:"<<bitCounter<<endl;		-- was used for testing purposes
            //if the 2 lsb in the pixel have already been modified, move on to next pixel
            if (bitCounter == 2) {          
                bmp.setPixel(x, y, pixel);
                ++x;
                if (x >= bmp.info.getWidth()) {
                    x = 0;
                    ++y;
                }
                bitCounter = 0;
            }

            // Get next pixel
            if (x >= bmp.info.getWidth()) {
                x = 0;
                ++y;
            }
            if (y >= bmp.info.getHeight()) {
                cerr << "Text too large to embed in BMP." << endl;
                return EXIT_FAILURE;
            }

            pixel = bmp.getPixel(x, y);

			//Pixel bits before embedding, for testing purposes
            /*cout << "Before bit manipulation - Pixel at position (" << x << ", " << y << "): ";
            Pixel24 originalPixel = bmp.getPixel(x, y);
            cout << bitset<8>(originalPixel.r) << endl;*/

          
            // Embed bit into pixel, using bitwise operations
            pixel.r &= ~(1 << (bitCounter++));
            pixel.r |= ((c >> i) & 1) << (bitCounter - 1);

            bmp.setPixel(x, y, pixel);
            
            //Pixel bits after embedding, for testing purposes
            /*cout << "After bit manipulation - Pixel at position (" << x << ", " << y << "): ";
            Pixel24 modifiedPixel = bmp.getPixel(x, y);
            cout << bitset<8>(modifiedPixel.r) << endl;*/


        }
    }

    // Save modified BMP
    string outputFilename = "embedded_" + bmpFilename;
    try {
        bmp.saveBMP(outputFilename);
    } catch (const exception &e) {
        cerr << "Error saving BMP file: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    cout << "Text embedded successfully into BMP: " << outputFilename << endl;

    return EXIT_SUCCESS;
}


