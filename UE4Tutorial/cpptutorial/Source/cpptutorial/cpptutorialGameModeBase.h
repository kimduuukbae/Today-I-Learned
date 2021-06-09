#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "cpptutorialGameModeBase.generated.h"

UCLASS()
class CPPTUTORIAL_API AcpptutorialGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<class UUserProfile> userProfileClass;

public:
	virtual void BeginPlay() override;
};
