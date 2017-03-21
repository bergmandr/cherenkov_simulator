// ReconstructorTest.cpp
//
// Author: Matthew Dutson
//
// Tests of Reconstructr.h

#include <gtest/gtest.h>
#include <TFile.h>
#include <TVectorD.h>

#include "Reconstructor.h"
#include "Simulator.h"
#include "MonteCarlo.h"
#include "Utility.h"
#include "Analysis.h"

using boost::property_tree::ptree;
using std::vector;

namespace cherenkov_simulator
{
    class ReconstructorTest : public testing::Test
    {

    protected:

        Simulator* simulator;
        MonteCarlo* monte_carlo;
        Reconstructor* reconstructor;

        virtual void SetUp()
        {
            ptree config = Utility::ParseXMLFile("../../../Config.xml").get_child("config");
            simulator = new Simulator(config);
            monte_carlo = new MonteCarlo(config);
            reconstructor = new Reconstructor(config);
        }

        virtual void TearDown()
        {
            delete simulator;
            delete monte_carlo;
            delete reconstructor;
        }

        bool FriendFrameTriggered(int i, vector<vector<vector<bool>>>* triggers)
        {
            return reconstructor->FrameTriggered(i, triggers);
        }

        vector<vector<vector<bool>>> FriendTriggeringMatrices(PhotonCount data)
        {
            return reconstructor->GetTriggeringMatrices(data);
        }
    };

    TEST_F(ReconstructorTest, StraightShower)
    {
        // Construct and simulate a shower at 10 km. The axis coordinates are in the world frame.
        Shower shower = monte_carlo->GenerateShower(TVector3(0, 0, -1), 1e6, 0, 1e19);
        PhotonCount data = simulator->SimulateShower(shower);

        // Attempt to reconstruct the shower plane and geometry.
        TRotation to_sd_plane = reconstructor->FitSDPlane(data);
        double t_0, impact, angle;
        TGraphErrors time_profile = reconstructor->MonocularFit(data, to_sd_plane, &t_0, &impact, &angle);

        // Write results to a file.
        TFile file("StraightShowerRecon.root", "RECREATE");
        time_profile.Write("straight_shower_profile");
        TVectorD reconstruction(3);
        reconstruction[0] = t_0;
        reconstruction[1] = impact;
        reconstruction[2] = angle;
        reconstruction.Write("straight_shower_params");
    }

    TEST_F(ReconstructorTest, AngleShower)
    {
        // Construct and simulate a shower at 10km which is skewed at some angle.
        Shower shower = monte_carlo->GenerateShower(TVector3(1, 0, -2), 1e6, 0, 1e19);
        PhotonCount data = simulator->SimulateShower(shower);

        // Attempt to reconstruct the shower plane and geometry.
        TRotation to_sd_plane = reconstructor->FitSDPlane(data);
        double t_0, impact, angle;
        TGraph time_profile = reconstructor->MonocularFit(data, to_sd_plane, &t_0, &impact, &angle);

        // Draw a map of impacts.
        TFile file("AngleShowerRecon.root", "RECREATE");
        time_profile.Write("angle_shower_profile");
        TVectorD reconstruction(3);
        reconstruction[0] = t_0;
        reconstruction[1] = impact;
        reconstruction[2] = angle;
        reconstruction.Write("angle_shower_params");
    }

    TEST_F(ReconstructorTest, AddSubtractNoise)
    {
        TFile file("AddSubtractNoise.root", "RECREATE");
        Shower shower = monte_carlo->GenerateShower(TVector3(0, 0, -1), 1e6, 0, 1e19);
        PhotonCount data = simulator->SimulateShower(shower);

        Analysis::MakeProfileGraph(data).Write("before_noise_graph");
        Analysis::MakeSumMap(data).Write("before_noise_map");

        reconstructor->AddNoise(&data);
        Analysis::MakeProfileGraph(data).Write("after_noise_graph");
        Analysis::MakeSumMap(data).Write("after_noise_map");

        reconstructor->SubtractAverageNoise(&data);
        Analysis::MakeProfileGraph(data).Write("after_subtract_graph");
        Analysis::MakeSumMap(data).Write("after_subtract_map");

        reconstructor->ThreeSigmaFilter(&data);
        Analysis::MakeProfileGraph(data).Write("three_sigma_graph");
        Analysis::MakeSumMap(data).Write("three_sigma_map");

        reconstructor->ApplyTriggering(&data);
        Analysis::MakeProfileGraph(data).Write("after_trigger_graph");
        Analysis::MakeSumMap(data).Write("after_trigger_map");
    }

    TEST_F(ReconstructorTest, TriggeringMaps)
    {
        TFile file("TriggeringMaps.root", "RECREATE");
        Shower shower = monte_carlo->GenerateShower(TVector3(0, 0, -1), 1e6, 0, 1e19);
        PhotonCount data = simulator->SimulateShower(shower);

        std::vector<std::vector<std::vector<bool>>> triggering_matrices = FriendTriggeringMatrices(data);
        for (int i = 0; i < triggering_matrices.size(); i++)
        {
            TH2C frame_map = Analysis::GetBooleanMap(triggering_matrices[i]);
            std::string write_name;
            if (FriendFrameTriggered(i, &triggering_matrices))
            {
                write_name = std::to_string(i) + "tr";
            }
            else
            {
                write_name = std::to_string(i) + "nt";
            }
            frame_map.Write(write_name.c_str());
        }
    }

    TEST_F(ReconstructorTest, FindGroundImpact)
    {
        TFile file("ImpactPoint.root", "RECREATE");
        Shower shower = monte_carlo->GenerateShower(TVector3(0, 0, -1), 1e6, 0, 1e19);
        PhotonCount data = simulator->SimulateShower(shower);

        TVector3 impact;
        if (reconstructor->FindGroundImpact(data, &impact))
        {
            impact.Write("ground_impact");
        }
    }

    TEST_F(ReconstructorTest, StraightShowerGeometry)
    {
        TFile file("StraightGeometry.root", "RECREATE");
        Shower shower = monte_carlo->GenerateShower(TVector3(0, 0, -1), 1e6, 0, 1e19);
        PhotonCount data = simulator->SimulateShower(shower);

        bool triggered, ground_used;
        Shower reconstructed = reconstructor->Reconstruct(data, false, &triggered, &ground_used);
        reconstructed.Position().Write("straight_shower_position");
        reconstructed.Direction().Write("straight_shower_direction");
    }

    TEST_F(ReconstructorTest, AngleShowerGeometry)
    {
        TFile file("AngleGeometry.root", "RECREATE");
        Shower shower = monte_carlo->GenerateShower(TVector3(1, 0, -2), 1e6, 0, 1e19);
        PhotonCount data = simulator->SimulateShower(shower);

        bool triggered, ground_used;
        Shower reconstructed = reconstructor->Reconstruct(data, false, &triggered, &ground_used);
        reconstructed.Position().Write("angle_shower_position");
        reconstructed.Direction().Write("angle_shower_direction");
    }

    TEST_F(ReconstructorTest, StraightCherenkovRecon)
    {
        TFile file("StraightCherenkovRecon.root", "RECREATE");
        Shower shower = monte_carlo->GenerateShower(TVector3(0, 0, -1), 1e6, 0, 1e19);
        PhotonCount data = simulator->SimulateShower(shower);

        bool triggered, ground_used;
        Shower reconstructed = reconstructor->Reconstruct(data, true, &triggered, &ground_used);
        reconstructed.Position().Write("straight_shower_position");
        reconstructed.Direction().Write("straight_shower_direction");
    }

    TEST_F(ReconstructorTest, AngleCherenkovRecon)
    {
        TFile file("AngleCherenkovRecon.root", "RECREATE");
        Shower shower = monte_carlo->GenerateShower(TVector3(1, 0, -2), 1e6, 0, 1e19);
        PhotonCount data = simulator->SimulateShower(shower);

        bool triggered, ground_used;
        Shower reconstructed = reconstructor->Reconstruct(data, true, &triggered, &ground_used);
        reconstructed.Position().Write("angle_shower_position");
        reconstructed.Direction().Write("angle_shower_direction");
    }
}