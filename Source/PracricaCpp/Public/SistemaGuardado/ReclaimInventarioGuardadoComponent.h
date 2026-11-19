#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimInventarioGuardadoComponent.generated.h"

class UReclaimSaveGame;

UCLASS(
	Blueprintable,
	ClassGroup = (Reclaim),
	meta = (BlueprintSpawnableComponent)
)
class PRACRICACPP_API UReclaimInventarioGuardadoComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimInventarioGuardadoComponent();
	
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Reclaim|Guardado"
	)
	void CopiarInventarioAlGuardado(UReclaimSaveGame* DatosGuardado);
	
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Reclaim|Guardado"
	)
	void RestaurarInventarioDesdeGuardado(UReclaimSaveGame* DatosGuardado);
	
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Reclaim|Guardado"
	)
	void CopiarCheckpointAlGuardado(UReclaimSaveGame* DatosGuardado);
	
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Reclaim|Guardado"
	)
	void RestaurarCheckpointDesdeGuardado(UReclaimSaveGame* DatosGuardado);
	
};

