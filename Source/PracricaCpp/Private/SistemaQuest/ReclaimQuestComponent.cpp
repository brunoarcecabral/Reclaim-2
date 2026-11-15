#include "SistemaQuest/ReclaimQuestComponent.h"


#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"


UReclaimQuestComponent::UReclaimQuestComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Replica los datos del componente junto con su actor propietario.
    SetIsReplicatedByDefault(true);
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

        // No se puede seguir avanzando una misión completada.
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

        const int32 Requerido = DatosObjetivo->CantidadRequerida;

        if (Progreso->CantidadActual >= Requerido)
        {
            return false;
        }

        // Suma solamente lo que falta para completar el objetivo.
        const int32 Restante =
            Requerido - Progreso->CantidadActual;

        Progreso->CantidadActual += FMath::Min(Cantidad, Restante);

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

        // La misión se completa cuando todos sus objetivos terminan.
        Mision.bCompletada = bTodosCompletados;

        NotificarCambio();

        return true;
    }

    return false;
}


void UReclaimQuestComponent::OnRep_Misiones()
{
    // Se ejecuta en los clientes cuando reciben cambios del servidor.
    AlCambiarMisiones.Broadcast();
}


void UReclaimQuestComponent::NotificarCambio()
{
    // Avisa también al servidor y al host.
    AlCambiarMisiones.Broadcast();

    if (AActor* Propietario = GetOwner())
    {
        Propietario->ForceNetUpdate();
    }
}

