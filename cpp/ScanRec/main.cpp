#include <iostream>
#include "Block.h"

int main(void)
{
	Block bl = Block();
	float center[3] = { 0,0,0 };
	float data[9] = { 0.2, -0.1, 0.4
					, 0.0, 1.0, 0.0
					, 1.0, 0.0, 0.0 };
	uint8_t label = 4;
	bl.AddPoint(center, data, label);
	data[0] += 0.0002;
	bl.AddPoint(center, data, label);
	label = 3;
	bl.AddPoint(center, data, label);

	return 0;
}

