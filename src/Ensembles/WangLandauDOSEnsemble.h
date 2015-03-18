// #pragma once
//
// #include "DensityOfStatesEnsemble.h"
//
// namespace Ensembles
// {
//      // Class for Wang-Landau density of states sampling.
//      template<typename T>
//      class WangLandauDOSEnsemble : public DensityOfStatesEnsemble<T>
//      {
//              public:
//                      // Initializes a Wang-Landau sampler for a specified model. The binning will
//                      // be performed according to the specified minimum and maxiumum energies and
//                      // the bin count.
//                      WangLandauDOSEnsemble(BaseModel& model, double minE, double maxE, int binCount);
//
//                      // Initializes a Wang-Landau sampler for a specified model. The binning will
//                      // be performed according to the specified minimum and maxiumum energies and
//                      // the bin width.
//                      WangLandauDOSEnsemble(BaseModel& model, double minE, double maxE, double binWidth);
//
//                      // Performs one Monte-Carlo iteration. This is precisely one random draw
//                      // from the model (one function call to model->DrawSample()).
//                      void Iterate();
//
//                      // Wang-Landau acceptance probability based on density of states.
//                      // Note that the "H" passed in here is the total energy of the system, rather than
//                      // individual energy (of a site).
//                      double AcceptanceProbability(double prevH, double currH);
//
//                      // Accept visitor to class.
//                      virtual void AcceptVisitor(class Visitor& v)
//                      {
//                              v.Visit(this);
//                              this->model.AcceptVisitor(v);
//                              this->hist.AcceptVisitor(v);
//                      }
//      };
// }
