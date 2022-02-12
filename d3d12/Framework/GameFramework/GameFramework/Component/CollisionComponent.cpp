#include "stdafx.h"
#include "CollisionComponent.h"
#include "Object/Object.h"
#include "GameplayStatics.h"
#include "Scene.h"

CollisionComponent::CollisionComponent()
{
    GameplayStatics::GetCurrentScene()->AddCollision(*this);
}

CollisionComponent::~CollisionComponent()
{
    GameplayStatics::GetCurrentScene()->RemoveColliision(*this);
}

void CollisionComponent::DoOverlap(CollisionComponent& other)
{
    if (IsOverlap(other)) {

        for (auto& it : functions)
            it(other);

        for (auto& it : other.functions)
            it(*this);
    }
}

bool CollisionComponent::IsOverlap(const CollisionComponent& other) const
{
    if (!owner->IsActive() || !other.owner->IsActive())
        return false;

    return GetBoundingSphere().Contains(other.GetBoundingSphere())
        != DirectX::ContainmentType::DISJOINT;
}

bool CollisionComponent::IsOverlap(const DirectX::BoundingFrustum& ft)
{
    return ft.Intersects(GetBoundingSphere());
}

void CollisionComponent::SetRadius(float radius)
{
    boundSphere.Radius = radius;
}

void CollisionComponent::AddCallbackFunction(std::function<void(CollisionComponent&)>&& fn)
{
    functions.push_back(std::move(fn));
}

DirectX::BoundingSphere CollisionComponent::GetBoundingSphere() const
{
    DirectX::BoundingSphere my{ boundSphere };
    my.Transform(my, DirectX::XMLoadFloat4x4(&owner->GetTransform()->GetTransformDirect()));
    return my;
}
