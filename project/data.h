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

	Data(int value, DataType type) {
		this->value = value;
		this->type = type;
	}
};


#endif
