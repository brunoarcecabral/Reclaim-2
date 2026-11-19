#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReclaimPausaComponent.generated.h"

class APlayerController;
class UReclaimMenuPausaWidget;

UCLASS(ClassGroup = (Reclaim), meta = (BlueprintSpawnableComponent))
class PRACRICACPP_API UReclaimPausaComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UReclaimPausaComponent();
	
	void AbrirMenuPausa();

protected:

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

private:

	UPROPERTY()
	TObjectPtr<UReclaimMenuPausaWidget> MenuPausa;

	TWeakObjectPtr<APlayerController> ControladorMenu;

	bool bJuegoPausadoPorEsteMenu = false;
	bool bCursorEstabaVisible = false;

	bool HayVentanaBloqueante() const;

	UFUNCTION()
	void Continuar();

	UFUNCTION()
	void Salir();
	
	UFUNCTION()
	void GuardarPartida();
	
};

