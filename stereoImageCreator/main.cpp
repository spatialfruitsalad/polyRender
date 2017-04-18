#include <iostream>
#include "lodepng.h"

void GetPixelAt(std::vector<unsigned char>& image, unsigned width, unsigned height, unsigned x, unsigned y, unsigned char& r, unsigned char& g, unsigned char& b)
{

    if (x >= width || y >= height)
    {
        std::cerr << "trying to access pixel outside of image x: " << x << " y: " << y << std::endl;
    }
    unsigned int index = 4*(x + y * width);
    if (index >= image.size())
    {
        std::cerr << "trying to access pixel outside of image" << std::endl;
        throw "yolo";
    }
    
   r = image.at(index + 0);
   g = image.at(index + 1);
   b = image.at(index + 2);
}

void SetPixelAt(std::vector<unsigned char>& image, unsigned width, unsigned height, unsigned x, unsigned y, unsigned char r, unsigned char g, unsigned char b)
{

    if (x >= width || y >= height)
    {
        std::cerr << "trying to access pixel outside of image x: " << x << " y: " << y << std::endl;
    }
    unsigned int index = 4*(x + y * width);
    if (index >= image.size())
    {
        std::cerr << "trying to access pixel outside of image" << std::endl;
        throw "yolo";
    }
    
   image.at(index + 0) = r;
   image.at(index + 1) = g;
   image.at(index + 2) = b;
   image.at(index + 3) = 255;
}

void CopyPixel(std::vector<unsigned char>& src, std::vector<unsigned char>& dest, unsigned width, unsigned height, unsigned x, unsigned y, unsigned xoffs)
{
    unsigned char r, g, b;
    GetPixelAt(src, width, height, x,y, r,g,b);
    SetPixelAt(dest, width*2, height, x + xoffs, y, r,g,b);

}




int main (int argc, char* argv[])
{
    std::cout << "this is stereoImageCreator, a program to create one stereoImageFile from two images (left and right)\nUse as explained";
    std::cout << "./stereoImageCreator [LeftFile] [RightFile] [OutFile] \n\n";


    if (argc != 4)
    {
        std::cerr << "error: not enough arguments" << std::endl;
        return -1;
    }

    std::string leftFileName = argv[1];
    std::string rightFileName = argv[2];
    std::string outFileName = argv[3];

    std::vector<unsigned char> leftImage;
    std::vector<unsigned char> rightImage;
    unsigned wl, hl, wr, hr;

    std::cout << "loading left image" << std::endl;
    unsigned error = lodepng::decode(leftImage, wl, hl, leftFileName);
    
    if (error != 0) 
    {
        std::cerr << "Error opening left image" << std::endl;
        return -2;
    }


    std::cout << "loading right image" << std::endl;
    error = lodepng::decode(rightImage, wr, hr, rightFileName);
    if (error != 0) 
    {
        std::cerr << "Error opening right image" << std::endl;
        return -2;
    }

    if (wl != wr || hl != hr)
    {
        std::cerr << "ERROR: Image dimensions do not match: [" << wl << " x " << hl << "] and [ " << wr << " x "<< hr <<  " ]" << std::endl; 
        return -1;
    }


    std::cout << "combine images" << std::endl;
    std::vector<unsigned char> outImage(leftImage.size()*2, 0);

    // copy left image

    for(unsigned int i = 0; i != wl; ++i)
    for(unsigned int j = 1; j != hl; ++j)
    {
       CopyPixel(leftImage, outImage, wl, hl, i, j, 0); 
    }

    for(unsigned int i = 0; i != wl; ++i)
    for(unsigned int j = 1; j != hl; ++j)
    {
       CopyPixel(rightImage, outImage, wr, hr, i, j, wl); 
    }

    std::cout << "save image" << std::endl;
    error = lodepng::encode(outFileName, outImage, wl*2, hl);




}
