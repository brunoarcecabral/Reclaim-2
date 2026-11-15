#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimNotaQuestWidget.generated.h"


class UReclaimNotaQuestData;
class UTextBlock;


// Se dispara localmente cuando el jugador pulsa E.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlContinuarNota);


UCLASS()
class PRACRICACPP_API UReclaimNotaQuestWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FAlContinuarNota AlContinuarNota;

	// Carga el título y el contenido de una nota.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void MostrarNota(UReclaimNotaQuestData* DatosNota);

protected:

	virtual void NativeOnInitialized() override;

	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;

private:

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoTitulo;

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoContenido;
};

