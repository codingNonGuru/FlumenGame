#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenEngine/Utility/Color.hpp"

class Settlement;

class Economy;
class Tile;
class World;
struct SettlementRenderData;

struct Link
{
	Settlement* Other_;

	Float Distance_;

	Link() {}

	Link(Settlement* Other, Float Distance = 0.0f) : Other_(Other), Distance_(Distance) {}
};

struct LinkNetwork
{
	Link* Links_;

	Length Count_;

public:
	Link* GetFirst() {return Links_;}

	Link* GetLast() {return Links_ + Count_;}

	LinkNetwork() {}

	LinkNetwork(Link* Links, Length Count) : Links_(Links), Count_(Count) {}
};

class Settlement
{
	friend class Economy;

	static World* world_;

	Index key_;

	Tile* tile_;

	Position2 position_;

	int latitude_, longitude_;

	int population_;

	LinkNetwork linkNetwork_;

	Color banner_;

	SettlementRenderData* renderData_;

	Economy* economy_;

public:
	static void SetWorld(World*);

	Settlement();

	void Initialize(Position2);

	bool CheckCollision(Matrix &, Float2);

	void SetKey(Index key) {key_ = key;}

	Index GetKey() const {return key_;}

	int GetPopulation() const;

	Position2 GetPosition() const;

	LinkNetwork GetLinkNetwork() const;

	void SetLinkNetwork(LinkNetwork);

	const Economy& GetEconomy() {return *economy_;}

	float GetDistance(Settlement*) const;

	Color & GetBanner();

	SettlementRenderData* GetRenderData();

	void SetRenderData(SettlementRenderData*);

	void Update();
};
