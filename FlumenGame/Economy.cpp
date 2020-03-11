#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenGame/Economy.hpp"
#include "FlumenGame/Settlement.hpp"
#include "FlumenGame/World.hpp"
#include "FlumenGame/Tile.hpp"

#define BASE_DEVELOPMENT_RATE 0.005f

#define BASE_DECAY_RATE 0.001f

#define WORKFORCE_FACTOR 0.01f

void GrowthTrend::Update(const Economy& economy)
{
	if(Time_ >= Length_)
	{
		Factor_ = utility::GetRandom(-0.5f, 1.5f);

		Time_ = 0;
	}

	Time_++;
}

Economy::Economy() {}

Economy::Economy(Settlement* settlement) : settlement_(settlement)
{
	float productivity = 0.0f;

	auto & tiles = Settlement::world_->GetTiles();
	for(int x = settlement_->longitude_ - 1; x <= settlement_->longitude_ + 1; ++x)
	{
		for(int y = settlement_->latitude_ - 1; y <= settlement_->latitude_ + 1; ++y)
		{
			auto tile = tiles(x, y);
			auto & biome = tile->GetBiome();

			productivity += biome.Productivity_;
		}
	}

	float tonnesPerAcre = 0.15f;

	float effectiveAcres = productivity * 250.0f;

	float aggregateYield = tonnesPerAcre * effectiveAcres;

	float tonnesPerPerson = 0.3f;

	settlement_->population_ = aggregateYield / tonnesPerPerson;

	resource_ = productivity;

	technology_ = utility::GetRandom(1.0f, 3.0f);

	float potentialDevelopment = resource_ * technology_;

	development_ = potentialDevelopment * utility::GetRandom(0.1f, 0.4f);

	growthTrends_.Initialize(3);
	*growthTrends_.Allocate() = GrowthTrend(30);
	*growthTrends_.Allocate() = GrowthTrend(100);
	*growthTrends_.Allocate() = GrowthTrend(250);
}

void Economy::Update()
{
	float growthFactor = 0.0f;
	const float trendWeight = 1.0f / (float)growthTrends_.GetSize();
	for(auto trend = growthTrends_.GetStart(); trend != growthTrends_.GetEnd(); ++trend)
	{
		trend->Update(*this);

		growthFactor += trend->Factor_ * trendWeight;
	}
	growthFactor *= exp(-pow(development_, 2.0f) / (0.7f * pow(GetPotential(), 2.0f)));

	development_ *= 1.0f + BASE_DEVELOPMENT_RATE * growthFactor;

	development_ *= 1.0f - BASE_DECAY_RATE * pow(development_ / GetPotential(), 2.0f);
}

int Economy::GetNecessaryWorkforce()
{
	return development_ / WORKFORCE_FACTOR;
}
