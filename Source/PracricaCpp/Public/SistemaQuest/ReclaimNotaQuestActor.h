#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReclaimNotaQuestActor.generated.h"


class APawn;
class APlayerController;
class UBoxComponent;
class UReclaimNotaQuestData;
class UReclaimNotaQuestWidget;
class UStaticMeshComponent;


UCLASS()
class PRACRICACPP_API AReclaimNotaQuestActor : public AActor
{
    GENERATED_BODY()

public:

    AReclaimNotaQuestActor();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    // El servidor comprueba si el personaje puede continuar.
    bool PuedeContinuar(const APawn* Personaje) const;

    // El servidor completa la lectura e inicia la misión siguiente.
    bool FinalizarLectura();

protected:

    virtual void BeginPlay() override;

    virtual void EndPlay(
        const EEndPlayReason::Type EndPlayReason
    ) override;

    // Área que activa la nota cuando entra cualquier jugador.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nota")
    TObjectPtr<UBoxComponent> ZonaReunion;

    // Malla visual de la nota, configurable en el editor.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nota")
    TObjectPtr<UStaticMeshComponent> MallaNota;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nota")
    TObjectPtr<UReclaimNotaQuestData> DatosNota;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Nota",
        meta = (ClampMin = "0.1", UIMin = "0.1")
    )
    float IntervaloRevision = 0.25f;

    UPROPERTY(ReplicatedUsing = OnRep_EstadoNota)
    bool bNotaAbierta = false;

    UPROPERTY(ReplicatedUsing = OnRep_EstadoNota)
    bool bNotaFinalizada = false;

    UFUNCTION()
    void OnRep_EstadoNota();

private:

    FTimerHandle TimerRevision;
    FTimerHandle TimerInterfaz;

    // Una ventana por jugador local.
    UPROPERTY()
    TMap<
        TObjectPtr<APlayerController>,
        TObjectPtr<UReclaimNotaQuestWidget>
    > VentanasPorJugador;

    bool HayJugadorEnLaZona() const;

    void RevisarZona();

    void ActualizarVentanasLocales();

    void CerrarVentanasLocales();
};
	
