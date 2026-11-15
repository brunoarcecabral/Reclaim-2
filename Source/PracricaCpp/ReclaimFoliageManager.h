#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReclaimFoliageManager.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

USTRUCT(BlueprintType)
struct FReclaimFoliageEstado
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Reclaim|Foliage")
    FVector Ubicacion = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category="Reclaim|Foliage")
    int32 Golpes = 0;
};

// Identifica un árbol eliminado por su malla y su posición.
USTRUCT()
struct FReclaimFoliageTalado
{
    GENERATED_BODY()

    UPROPERTY()
    FVector Ubicacion = FVector::ZeroVector;

    UPROPERTY()
    FString RutaMalla;
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

    // Debe ejecutarse en el servidor.
    UFUNCTION(BlueprintCallable, Category="Reclaim|Foliage")
    int32 RegistrarGolpe(FVector Ubicacion);

    UFUNCTION(BlueprintPure, Category="Reclaim|Foliage")
    int32 ObtenerGolpes(FVector Ubicacion) const;

    // Registra y elimina una instancia desde el servidor.
    // Todavía no la estamos llamando desde el ataque.
    UFUNCTION(BlueprintCallable, Category="Reclaim|Foliage")
    bool RegistrarTala(
        UHierarchicalInstancedStaticMeshComponent* Componente,
        int32 IndiceInstancia
    );

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(
        const EEndPlayReason::Type EndPlayReason
    ) override;

private:
    UPROPERTY(Replicated)
    TArray<FReclaimFoliageEstado> EstadosFoliage;

    UPROPERTY(ReplicatedUsing=OnRep_ArbolesTalados)
    TArray<FReclaimFoliageTalado> ArbolesTalados;

    UFUNCTION()
    void OnRep_ArbolesTalados();

    int32 BuscarIndice(FVector Ubicacion) const;

    bool YaEstaTalado(
        const FString& RutaMalla,
        const FVector& Ubicacion
    ) const;

    void AplicarArbolesTalados();

    FTimerHandle TemporizadorAplicarTalados;
};

