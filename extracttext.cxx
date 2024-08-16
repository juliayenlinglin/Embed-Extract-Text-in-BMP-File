/* Julia Lin
 * jylin@scu.edu
 * Description: Program extracts text from the bmp
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include "bmp.h"
int charCount = 0;

using namespace std;
using namespace csenbmp;

// Function to read text from a file
string readTextFromFile(const string& filename) {
    string text = "";
    ifstream file(filename);
    char ch;
    if (!file.is_open()) {
        cerr << "Error opening text file: " << filename << endl;
        exit(EXIT_FAILURE);
    }
    while (file.get(ch)) {
        charCount++;
        text += ch;
    }
    return text;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "To run the program: " << argv[0] << " <embedded BMP filename> <original text filename>" << endl;
        return EXIT_FAILURE;
    }

    string bmpFilename = argv[1];
    string originalTextFilename = argv[2];

    // Load embedded BMP file
    BMP bmp;
    try {
        bmp.loadBMP(bmpFilename);
    } catch (const exception &e) {
        cerr << "Error loading BMP file: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    // Extract text from BMP
    string extractedText = "";
    int x = 0, y = 0; // Variables to track pixel position
    int c[8] = {0};
    int bitCounter = 0;
    int count = 0;
    Pixel24 pixel;
    string originalText = readTextFromFile(originalTextFilename);

    // Iterate over pixels
    while (true) {
		//a for loop iterating through the 8 bits of a character
        for(int k = 0; k<8;){
            //getting the pixel
            pixel = bmp.getPixel(x, y);

            bitset<8> pixelBits(pixel.r);
            int pixelArray[8];
			//inputting the bits into an array, pixelArray
            for(int j=0; j<8; j++){
                pixelArray[j]=pixelBits[j];
                //std::cout<<pixelArray[j];    -- was used for testing purposes
            }
			//for loop to transfer the pixel bits into an array representing the bits of the character
            for(int p=0; p<2;p++){
                c[7-k] = pixelArray[p];
                //std::cout<<endl<<c[7-k];		-- was used for testing purposes
                k++;
                
            }
            x++;
        }
		
        //Using bitwise OR to transfer the array c to the char letter's bits
        char letter = 0;
        for (int i = 0; i < 8; ++i) {
            letter |= c[i] << (7 - i);
        }
 
            count++;
            //adding the letter to the extractedText string
            extractedText += letter;
            bitCounter = 0;
            
            //variables to create temporary values for x,y of the pixel for checking boundaries
            int s = x;
            int t = y;

            // If at the end of the row, will go to the next row of pixels
            if (++s >= bmp.info.getWidth()) {
                x = 0;
                y++;
                if (++t >= bmp.info.getHeight()) {
                    cerr << "End of image reached before end of text." << endl;
                    return EXIT_FAILURE;
                }
            } 
		//if number of characters extracted is equal to the number of characters in the text file, done
        if(count==charCount){
            break;
        }
        
    }


    // Compare extracted text with original text
    cout<<extractedText<<endl;
    cout<<"Original Text:"<<endl<<originalText<<endl;
    if (extractedText == originalText) {
        cout << "Extracted text matches the original text." << endl;
    } else {
        cout << "Extracted text does not match the original text." << endl;
    }

    return EXIT_SUCCESS;
}
