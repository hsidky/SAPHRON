#pragma once

#include "SimObserver.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>

namespace Simulation
{
	class CSVObserver : public SimObserver
	{
		private:
			std::unique_ptr<std::ofstream> _ensemblefs;
			std::unique_ptr<std::ofstream> _modelfs;
			std::unique_ptr<std::ofstream> _sitefs;
			std::unique_ptr<std::ofstream> _dosfs;
			std::unique_ptr<std::ofstream> _histfs;

			bool _printedHeaders = false;

		public:
			CSVObserver(std::string prefix, SimFlags flags, unsigned int frequency = 1)
				: SimObserver(flags, frequency)
			{
				if(flags.ensemble)
				{
					_ensemblefs = std::unique_ptr<std::ofstream>(
					        new std::ofstream(prefix + ".ensemble.csv")
					        );
					_ensemblefs->precision(20);

					if (this->Flags.iterations)
						*_ensemblefs << "Iterations,";
					if (this->Flags.energy)
						*_ensemblefs << "Energy,";
					if(this->Flags.temperature)
						*_ensemblefs << "Temperature,";
					if(this->Flags.pressure)
						*_ensemblefs << "Pressure,";
					if(this->Flags.composition)
						*_ensemblefs << "Composition,";

					*_ensemblefs << std::endl;
				}

				if(flags.dos)
				{
					_dosfs = std::unique_ptr<std::ofstream>(
					        new std::ofstream(prefix + ".dos.csv")
					        );
					_dosfs->precision(20);

					if(this->Flags.iterations)
						*_dosfs << "Iterations,";
					if(this->Flags.dos_walker)
						*_dosfs << "Walker ID,";
					if(this->Flags.dos_scale_factor)
						*_dosfs << "Scale Factor,";
					if (this->Flags.dos_flatness)
						*_dosfs << "Flatness,";
					if(this->Flags.dos_interval)
						*_dosfs << "Interval Min,Interval Max,";
					if(this->Flags.dos_values)
						*_dosfs << "DOS,";

					*_dosfs << std::endl;
				}

				if(flags.model)
				{
					_modelfs = std::unique_ptr<std::ofstream>(
					        new std::ofstream(prefix + ".model.csv")
					        );
					_modelfs->precision(20);
				}

				if(flags.histogram)
				{
					_histfs = std::unique_ptr<std::ofstream>(
					        new std::ofstream(prefix + ".hist.csv")
					        );
					_histfs->precision(20);

					if (this->Flags.iterations)
						*_histfs << "Iterations,";
					if(this->Flags.hist_bin_count)
						*_histfs << "Bin Count,";
					if(this->Flags.hist_lower_outliers)
						*_histfs << "Lower Outlier Count,";
					if(this->Flags.hist_upper_outliers)
						*_histfs << "Upper Outlier Count,";
					if(this->Flags.hist_values)
						*_histfs << "HIST,";

					*_histfs << std::endl;
				}

				if(flags.site)
				{
					_sitefs = std::unique_ptr<std::ofstream>(
					        new std::ofstream(prefix + ".site.csv")
					        );
					_sitefs->precision(20);

					if (this->Flags.iterations)
						*_sitefs << "Iterations,";
					if(this->Flags.site_coordinates)
						*_sitefs << "X,Y,Z,";
					if(this->Flags.site_unit_vectors)
						*_sitefs << "UX,UY,UZ,";
					if(this->Flags.site_species)
						*_sitefs << "Species ID,";
					if(this->Flags.site_neighbors)
						*_sitefs << "Neighbor List,";

					*_sitefs << std::endl;
				}
			}

			~CSVObserver()
			{
				if(_ensemblefs)
					_ensemblefs->close();
				if(_modelfs)
					_modelfs->close();
				if(_sitefs)
					_sitefs->close();
				if(_dosfs)
					_dosfs->close();
				if(_histfs)
					_histfs->close();
			}

			virtual void VisitInternal(Ensembles::DensityOfStatesEnsemble<Site>* e) override;
			virtual void VisitInternal(Ensembles::WangLandauDOSEnsemble<Site>* e) override;
			virtual void VisitInternal(Ensembles::SemiGrandDOSEnsemble<Site>* e) override;
			virtual void VisitInternal(Ensembles::NVTEnsemble<Site>* e) override;
			virtual void VisitInternal(Models::BaseModel* m) override;
			virtual void VisitInternal(Site* s) override;
			virtual void VisitInternal(Histogram* h) override;
	};
}
