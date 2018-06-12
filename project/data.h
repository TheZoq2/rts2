#ifndef H_DATA
#define H_DATA
enum DataType {
	RawReading,
	PeakToPeakVoltage,
	CurrentVoltage,
};

struct Data{
	int value;
	DataType type;
};


#endif
