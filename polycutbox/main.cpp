#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "splitstring.hpp"

struct pT 
{
    unsigned long l;
    double x[3];
    ~pT() = default;
    // rule of five, since we have a normal value type, the default versions are ok
    pT () = default;
    pT(const pT&) = default;
    pT (pT&& other) = default;
    pT& operator=(const pT& other) = default;
    pT& operator=(pT&& other) = default;
} ;

typedef struct face {unsigned long l; unsigned long c; std::vector<unsigned long> vx;} face;


int main (int argc, char* argv[])
{

    std::cout << "this is polyCut, a program to cut objects out of poly files. Use explained:\n";
    std::cout << "\t./polyCut [input file path] [outFileName] [x1, y1, z1, x2,y2, z2]\n";
    std::cout << "\nfor example\n\t ./polyCut cell.poly out.poly 0 0 0 8 8 8\n will cut out all objects which are in range 0 to 8\n\n\n" << std::flush;


    if (argc < 1) throw std::string ("error: not enough arguments");

    std::string polyFileName = argv[1];
    std::string outFileName = argv[2];
    std::cout << "outfile: " << outFileName << std::endl;

    double x1 = std::stod(argv[3]);
    double y1 = std::stod(argv[4]);
    double z1 = std::stod(argv[5]);

    double x2 = std::stod(argv[6]);
    double y2 = std::stod(argv[7]);
    double z2 = std::stod(argv[8]);

    std::vector<pT> points;
    std::vector<face> faces;
    std::vector<unsigned long> selectp;


    std::cout << "going to parse from \nx\t"<< x1 << "\t" <<x2 << "\ny:\t" << y1 << "\t" << y2 << "\nz:\t" << z1<< "\t" << z2 <<"\n\n";

    // parse poly file
    std::cout << "parsing poly file" << std::endl;

    std::ifstream infile;
    infile.open(polyFileName);
    if (infile.fail())
    {
        std::cerr << "cannot open poly input file" << std::endl;
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
            
            //std::cout << split[0] << std::endl;
            p.l = stoi(split[0]);
            std::istringstream iss(split[1]);
            
            if (!(iss >> p.x[0] >> p.x[1] >> p.x[2]))
            {
                std::cerr << "Error parsing line\n" << line << std::endl;
                return -2;
            } 
            if (p.x[0] >= x1 && p.x[0] <= x2)
            if (p.x[1] >= y1 && p.x[1] <= y2)
            if (p.x[2] >= z1 && p.x[2] <= z2)
            {
                selectp.push_back(p.l);
                points.push_back(std::move(p));
            }

        }
        else
        {
            face f;

            splitstring ss(line.c_str());
            std::vector<std::string> split = ss.split(':');

            //std::cout << split[0] << std::endl;
            f.l = stoi(split[0]);

            {
                bool useface = true;
                splitstring ss2(split[1].c_str());
                std::vector<std::string> split2 = ss2.split('<');

                splitstring ss3(split2[0].c_str());
                std::vector<std::string> split3 = ss3.split(' ');


                for (unsigned long j = 0; j != split3.size(); ++j)
                {
                    unsigned long vid = std::stoi(split3[j]); 
                    
                    bool keep = false;
                    for(unsigned long p : selectp)
                    {
                        if (vid == p)
                        {
                            keep = true;
                            break;      
                        }
                    }

                    if (!keep)
                    {
                        useface = false;
                        break;
                    }
                    f.vx.push_back(std::move(vid));
                }
                //std::cout << std::endl;
    
                std::string strX = split2[1].substr(11);
                std::string strY = strX.substr(0,strX.length() - 1 );

                //std::cout << strY << std::endl;
                f.c = std::stoi(strY);
                
                if (useface)
                    faces.push_back(std::move(f));
            }
        }
    }

    std::cout << "done parsing file." << std::endl;

    std::cout << "selected N points: " << points.size() << " " << selectp.size() << std::endl;

   //std::cout << "printing points: ";
   //for (auto x :selectp)
   //    std::cout << x << " ";

   //std::cout <<"\n\n";

   
    std::map < unsigned long, unsigned long> remap;
    for (unsigned int i = 0; i != selectp.size(); ++i)
    {
        remap[selectp[i]] = i + 1;
    }




    // mapke them unique with the new labels
    std::sort(selectp.begin(), selectp.end(), [&remap](unsigned long i1, unsigned long i2) {return (remap[i1]<remap[i2]);});
    selectp.erase(unique(selectp.begin(), selectp.end(), [&remap](unsigned long i1, unsigned long i2){return (remap[i1]==remap[i2]); }), selectp.end( ));


   //std::cout << "printing points after reorder: ";
   //for (auto x :selectp)
   //{
   //    std::cout << x << " ";
   //}
   //std::cout <<"\n\n";
   //
   //std::cout << "printing remap after reorder: ";
   //for (auto x :selectp)
   //{
   //    std::cout << remap[x] << " ";
   //}

   //std::cout <<"\n\n";
    std::cout << " finished selecting vertices.\n" << std::endl;

   // print poly file

    std::cout << "print poly file: " << outFileName  << std::endl;
    std::ofstream out(outFileName);

    std::cout << "\n\twrite POINTS" << std::endl;
    out << "POINTS" << std::endl;
    unsigned long n = 0;
    unsigned long k = 0;
    unsigned long percentstep = std::ceil(static_cast<double>(selectp.size())/1000);
    std::vector<unsigned long> printed;
    out<< std::fixed;
    out << std::setprecision(12);
    for (const auto pid : selectp)
    {
        n++;
        if (n == percentstep)
        {
            n = 0;
            k++;
            std::cout << static_cast<double>(k)*0.1 << "% " << std::flush;
        }
        //std::cout << pid << " ";
        //std::cout << remap[pid] << std::endl;
        const auto& p = points.at(remap[pid]-1);
        unsigned long rpid = remap[pid];
        
        bool upperbreak = false;
        for (const auto& x : printed)
        {
            if (x == rpid)
            {
                upperbreak = true;
            continue;
        }
        }
        if (upperbreak) continue;

        out << rpid  << ": " << p.x[0] << "  " << p.x[1] << " " << p.x[2] << "\n";
        printed.push_back(rpid);
    }

    std::cout << "\twrite POLYS" << std::endl;
    out << "POLYS\n";
    for (const auto& f : faces)
    {
        out << f.l << ":";
        for (const auto& pid : f.vx)
        {
            out << " " << remap[pid];   
        }
        out << " < c(0, 0, 0, " << f.c << ")"  << std::endl;
    }

    out << "END\n";
    out.close();

    return 0;
}
