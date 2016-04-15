/*
 * CherenkovSimulator - TCoordinates.cpp
 *
 * Copyright © 2016 Matthew Dutson. All rights reserved.
 *
 * Contains the implementation of TCoordinates.
 */

#include "TCoordinates.hpp"

TCoordinates::TCoordinates() {}

TCoordinates::TCoordinates(Double_t azimuth, Double_t elevation, TVector3 centerOfCurvature) {
    fAzimuth = azimuth;
    fElevation = elevation;
    fCenterOfCurvature = centerOfCurvature;
}

void TCoordinates::PositionToObservatoryFrame(TVector3& position) {
    position -= fCenterOfCurvature;
    DirectionToObservatoryFrame(position);
}

void TCoordinates::PositionToExternalFrame(TVector3& position) {
    DirectionToExternalFrame(position);
    position += fCenterOfCurvature;
}

void TCoordinates::DirectionToObservatoryFrame(TVector3& direction) {
    direction.RotateX(-fElevation);
    direction.RotateY(-fAzimuth);
}

void TCoordinates::DirectionToExternalFrame(TVector3& direction) {
    direction.RotateX(fElevation);
    direction.RotateY(fAzimuth);
}

TVector3 TCoordinates::GetMirrorAxis() {
    TVector3 output = TVector3(0, 0, 1);
    DirectionToExternalFrame(output);
    return output;
}

TVector3 TCoordinates::GetCenterOfCurvature() {
    return fCenterOfCurvature;
}