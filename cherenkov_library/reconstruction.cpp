// reconstruction.cpp
// cherenkov_simulator
//
// Created by Matthew Dutson on 9/8/16.
//
//

#include "simulator.h"
#include "data_containers.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"

namespace cherenkov_simulator
{
    TVector3 Simulator::FitSDPlane(PhotonCount data)
    {
        // Compute the matrix specified in Stratton 3.4.
        // TODO: Check that a TMatrixDSym doesn't have unexpected behavior when setting elements individually.
        // Maybe just iterate over elements whose symmetric counterparts haven't been found yet.
        PhotonCount::SignalIterator iter = data.Iterator();
        TMatrixDSym matrix(3, 3);
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                double mat_element = 0;
                iter.Reset();
                while (iter.Next())
                {
                    TVector3 direction = CameraIndexToViewDirection(iter.X(), iter.Y());
                    int pmt_sum = data.SumBins(iter);
                    mat_element += direction[j] * direction[k] * pmt_sum;
                }
                matrix[j][k] = mat_element;
            }
        }

        // Find the eigenvector corresponding to the minimum eigenvalue.
        TMatrixDSymEigen eigen = TMatrixDSymEigen(matrix);
        TVectorD eigen_val = eigen.GetEigenValues();
        TMatrixD eigen_vec = eigen.GetEigenVectors();
        double min_val = eigen_val[0];
        int min_index = 0;
        for (int i = 0; i < eigen_val.GetNoElements(); i++)
        {
            if (eigen_val[i] < min_val)
            {
                min_val = eigen_val[i];
                min_index = i;
            }
        }
        // TODO: Make sure this indexing on the matrix is correct.
        TVector3 result = TVector3(eigen_vec[min_index][0], eigen_vec[min_index][1], eigen_vec[min_index][2]);
        return rotate_to_world * result;
    }
}
