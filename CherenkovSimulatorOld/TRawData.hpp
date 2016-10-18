/*
 * CherenkovSimulator - TRawData.hpp
 *
 * Copyright © 2016 Matthew Dutson. All rights reserved.
 *
 * Contains data points, each of which represents the time, x position, and y position of a photon hitting the focal plane. This data has not yet been separated between photomultipler tubes.
 */

#ifndef TRawData_hpp
#define TRawData_hpp

#include <stdio.h>
#include "TMath.h"

class TRawData {
    
private:
    
    std::vector<Double_t> fXData;
    
    std::vector<Double_t> fYData;
    
    std::vector<Double_t> fTData;
    
public:
    
    TRawData();
    
    void PushBack(Double_t x, Double_t y, Double_t t);
    
    void Clear();
    
    std::vector<Double_t> GetXData();
    
    std::vector<Double_t> GetYData();
    
    std::vector<Double_t> GetTData();
    
    Double_t GetX(Int_t index);
    
    Double_t GetY(Int_t index);
    
    Double_t GetT(Int_t index);
    
    Double_t Size();
    
};

#endif /* TRawData_h */
