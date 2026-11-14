#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReclaimFoliageManager.generated.h"

USTRUCT(BlueprintType)
struct FReclaimFoliageEstado
{
	GENERATED_BODY()

	// Ubicación de la instancia en el mundo.
	UPROPERTY(BlueprintReadOnly, Category="Reclaim|Foliage")
	FVector Ubicacion = FVector::ZeroVector;

	// Cantidad de golpes recibidos por esa instancia.
	UPROPERTY(BlueprintReadOnly, Category="Reclaim|Foliage")
	int32 Golpes = 0;
};

UCLASS()
class PRACRICACPP_API AReclaimFoliageManager : public AActor
{
	GENERATED_BODY()

public:
	AReclaimFoliageManager();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	// Registra un golpe y devuelve el total acumulado.
	// Debe ejecutarse en el servidor.
	UFUNCTION(BlueprintCallable, Category="Reclaim|Foliage")
	int32 RegistrarGolpe(FVector Ubicacion);

	// Consulta el estado compartido.
	UFUNCTION(BlueprintPure, Category="Reclaim|Foliage")
	int32 ObtenerGolpes(FVector Ubicacion) const;

private:
	UPROPERTY(Replicated)
	TArray<FReclaimFoliageEstado> EstadosFoliage;

	int32 BuscarIndice(FVector Ubicacion) const;
};

