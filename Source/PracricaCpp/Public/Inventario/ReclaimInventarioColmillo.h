#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ReclaimInventarioColmillo.generated.h"


UINTERFACE(BlueprintType)
class PRACRICACPP_API UReclaimInventarioColmillo : public UInterface
{
	GENERATED_BODY()
};


class PRACRICACPP_API IReclaimInventarioColmillo
{
	GENERATED_BODY()

public:

	// Comprueba si el inventario está preparado para recibir el colmillo.
	UFUNCTION(
		BlueprintCallable,
		BlueprintImplementableEvent,
		Category = "Reclaim|Colmillo"
	)
	bool PuedeRecibirColmillo() const;

	// Agrega una unidad al inventario.
	UFUNCTION(
		BlueprintCallable,
		BlueprintImplementableEvent,
		Category = "Reclaim|Colmillo"
	)
	bool AgregarColmillo();

	// Quita una unidad del inventario.
	UFUNCTION(
		BlueprintCallable,
		BlueprintImplementableEvent,
		Category = "Reclaim|Colmillo"
	)
	bool QuitarColmillo();
};

