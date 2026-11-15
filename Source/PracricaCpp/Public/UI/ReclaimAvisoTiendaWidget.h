#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimAvisoTiendaWidget.generated.h"


class APlayerController;
class UTextBlock;


UCLASS()
class PRACRICACPP_API UReclaimAvisoTiendaWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// Muestra el aviso en la pantalla del jugador indicado.
	UFUNCTION(
		BlueprintCallable,
		Category = "Reclaim|Tienda",
		meta = (DisplayName = "Mostrar aviso de tienda")
	)
	static void MostrarAviso(
		APlayerController* Jugador,
		FText Mensaje,
		bool bExito,
		float Duracion = 3.0f
	);

protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeDestruct() override;

private:

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoAviso;

	FTimerHandle TimerCerrar;

	void ConfigurarMensaje(
		const FText& Mensaje,
		bool bExito
	);

	void CerrarAviso();
};

