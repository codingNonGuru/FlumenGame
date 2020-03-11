#pragma once

class Settlement;
class Economy;

struct GrowthTrend
{
	float Factor_;

	int Time_;

	int Length_;

	GrowthTrend() {}

	GrowthTrend(int Length) : Length_(Length), Time_(Length), Factor_(0.0f) {}

	void Update(const Economy& economy);
};

class Economy
{
	friend struct GrowthTrend;

	Settlement* settlement_;

	float resource_;

	float development_;

	float infrastructure_;

	float technology_;

	Array <GrowthTrend> growthTrends_;

	int GetNecessaryWorkforce();

public:
	Economy();

	Economy(Settlement*);

	void Update();

	float GetDevelopment() const {return development_;}

	float GetTechnology() const {return technology_;}

	float GetPotential() const {return resource_ * technology_;}
};
