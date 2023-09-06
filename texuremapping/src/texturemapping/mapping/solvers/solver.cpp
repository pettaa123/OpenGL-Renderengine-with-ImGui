#include "texturemapping/mapping/solvers/dltSolver.h"

namespace TextureMapping {
    //Solver Factory
    std::shared_ptr<Solver> Solver::create(const SolvingMethod solvingMethod,const bool normalizeData) { // Matrix3 intrinsicMatrix, float[] distortionCoefficients) {
        if (solvingMethod == SolvingMethod::DLT) {
            return std::make_shared<DLTSolver>(normalizeData);
        }

        //else if (solvingMethod == SolvingMethod::EPnP) {
        //    return std::make_shared < EPnPSolver>(normalizeData);
        //}
        //else if (solvingMethod == SolvingMethod::LHM)
        //{
        //    return new LHMSolver(normalizeData);
        //}
        else {
            Log::error("Unknown solving method");
            return nullptr;
        }
    }
}