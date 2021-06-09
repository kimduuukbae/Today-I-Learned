#include "cpptutorialGameModeBase.h"
#include "UserProfile.h"

void AcpptutorialGameModeBase::BeginPlay()
{
	userProfileClass = UUserProfile::StaticClass();
	
	auto profile = NewObject<UUserProfile>(userProfileClass);

	if (profile)
	{
		profile->ConditionalBeginDestroy();
		profile = nullptr;
	}

	TArray<int*> v;
}
