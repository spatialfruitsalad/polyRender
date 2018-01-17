#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

#include "splitstring.hpp"
#include "colorTable.hpp"

#include "parser.hpp"



int main (int argc, char* argv[])
{

    std::cout << "this is setpoly2pov, a program to convert a poly file (of set voronoi cells) to be used in povray. Use explained:\n";
    std::cout << "\t./stpoly2pov [polyFile]  [particleFile] [angle] \n";


    if (argc != 4) throw std::string ("error: not enough arguments");

    std::string polyFileName = argv[1];
    std::string xyzrFileName = argv[2];
    double angle = std::stod(argv[3]);

    std::vector<pT> cellPoints;
    std::vector<face> faces;
    parsePolyFile(polyFileName, cellPoints, faces); 
   

    std::vector<unsigned long> labelList;
    // TODO fill with particle labels
    labelList.push_back(6449);




    pT mean;
    for (const pT& p : cellPoints)
    {
        mean.x[0] += p.x[0];
        mean.x[1] += p.x[1];
        mean.x[2] += p.x[2];
    }
    mean.x[0] /= cellPoints.size();
    mean.x[1] /= cellPoints.size();
    mean.x[2] /= cellPoints.size();


    double radiusMax = 0;
    
    for (pT p : cellPoints)
    {
        double dx = mean.x[1] - p.x[1];
        double dy = mean.x[2] - p.x[2];

        double r = dx*dx + dy*dy;
        if (r > radiusMax) radiusMax = r;

    }
    radiusMax = std::sqrt(radiusMax) * 2.1;
    std::cout << "mean position " << mean.x[0] << ", "<<  mean.x[1] << ", " << mean.x[2]  << std::endl;
    std::cout << "max radius " << radiusMax << std::endl;


    std::cout << "done parsing files.\nconverting to pov" << std::endl;

    
{
    std::cout << "print basic pov file" << std::endl;
    std::string povFileName = "out_setpoly.pov";
    std::ofstream out(povFileName);

    std::cout << "\tprint default properties" << std::endl;
    double cameraPosX =  0;
    double cameraPosY =  - radiusMax;
    out << "light_source { <411, -812,  800> color rgb <1,1,1> }\n";
    //out << "light_source { <412, -812, -1000> color rgb<0.5, 0.5, 0.5> shadowless }\n";
    out << "light_source { <512, -812,  212> color rgb<0.5, 0.5, 0.5> shadowless }\n";

    out << "camera { \n";
    out << "  orthographic angle 45\n";
    out << "  location <" << cameraPosX  <<  ", " << cameraPosY << ", "<< mean.x[2] << "> \n";
    out << "  look_at <0, 0, 0> \n";
    out << "  right x*image_width/image_height\n";
    out << "}\n";
    out << "background { color rgb <1.0, 1.0, 1.0> }\n";

    std::cout << "\tprint voronoi cells" << std::endl;

    printPovCells(out, faces, cellPoints);
    
    std::string translate2Center =  " translate -< " + std::to_string(mean.x[0]) + ", " + std::to_string(mean.x[1]) + ", " + std::to_string(mean.x[2]) +  ">";
    std::string rotateString = " rotate < 0, 0, " + std::to_string(angle) + "> ";
    out << "\nobject {cylinders ";
    out << translate2Center << rotateString << " pigment { rgb <0.3, 0.3, 0.3> } finish {specular 0.5 phong_size 4}}\n";
    
    
    out.close();
}
    return 0;
}
