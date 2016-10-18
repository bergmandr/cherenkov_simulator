// shower_simulator.cpp
// cherenkov_simulator
//
// Created by Matthew Dutson on 9/8/16.
//
//

#include "simulator.h"
#include "geometric_objects.h"

namespace cherenkov_simulator
{
    VoltageSignal Simulator::SimulateShower(Shower shower)
    {
        double time_delay = config.Get<double>("time_delay");
        Plane ground_plane = Plane(config.Get<TVector3>("ground_normal"), config.Get<TVector3>("ground_point"));
        int n_steps = (int) (shower.TimeToPlane(ground_plane) / time_delay) + 2;
        
        PhotonCount photon_count = PhotonCount(config);
        
        for(int i = 0; i < n_steps; i++) {
            ViewFluorescencePhotons(shower, &photon_count);
            ViewCherenkovPhotons(shower, ground_plane, &photon_count);
            shower.IncrementPosition(time_delay);
        }
        
        AddNoise(&photon_count);
        
        return VoltageResponse(photon_count);
    }
    
    void Simulator::ViewFluorescencePhotons(Shower shower, PhotonCount* photon_count)
    {
        int total_photons = shower.NumberFluorescencePhotons();
        double fraction_captured = FluorescenceFractionCaptured(shower);
        int number_detected = (int) (total_photons * fraction_captured);
        
        for (int i = 0; i < number_detected; i++)
        {
            TVector3 lens_impact = RandomStopImpact();
            Ray photon = Ray(shower.GetTime(), shower.GetPosition(), lens_impact - shower.GetPosition());
            photon.PropagateToPoint(lens_impact);
            
            SimulateOptics(photon, photon_count);
        }
    }
    
    void Simulator::ViewCherenkovPhotons(Shower shower, Plane ground_plane, PhotonCount* photon_count)
    {
        int cherenkov_photons = shower.NumberCherenkovPhotons();
        
        for (int i = 0; i < cherenkov_photons; i++)
        {
            Ray photon = shower.GenerateCherenkovPhoton();
            photon.PropagateToPlane(ground_plane);
            if (!ReflectFromGround(&photon)) continue;
            TVector3 stop_impact;
            if (!LensImpactPoint(photon, &stop_impact)) continue;
            photon.PropagateToPoint(stop_impact);
            
            SimulateOptics(photon, photon_count);
        }
    }
    
    void Simulator::SimulateOptics(Ray photon, PhotonCount* photon_count)
    {
        DeflectFromLens(&photon);
        
        TVector3 reflect_point;
        if (!MirrorImpactPoint(photon, &reflect_point)) return;
        if (BlockedByCamera(photon.GetPosition(), reflect_point)) return;
        photon.PropagateToPoint(reflect_point);
        photon.Reflect(MirrorNormal(reflect_point));
        
        TVector3 camera_impact;
        if (!CameraImpactPoint(photon, &camera_impact)) return;
        photon.PropagateToPoint(camera_impact);
        
        photon_count->AddPoint(GetViewDirection(camera_impact), photon.GetTime());
    }
}