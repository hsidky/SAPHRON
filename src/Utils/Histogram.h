#pragma once

#include "Observers/Visitable.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <limits>
#include "json/json.h"

namespace SAPHRON
{
	// A Histogram class. Along with the traditional histogram and binning, this class
	// provides "value" storage, which can be used to store custom data associated with
	// each entry in a bin. The interval specified in the histogram is [min, max). 
	class Histogram : public Visitable
	{
	private:
		// Width of bins.
		double _binWidth;

		// Number of bins.
		int _binCount;

		// Min and max values of bins.
		double _min, _max;

		// Count of histogram outliers.
		unsigned int _lowerOutlierCount = 0, _upperOutlierCount = 0;

		// Histogram counts.
		std::vector<unsigned int> _counts;

		// Histogram values.
		std::vector<double> _values;

	public:
		Histogram(double min, double max, int numberOfBins) :
			_binCount(numberOfBins), _min(min), _max(max)
		{
			_binWidth = (max-min)/numberOfBins;
			_counts.resize(numberOfBins, 0);
			_values.resize(numberOfBins, 0.0);
		}

		Histogram(double min, double max, double binWidth) :
			_binWidth(binWidth), _min(min), _max(max)
		{
			_binCount = (int) ceil((max-min)/binWidth);
			_counts.resize(_binCount, 0);
			_values.resize(_binCount, 0.0);
		}

		// Record a data point in the histogram and returns the bin index updated.
		inline int Record(double datum)
		{
			int bin = floor((datum - _min) / _binWidth);

			if (bin < 0)
				_lowerOutlierCount++;
			else if(bin >= _binCount)
				_upperOutlierCount++;
			else
				_counts[bin]++;
	
			return bin;
		}

		// Get the bin index associated with a data point.
		int GetBin(double datum) const
		{
			int bin = floor((datum - _min) / _binWidth);
			if(bin >= _binCount || bin < 0)
				return -1;
			
			return bin;
		}

		// Gets an associated value from a bin.
		double GetValue(int bin) const
		{
			if(bin >= _binCount)
				bin = _binCount -1;
			if(bin < 0)
				bin = 0;

			return _values[bin];
		}

		// Gets an associated value with the bin in which datum would reside.
		double GetValue(double datum) const
		{
			int bin = floor((datum - _min)/_binWidth);

			if(bin >= _binCount)
				bin = _binCount -1;
			if(bin < 0)
				bin = 0;

			return _values[bin];
		}

		// Update associated values in a bin.
		void UpdateValue(int bin, double value)
		{
			if(bin >= _binCount)
				return;
			if(bin < 0)
				return;

			_values[bin] = value;
		}

		// Get the number of data points in some bin.
		int Count(int bin) const
		{
			if(bin >= _binCount)
				bin = _binCount -1;
			if(bin < 0)
				bin = 0;

			return _counts[bin];
		}

		// Set the bin count.
		void SetCount(int bin, unsigned int count)
		{
			if(bin >= _binCount)
				bin = _binCount - 1;
			if(bin < 0)
				bin = 0;

			_counts[bin] = count;
		} 

		// Gets vector of bin values.
		const std::vector<double>& GetValues() const { return _values; }

		const std::vector<unsigned int>& GetHistogram() const { return _counts;	}

		// Gets the number of bins in the histogram.
		int GetBinCount() const { return _counts.size(); }

		// Gets the width of a bin.
		double GetBinWidth() const { return _binWidth; }

		// Gets the count of values below the minimum.
		int GetLowerOutlierCount() const { return _lowerOutlierCount; }

		// Gets the count of values above the maximum.
		int GetUpperOutlierCount() const { return _upperOutlierCount; }

		// Gets the minimum histogram value.
		double GetMinimum() const { return _min; }

		// Gets the maximum histogram value.
		double GetMaximum() const { return _max; }

		// Resets the histogram.
		void ResetHistogram()
		{
			for(size_t i = 0; i < _counts.size(); i++)
				_counts[i] = 0;
		}

		// Calculates the "flatness" of the histogram. That is, the maximum
		// (lower end) deviation of a bin count from the average counts of the bin.
		double CalculateFlatness() const 
		{
			double avg = 0;
			double minVal = 10.0e10;
			for(auto& count : _counts)
			{
				if(count < minVal)
					minVal = count;
				avg += count;
			}
			avg /= _binCount;

			if(avg == 0)
				return 0;

			return minVal/avg;
		}

		// Visitable interface.
		virtual void AcceptVisitor(Visitor& v) const override
		{
			v.Visit(*this);
		}

		// Builds a histogram from a JSON node. Returns a pointer to the built Histogram.
		// If return value is nullptr, then an unknown error occurred. It will throw 
		// a BuildException on failure. Object lifetime is the caller's responsibility. 
		static Histogram* BuildHistogram(const Json::Value& json);

		virtual ~Histogram(){}
	};
}
