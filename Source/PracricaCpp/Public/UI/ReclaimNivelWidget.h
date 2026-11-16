#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimNivelWidget.generated.h"


class UProgressBar;
class UReclaimNivelComponent;
class UTextBlock;


UCLASS()
class PRACRICACPP_API UReclaimNivelWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// Conecta este HUD con el nivel del jugador local.
	void ConectarNivel(UReclaimNivelComponent* NuevoComponente);

protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:

	UPROPERTY()
	TObjectPtr<UReclaimNivelComponent> ComponenteNivel;

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoNivel;

	UPROPERTY()
	TObjectPtr<UTextBlock> TextoExperiencia;

	UPROPERTY()
	TObjectPtr<UProgressBar> BarraExperiencia;

	UFUNCTION()
	void ActualizarNivel();
};

