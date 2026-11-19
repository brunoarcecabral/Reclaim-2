#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimGuardadoJugadorComponent.generated.h"

UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimGuardadoJugadorComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimGuardadoJugadorComponent();
	
	bool GuardarJugador();

protected:

	virtual void BeginPlay() override;

private:
	
	void AplicarGuardadoPendiente();
};

