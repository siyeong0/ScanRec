#include <iostream>
#include "Chunk.h"
#include <random>

int main(void)
{
	Chunk chunk000 = Chunk();
	Chunk chunk100 = Chunk();
	Chunk chunk010 = Chunk();
	float center000[3] = {0.f, 0.f, 0.f};
	float center100[3] = {8.f, 0.f, 0.f};
	float center010[3] = {0.f, 8.f ,0.f};

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
	for (size_t i = 0; i < 256; i++)
	{
		PointData data;
		data.X = dis(gen) * 4.f + 8.f;
		data.Y = dis(gen) * 4.f;
		data.Z = dis(gen) * 4.f;
		data.R = 128;
		data.G = 128;
		data.B = 128;
		chunk100.AddPoint(center100, data, 3);
	}

	return 0;
}

