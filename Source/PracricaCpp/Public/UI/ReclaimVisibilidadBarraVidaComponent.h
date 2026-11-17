#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimVisibilidadBarraVidaComponent.generated.h"

class UWidgetComponent;


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimVisibilidadBarraVidaComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimVisibilidadBarraVidaComponent();

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

	// Nombre del componente que muestra la barra del enemigo.
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Barra de vida"
	)
	FName NombreComponenteBarra = TEXT("BarraDeVida");

	// Distancia máxima a la que se mostrará la barra.
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Barra de vida",
		meta = (ClampMin = "0.0", UIMin = "0.0")
	)
	float DistanciaVisible = 800.0f;

private:

	UPROPERTY(Transient)
	TObjectPtr<UWidgetComponent> BarraDeVida;

	FTimerHandle TimerRevisionBarra;

	void ActualizarVisibilidadBarra();
};

