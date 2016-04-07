//
//  TCamera.hpp
//  CherenkovSimulator
//
//  Created by Matthew Dutson on 3/14/16.
//  Copyright © 2016 Matthew Dutson. All rights reserved.
//

#ifndef TCamera_h
#define TCamera_h

#include "TPlane3.h"
#include "TSegmentedData.h"
#include "THistogramArray.h"
#include "TRawData.h"
#include "TRay.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TResponseFunction.h"

class TCamera {
    
private:
    
    Double_t fFocalLength;
    
    Double_t fHeight;
    
    Int_t fNumberTubesY;
    
    Double_t fWidth;
    
    Int_t fNumberTubesX;
    
    Double_t fPMTResolution;
    
    TResponseFunction* fResponseFunction;
    
    Bool_t fCheckBackCollision = false;
    
public:
    
    TCamera(Double_t focalLength, Double_t width, Int_t numberTubesX, Double_t height, Int_t numberTubesY, Double_t PMTResolution, TResponseFunction* responseFunction, Bool_t checkBackCollision);
    
    Double_t FocalLength();
    
    Bool_t CheckCollision(TVector3 position);
    
    TVector3 GetViewDirection(Int_t bin);
    
    TVector2 GetPixelPosition(Int_t bin);
    
    Int_t GetBin(TVector2 pixelPosition);
    
    TSegmentedData SegmentedData(TRawData data);
    
    THistogramArray PhotonHistograms(TSegmentedData data);
    
    THistogramArray VoltageHistograms(THistogramArray photonHistograms, Int_t nFrequencyBins);

};

#endif /* TCamera_h */
