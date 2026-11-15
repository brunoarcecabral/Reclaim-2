#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ReclaimEnemigoBase.generated.h"

class APracricaCppCharacter;
class AAIController;

UCLASS()
class PRACRICACPP_API AReclaimEnemigoBase : public ACharacter
{
    GENERATED_BODY()

public:
    AReclaimEnemigoBase();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    virtual float TakeDamage(
        float DamageAmount,
        FDamageEvent const& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser
    ) override;

protected:
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
    Category = "Enemigo|Sonido")
    TObjectPtr<class USoundBase> SonidoImpacto;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
    Category = "Enemigo|Patrulla")
    bool PatrullaActivada = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "Enemigo|Patrulla",
        meta = (ClampMin = "0.0"))
    float RadioPatrulla = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "Enemigo|Patrulla",
        meta = (ClampMin = "0.1"))
    float EsperaEntrePatrullas = 3.0f;
    
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // ----- Salud -----

    UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly,
        Category = "Enemigo|Salud", meta = (ClampMin = "1.0"))
    float SaludMaxima = 100.0f;

    UPROPERTY(ReplicatedUsing = OnRep_SaludActual, BlueprintReadOnly,
        Category = "Enemigo|Salud")
    float SaludActual = 100.0f;

    UFUNCTION(BlueprintImplementableEvent, Category = "Enemigo|Salud")
    void AlCambiarSalud(float NuevaSalud, float VidaMaxima);

    // ----- Vision y movimiento -----

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemigo|Vision",
        meta = (ClampMin = "0.0"))
    float DistanciaVision = 1500.0f;

    //Angulo total del cono de vision, en grados. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemigo|Vision",
        meta = (ClampMin = "0.0", ClampMax = "360.0"))
    float AnguloVision = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemigo|Movimiento",
        meta = (ClampMin = "0.0"))
    float VelocidadPersecucion = 350.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemigo|Movimiento",
        meta = (ClampMin = "0.0"))
    float DistanciaDetencion = 150.0f;
    
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
            Category = "Enemigo|Ataque",
            meta = (ClampMin = "0.0"))
    float AlcanceDisparo = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "Enemigo|Ataque",
        meta = (ClampMin = "0.0"))
    float DanioDisparo = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "Enemigo|Ataque",
        meta = (ClampMin = "0.1"))
    float TiempoEntreDisparos = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "Enemigo|Ataque")
    TObjectPtr<class UMaterialInterface> MaterialLaser;

private:
    
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastSonidoImpacto();
    
    UFUNCTION()
    void OnRep_SaludActual();

    void RevisarObjetivos();
    bool PuedeDetectarJugador(APracricaCppCharacter* Jugador) const;
    bool JugadorDisponible(APracricaCppCharacter* Jugador) const;

    UPROPERTY()
    TObjectPtr<APracricaCppCharacter> ObjetivoActual;
    
    UPROPERTY()
    TObjectPtr<APracricaCppCharacter> UltimoAtacante;

    FTimerHandle TimerRevisionObjetivos;
    bool bMuerto = false;
    
    void IntentarDisparar(APracricaCppCharacter* Jugador);

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastMostrarLaser(FVector Inicio, FVector Fin);

    float UltimoDisparo = -10000.0f;
    
    void Patrullar(class AAIController* Controlador);

    FVector PuntoInicialPatrulla = FVector::ZeroVector;

    float ProximoIntentoPatrulla = 0.0f;
    
};

