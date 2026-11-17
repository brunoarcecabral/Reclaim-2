#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "ReclaimAvisoInicioSubsystem.generated.h"


class APlayerController;
class UReclaimNotaQuestWidget;


UCLASS()
class PRACRICACPP_API UReclaimAvisoInicioSubsystem
	: public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:

	virtual void Deinitialize() override;

	// Programa el aviso para el jugador local.
	void ProgramarAviso(APlayerController* Controlador);

private:

	UPROPERTY()
	TWeakObjectPtr<APlayerController> ControladorAviso;

	UPROPERTY()
	TObjectPtr<UReclaimNotaQuestWidget> VentanaAviso;

	FTimerHandle TimerAviso;

	bool bAvisoProgramado = false;
	bool bAvisoMostrado = false;
	bool bCursorEstabaVisible = false;

	void MostrarAviso();

	UFUNCTION()
	void CerrarAviso();
};

