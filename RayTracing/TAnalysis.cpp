/*
 * Created by Matthew Dutson on 2/8/16.
 * Copyright © 2016 Matthew Dutson. All rights reserved.
 *
 * This file contains the implementation of "TAnalysis.h". See the header file for method descriptions.
 */

#include "TAnalysis.h"
#include "TMath.h"
#include "TConstantIntensity.h"

Double_t TAnalysis::SumArray(std::vector<Double_t> array) {
    Double_t sum = 0;
    for (Double_t d: array) {
        sum += d;
    }
    return sum;
}

Double_t TAnalysis::FindRMSDeviation(TDataCollection data) {
    
    // Compute the average x and y values
    Long_t n = data.Size();
    Double_t xAverage = SumArray(data.GetXData()) / n;
    Double_t yAverage = SumArray(data.GetYData()) / n;
    
    // Compute the variance by repeatedly adding the distance squaared to the variance and then dividing it by n
    Double_t variance = 0;
    for (Int_t i = 0; i < n; i++) {
        variance += (data.GetX(i) - xAverage) * (data.GetX(i) - xAverage) + (data.GetY(i) - yAverage) * (data.GetY(i) - yAverage);
    }
    variance = variance / n;
    
    // Find the RMS deviation from the variance
    return TMath::Sqrt(variance);
}

void TAnalysis::FindRMSVsAngle(std::vector<Double_t>& RMS, std::vector<Double_t>& angle, TTelescope telescope, Int_t sampleNumber, Double_t timeDelay, Double_t minAngle, Double_t maxAngle, Double_t zDistance) {
    
    // Make sure the arrays are clear
    RMS.clear();
    angle.clear();
    
    // Check that the min and max angles are appropriate
    if (minAngle > maxAngle) {
        Double_t temp = minAngle;
        minAngle = maxAngle;
        maxAngle = temp;
    }
    else if (minAngle == maxAngle) {
        throw std::invalid_argument("The minimum and the maximum angles must be different");
    }

    // Find the starting height, the ending height, and the number of points where data will be collected
    Double_t startingHeight = zDistance * TMath::Tan(minAngle);
    Double_t endingHeight = zDistance * TMath::Tan(maxAngle);
    Int_t nSteps = (Int_t) ((endingHeight - startingHeight) / (timeDelay * TRay::fLightSpeed)) + 1;
    TRay showerRay = TRay(0, TVector3(startingHeight, 0, zDistance), TVector3(1, 0, 0));
    TConstantIntensity* intensityFunction = new TConstantIntensity(sampleNumber);
    TShower shower = TShower(showerRay, intensityFunction);
    
    // These vectors store the detection data at each point
    TDataCollection data = TDataCollection();
    
    for (Int_t i = 0; i < nSteps; i++) {
        
        // Collect data at each point along the ray's path and store it in xArray and yArray
        telescope.ViewPoint(shower, data);
        shower.IncrementPosition(timeDelay);
        
        // Compute the RMS deviation at each point and store it in the output array
        Double_t standardDeviation = FindRMSDeviation(data);
        RMS.push_back(standardDeviation);
        angle.push_back(shower.GetPosition().Theta());
    }
    delete intensityFunction;
}

void TAnalysis::FillHistogram(std::vector<Double_t> array1, std::vector<Double_t> array2, TH2D& histogram) {
    Long_t n = array1.size();
    for (Int_t i = 0; i < n; i++) {
        histogram.Fill(array1[i], array2[i]);
    }
}

void TAnalysis::FillProfile(std::vector<Double_t> array1, std::vector<Double_t> array2, TProfile& profile) {
    Long_t n = array1.size();
    for (Int_t i = 0; i < n; i++) {
        profile.Fill(array1[i], array2[i]);
    }
}