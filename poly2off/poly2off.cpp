#include <iostream>
#include <iomanip>

#include "colorTable.hpp"
#include "../poly2pov/parser.hpp"


int main (int argc, char* argv[])
{

    std::cout << "this is poly2pov, a program to convert a poly file (of set voronoi cells) to be used in povray. Use explained:\n";
    std::cout << "\t./poly2pov [polyFile] [outFile] \n";


    if (argc != 3) throw std::string ("error: not enough arguments");

    std::string polyFileName = argv[1];
    std::string outFile = argv[2];

    std::vector<pT> cellPoints;
    std::vector<face> faces;

    parsePolyFile(polyFileName, cellPoints, faces); 

    // create color table
    unsigned int maxFaceID = 0;
    for(auto f : faces)
    {
        maxFaceID = (maxFaceID > f.l) ? maxFaceID : f.l;
    }
    std::vector<rgb> colors = colorTable::getRandomColors(maxFaceID);



    std::cout << "done parsing files.\n\nconverting to off" << std::endl;

   // print off file
{
    std::cout << "print off file" << std::endl;
    std::ofstream out(outFile);

    std::cout << "\tprint default properties" << std::endl;
    
    out << "OFF\n" << cellPoints.size() << " " << faces.size() << " 0\n";
    out << std::fixed;
    
    for(auto p : cellPoints )
    {
        out << std::setprecision(12) << p.x[0] << " " << std::setprecision(12) << p.x[1] << " " << std::setprecision(12) << p.x[2] << "\n";
    }

    for (auto f : faces )
    {
        unsigned int faceID = f.l;
        //unsigned int cellID = f.c;
        double r = colors.at(faceID).r;
        double g = colors.at(faceID).g;
        double b = colors.at(faceID).b;
        out << f.vx.size() << " "; 
        for (auto idx : f.vx)
        {
            out << idx - 1 << " ";
        }
        out << r << " " << g << " " << b <<  " 1" << "" << std::endl;
    }

        out << "\n";
        

    out.close();
}
    return 0;
}








