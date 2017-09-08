
#include "parser.hpp"


int main (int argc, char* argv[])
{

    std::cout << "this is poly2pov, a program to convert a poly file (of set voronoi cells) to be used in povray. Use explained:\n";
    std::cout << "\t./poly2pov [polyFile] [ellipFile] [w020_eigsysFile] [infoName] (angle) (xshift)\n";


    if (argc < 5) throw std::string ("error: not enough arguments");

    std::string polyFileName = argv[1];
    std::string ellipFileName = argv[2];
    std::string w020FileName = argv[3];
    std::string povIDName = argv[4];
    double angle = 0;
    if( argc>=6 )
        angle = std::stod(argv[5]);
    double xshift = 0;

    if( argc>=7 )
        xshift = std::stod(argv[6]);

    std::vector<pT> cellPoints;
    std::vector<face> faces;
    parsePolyFile(polyFileName, cellPoints, faces); 
   
    std::vector<unsigned long> labelList;
    for (face f : faces)
    {
        labelList.push_back(f.c);
    } 
    // make labelList unique
    labelList.erase( std::unique( labelList.begin(), labelList.end() ), labelList.end() );

    std::vector<pT> spherePoints;
    parseEllipFile(ellipFileName, spherePoints, labelList);

    

    std::vector<pT> a1;
    std::vector<pT> a2;
    std::vector<pT> a3;
    std::vector<pT> lengths;
    std::map<unsigned long, double> betaMap;
    parseW020File(w020FileName, a1, a2, a3, lengths, labelList, betaMap);

    pT mean;
    for (pT p : cellPoints)
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
    radiusMax = std::sqrt(radiusMax) * 3;
    std::cout << "mean position " << mean.x[0] << ", "<<  mean.x[1] << ", " << mean.x[2]  << std::endl;
    std::cout << "max radius " << radiusMax << std::endl;

    double betamin = 10000000000, betamax = 0;
    
    for (auto it = betaMap.begin(); it != betaMap.end(); ++it)
    {
        std::cout << it->first << " " <<   it->second << std::endl;
        if (it->second > betamax ) betamax = it->second;
        if (it->second < betamin ) betamin = it->second;
    }

    std::cout << "done parsing files.\nconverting to pov" << std::endl;

   // print pov file
if(true)
{
    std::cout << "print basic pov file" << std::endl;
    std::string povFileName = povIDName + "_angle" + std::to_string(angle) +  "_basic.pov";
    std::ofstream out(povFileName);

    std::cout << "\tprint default properties" << std::endl;
    double cameraPosX =  xshift;
    double cameraPosY =  0.25*radiusMax;
    double cameraPosZ = - radiusMax;
    out << "light_source { <411, 812,  -800> color rgb <1,1,1> }\n";
    //out << "light_source { <412, -812, -1000> color rgb<0.5, 0.5, 0.5> shadowless }\n";
    out << "light_source { <512, 812,  -212> color rgb<0.5, 0.5, 0.5> shadowless }\n";

    out << "camera { \n";
//    out << "  orthographic angle 60\n";
    out << "  location <" << cameraPosX  <<  ", " << cameraPosY << ", "<< cameraPosZ << "> \n";
    out << "  look_at <0, 20, 0> \n";
    out << "  right x*image_width/image_height\n";
    out << "}\n";
    out << "background { color rgb <1.0, 1.0, 1.0> }\n";

    printPovCells(out, faces, cellPoints);
    printSpheres_basic(out, spherePoints); 
    
    std::string translate2Center =  " translate -< " + std::to_string(mean.x[0]) + ", " + std::to_string(mean.x[1]) + ", " + std::to_string(mean.x[2]) +  ">";
    std::string rotateString = " rotate < 0, " + std::to_string(angle) + ", 0> ";
    out << "\nobject { spheres ";
    out << translate2Center <<  rotateString <<  " pigment { rgb <1, 0.55, 0.13> }   finish {specular 0.6 phong_size 0.0002 ambient rgb <0.2, 0.2, 0.2>} }\n";
    out << "\nobject {cylinders ";
    out << translate2Center << rotateString << " pigment { rgb <0.3, 0.3, 0.3> } finish {specular 0.5 phong_size 4}}\n";
    
    
    out.close();
}




    std::cout << "print w020 pov" << std::endl;
{
    std::string povFileName = povIDName + "_angle" + std::to_string(angle) +  "_w020.pov";
    std::ofstream out(povFileName);

    std::cout << "\tprint default properties" << std::endl;
    double cameraPosX =  xshift;
    double cameraPosY =  0.25*radiusMax;
    double cameraPosZ = - radiusMax;
    out << "light_source { <411, 812,  -800> color rgb <1,1,1> }\n";
    //out << "light_source { <412, -812, -1000> color rgb<0.5, 0.5, 0.5> shadowless }\n";
    out << "light_source { <512, 812,  -212> color rgb<0.5, 0.5, 0.5> shadowless }\n";

    out << "camera { \n";
//    out << "  orthographic angle 60\n";
    out << "  location <" << cameraPosX  <<  ", " << cameraPosY << ", "<< cameraPosZ << "> \n";
    out << "  look_at <0, 20, 0> \n";
    out << "  right x*image_width/image_height\n";
    out << "}\n";
    out << "background { color rgb <1.0, 1.0, 1.0> }\n";


    printPovCells(out, faces, cellPoints);
    printSpheres_w020(out, spherePoints, a1, a2, a3, lengths, betaMap);

    std::string translate2Center =  " translate -< " + std::to_string(mean.x[0]) + ", " + std::to_string(mean.x[1]) + ", " + std::to_string(mean.x[2]) +  ">";
    std::string rotateString = " rotate < 0, " + std::to_string(angle) + ", 0> ";
    out << "\nobject {cylinders ";
    out << translate2Center << rotateString << " pigment { rgb <0.3, 0.3, 0.3> } finish {specular 0.5 phong_size 4}}\n";

    out <<"\nobject { spheres "<< translate2Center << rotateString <<  " }\n";
}


    return 0;
}








