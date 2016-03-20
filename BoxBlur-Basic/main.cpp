#include <iostream>
#include "lodepng.h"

using namespace std;
using namespace lodepng;

int main()
{
    // Adjustable input file name
    string filename = "apple.png";

    
    
    
    vector<unsigned char> image; //the raw pixels in a list e.g. {R, G, B, A, R, G, B, A, R, ...}
    unsigned width, height;

    //decode initializes image, width, and height
    unsigned error = decode(image, width, height, filename);

    //if there's an error, display it
    if(error)
        cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;

    // There are 4 bytes per pixel.
    cout << "Size: " << image.size() << endl;

    // make a 3-D array from the 1-D image array
    int i = 0;
    vector<vector <vector <unsigned char> > > original;
    original.resize(height);
    for (int row = 0; row < height; row++)
    {
        original[row].resize(width);
        for (int col = 0; col < width; col++)
        {
            original[row][col].resize(4);
            for (int colour = 0; colour < 4; colour++)
            {
                original[row][col][colour] = image[i];
                i++;
            }
        }
    }




    vector<vector <vector <unsigned char> > > blurred;


    // resize blurred 3-d vector to height x width x 4(colours)
    blurred.resize(height);
    for (int row = 0; row < height; row++)
    {
        blurred[row].resize(width);
        for (int col = 0; col < width; col++)
        {
            blurred[row][col].resize(4);
            for (int colour = 0; colour < 4; colour++)
            {
                blurred[row][col][colour] = image[i];
            }
        }
    }


    // for each pixel, calculate the average of the surrounding 3x3 group of pixels for each colour

    // blur centre of image (everything except 1 pixel on each side)
    // skip first and last rows to keep (row - 1) and (row + 1) in range
    for (int row = 1; row < height - 1; row++)
    {
        // skip first and last columns to keep (col - 1) and (col + 1) in range
        for (int col = 1; col < width - 1; col++)
        {
            for (int colour = 0; colour < 4; colour++)
            {
                int total = 0;
                for (int r = row - 1; r <= row + 1; r++)
                {
                    for (int c = col - 1; c <= col + 1; c++)
                    {
                        total += original[r][c][colour];
                    }
                }
                blurred[row][col][colour] = (total / 9.0) + 0.5;    // add 0.5 after division for proper int rounding
            }
        }
    }

    // blur top row (except for corners)
    for (int col = 1; col < width - 1; col++)
    {
        for (int colour = 0; colour < 4; colour++)
        {
            int total = 0;
            for (int r = 0; r <= 1; r++)
            {
                for (int c = col - 1; c <= col + 1; c++)
                {
                    total += original[r][c][colour];
                }
            }
            blurred[0][col][colour] = (total / 6.0) + 0.5;
        }
    }

    // blur bottom row (except for corners)
    for (int col = 1; col < width - 1; col++)
    {
        for (int colour = 0; colour < 4; colour++)
        {
            int total = 0;
            for (int r = height - 2; r <= height - 1; r++)
            {
                for (int c = col - 1; c <= col + 1; c++)
                {
                    total += original[r][c][colour];
                }
            }
            blurred[height - 1][col][colour] = (total / 6.0) + 0.5;
        }
    }

    // blur left side (except for corners)
    for (int row = 1; row < height - 1; row++)
    {
        for (int colour = 0; colour < 4; colour++)
        {
            int total = 0;
            for (int r = row - 1; r <= row + 1; r++)
            {
                for (int c = 0; c <= 1; c++)
                {
                    total += original[r][c][colour];
                }
            }
            blurred[row][0][colour] = (total / 6.0) + 0.5;
        }
    }

    // blur right side (except for corners)
    for (int row = 1; row < height - 1; row++)
    {
        for (int colour = 0; colour < 4; colour++)
        {
            int total = 0;
            for (int r = row - 1; r <= row + 1; r++)
            {
                for (int c = width - 2; c <= width - 1; c++)
                {
                    total += original[r][c][colour];
                }
            }
            blurred[row][width - 1][colour] = (total / 6.0) + 0.5;
        }
    }

    // blur top left corner
    for (int colour = 0; colour < 4; colour++)
    {
        int total = 0;
        for (int r = 0; r <= 1; r++)
        {
            for (int c = 0; c <= 1; c++)
            {
                total += original[r][c][colour];
            }
        }
        blurred[0][0][colour] = (total / 4.0) + 0.5;
    }

    // blur top right corner
    for (int colour = 0; colour < 4; colour++)
    {
        int total = 0;
        for (int r = 0; r <= 1; r++)
        {
            for (int c = width - 2; c <= width - 1; c++)
            {
                total += original[r][c][colour];
            }
        }
        blurred[0][width - 1][colour] = (total / 4.0) + 0.5;
    }

    // blur bottom left corner
    for (int colour = 0; colour < 4; colour++)
    {
        int total = 0;
        for (int r = height - 2; r <= height - 1; r++)
        {
            for (int c = 0; c <= 1; c++)
            {
                total += original[r][c][colour];
            }
        }
        blurred[height - 1][0][colour] = (total / 4.0) + 0.5;
    }

    // blur bottom right corner
    for (int colour = 0; colour < 4; colour++)
    {
        int total = 0;
        for (int r = height - 2; r <= height - 1; r++)
        {
            for (int c = width - 2; c <= width - 1; c++)
            {
                total += original[r][c][colour];
            }
        }
        blurred[height - 1][width - 1][colour] = (total / 4.0) + 0.5;
    }




    // convert 3-D array to 1-D so that it can be encoded back as a PNG file
    vector<unsigned char> flat;
    flat.resize(height*width*4);

    int x = 0;
    for (int i = 0; i < blurred.size(); i++)
    {
        for (int j = 0; j < blurred[i].size(); j++)
        {
            for (int c = 0; c < blurred[i][j].size(); c++)
            {
                flat[x] = blurred[i][j][c];
                x++;
            }
        }
    }

    
    
    
    // Output to file
    // (adjustable output file name)
    encode("blurred.png", flat, width, height);

    
    return 0;
}
