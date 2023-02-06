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

    std::cout << "this is xyzr2pov, a program to convert a xyzr file (spheres with different radius) to be used in povray. Use explained:\n";
    std::cout << "\t./stpoly2pov [xyzrFile] [labels] \n";


    if (argc < 3) throw std::string ("error: not enough arguments");

    std::string xyzrFileName = argv[1];

    std::set<unsigned long> labelList;
    for (long i= 2; i != argc; ++i)
    {
        unsigned long id = std::atoi(argv[i]);
        labelList.insert(id);
    }

    std::cout << "going to use N= " << labelList.size() << " spheres for drawing" << std::endl;

    std::vector<sphere> spheres;
    parseXYZRSpheres (xyzrFileName, spheres, labelList);

    std::cout << "parsed S= " << spheres.size() << " spheres" << std::endl;

    for (const auto& s :spheres)
        std::cout << s.l << " " << s.r  << std::endl;

    std::cout << "print spheres to pov file" << std::endl;
    std::string povFileName = "spheres.pov";
    std::ofstream out(povFileName);

   printSpheres_basic_spheres(out, spheres);

}
