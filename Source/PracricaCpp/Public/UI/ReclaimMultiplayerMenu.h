#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FindSessionsCallbackProxy.h"
#include "ReclaimMultiplayerMenu.generated.h"

class UButton;
class UEditableTextBox;
class UScrollBox;
class UTextBlock;

UCLASS()
class PRACRICACPP_API UReclaimMultiplayerMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY()
	TObjectPtr<UButton> BotonCrearPartida;

	UPROPERTY()
	TObjectPtr<UButton> BotonBuscarPartidas;

	UPROPERTY()
	TObjectPtr<UButton> BotonUnirse;

	UPROPERTY()
	TObjectPtr<UButton> BotonSalir;

	UPROPERTY()
	TObjectPtr<UEditableTextBox> EntradaNumeroPartida;

	UPROPERTY()
	TObjectPtr<UScrollBox> ListaPartidas;

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoEstado;

	UPROPERTY()
	TArray<FBlueprintSessionResult> PartidasEncontradas;

	UFUNCTION()
	void CrearPartida();

	UFUNCTION()
	void BuscarPartidas();

	UFUNCTION()
	void UnirseAPartida();

	UFUNCTION()
	void SalirDelJuego();

	UFUNCTION()
	void AlEncontrarPartidas(
		const TArray<FBlueprintSessionResult>& SessionResults,
		bool bWasSuccessful
	);

	UFUNCTION()
	void AlCrearPartida(bool bWasSuccessful);

	UFUNCTION()
	void AlUnirseAPartida(bool bWasSuccessful);
};

