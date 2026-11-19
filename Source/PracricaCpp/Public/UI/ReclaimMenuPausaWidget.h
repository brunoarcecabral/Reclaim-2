#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimMenuPausaWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReclaimMenuPausaEvento);

UCLASS()
class PRACRICACPP_API UReclaimMenuPausaWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Pausa")
	FReclaimMenuPausaEvento AlContinuar;

	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Pausa")
	FReclaimMenuPausaEvento AlSalir;
	
	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Pausa")
	FReclaimMenuPausaEvento AlGuardar;
	
	void HabilitarGuardado(bool bHabilitado);
	void MostrarEstadoGuardado(const FText& Mensaje);

protected:

	virtual void NativeOnInitialized() override;

private:

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoEstadoGuardado;
	
	UPROPERTY()
	TObjectPtr<UButton> BotonContinuar;

	UPROPERTY()
	TObjectPtr<UButton> BotonGuardar;

	UPROPERTY()
	TObjectPtr<UButton> BotonSalir;

	UFUNCTION()
	void PresionarContinuar();

	UFUNCTION()
	void PresionarSalir();
	
	UFUNCTION()
	void PresionarGuardar();
	
};

