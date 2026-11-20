#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimRanurasWidget.generated.h"

class UBorder;
class UImage;

UCLASS()
class PRACRICACPP_API UReclaimRanurasWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeDestruct() override;

private:
	UPROPERTY()
	TObjectPtr<UBorder> RanuraArma;

	UPROPERTY()
	TObjectPtr<UImage> ImagenArma;

	UPROPERTY()
	TObjectPtr<UBorder> RanuraTarjeta;
	
	UPROPERTY()
	TObjectPtr<UImage> ImagenTarjeta;
	
	FTimerHandle TimerActualizarArma;

	void ActualizarEstadoArma();
	
	void ActualizarEstadoTarjeta();
};
