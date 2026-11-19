#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimNotaQuestWidget.generated.h"


class UReclaimNotaQuestData;
class UTextBlock;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlContinuarNota);


UCLASS()
class PRACRICACPP_API UReclaimNotaQuestWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FAlContinuarNota AlContinuarNota;
	
	void MostrarNota(UReclaimNotaQuestData* DatosNota);
	
	void UsarEscapeParaContinuar();
	
	void UsarCierreAutomatico();

protected:

	virtual void NativeOnInitialized() override;

	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;
	
	virtual void NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
) override;

private:

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoTitulo;

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoContenido;

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoContinuar;

	bool bContinuarConEscape = false;
	
	bool bEsCierreAutomatico = false;

};

