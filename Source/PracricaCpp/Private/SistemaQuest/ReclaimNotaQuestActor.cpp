#include "SistemaQuest/ReclaimNotaQuestActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SistemaQuest/ReclaimNotaJugadorComponent.h"
#include "SistemaQuest/ReclaimNotaQuestData.h"
#include "SistemaQuest/ReclaimNotaQuestWidget.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"
#include "TimerManager.h"


AReclaimNotaQuestActor::AReclaimNotaQuestActor()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    bAlwaysRelevant = true;

    ZonaReunion =
        CreateDefaultSubobject<UBoxComponent>(
            TEXT("ZonaReunion")
        );

    SetRootComponent(ZonaReunion);

    ZonaReunion->SetBoxExtent(
        FVector(400.0f, 400.0f, 200.0f)
    );

    ZonaReunion->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly
    );

    ZonaReunion->SetCollisionObjectType(ECC_WorldDynamic);
    ZonaReunion->SetCollisionResponseToAllChannels(ECR_Ignore);
    ZonaReunion->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    ZonaReunion->SetGenerateOverlapEvents(true);

    MallaNota =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("MallaNota")
        );

    MallaNota->SetupAttachment(ZonaReunion);
    
    MallaNota->SetCollisionEnabled(
        ECollisionEnabled::NoCollision
    );
}


void AReclaimNotaQuestActor::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AReclaimNotaQuestActor, bNotaAbierta);
    DOREPLIFETIME(AReclaimNotaQuestActor, bNotaFinalizada);
}


void AReclaimNotaQuestActor::BeginPlay()
{
    Super::BeginPlay();
    
    if (GetNetMode() != NM_DedicatedServer)
    {
        GetWorldTimerManager().SetTimer(
            TimerInterfaz,
            this,
            &AReclaimNotaQuestActor::ActualizarVentanasLocales,
            0.25f,
            true
        );
    }
    
    if (!HasAuthority())
    {
        return;
    }

    if (!IsValid(DatosNota))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("NOTA QUEST: %s no tiene datos de nota asignados."),
            *GetName()
        );

        return;
    }

    GetWorldTimerManager().SetTimer(
        TimerRevision,
        this,
        &AReclaimNotaQuestActor::RevisarZona,
        FMath::Max(0.1f, IntervaloRevision),
        true
    );
}


void AReclaimNotaQuestActor::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    GetWorldTimerManager().ClearTimer(TimerRevision);
    GetWorldTimerManager().ClearTimer(TimerInterfaz);

    CerrarVentanasLocales();

    Super::EndPlay(EndPlayReason);
}


bool AReclaimNotaQuestActor::HayJugadorEnLaZona() const
{
    const UWorld* Mundo = GetWorld();

    if (!Mundo || !IsValid(ZonaReunion))
    {
        return false;
    }

    for (
        FConstPlayerControllerIterator It =
            Mundo->GetPlayerControllerIterator();
        It;
        ++It
    )
    {
        const APlayerController* Controlador = It->Get();

        if (!IsValid(Controlador))
        {
            continue;
        }

        const APlayerState* EstadoJugador =
            Controlador->GetPlayerState<APlayerState>();

        if (EstadoJugador && EstadoJugador->IsOnlyASpectator())
        {
            continue;
        }

        const APawn* Personaje = Controlador->GetPawn();

        if (IsValid(Personaje) &&
            ZonaReunion->IsOverlappingActor(Personaje))
        {
            return true;
        }
    }

    return false;
}


void AReclaimNotaQuestActor::RevisarZona()
{
    if (!HasAuthority() ||
        bNotaAbierta ||
        bNotaFinalizada ||
        !IsValid(DatosNota) ||
        !HayJugadorEnLaZona())
    {
        return;
    }

    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return;
    }

    AGameStateBase* EstadoJuego = Mundo->GetGameState();

    if (!IsValid(EstadoJuego))
    {
        return;
    }

    UReclaimQuestComponent* ComponenteQuest =
        EstadoJuego->FindComponentByClass<UReclaimQuestComponent>();

    if (!IsValid(ComponenteQuest))
    {
        return;
    }

    const TArray<FEstadoQuest> Misiones =
        ComponenteQuest->ObtenerMisiones();

    bool bPuedeAbrirNota = false;

    for (const FEstadoQuest& Estado : Misiones)
    {
        if (!IsValid(Estado.DatosMision) ||
            Estado.DatosMision->IdMision != DatosNota->IdMisionActual ||
            Estado.bCompletada)
        {
            continue;
        }
        
        if (DatosNota->IdObjetivoLlegada.IsNone())
        {
            bPuedeAbrirNota = true;
            break;
        }

        const FObjetivoQuest* Objetivo =
            Estado.DatosMision->Objetivos.FindByPredicate(
                [this](const FObjetivoQuest& Actual)
                {
                    return Actual.IdObjetivo ==
                        DatosNota->IdObjetivoLlegada;
                }
            );

        const FProgresoObjetivoQuest* Progreso =
            Estado.ProgresoObjetivos.FindByPredicate(
                [this](const FProgresoObjetivoQuest& Actual)
                {
                    return Actual.IdObjetivo ==
                        DatosNota->IdObjetivoLlegada;
                }
            );

        bPuedeAbrirNota =
            Objetivo &&
            Progreso &&
            Progreso->CantidadActual >= Objetivo->CantidadRequerida;

        break;
    }

    if (!bPuedeAbrirNota)
    {
        return;
    }

    bNotaAbierta = true;

    ForceNetUpdate();
    GetWorldTimerManager().ClearTimer(TimerRevision);

    UE_LOG(
        LogTemp,
        Log,
        TEXT("NOTA QUEST: Nota abierta: %s"),
        *DatosNota->IdNota.ToString()
    );
    
    ActualizarVentanasLocales();
}
bool AReclaimNotaQuestActor::EstaFinalizada() const
{
    return bNotaFinalizada;
}

bool AReclaimNotaQuestActor::RestaurarComoFinalizada()
{
    if (!HasAuthority() || !IsValid(DatosNota))
    {
        return false;
    }

    bNotaAbierta = false;
    bNotaFinalizada = true;
    
    GetWorldTimerManager().ClearTimer(TimerRevision);

    ForceNetUpdate();

    return true;
}

bool AReclaimNotaQuestActor::PuedeContinuar(
    const APawn* Personaje
) const
{
    if (!HasAuthority() ||
        !bNotaAbierta ||
        bNotaFinalizada ||
        !IsValid(Personaje))
    {
        return false;
    }
    
    return IsValid(
        Cast<APlayerController>(Personaje->GetController())
    );
}


bool AReclaimNotaQuestActor::FinalizarLectura()
{
    if (!HasAuthority() ||
        !bNotaAbierta ||
        bNotaFinalizada ||
        !IsValid(DatosNota) ||
        DatosNota->IdObjetivoLectura.IsNone())
    {
        return false;
    }

    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return false;
    }

    AGameStateBase* EstadoJuego = Mundo->GetGameState();

    if (!IsValid(EstadoJuego))
    {
        return false;
    }

    UReclaimQuestComponent* ComponenteQuest =
        EstadoJuego->FindComponentByClass<UReclaimQuestComponent>();

    if (!IsValid(ComponenteQuest))
    {
        return false;
    }
    
    const bool bLecturaCompletada =
        ComponenteQuest->AvanzarObjetivo(
            DatosNota->IdMisionActual,
            DatosNota->IdObjetivoLectura,
            1
        );

    if (!bLecturaCompletada)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("NOTA QUEST: No se pudo completar el objetivo de lectura.")
        );

        return false;
    }
    
    if (IsValid(DatosNota->MisionSiguiente))
    {
        if (!ComponenteQuest->IniciarMision(DatosNota->MisionSiguiente))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("NOTA QUEST: No se pudo iniciar la mision siguiente.")
            );
        }
    }

    bNotaAbierta = false;
    bNotaFinalizada = true;

    ForceNetUpdate();
    
    ActualizarVentanasLocales();

    UE_LOG(
        LogTemp,
        Log,
        TEXT("NOTA QUEST: Lectura finalizada.")
    );

    return true;
}


void AReclaimNotaQuestActor::OnRep_EstadoNota()
{
    ActualizarVentanasLocales();
}


void AReclaimNotaQuestActor::ActualizarVentanasLocales()
{
    UWorld* Mundo = GetWorld();

    if (!Mundo || Mundo->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    if (!bNotaAbierta || bNotaFinalizada)
    {
        CerrarVentanasLocales();
        return;
    }

    if (!IsValid(DatosNota))
    {
        return;
    }

    for (
        FConstPlayerControllerIterator It =
            Mundo->GetPlayerControllerIterator();
        It;
        ++It
    )
    {
        APlayerController* Controlador = It->Get();

        if (!IsValid(Controlador) ||
            !Controlador->IsLocalController() ||
            VentanasPorJugador.Contains(Controlador))
        {
            continue;
        }

        APawn* Personaje = Controlador->GetPawn();

        if (!IsValid(Personaje))
        {
            continue;
        }

        UReclaimNotaJugadorComponent* ComponenteNota =
            Personaje->FindComponentByClass<UReclaimNotaJugadorComponent>();

        if (!IsValid(ComponenteNota))
        {
            continue;
        }

        UReclaimNotaQuestWidget* Ventana =
            CreateWidget<UReclaimNotaQuestWidget>(
                Controlador,
                UReclaimNotaQuestWidget::StaticClass()
            );

        if (!IsValid(Ventana))
        {
            continue;
        }

        Ventana->MostrarNota(DatosNota);
        ComponenteNota->PrepararNota(this);

        Ventana->AlContinuarNota.AddDynamic(
            ComponenteNota,
            &UReclaimNotaJugadorComponent::SolicitarContinuarNota
        );

        if (!Ventana->AddToPlayerScreen())
        {
            ComponenteNota->LimpiarNota();
            continue;
        }

        VentanasPorJugador.Add(Controlador, Ventana);
        
        FInputModeUIOnly ModoEntrada;
        ModoEntrada.SetWidgetToFocus(Ventana->TakeWidget());

        Controlador->SetInputMode(ModoEntrada);
        Ventana->SetUserFocus(Controlador);
    }
}


void AReclaimNotaQuestActor::CerrarVentanasLocales()
{
    for (const auto& Entrada : VentanasPorJugador)
    {
        APlayerController* Controlador = Entrada.Key.Get();
        UReclaimNotaQuestWidget* Ventana = Entrada.Value.Get();

        if (IsValid(Controlador))
        {
            APawn* Personaje = Controlador->GetPawn();

            if (IsValid(Personaje))
            {
                UReclaimNotaJugadorComponent* ComponenteNota =
                    Personaje->FindComponentByClass<UReclaimNotaJugadorComponent>();

                if (IsValid(ComponenteNota))
                {
                    if (IsValid(Ventana))
                    {
                        Ventana->AlContinuarNota.RemoveDynamic(
                            ComponenteNota,
                            &UReclaimNotaJugadorComponent::SolicitarContinuarNota
                        );
                    }

                    ComponenteNota->LimpiarNota();
                }
            }
        }

        if (IsValid(Ventana))
        {
            Ventana->RemoveFromParent();
        }

        if (IsValid(Controlador))
        {
            FInputModeGameOnly ModoEntrada;
            Controlador->SetInputMode(ModoEntrada);
        }
    }

    VentanasPorJugador.Empty();
}

