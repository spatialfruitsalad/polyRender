#pragma once
#include <string>
#include <cmath>
#include <iomanip>



struct ellipsoid
{
    unsigned long l;
    unsigned long c;
    double x[3];
    double r1, r2, r3;
    double a1[3], a2[3], a3[3];
    ~ellipsoid() = default;
    // rule of five, since we have a normal value type, the default versions are ok
    ellipsoid () = default;
    ellipsoid(const ellipsoid&) = default;
    ellipsoid (ellipsoid&& other) = default;
    ellipsoid& operator=(const ellipsoid& other) = default;
    ellipsoid& operator=(ellipsoid&& other) = default;
};



struct sphere
{
    unsigned long l;
    unsigned long c;
    double x[3];
    double r;
    ~sphere() = default;
    // rule of five, since we have a normal value type, the default versions are ok
    sphere () = default;
    sphere(const sphere&) = default;
    sphere (sphere&& other) = default;
    sphere& operator=(const sphere& other) = default;
    sphere& operator=(sphere&& other) = default;
};

typedef struct pT {unsigned long l; unsigned long c; double x[3];} pT;
typedef std::vector<pT> ptv;
typedef struct face {unsigned long l; unsigned long c; std::vector<unsigned long> vx;} face;

auto getPoint(std::vector<pT>& points, unsigned long idx)
{
    for (auto it = points.begin(); it != points.end(); ++it)
    {
        if (it->l == idx) return it;
    }   
    std::cout << "no valid point found for idx " << idx << " !" << std::endl;
    return points.end();
}

pT& getMyPoint(std::vector<pT>& points, unsigned long idx)
{
    for (auto& p : points)
    {
        if (p.l == idx) return p;
    }
    throw std::string("no valid point found");
}


bool comparePoints (const pT& p1, const pT& p2, double epsilon = 1e-15)
{
    double deviation = 0;
    for (unsigned int i = 0; i != 3; ++i)
    {
        deviation += (p1.x[i] - p2.x[i]) * (p1.x[i] - p2.x[i]);
    }
    return (deviation <epsilon * epsilon);
}


bool checkPointConnectivityLargerTwo (const pT& p, const std::vector<pT>& cellPoints, int& nn)
{
    unsigned long numberOfOtherPointsHere = 0;
    std::vector<unsigned long> otherCellsHere;
    if (p.c == 0) return false;

    for (unsigned int j = 0; j != cellPoints.size(); ++j)
    {
        const pT& q = cellPoints[j];
        if (q.c == 0 || q.c == p.c)
           continue;

        if (comparePoints(p, q, 1e-10))
        {
            numberOfOtherPointsHere++;
            otherCellsHere.push_back(q.c);
        }
    }

    otherCellsHere.erase( unique( otherCellsHere.begin(), otherCellsHere.end() ), otherCellsHere.end() );
    nn =otherCellsHere.size();
    return (otherCellsHere.size() > 1);
}


void normalize (pT& v)
{
    double l = std::sqrt( v.x[0]*v.x[0] + v.x[1]*v.x[1] + v.x[2]*v.x[2] );
    if (l == 0) return;
    v.x[0] /= l;
    v.x[1] /= l;
    v.x[2] /= l;
}

void printXZRFine (std::string fileName, const std::vector<unsigned long>& neighbours)
{

    std::cout << "parsing xyzr file" << std::endl;
    std::ifstream infile;
    infile.open(fileName);
    if (infile.fail())
    {
        throw std::string ("cannot open xyzr input file");
    }
    std::string line = "";

    std::string outFileName = fileName + ".fine";
    std::ofstream outfile(outFileName);

    // ignore the top two lines
    std::getline(infile, line);
    outfile << line << std::endl;
    std::getline(infile, line);
    outfile << line << std::endl;


    unsigned long lineID = 0;
    while(std::getline(infile, line))   // parse lines
    {
        if (line.find("#") != std::string::npos) continue;  // ignore comment lines
        
        //std::cout << std::endl <<  line << " " << std::endl;
        splitstring ss(line.c_str());
        std::vector<std::string> split = ss.split(' ');
        if(split.size() != 6) std::cerr << "WARNING: Line Size not correct!" << std::endl;
        lineID++;
        

        bool isNeighbour = false;
        for (auto x : neighbours)
        {
            if (lineID == x)
            {
                isNeighbour = true;
                break;
            }
        }
        if (!isNeighbour) continue;
        outfile << line << std::endl;
    }
}

void parseNeighbours (std::string neighbourFileName, std::vector<unsigned long>& neighbours, std::set<unsigned long> cID)
{
    std::cout << "parsing neighbour file" << std::endl;

    std::ifstream infile;
    infile.open(neighbourFileName);
    if (infile.fail())
    {
        throw std::string("cannot open neighbour input file");
    }

    for (auto x : cID)
    {
        neighbours.push_back(x);
    }

    std::string line = "";
    while(std::getline(infile, line))   // parse lines
    {
        if(line.find("#") != std::string::npos) continue;

        splitstring ss(line.c_str());
        auto ssv = ss.split('(');
        splitstring ssfirst(ssv[0].c_str());
        auto ssfirstv = ssfirst.split(' ');

        unsigned long ID = stoul(ssfirstv[0]);
        bool skip = true;
        for (auto x : cID)
        {
            if (ID == x)
            {
                skip = false;
                break;
            }
        }
        if (skip) continue;
        std::cout << line << std::endl;
        std::string nn = ssv[1];
        std::replace(nn.begin(), nn.end(), ')', ' ');
        
        splitstring sssecond(nn.c_str());
        auto sssecondv = sssecond.split(' ');
        for (auto k : sssecondv)
        {
            unsigned long idx = std::stoul(k);
            neighbours.push_back(idx);
        }
    }
}

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
            p.c = 0;
            std::istringstream iss(split[1]); 
            if (!(iss >> p.x[0] >> p.x[1] >> p.x[2]))
            {
                throw std::string("Error parsing line\n" + line);
            } 
            cellPoints.push_back(std::move(p));
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
                faces.push_back(std::move(f));
            }
        }
    }
}


void printPovCells (std::ofstream& out, std::vector<face>& faces, std::vector<pT>& cellPoints)
{

    std::cout << "\tprint voronoi cells" << std::endl;
    out << "#declare cylinders =  union {\n";

    out << std::fixed << std::setprecision(12);

    double cylinderRadius = 0.02;

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
            if (comparePoints(*p1, *p2)) continue;
            

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

        if (!comparePoints(*pe, *ps))
        {
            out << "cylinder {<" ;
            out << pe->x[0] <<", " ;
            out << pe->x[1] << ", " ;
            out << pe->x[2] <<"> ,<";

            out << ps->x[0] <<", ";
            out << ps->x[1] << ", ";
            out << ps->x[2] <<">," << cylinderRadius << "}" << std::endl;
        }
    }

    out <<"}\n";
}

void printEllipsoids (std::ofstream& out, std::vector<ellipsoid>& ellipsoids)
{
    std::cout << "\tprint ellipsoids" << std::endl;

    out << "#declare spheres =  union {\n";
    for(const auto& e : ellipsoids)
    {
        // spawn ellipsoid at origin
            out << "sphere {<"   << 0 << ", ";
            out <<                  0 << ", ";
            out <<                  0 << "> ," << 0.97;// "}\n";

            // scale ellipsoid r1 times two
            out << " matrix  < " <<  e.r1 << ", " << 0 << ", " << 0         << ", ";
            out <<                     0               << ", " << e.r2 << ", " << 0         << ", ";
            out <<                     0               << ", " << 0 << ", " << e.r3 << ", ";
            out <<                     "0, 0, 0> ";
            //rotate ellipsoid
//          out << " matrix  < " <<    e.a1[0] << ", " << e.a2[0] << ", " << e.a3[0] << ", ";
//          out <<                     e.a1[1] << ", " << e.a2[1] << ", " << e.a3[1] << ", ";
//          out <<                     e.a1[2] << ", " << e.a2[2] << ", " << e.a3[2] << ", ";
//          out <<                     "0         , 0               , 0>";
         out << " matrix  < " <<    e.a1[0] << ", " << e.a1[1] << ", " << e.a1[2] << ", ";
         out <<                     e.a2[0] << ", " << e.a2[1] << ", " << e.a2[2] << ", ";
         out <<                     e.a3[0] << ", " << e.a3[1] << ", " << e.a3[2] << ", ";
         out <<                     "0         , 0               , 0>";
            //translate
            out << " translate <"   << e.x[0] << ", ";
            out <<                    e.x[1] << ", ";
            out <<                    e.x[2] << "> "; 
            out << "}\n";
    }
    out << "}\n";
}


void parseXYZR (std::string xyzrFileName, std::vector<ellipsoid>& spheres, const std::set<unsigned long> & labelList)
{
    std::cout << "parsing xyzr file" << std::endl;
    std::ifstream infile;
    infile.open(xyzrFileName);
    if (infile.fail())
    {
        throw std::string ("cannot open xyzr input file");
    }
    std::string line = "";

    // ignore the top two lines
    std::getline(infile, line);
    std::getline(infile, line);


    unsigned long lineID = 0;
    while(std::getline(infile, line))   // parse lines
    {
        if (line.find("#") != std::string::npos) continue;  // ignore comment lines
        
        //std::cout << std::endl <<  line << " " << std::endl;
        splitstring ss(line.c_str());
        std::vector<std::string> split = ss.split(' ');
        if(split.size() != 6) std::cerr << "WARNING: Line Size not correct!" << std::endl;
        lineID++;
        

        ellipsoid s;
        s.l = lineID;
        s.x[0] = std::stod(split[1]);
        s.x[1] = std::stod(split[2]);
        s.x[2] = std::stod(split[3]);
       
        s.r1    = std::stod(split[4]); 
        s.r2    = std::stod(split[4]); 
        s.r3    = std::stod(split[4]); 

        s.a1[0] = 1;
        s.a1[1] = 0;
        s.a1[2] = 0;
        
        s.a2[0] = 0;
        s.a2[1] = 1;
        s.a2[2] = 0;

        s.a3[0] = 0;
        s.a3[1] = 0;
        s.a3[2] = 1;
        


        bool foundLabel = false;        
        for (auto l : labelList)
            if (l == s.l){foundLabel= true; break;} 
        if (!foundLabel) continue;
       spheres.push_back(s); 
    }
    infile.close();
}


void findOtherFace( std::vector<face>& faces, unsigned long u, face& f2f, unsigned long idx11, unsigned long idx12, bool& foundOne)
{
    face f1 = faces[u];
    // loop over all other faces
    for (unsigned int v = 0; v != faces.size(); ++v)
    {
        if (u == v) continue;       // ignore if same face
        face f2 = faces[v];

        if (f1.c != f2.c) continue; // ignore all faces from other cells
        if (f1.l == f2.l) continue; // ignore if we have the same face

        bool foundBoth = false;
        //loop over all vertices of the second face
        for (unsigned long j = 0; j != f2.vx.size(); ++j)
        {
            // check if we can find both points on the second face f2
            unsigned long idx21 = f2.vx[j];
            // if the points do not match, continue
            if(idx21 != idx11) continue;

            for (unsigned long j2 = 0; j2 != f2.vx.size(); ++j2)
            {
                unsigned long idx22 = f2.vx[j2];
                // if the points do not match, continue
                if(idx22 != idx12) continue;
                foundBoth = true;
            }
        }


        if (!foundBoth) continue; // ignore  the second face (f2) if it does not share the edge we are looking at
        f2f = f2;
        foundOne = true;
        break;

    }
}


void printEdge(std::ofstream& out, std::vector<pT>& cellPoints, face& f1, face& f2f, pT& p11, pT& p12, double normalThresh, std::vector<double>& histPhi, double deltaPhi)
{
                double cylinderRadius = 0.02;
                // only call if face f1 and f2 share a common edge
                // get three points from face 1 (u)
                pT f1p1 = *(getPoint(cellPoints,f1.vx[0]));
                pT f1p2 = *(getPoint(cellPoints,f1.vx[1]));
                pT f1p3 = *(getPoint(cellPoints,f1.vx[2]));

                // get three points from face 2 (v)
                pT f2p1 = *(getPoint(cellPoints,f2f.vx[0]));
                pT f2p2 = *(getPoint(cellPoints,f2f.vx[1]));
                pT f2p3 = *(getPoint(cellPoints,f2f.vx[2]));

                // calculate normal for face 1
                pT d11;
                for (unsigned int k = 0; k != 3; ++k)
                    d11.x[k] =f1p2.x[k] - f1p1.x[k];
                pT d12;
                for (unsigned int k = 0; k != 3; ++k)
                    d12.x[k] =f1p2.x[k] - f1p3.x[k];

                pT n1;
                n1.x[0] = d11.x[1] * d12.x[2] - d12.x[1]*d11.x[2];
                n1.x[1] = d11.x[2] * d12.x[0] - d12.x[2]*d11.x[0];
                n1.x[2] = d11.x[0] * d12.x[1] - d12.x[0]*d11.x[1];

                // calculate normal for face 2
                pT d21;
                for (unsigned int k = 0; k != 3; ++k)
                    d21.x[k] =f2p2.x[k] - f2p1.x[k];
                pT d22;
                for (unsigned int k = 0; k != 3; ++k)
                    d22.x[k] =f2p2.x[k] - f2p3.x[k];

                pT n2;
                n2.x[0] = d21.x[1] * d22.x[2] - d22.x[1]*d21.x[2];
                n2.x[1] = d21.x[2] * d22.x[0] - d22.x[2]*d21.x[0];
                n2.x[2] = d21.x[0] * d22.x[1] - d22.x[0]*d21.x[1];

                // to get rid of normalisation constant in skalarproduct, normalize normals (let's wait until we get denormalized floats dafuq??)
                normalize(n1);
                normalize(n2);

                // calculate angle between normals here
                double phi = 0;
                for (unsigned int k = 0; k != 3; ++k)
                    phi += n1.x[k]*n2.x[k];

                phi = std::acos(phi);
                unsigned long histIDX = static_cast<unsigned long>(phi / deltaPhi);
                if (histIDX > histPhi.size()) histIDX = histPhi.size();
                histPhi[histIDX] += 1;
                if (phi < 0 ) std::cout << "negative phi: " << phi << std::endl;
                if (phi > normalThresh)
                { 
                    //std::cout << "phi: " << phi << std::endl;
                    out << "cylinder {<" << p11.x[0] << ", ";
                    out <<                  p11.x[1] << ", ";
                    out <<                  p11.x[2] << "> ,<";
                    out <<                  p12.x[0] << ", ";
                    out <<                  p12.x[1] << ", ";
                    out <<                  p12.x[2] << ">,"<< cylinderRadius << " }" << std::endl;
                }
}  




void parseEllipFile (std::string ellipFileName, std::vector<ellipsoid>& ellipsoids, std::set<unsigned long> & labelList)
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
        if(split.size() != 16) std::cerr << "WARNING: Line Size not correct!" << std::endl << line << std::endl;

        ellipsoid e;
        e.l = std::stoi(split[0]);
        e.x[0] = std::stod(split[1]);
        e.x[1] = std::stod(split[2]);
        e.x[2] = std::stod(split[3]);

        e.r1 = std::stod(split[4]);
        e.a1[0] = std::stod(split[5]);
        e.a1[1] = std::stod(split[6]);
        e.a1[2] = std::stod(split[7]);
        
        e.r2 = std::stod(split[8]);
        e.a2[0] = std::stod(split[9]);
        e.a2[1] = std::stod(split[10]);
        e.a2[2] = std::stod(split[11]);

        e.r3 = std::stod(split[12]);
        e.a3[0] = std::stod(split[13]);
        e.a3[1] = std::stod(split[14]);
        e.a3[2] = std::stod(split[15]);

        bool foundLabel = false;        
        for (auto l : labelList)
            if (l == e.l){foundLabel= true; break;} 
        if (!foundLabel) continue;
        std::cout << "\n" << line << "\n";
        std::cout << "ellipsoid: " << e.l << "\n";
        std::cout << "\nr1:\t" << e.r1 << ""; 
        std::cout << "\nr2:\t" << e.r2 << ""; 
        std::cout << "\nr3:\t" << e.r3 << "\n"; 
        std::cout << "\na1:\t" << e.a1[0] << " " << e.a1[1] << " " << e.a1[2] << ""; 
        std::cout << "\t\t " << (split[5]) << " " << (split[6]) << " " << (split[7]) << std::endl;
        std::cout << "\t\t " << std::stod(split[5]) << " " << std::stod(split[6]) << " " << std::stod(split[7]) << std::endl;
        std::cout << "\na2:\t" << e.a2[0] << " " << e.a2[1] << " " << e.a2[2] << ""; 
        std::cout << "\na3:\t" << e.a3[0] << " " << e.a3[1] << " " << e.a3[2] << "\n"; 
       ellipsoids.push_back(e); 
    }
    std::cout << "parsed N= " << ellipsoids.size() << " particles from ellip file" << std::endl;
    infile.close();
}
