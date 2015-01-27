#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

// A Histogram class. Along with the traditional histogram and binning, this class
// provides "value" storage, which can be used to store custom data associated with
// each entry in a bin.
class Histogram
{
	private:
		// Width of bins.
		double _binWidth;

		// Number of bins.
		int _binCount;

		// Min and max values of bins.
		int _min;

		// Count of histogram outliers.
		int _lowerOutlierCount = 0, _upperOutlierCount = 0;

		// Histogram counts.
		std::vector<int> _counts;

		// Histogram values.
		std::vector<double> _values;

	public:
		Histogram(double min, double max, int numberOfBins) :
			_binCount(numberOfBins), _min(min)
		{
			_binWidth = (max-min)/numberOfBins;
			_counts.resize(numberOfBins, 0);
			_values.resize(numberOfBins, 0.0);
		}

		Histogram(double min, double max, double binWidth) :
			_min(min)
		{
			_binCount = (int) ceil(std::abs(max-min)/binWidth);
			_counts.resize(_binCount, 0);
			_values.resize(_binCount, 0.0);
		}

		// Record a data point in the histogram and returns the bin index updated.
		int Record(double datum)
		{
			int bin = (int)((datum - _min) / _binWidth);
			if (bin < 0)
				_lowerOutlierCount++;

			else
				_counts[bin]++;

			return bin;
		}

		// Gets an associated value from a bin.
		double GetValue(int bin)
		{
			if(bin >= _binCount || bin < 0)
				return -1;

			return _values[bin];
		}

		// Gets an associated value with the bin in which datum would reside.
		// Returns back the value of datum if it is out of range.
		double GetValue(double datum)
		{
			int bin = (int)((datum - _min) / _binWidth);

			if (bin < 0 || bin >= _binCount)
				return datum;

			return _values[bin];
		}

		// Gets vector of bin values.
		std::vector<double> GetValues()
		{
			return _values;
		}

		// Update associated values in a bin.
		void UpdateValue(int bin, double value)
		{
			if(bin >= _binCount || bin < 0)
				return;

			_values[bin] = value;
		}

		// Get the number of data points in some bin.
		int Count(int bin) const
		{
			if(bin >= _binCount || bin < 0)
				return -1;

			return _counts[bin];
		}

		// Gets the number of bins in the histogram.
		int GetBinCount()
		{
			return _counts.size();
		}

		// Gets the width of a bin.
		double GetBinWidth()
		{
			return _binWidth;
		}

		// Gets the count of values below the minimum.
		int GetLowerOutlierCount()
		{
			return _lowerOutlierCount;
		}

		// Gets the count of values above the maximum.
		int GetUpperOutlierCount()
		{
			return _upperOutlierCount;
		}

		// Resets the histogram.
		void ResetHistogram()
		{
			std::fill(_counts.begin(), _counts.end(), 0);
		}

		// Calculates the "flatness" of the histogram. That is, the maximum
		// (lower end) deviation of a bin count from the average counts of the bin.
		double CalculateFlatness()
		{
			double avg = 0;
			for(auto& count : _counts)
				avg += count;
			avg /= _binCount;

			if(avg == 0)
				return 0;

			double dev = 1.0;
			for(auto& count : _counts)
			{
				double id = count/avg;
				if(id < dev)
					dev = id;
			}

			return dev;
		}
};
