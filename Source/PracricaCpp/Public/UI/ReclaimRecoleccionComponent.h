#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimRecoleccionComponent.generated.h"

class UReclaimRecoleccionWidget;

struct FReclaimAvisoRecurso
{
	int32 Cantidad = 0;
	FTimerHandle Timer;
};

UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimRecoleccionComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimRecoleccionComponent();

	// Llamar desde el servidor cuando el recurso se agregó correctamente.
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Recoleccion")
	void NotificarRecoleccion(FText NombreRecurso, int32 Cantidad);

protected:

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Reclaim|Recoleccion",
		meta = (ClampMin = "0.1", UIMin = "0.1")
	)
	float DuracionMensaje = 2.0f;

private:

	UPROPERTY()
	TObjectPtr<UReclaimRecoleccionWidget> VentanaRecoleccion;

	TMap<FString, FReclaimAvisoRecurso> AvisosActivos;

	UFUNCTION(Client, Reliable)
	void ClienteMostrarRecoleccion(
		const FText& NombreRecurso,
		int32 Cantidad
	);

	void OcultarRecurso(const FString& Clave);
};

