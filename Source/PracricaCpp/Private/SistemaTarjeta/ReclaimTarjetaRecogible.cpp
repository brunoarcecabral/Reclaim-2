#include "SistemaTarjeta/ReclaimTarjetaRecogible.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"
#include "SistemaTarjeta/ReclaimTarjetaComponent.h"

AReclaimTarjetaRecogible::AReclaimTarjetaRecogible()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    Raiz = CreateDefaultSubobject<USceneComponent>(TEXT("Raiz"));
    SetRootComponent(Raiz);

    ModeloTarjeta =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("ModeloTarjeta")
        );

    ModeloTarjeta->SetupAttachment(Raiz);

    CartelRecoger =
        CreateDefaultSubobject<UTextRenderComponent>(
            TEXT("CartelRecoger")
        );

    CartelRecoger->SetupAttachment(Raiz);
    CartelRecoger->SetText(FText::FromString(TEXT("E - Recoger")));
    CartelRecoger->SetHorizontalAlignment(EHTA_Center);
    CartelRecoger->SetTextRenderColor(FColor::White);
    CartelRecoger->SetWorldSize(20.0f);
    CartelRecoger->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
    CartelRecoger->SetVisibility(false);
}

void AReclaimTarjetaRecogible::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AReclaimTarjetaRecogible, bRecogida);
}

bool AReclaimTarjetaRecogible::EstaRecogida() const
{
    return bRecogida;
}

bool AReclaimTarjetaRecogible::EstaDisponibleParaRecoger() const
{
    if (bRecogida)
    {
        return false;
    }

    const UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return false;
    }

    AGameStateBase* EstadoJuego = Mundo->GetGameState();

    if (!IsValid(EstadoJuego))
    {
        return false;
    }

    UReclaimQuestComponent* Quest =
        EstadoJuego->FindComponentByClass<UReclaimQuestComponent>();

    if (!IsValid(Quest) || !IsValid(MisionAbrirPuerta))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("TARJETA: Falta el componente de misiones o asignar la mision de abrir la puerta.")
        );

        return false;
    }
    
    for (const FEstadoQuest& Estado : Quest->ObtenerMisiones())
    {
        if (IsValid(Estado.DatosMision) &&
            Estado.DatosMision->IdMision ==
                FName(TEXT("Mision_TarjetaAcceso")) &&
            !Estado.bCompletada)
        {
            return true;
        }
    }

    return false;
}

bool AReclaimTarjetaRecogible::EntregarAlJugador(
    UReclaimTarjetaComponent* TarjetaJugador
)
{
    if (!HasAuthority() ||
        !IsValid(TarjetaJugador) ||
        TarjetaJugador->TieneTarjeta() ||
        !EstaDisponibleParaRecoger())
    {
        return false;
    }

    UWorld* Mundo = GetWorld();

    AGameStateBase* EstadoJuego = Mundo
        ? Mundo->GetGameState()
        : nullptr;

    UReclaimQuestComponent* Quest = IsValid(EstadoJuego)
        ? EstadoJuego->FindComponentByClass<UReclaimQuestComponent>()
        : nullptr;

    if (!IsValid(Quest))
    {
        return false;
    }

    if (!TarjetaJugador->RecibirTarjeta())
    {
        return false;
    }

    const bool bObjetivoAvanzado = Quest->AvanzarObjetivo(
        FName(TEXT("Mision_TarjetaAcceso")),
        FName(TEXT("Obtener_TarjetaAcceso")),
        1
    );

    if (!bObjetivoAvanzado)
    {
        TarjetaJugador->ConsumirTarjeta();

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("TARJETA: No se pudo avanzar el objetivo de la mision.")
        );

        return false;
    }
    if (!Quest->IniciarMision(MisionAbrirPuerta))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("TARJETA: Se recogio la tarjeta, pero no se pudo iniciar la mision de abrir la puerta.")
        );
    }
    
    bRecogida = true;

    ActualizarApariencia();
    ForceNetUpdate();

    return true;
}

void AReclaimTarjetaRecogible::OnRep_Recogida()
{
    ActualizarApariencia();
}

void AReclaimTarjetaRecogible::ActualizarApariencia()
{
    ModeloTarjeta->SetVisibility(!bRecogida, true);

    ModeloTarjeta->SetCollisionEnabled(
        bRecogida
            ? ECollisionEnabled::NoCollision
            : ECollisionEnabled::QueryAndPhysics
    );
}

void AReclaimTarjetaRecogible::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    const bool bDisponible = EstaDisponibleParaRecoger();

    ModeloTarjeta->SetVisibility(bDisponible);

    if (!bDisponible)
    {
        CartelRecoger->SetVisibility(false);
        return;
    }

    APawn* PersonajeLocal =
        UGameplayStatics::GetPlayerPawn(this, 0);

    if (!IsValid(PersonajeLocal))
    {
        CartelRecoger->SetVisibility(false);
        return;
    }

    const float DistanciaMaxima = 250.0f;

    const float DistanciaCuadrada = FVector::DistSquared(
        PersonajeLocal->GetActorLocation(),
        GetActorLocation()
    );

    CartelRecoger->SetVisibility(
        DistanciaCuadrada <= FMath::Square(DistanciaMaxima)
    );
}

void AReclaimTarjetaRecogible::RestaurarRecogida(bool bGuardada)
{
    UWorld* Mundo = GetWorld();

    if (!HasAuthority() ||
        !Mundo ||
        Mundo->GetNetMode() != NM_Standalone)
    {
        return;
    }

    bRecogida = bGuardada;

    ActualizarApariencia();
    ForceNetUpdate();
}
