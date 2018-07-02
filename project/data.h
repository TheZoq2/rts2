#ifndef H_DATA
#define H_DATA

enum DataType {
	// Raw reading, in 100ths of a volt
	RawReading,
	PeakToPeakVoltage,
	CurrentVoltage,
	// Real value of the next FFT datapoint
	FFT_Real,
	// Imaginary value of the next FFT datapoint
	FFT_Img,
	// Signal to indicate that a new burst of FFT data is arriving
	FFT_Sync
};

struct Data{
	int value;
	DataType type;

	Data(int value, DataType type) {
		this->value = value;
		this->type = type;
	}
};


#endif
