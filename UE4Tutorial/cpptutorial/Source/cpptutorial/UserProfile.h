#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UserProfile.generated.h"

UCLASS(Blueprintable)
class CPPTUTORIAL_API UUserProfile : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = stats)
	float armor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = stats)
	float hp{ 10.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Unit)
	TSubclassOf<UObject> classOfPlayer;

	UPROPERTY(EditAnywhere, meta = (MetaClass = "GameMode"))
	FStringClassReference UClassGameMode;
};
