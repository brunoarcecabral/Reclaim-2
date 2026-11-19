#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimBibliotecaTutorialWidget.generated.h"


class UReclaimBibliotecaTutorialComponent;
class UScrollBox;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAlCerrarBibliotecaTutorial);


UCLASS()
class PRACRICACPP_API UReclaimBibliotecaTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Reclaim|Tutoriales")
	FAlCerrarBibliotecaTutorial AlCerrarBiblioteca;
	
	void Configurar(UReclaimBibliotecaTutorialComponent* Biblioteca);

protected:

	virtual void NativeOnInitialized() override;

	virtual FReply NativeOnPreviewKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;

private:

	UPROPERTY()
	TObjectPtr<UScrollBox> ListaTutoriales;
};

