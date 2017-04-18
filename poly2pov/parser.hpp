#pragma once
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



typedef struct pT {unsigned long l;double x[3];} pT;
typedef std::vector<pT> ptv;
typedef struct face {unsigned long l; unsigned long c; std::vector<unsigned long> vx;} face;

auto getPoint(std::vector<pT>& points, unsigned long idx)
{
    for (auto it = points.begin(); it != points.end(); ++it)
    {
        if (it->l == idx) return it;
    }   
    return points.end();
}
void parsePolyFile  (std::string polyFileName, std::vector<pT>& cellPoints, std::vector<face>& faces);
void parseEllipFile (std::string ellipFileName, std::vector<pT>& points, std::vector<unsigned long> & labelList);
void parseW020File (std::string w020FileName, std::vector<pT>& a1, std::vector<pT>& a2, std::vector<pT>& a3, std::vector<pT>& lengths, std::vector<unsigned long>& labelList, std::map<unsigned long, double>& betaMap );
void printPovCells (std::ofstream& out, std::vector<face>& faces, std::vector<pT>& cellPoints);
void printSpheres_basic (std::ofstream& out, std::vector<pT>& points);
void printSpheres_w020 (std::ofstream& out, ptv& points, ptv& a1, ptv& a2, ptv& a3, ptv& lengths, std::map<unsigned long, double>& betaMap);


void parsePolyFile (std::string polyFileName, std::vector<pT>& cellPoints, std::vector<face>& faces)
{

    // parse poly file
    std::cout << "parsing poly file" << std::endl;

    std::ifstream infile;
    infile.open(polyFileName);
    if (infile.fail())
    {
        throw std::string("cannot open poly input file");
    }
    std::string line = "";
    unsigned long linesloaded = 0;
    bool isInPointMode = true;

    std::cout << "\tparse POINTS" << std::endl;

    while(std::getline(infile, line))   // parse lines
    {
        linesloaded++;
        if(line.find("POINTS") != std::string::npos) continue;
        if (line.find("POLYS") != std::string::npos) 
        {
            std::cout << "\tparse POLY" << std::endl;
            isInPointMode = false;
            continue;
        }
        if(line.find("END") != std::string::npos) break;
        if (isInPointMode)
        {
            pT p;
            splitstring ss(line.c_str());
            std::vector<std::string> split = ss.split(':');
            
            p.l = stoi(split[0]);
            std::istringstream iss(split[1]); 
            if (!(iss >> p.x[0] >> p.x[1] >> p.x[2]))
            {
                throw std::string("Error parsing line\n" + line);
            } 
            cellPoints.push_back(p);
        }
        else
        {
            face f;
            // 43775   :    69450 69476 69449 69448 69447 < c(0, 0, 0, 3059)
            // split  {|                                                    }
            // split2  {                                  |                 }
            // split3      {     |     |     |     |      }
            splitstring ss(line.c_str());
            std::vector<std::string> split = ss.split(':');
            f.l = stoi(split[0]);
            {
                splitstring ss2(split[1].c_str());
                std::vector<std::string> split2 = ss2.split('<');

                splitstring ss3(split2[0].c_str());
                std::vector<std::string> split3 = ss3.split(' ');

                std::string t1 =split2[1].substr(12);   // get rid of c(0, 0, 0,
                std::string t2 = t1.substr(0, t1.find(")")); // get rid of closing bracket ")"
                unsigned long cID = std::stoi(t2);
                f.c = cID;

                for (unsigned long j = 0; j != split3.size(); ++j)
                {
                    unsigned long vid = std::stoi(split3[j]); 
                    f.vx.push_back(vid);
                }
                faces.push_back(f);
            }
        }
    }
}

void parseEllipFile (std::string ellipFileName, std::vector<pT>& points, std::vector<unsigned long> & labelList)
{
    std::cout << "parsing ellip file: " << ellipFileName << std::endl;
    std::ifstream infile;
    infile.open(ellipFileName);
    if (infile.fail())
    {
        throw std::string ("cannot open ellip input file");
    }
    std::string line = "";
    while(std::getline(infile, line))   // parse lines
    {
        if (line.find("#") != std::string::npos) continue;  // ignore comment lines
        
        splitstring ss(line.c_str());
        std::vector<std::string> split = ss.split(' ');
        if(split.size() != 16) std::cerr << "WARNING: Line Size not correct!" << std::endl;

        pT p;
        p.l = std::stoi(split[0]);
        p.x[0] = std::stod(split[1]);
        p.x[1] = std::stod(split[2]);
        p.x[2] = std::stod(split[3]);

        
        bool foundLabel = false;        
        for (auto l : labelList)
            if (l == p.l){foundLabel= true; break;} 
        if (!foundLabel) continue;
       points.push_back(p); 
    }
    std::cout << "parsed N= " << points.size() << " particles from ellip file" << std::endl;
    infile.close();
}


void parseW020File (std::string w020FileName, std::vector<pT>& a1, std::vector<pT>& a2, std::vector<pT>& a3, std::vector<pT>& lengths, std::vector<unsigned long>& labelList, std::map<unsigned long, double>& betaMap )
{

    std::cout << " parse w020 File" << std::endl;
    
    std::ifstream w020file;
    std::cout << w020FileName << std::endl;
    w020file.open(w020FileName);
    if (w020file.fail())
    {
        std::cerr << "cannot open w020 input file";
        throw std::string("cannot open w020 input file");
    }
    std::string line = "";
    while(std::getline(w020file, line))   // parse lines
    {
        if (line.find("#") != std::string::npos) continue;  // ignore comment lines
        
        splitstring ss(line.c_str());
        std::vector<std::string> split = ss.split(' ');
        if(split.size() != 16) std::cerr << "WARNING: Line Size not correct!" << std::endl;

        pT u;
        u.l = std::stoi(split[0]);
        u.x[0] = std::stod(split[2]);
        u.x[1] = std::stod(split[3]);
        u.x[2] = std::stod(split[4]);

        pT v;
        v.l = std::stoi(split[0]);
        v.x[0] = std::stod(split[6]);
        v.x[1] = std::stod(split[7]);
        v.x[2] = std::stod(split[8]);
        
        pT w;
        w.l = std::stoi(split[0]);
        w.x[0] = std::stod(split[10]);
        w.x[1] = std::stod(split[11]);
        w.x[2] = std::stod(split[12]);

        pT le;
        le.l = std::stoi(split[0]);
        le.x[0] = std::stod(split[1]);
        le.x[1] = std::stod(split[5]);
        le.x[2] = std::stod(split[9]);
        
        unsigned long la = std::stoi(split[0]);
        double e1 = std::stod(split[1]);
        double e3 = std::stod(split[9]);
        bool foundLabel = false;
        for (auto l : labelList)
            if (l == la){foundLabel= true; break;} 
        if (!foundLabel) continue;
        betaMap[la] = e1/e3;
        a1.push_back(u);
        a2.push_back(v);
        a3.push_back(w);
        lengths.push_back(le);
    }
    w020file.close();
}



void printPovCells (std::ofstream& out, std::vector<face>& faces, std::vector<pT>& cellPoints)
{

    std::cout << "\tprint voronoi cells" << std::endl;
    out << "#declare cylinders =  union {\n";

    double cylinderRadius = 1.5;

    for (unsigned int u = 0; u != faces.size(); ++u)
    {
        //std::cout << u << " / " << faces.size() << std::endl;
        face f = faces[u];
        for (unsigned long i = 0; i != f.vx.size()-1; ++i)
        {
            unsigned long idx1 = f.vx[i];
            unsigned long idx2 = f.vx[i+1];

            auto p1 = getPoint(cellPoints, idx1);
            auto p2 = getPoint(cellPoints, idx2);

            out << "cylinder {<" << p1->x[0] << ", ";
            out <<                  p1->x[1] << ", ";
            out <<                  p1->x[2] << "> ,<";
            out <<                  p2->x[0] << ", ";
            out <<                  p2->x[1] << ", ";
            out <<                  p2->x[2] << ">,"<< cylinderRadius << " }" << std::endl;
        }

            unsigned long idxs = f.vx[0];
            unsigned long idxe = f.vx[f.vx.size()-1];

            auto ps = getPoint(cellPoints, idxs);
            auto pe = getPoint(cellPoints, idxe);

        out << "cylinder {<" ;
        out << pe->x[0] <<", " ;
        out << pe->x[1] << ", " ;
        out << pe->x[2] <<"> ,<";

        out << ps->x[0] <<", ";
        out << ps->x[1] << ", ";
        out << ps->x[2] <<">," << cylinderRadius << "}" << std::endl;
    }

    out <<"}\n";
}


void printSpheres_basic (std::ofstream& out, std::vector<pT>& spherePoints)
{
    std::cout << "\tprint spheres" << std::endl;
    
    double sphereRadius = 26;

    out << "#declare spheres =  union {\n";
    for(pT p : spherePoints)
    {
            out << "sphere {<"   << p.x[0] << ", ";
            out <<                  p.x[1] << ", ";
            out <<                  p.x[2] << "> ," << sphereRadius << "}\n";
    }
    out << "}\n";
}

void printSpheres_w020 (std::ofstream& out, ptv& points, ptv& a1, ptv& a2, ptv& a3, ptv& lengths, std::map<unsigned long, double>& betaMap)
{
    double sphereRadius = 26;

    std::cout << "\n\nbetas: \n";
    out << "#declare spheres =  union {\n";
    for(unsigned int i = 0; i != points.size(); ++i)
    {
        pT p = points[i];
        pT u = a1.at(i);
        pT v = a2.at(i);
        pT w = a3.at(i);
        pT le = lengths.at(i);
        double lem = (le.x[0]+ le.x[1] + le.x[2])/3.0;
        
        if (p.l != u.l ) std::cerr << "WARNING: Labels do not match!" << std::endl;

            
            out << "sphere {<"   << 0 << ", ";
            out <<                  0 << ", ";
            out <<                  0 << "> ," << sphereRadius;
            // scale ellipsoid
            out << " matrix  < " <<  le.x[0]/lem << ", " << 0 << ", " << 0         << ", ";
            out <<                     0               << ", " << le.x[1]/lem << ", " << 0         << ", ";
            out <<                     0               << ", " << 0 << ", " << le.x[2]/lem << ", ";
            out <<                     "0, 0, 0> ";
            // rotate ellipsoid
            // method 1. looks kinda fishy and the ellipsoids do not seem to be oriented quite correclty
            /*out << " matrix  < " <<    u.x[0] << ", " << v.x[0] << ", " << w.x[0] << ", ";
            out <<                     u.x[1] << ", " << v.x[1] << ", " << w.x[1] << ", ";
            out <<                     u.x[2] << ", " << v.x[2] << ", " << w.x[2] << ", ";
            out <<                     "0         , 0               , 0>";*/
            //translate
            out << " matrix  < " <<    u.x[0] << ", " << u.x[1] << ", " << u.x[2] << ", ";
            out <<                     v.x[0] << ", " << v.x[1] << ", " << v.x[2] << ", ";
            out <<                     w.x[0] << ", " << w.x[1] << ", " << w.x[2] << ", ";
            out <<                     "0         , 0               , 0>";
            out << " translate <"   << p.x[0] << ", ";
            out <<                    p.x[1] << ", ";
            out <<                    p.x[2] << "> "; 


            //double val = (betaMap[p.l] - betamin) / ( betamax - betamin);
            //double val = betaMap[p.l];
            double val = (betaMap[p.l] - 0.625) /0.375 + 0.08 ;
            //std::cout << p.l << " " << betaMap[p.l]  << " " << val << std::endl;
            hsv c;
            c.h = val*0.9*360;
            c.s = 0.85;
            c.v = 0.9;

            rgb crgb = colorTable::hsv2rgb(c);
//            std::cout << v << std::endl;
            out << " pigment { rgb <" << crgb.r << ", " << crgb.g << ", " << crgb.b <<  ">}  finish {specular 0.6 phong_size 0.0002 ambient rgb <0.2, 0.2, 0.2> }  ";
            out << "}\n";
    }
    out << "}\n";
}

