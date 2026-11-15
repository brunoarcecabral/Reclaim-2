#include "SistemaQuest/ReclaimQuestComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "SistemaQuest/ReclaimMisionesWidget.h"
#include "TimerManager.h"


UReclaimQuestComponent::UReclaimQuestComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Replica los datos junto con el actor propietario.
    SetIsReplicatedByDefault(true);
}


void UReclaimQuestComponent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("QUEST INTERFAZ: BeginPlay del componente. Actor: %s"),
        *GetNameSafe(GetOwner())
    );

    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("QUEST INTERFAZ: El componente no tiene mundo.")
        );

        return;
    }

    if (Mundo->GetNetMode() == NM_DedicatedServer)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("QUEST INTERFAZ: Servidor dedicado, no necesita interfaz.")
        );

        return;
    }

    Mundo->GetTimerManager().SetTimer(
        TimerInterfaces,
        this,
        &UReclaimQuestComponent::RevisarInterfacesLocales,
        0.5f,
        true
    );

    RevisarInterfacesLocales();
}


void UReclaimQuestComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerInterfaces);
    }

    QuitarInterfacesLocales();

    Super::EndPlay(EndPlayReason);
}


void UReclaimQuestComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UReclaimQuestComponent, Misiones);
}


TArray<FEstadoQuest> UReclaimQuestComponent::ObtenerMisiones() const
{
    return Misiones;
}


bool UReclaimQuestComponent::IniciarMision(
    UReclaimQuestData* DatosMision
)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return false;
    }

    if (!IsValid(DatosMision) ||
        DatosMision->IdMision.IsNone() ||
        DatosMision->Objetivos.IsEmpty())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("No se puede iniciar la mision: faltan datos, ID u objetivos.")
        );

        return false;
    }

    // Evita duplicados, incluso si la misión ya fue completada.
    for (const FEstadoQuest& MisionExistente : Misiones)
    {
        if (IsValid(MisionExistente.DatosMision) &&
            MisionExistente.DatosMision->IdMision == DatosMision->IdMision)
        {
            return false;
        }
    }

    // Comprueba que los objetivos tengan IDs válidos y diferentes.
    TSet<FName> IdsObjetivos;

    for (const FObjetivoQuest& Objetivo : DatosMision->Objetivos)
    {
        if (Objetivo.IdObjetivo.IsNone() ||
            Objetivo.CantidadRequerida < 1 ||
            IdsObjetivos.Contains(Objetivo.IdObjetivo))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("La mision %s tiene un objetivo invalido o repetido."),
                *DatosMision->IdMision.ToString()
            );

            return false;
        }

        IdsObjetivos.Add(Objetivo.IdObjetivo);
    }

    FEstadoQuest NuevaMision;
    NuevaMision.DatosMision = DatosMision;

    for (const FObjetivoQuest& Objetivo : DatosMision->Objetivos)
    {
        FProgresoObjetivoQuest NuevoProgreso;
        NuevoProgreso.IdObjetivo = Objetivo.IdObjetivo;
        NuevoProgreso.CantidadActual = 0;

        NuevaMision.ProgresoObjetivos.Add(NuevoProgreso);
    }

    Misiones.Add(NuevaMision);
    NotificarCambio();

    return true;
}


bool UReclaimQuestComponent::AvanzarObjetivo(
    FName IdMision,
    FName IdObjetivo,
    int32 Cantidad
)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return false;
    }

    if (IdMision.IsNone() ||
        IdObjetivo.IsNone() ||
        Cantidad <= 0)
    {
        return false;
    }

    for (FEstadoQuest& Mision : Misiones)
    {
        if (!IsValid(Mision.DatosMision) ||
            Mision.DatosMision->IdMision != IdMision)
        {
            continue;
        }

        if (Mision.bCompletada)
        {
            return false;
        }

        const FObjetivoQuest* DatosObjetivo =
            Mision.DatosMision->Objetivos.FindByPredicate(
                [IdObjetivo](const FObjetivoQuest& Objetivo)
                {
                    return Objetivo.IdObjetivo == IdObjetivo;
                }
            );

        FProgresoObjetivoQuest* Progreso =
            Mision.ProgresoObjetivos.FindByPredicate(
                [IdObjetivo](const FProgresoObjetivoQuest& Objetivo)
                {
                    return Objetivo.IdObjetivo == IdObjetivo;
                }
            );

        if (!DatosObjetivo || !Progreso)
        {
            return false;
        }

        const int32 Requerido =
            DatosObjetivo->CantidadRequerida;

        if (Progreso->CantidadActual >= Requerido)
        {
            return false;
        }

        const int32 Restante =
            Requerido - Progreso->CantidadActual;

        Progreso->CantidadActual +=
            FMath::Min(Cantidad, Restante);

        bool bTodosCompletados = true;

        for (const FObjetivoQuest& Objetivo :
             Mision.DatosMision->Objetivos)
        {
            const FProgresoObjetivoQuest* EstadoObjetivo =
                Mision.ProgresoObjetivos.FindByPredicate(
                    [&Objetivo](const FProgresoObjetivoQuest& Estado)
                    {
                        return Estado.IdObjetivo == Objetivo.IdObjetivo;
                    }
                );

            if (!EstadoObjetivo ||
                EstadoObjetivo->CantidadActual < Objetivo.CantidadRequerida)
            {
                bTodosCompletados = false;
                break;
            }
        }

        Mision.bCompletada = bTodosCompletados;
        NotificarCambio();

        return true;
    }

    return false;
}


void UReclaimQuestComponent::OnRep_Misiones()
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("QUEST INTERFAZ: Cliente recibio %d misiones."),
        Misiones.Num()
    );

    AlCambiarMisiones.Broadcast();
}


void UReclaimQuestComponent::NotificarCambio()
{
    AlCambiarMisiones.Broadcast();

    if (AActor* Propietario = GetOwner())
    {
        Propietario->ForceNetUpdate();
    }
}


void UReclaimQuestComponent::RevisarInterfacesLocales()
{
    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return;
    }

    TSet<APlayerController*> JugadoresLocales;

    for (
        FConstPlayerControllerIterator It =
            Mundo->GetPlayerControllerIterator();
        It;
        ++It
    )
    {
        APlayerController* Controlador = It->Get();

        if (!IsValid(Controlador) ||
            !Controlador->IsLocalController())
        {
            continue;
        }

        JugadoresLocales.Add(Controlador);

        if (!InterfacesPorJugador.Contains(Controlador))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("QUEST INTERFAZ: Jugador local encontrado: %s"),
                *GetNameSafe(Controlador)
            );

            UReclaimMisionesWidget* Interfaz =
                CreateWidget<UReclaimMisionesWidget>(
                    Controlador,
                    UReclaimMisionesWidget::StaticClass()
                );

            if (!IsValid(Interfaz))
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("QUEST INTERFAZ: No se pudo crear el widget.")
                );
            }
            else if (Interfaz->AddToPlayerScreen())
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("QUEST INTERFAZ: Widget agregado a la pantalla.")
                );

                InterfacesPorJugador.Add(Controlador, Interfaz);
            }
            else
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("QUEST INTERFAZ: El widget se creo, pero no se pudo agregar.")
                );
            }
        }
    }

    for (auto It = InterfacesPorJugador.CreateIterator(); It; ++It)
    {
        APlayerController* Controlador = It.Key().Get();

        if (!IsValid(Controlador) ||
            !JugadoresLocales.Contains(Controlador))
        {
            if (IsValid(It.Value()))
            {
                It.Value()->RemoveFromParent();
            }

            It.RemoveCurrent();
        }
    }
}


void UReclaimQuestComponent::QuitarInterfacesLocales()
{
    for (const auto& Entrada : InterfacesPorJugador)
    {
        if (IsValid(Entrada.Value))
        {
            Entrada.Value->RemoveFromParent();
        }
    }

    InterfacesPorJugador.Empty();
}

