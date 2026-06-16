#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MisClases/TPKent/IPuzzleInteractable.h"
#include "APasswordDoor.generated.h"

UCLASS()
class PRACRICACPP_API AAPasswordDoor : public AActor, public IIPuzzleInteractable
{
	GENERATED_BODY()

public:
	AAPasswordDoor();

protected:
	

public:
	
};