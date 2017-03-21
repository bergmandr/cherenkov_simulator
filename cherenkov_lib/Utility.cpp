// Utility.cpp
//
// Author: Matthew Dutson
//
// Implementation of Utility.h

#include <fstream>
#include <boost/property_tree/xml_parser.hpp>
#include <TRotation.h>
#include <TMath.h>

#include "Utility.h"

using namespace std;
using namespace TMath;

using boost::property_tree::ptree;

namespace cherenkov_simulator
{
    TVector3 Utility::ToVector(string s)
    {
        // Clear out everything before the first parenthesis.
        int current = s.find('(');
        s.erase(0, current + 1);

        // Pull double values from the vector.
        TVector3 output = TVector3();
        output.SetX(ParseTo(&s, ','));
        output.SetY(ParseTo(&s, ','));
        output.SetZ(ParseTo(&s, ')'));
        return output;
    }

    TVector3 Utility::RandNormal(TVector3 vec, TRandom3* rng)
    {
        if (vec.X() == 0 && vec.Y() == 0 && vec.Z() == 0)
        {
            return TVector3(1, 0, 0);
        }
        else
        {
            TVector3 other_vec = vec + TVector3(1, 0, 0);
            TVector3 normal = (vec.Cross(other_vec)).Unit();
            normal.Rotate(rng->Uniform(2 * TMath::Pi()), vec);
            return normal;
        }
    }

    ptree Utility::ParseXMLFile(string filename)
    {
        // Try opening the specified file
        ifstream config_file = ifstream();
        try
        {
            config_file.open(filename);
        }
        catch (...)
        {
            std::string message = "The file " + filename + " could not be opened. Check the path.";
            throw std::runtime_error(message);
        }

        // Parse the file to XML.
        try
        {
            ptree xml_file = ptree();
            read_xml(config_file, xml_file);
            return xml_file;
        }
        catch (...)
        {
            throw std::runtime_error("There was a problem parsing the file to XML. Check for syntax errors.");
        }
    }

    bool Utility::WithinXYDisk(TVector3 vec, double radius)
    {
        return Sqrt(Sq(vec.X()) + Sq(vec.Y())) < radius;
    }

    TRotation Utility::MakeRotation(double elevation_angle)
    {
        TRotation rotate = TRotation();
        rotate.RotateX(-PiOver2() + elevation_angle);
        return rotate;
    }

    double Utility::RandLinear(TRandom3* rng, double max)
    {
        return max * Sqrt(rng->Uniform());
    }

    int Utility::RandomRound(double value, TRandom3* rng)
    {
        double decimal = value - Floor(value);
        int base = (int) (value - decimal);
        if (rng->Rndm() < decimal) return base + 1;
        else return base;
    }

    double Utility::ParseTo(string* s, char c)
    {
        size_t index = s->find(c);
        double out = stod(s->substr(0, index));
        s->erase(0, index + 1);
        return out;
    }
}
