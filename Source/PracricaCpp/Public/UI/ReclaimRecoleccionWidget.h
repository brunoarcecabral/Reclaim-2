#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimRecoleccionWidget.generated.h"

class UTextBlock;
class UVerticalBox;

UCLASS()
class PRACRICACPP_API UReclaimRecoleccionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void ActualizarRecurso(
		const FString& Clave,
		const FText& Nombre,
		int32 Cantidad
	);

	void QuitarRecurso(const FString& Clave);

protected:

	virtual void NativeOnInitialized() override;

private:

	UPROPERTY()
	TObjectPtr<UVerticalBox> ListaRecursos;

	UPROPERTY()
	TMap<FString, TObjectPtr<UTextBlock>> TextosPorRecurso;
};

