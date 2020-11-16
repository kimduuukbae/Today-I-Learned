#pragma once

#include "IComponent.h"

class CollisionComponent : public IComponent
{
public:
	CollisionComponent();
	virtual ~CollisionComponent();

	void DoOverlap(CollisionComponent& other);

	[[nodiscard]] 
	bool IsOverlap(const CollisionComponent& other) const;

	void SetRadius(float radius);

	void AddCallbackFunction(std::function<void(CollisionComponent&)>&& fn);

private:
	DirectX::BoundingSphere GetBoundingSphere() const;


	DirectX::BoundingSphere boundSphere;
	
	std::vector <std::function<void(CollisionComponent&)>> functions;
};

