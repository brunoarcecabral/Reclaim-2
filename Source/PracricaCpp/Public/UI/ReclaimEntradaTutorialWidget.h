#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimEntradaTutorialWidget.generated.h"


class UButton;
class UReclaimNotaQuestData;
class UTextBlock;


UCLASS()
class PRACRICACPP_API UReclaimEntradaTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// Asigna el título y el contenido a esta sección.
	void Configurar(UReclaimNotaQuestData* DatosTutorial);

protected:

	virtual void NativeOnInitialized() override;

private:

	UPROPERTY()
	TObjectPtr<UButton> BotonTitulo;

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoTitulo;

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoContenido;

	UFUNCTION()
	void AlternarContenido();
};

