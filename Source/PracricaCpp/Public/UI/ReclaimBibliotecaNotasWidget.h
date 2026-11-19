#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimBibliotecaNotasWidget.generated.h"

class UScrollBox;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlCerrarBibliotecaNotas);

UCLASS()
class PRACRICACPP_API UReclaimBibliotecaNotasWidget
	: public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Notas")
	FAlCerrarBibliotecaNotas AlCerrarBiblioteca;
	
	void ActualizarLista();

protected:

	virtual void NativeOnInitialized() override;

	virtual FReply NativeOnPreviewKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;

private:

	UPROPERTY()
	TObjectPtr<UScrollBox> ListaNotas;

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoSinNotas;
};

