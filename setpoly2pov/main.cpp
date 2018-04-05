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
    std::cout << "\t./stpoly2pov [polyFile] [ellipFile] [neigbhour file] [labels] \n";


    if (argc < 5) throw std::string ("error: not enough arguments");

    std::string polyFileName = argv[1];
    std::string ellipFile = argv[2];
    std::string neighbourFileName = argv[3];

    std::set<unsigned long> labelList;
    for (long i= 4; i != argc; ++i)
    {
        unsigned long id = std::atoi(argv[i]);
        labelList.insert(id);
    }

    std::cout << "going to use N= " << labelList.size() << " cells for drawing" << std::endl;






    std::vector<pT> cellPoints;
    std::vector<face> faces;
    parsePolyFile(polyFileName, cellPoints, faces); 
    std::vector<ellipsoid> ellipsoids;

    if (ellipFile.find(".xyzr") != std::string::npos)
    {   
        std::cout << "parsing xyzr because of file extension" << std::endl;
        parseXYZR (ellipFile, ellipsoids, labelList);
    }
    else
        parseEllipFile(ellipFile, ellipsoids, labelList);
     

    std::cout << "I Have parsed V= " << cellPoints.size() << " points and F= " << faces.size() << " faces" << std::endl;


    std::vector<unsigned long> neighbours;
    parseNeighbours(neighbourFileName, neighbours, labelList); 

//   for (auto n : neighbours)
//   {
//       std::cout << n << " ";
//   }

    std::cout << std::endl;


    std::cout << "\nconverting to pov" << std::endl;

    if (ellipFile.find(".xyzr") != std::string::npos)
    {   
        std::cout << "pasting fine xyzr file" << std::endl;
        printXZRFine (ellipFile, neighbours);
    }

    std::vector<pT> newPoints;
    std::cout << "labelling points to cells" << std::endl;
    unsigned int kc = 0;
    unsigned int nc = 0;
    unsigned int percentstepc = std::ceil(static_cast<double>(faces.size())/100.0);
    std::set<unsigned long> alreadyPushed;
    for (unsigned int u = 0; u != faces.size(); ++u)
    {
        kc++;
        if(kc == percentstepc)
        {
            kc = 0;
            nc++;
            std::cout << nc << "% " << std::flush;
        }
        const face& f = faces[u];

        bool skip = true;
        for (auto x : neighbours)
        {
            if (f.c == x)
            {
                skip = false;
                break;
            }
        }
        if (skip) continue;


        for (unsigned long i = 0; i != f.vx.size(); ++i)
        {
            //std::cout << f.vx[i] << " " << cellPoints.at(f.vx[i]-1).l << "\n";
            if (f.vx[i]  == cellPoints.at(f.vx[i]-1).l)
            {
                if(alreadyPushed.count(f.vx[i]) ==0)
                {
                    alreadyPushed.insert(f.vx[i]);
                    cellPoints.at(f.vx[i]-1).c = f.c;
                    newPoints.push_back(std::move(cellPoints.at(f.vx[i]-1)));
                }
            }
            else
            {
                std::cout << "label mismatch. " << f.vx[i] << std::endl;
                throw "blarz";

            }
        }
    }


    std::cout << "\nreducing complexity by just looking at neighbouring cells: \n" << cellPoints.size() << " to\n" << newPoints.size() << "\n\n";

    cellPoints = newPoints;

    pT mean;
    mean.x[0] = 0;
    mean.x[1] = 0;
    mean.x[2] = 0;
    for (const pT& p : cellPoints)
    {
        //std::cout << p.x[0] << "\n";
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
    radiusMax = std::sqrt(radiusMax) * 1.3;
    std::cout << "mean position " << mean.x[0] << ", "<<  mean.x[1] << ", " << mean.x[2]  << std::endl;
    std::cout << "max radius " << radiusMax << std::endl;
    std::cout << "\n\ncounting points without ID" << std::endl;
    unsigned long pointsWithoutID = 0;
    for (const auto& p : cellPoints)
    {
        if (p.c == 0)
        {
            //std::cout << "point without cell ID!\n";
            pointsWithoutID++;
            //std::cout << "\t" << p.l << " " << p.x[0] << " " << p.x[1]    << " " << p.x[2] << " " << p.c << std::endl;
        }
    }
    std::cout << "pointsWithoutID: " << pointsWithoutID << " / " << cellPoints.size() << " = " << static_cast<double>(pointsWithoutID)/cellPoints.size() *100 << "% " << std::endl;






{
    std::cout << "print pov file" << std::endl;
    std::string povFileName = "out_setpoly.pov";
    std::ofstream out(povFileName);

    std::cout << "\tprint default properties" << std::endl;
    double cameraPosX =  0;
    double cameraPosY =  -radiusMax;
    double cameraPosZ = 0;
    out << "light_source { <411, 812,  -800> color rgb <1,1,1> }\n";
    out << "light_source { <2, " << -radiusMax <<   "  , " <<  -0.5 * radiusMax << "> color rgb <1,1,1> }\n";
    //out << "light_source { <412, -812, -1000> color rgb<0.5, 0.5, 0.5> shadowless }\n";
    out << "light_source { <512, 812,  -212> color rgb<0.5, 0.5, 0.5> shadowless }\n";

    out << "camera { \n";
    out << "  orthographic angle 60\n";
    out << "  location <" << cameraPosX  <<  ", " << cameraPosY << ", "<< cameraPosZ << "> \n";
    out << "  look_at <0, 0, 0> \n";
    out << "  right x*image_width/image_height\n";
    out << "}\n";
    out << "background { color rgb <1.0, 1.0, 1.0> }\n";

    std::cout << "\tprint ellipsoids\n";
    printEllipsoids (out, ellipsoids);

    std::cout << "\tprint voronoi cells" << std::endl;

    //printPovCells(out, faces, cellPoints);
    out << "#declare cylinders =  union {\n";
    out << std::fixed;
    out << std::setprecision(12);


    double cylinderRadius = 0.035;

    unsigned int k = 0;
    unsigned int n = 0;
    unsigned int percentstep = std::ceil(static_cast<double>(faces.size())/100.0);
    for (unsigned int u = 0; u != faces.size(); ++u)
    {
        k++;
        if(k == percentstep)
        {
            k = 0;
            n++;
            std::cout << n << "%  " << std::flush;
        }
        const face& f = faces[u];
        if (std::find(labelList.begin(), labelList.end(), f.c) == labelList.end()) continue;
        //std::cout << "correct cell found" << std::endl;


        for (unsigned long i = 0; i != f.vx.size(); ++i)
        {
            unsigned long idx1 = f.vx[i];
            unsigned long idx2 = f.vx[i+1];
            // we need to print also the last edge going from last - first
            if (i == f.vx.size()-1)
            {
                //std::cout << "print last edge"  << std::endl;
                idx2 = f.vx[0];
            }

            auto p1 = getPoint(cellPoints, idx1);
            auto p2 = getPoint(cellPoints, idx2);
            
            // to avoid degenerate cylinders
            if (comparePoints(*p1, *p2))
            {
                std::cout << "degenerater cylinder detected" << std::endl;
                continue;
            }
            
            if (p1->c == 0) continue;
            if (p2->c == 0) continue;
            int nn1 = 0;
            if(!checkPointConnectivityLargerTwo(*p1, cellPoints, nn1)) continue;
            //std::cout << "nn= " << nn << "\n";
            if(!checkPointConnectivityLargerTwo(*p2, cellPoints, nn1)) continue;
            //if (nn1 != nn2) continue;
            //double nn = (static_cast<double>(nn1) + static_cast<double>(nn2) )/4;

            //std::cout << "nn= " << nn << "\n";
            //std::cout << "print edge" << std::endl;

            out << "cylinder {<" << p1->x[0] << ", ";
            out <<                  p1->x[1] << ", ";
            out <<                  p1->x[2] << "> ,<";
            out <<                  p2->x[0] << ", ";
            out <<                  p2->x[1] << ", ";
            out <<                  p2->x[2] << ">,"<< cylinderRadius << " }\n";
        }
    }

    out << "\n}\n" ;
    
    std::string translate2Center =  " translate -< " + std::to_string(mean.x[0]) + ", " + std::to_string(mean.x[1]) + ", " + std::to_string(mean.x[2]) +  ">";
    std::string rotateString = " rotate < 0,0, clock*360> ";
    out << "\nobject {cylinders ";
    out << translate2Center << rotateString << " pigment { rgb <0.3, 0.3, 0.3> } finish {specular 0.5 phong_size 4}}\n";
    
    out << "\nobject {spheres ";
    out << translate2Center <<  rotateString <<  " pigment { rgb <1, 0.55, 0.13> }   finish {specular 0.6 phong_size 0.0002 ambient rgb <0.2, 0.2, 0.2>} }\n";
    
    out.close();
}
    std::cout << std::endl;
    return 0;
}
