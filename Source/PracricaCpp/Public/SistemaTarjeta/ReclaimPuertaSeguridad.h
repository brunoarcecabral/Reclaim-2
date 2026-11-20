#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReclaimPuertaSeguridad.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UReclaimTarjetaComponent;
class UReclaimQuestData;

UCLASS()
class PRACRICACPP_API AReclaimPuertaSeguridad : public AActor
{
	GENERATED_BODY()

public:
	AReclaimPuertaSeguridad();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;
	
	UFUNCTION(BlueprintCallable, Category = "Reclaim|Puerta")
	void RestaurarPuertaAbierta(bool bGuardada);
	
	UFUNCTION(BlueprintPure, Category = "Reclaim|Puerta")
	bool EstaAbierta() const;
	
	bool AbrirConTarjeta(UReclaimTarjetaComponent* TarjetaJugador);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|Puerta")
	TObjectPtr<UReclaimQuestData> MisionEntrarSala;
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reclaim|Puerta")
	TObjectPtr<USceneComponent> Raiz;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reclaim|Puerta")
	TObjectPtr<UStaticMeshComponent> ModeloPuerta;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reclaim|Puerta")
	FVector DesplazamientoApertura = FVector(0.0f, 0.0f, -250.0f);

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Reclaim|Puerta",
		meta = (ClampMin = "1.0")
	)
	float VelocidadApertura = 100.0f;

private:
	UPROPERTY(Replicated)
	bool bAbierta = false;

	FVector PosicionCerrada = FVector::ZeroVector;
};

