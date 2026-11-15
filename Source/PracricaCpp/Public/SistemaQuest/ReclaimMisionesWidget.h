#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimMisionesWidget.generated.h"


class UReclaimQuestComponent;
class UVerticalBox;


UCLASS()
class PRACRICACPP_API UReclaimMisionesWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:

	UPROPERTY()
	TObjectPtr<UVerticalBox> ListaMisiones;

	UPROPERTY()
	TObjectPtr<UReclaimQuestComponent> ComponenteQuest;

	// Misiones que esta pantalla vio mientras estaban activas.
	TSet<FName> MisionesObservadasActivas;

	// Tiempo hasta el que se muestra cada misión completada.
	TMap<FName, double> MisionesEnDespedida;

	FTimerHandle TimerDespedida;

	UFUNCTION()
	void ActualizarMisiones();

	void AgregarTitulo(const FText& Titulo);

	void AgregarObjetivo(
		const FText& Descripcion,
		int32 CantidadActual,
		int32 CantidadRequerida,
		bool bCompletado
	);
};

