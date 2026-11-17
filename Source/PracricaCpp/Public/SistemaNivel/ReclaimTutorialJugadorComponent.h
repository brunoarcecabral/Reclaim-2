#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimTutorialJugadorComponent.generated.h"


class UReclaimNotaQuestWidget;
class UReclaimTutorialNivelComponent;


UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimTutorialJugadorComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimTutorialJugadorComponent();

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

private:

	UPROPERTY()
	TObjectPtr<UReclaimTutorialNivelComponent> TutorialCompartido;

	UPROPERTY()
	TObjectPtr<UReclaimNotaQuestWidget> VentanaTutorial;

	FTimerHandle TimerBuscarTutorial;

	bool bCursorEstabaVisible = false;

	void BuscarTutorialCompartido();

	UFUNCTION()
	void ActualizarTutorial();

	UFUNCTION()
	void SolicitarCerrarTutorial();

	UFUNCTION(Server, Reliable)
	void ServidorCerrarTutorial();

	// Devuelve el foco a la ventana después del clic en la tienda.
	void ReenfocarVentana();

	void CerrarVentanaLocal();
};

