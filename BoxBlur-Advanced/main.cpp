#include <iostream>
#include "lodepng.h"

using namespace std;
using namespace lodepng;

int main()
{
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
    //TODO: Your code goes here.


    const int radius = 5;   // blur radius


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


    // for each pixel, calculate the average of the surrounding square group of pixels for each colour

    // blur centre of image (everything except (radius) pixels on each side)
    // skip first and last (radius) rows to keep (row - radius) and (row + radius) in range
    for (int row = radius; row < height - radius; row++)
    {
        // skip first and last columns to keep (col - radius) and (col + radius) in range
        for (int col = radius; col < width - radius; col++)
        {
            for (int colour = 0; colour < 4; colour++)
            {
                int total = 0;
                for (int r = row - radius; r <= row + radius; r++)
                {
                    for (int c = col - radius; c <= col + radius; c++)
                    {
                        total += original[r][c][colour];
                    }
                }
                blurred[row][col][colour] = (  (double)total / ( (2 * radius + 1) * (2 * radius + 1) )  ) + 0.5;   // add 0.5 after division for proper int rounding
            }
        }
    }


    // blur top (radius) rows (except for corners)
    for (int row = 0; row < radius; row++)
    {
        for (int col = radius; col < width - radius; col++)
        {
            int n_radius = row;     // new (temporary) radius due to limited surrounding pixels
            for (int colour = 0; colour < 4; colour++)
            {
                int total = 0;
                for (int r = row - n_radius; r <= row + n_radius; r++)
                {
                    for (int c = col - n_radius; c <= col + n_radius; c++)
                    {
                        total += original[r][c][colour];
                    }
                }
                blurred[row][col][colour] = (  (double)total / ( (2 * n_radius + 1) * (2 * n_radius + 1) )  ) + 0.5;
            }
        }
    }

    // blur bottom (radius) rows (except for corners)
    for (int row = height - radius; row < height; row++)
    {
        for (int col = radius; col < width - radius; col++)
        {
            int n_radius = height - row - 1;
            for (int colour = 0; colour < 4; colour++)
            {
                int total = 0;
                for (int r = row - n_radius; r <= row + n_radius; r++)
                {
                    for (int c = col - n_radius; c <= col + n_radius; c++)
                    {
                        total += original[r][c][colour];
                    }
                }
                blurred[row][col][colour] = (  (double)total / ( (2 * n_radius + 1) * (2 * n_radius + 1) )  ) + 0.5;
            }
        }
    }

    // blur left (radius) columns (except for corners)
    for (int row = radius; row < height - radius; row++)
    {
        for (int col = 0; col < radius; col++)
        {
            int n_radius = col;
            for (int colour = 0; colour < 4; colour++)
            {
                int total = 0;
                for (int r = row - n_radius; r <= row + n_radius; r++)
                {
                    for (int c = col - n_radius; c <= col + n_radius; c++)
                    {
                        total += original[r][c][colour];
                    }
                }
                blurred[row][col][colour] = (  (double)total / ( (2 * n_radius + 1) * (2 * n_radius + 1) )  ) + 0.5;
            }
        }
    }

    // blur right (radius) columns (except for corners)
    for (int row = radius; row < height - radius; row++)
    {
        for (int col = width - radius; col < width; col++)
        {
            int n_radius = width - col - 1;
            for (int colour = 0; colour < 4; colour++)
            {
                int total = 0;
                for (int r = row - n_radius; r <= row + n_radius; r++)
                {
                    for (int c = col - n_radius; c <= col + n_radius; c++)
                    {
                        total += original[r][c][colour];
                    }
                }
                blurred[row][col][colour] = (  (double)total / ( (2 * n_radius + 1) * (2 * n_radius + 1) )  ) + 0.5;
            }
        }
    }


    // blur top left corner (radius x radius square)
    for (int row = 0; row < radius; row++)
    {
        for (int col = 0; col < radius; col++)
        {
            int n_radius;
            // find largest usable radius
            if (row <= col)
                n_radius = row;
            else
                n_radius = col;
            for (int colour = 0; colour < 4; colour++)
            {
                int total = 0;
                for (int r = row - n_radius; r <= row + n_radius; r++)
                {
                    for (int c = col - n_radius; c <= col + n_radius; c++)
                    {
                        total += original[r][c][colour];
                    }
                }
                blurred[row][col][colour] = (  (double)total / ( (2 * n_radius + 1) * (2 * n_radius + 1) )  ) + 0.5;
            }
        }
    }

    // blur top right corner (radius x radius square)
    for (int row = 0; row < radius; row++)
    {
        for (int col = width - radius; col < width; col++)
        {
            int n_radius;
            if (row <= width - col - 1)
                n_radius = row;
            else
                n_radius = width - col - 1;
            for (int colour = 0; colour < 4; colour++)
            {
                int total = 0;
                for (int r = row - n_radius; r <= row + n_radius; r++)
                {
                    for (int c = col - n_radius; c <= col + n_radius; c++)
                    {
                        total += original[r][c][colour];
                    }
                }
                blurred[row][col][colour] = (  (double)total / ( (2 * n_radius + 1) * (2 * n_radius + 1) )  ) + 0.5;
            }
        }
    }

    // blur bottom left corner (radius x radius square)
    for (int row = height - radius; row < height; row++)
    {
        for (int col = 0; col < radius; col++)
        {
            int n_radius;
            if (height - row - 1 <= col)
                n_radius = height - row - 1;
            else
                n_radius = col;
            for (int colour = 0; colour < 4; colour++)
            {
                int total = 0;
                for (int r = row - n_radius; r <= row + n_radius; r++)
                {
                    for (int c = col - n_radius; c <= col + n_radius; c++)
                    {
                        total += original[r][c][colour];
                    }
                }
                blurred[row][col][colour] = (  (double)total / ( (2 * n_radius + 1) * (2 * n_radius + 1) )  ) + 0.5;
            }
        }
    }

    // blur bottom right corner (radius x radius square)
    for (int row = height - radius; row < height; row++)
    {
        for (int col = width - radius; col < width; col++)
        {
            int n_radius;
            if (height - row - 1 <= width - col - 1)
                n_radius = height - row - 1;
            else
                n_radius = width - col - 1;
            for (int colour = 0; colour < 4; colour++)
            {
                int total = 0;
                for (int r = row - n_radius; r <= row + n_radius; r++)
                {
                    for (int c = col - n_radius; c <= col + n_radius; c++)
                    {
                        total += original[r][c][colour];
                    }
                }
                blurred[row][col][colour] = (  (double)total / ( (2 * n_radius + 1) * (2 * n_radius + 1) )  ) + 0.5;
            }
        }
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

    //
    encode("blurred.png", flat, width, height);

    return 0;
}
