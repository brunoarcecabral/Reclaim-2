#include "SistemaQuest/ReclaimColmilloComponent.h"

#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"


UReclaimColmilloComponent::UReclaimColmilloComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}


void UReclaimColmilloComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario))
    {
        return;
    }

    // Este componente estará en el mismo GameState que las quests.
    ComponenteQuest =
        Propietario->FindComponentByClass<UReclaimQuestComponent>();

    if (!IsValid(ComponenteQuest))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("COLMILLO: No se encontro ReclaimQuestComponent en el GameState.")
        );

        return;
    }

    ComponenteQuest->AlCambiarMisiones.AddDynamic(
        this,
        &UReclaimColmilloComponent::AlCambiarMisiones
    );
}


void UReclaimColmilloComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (IsValid(ComponenteQuest))
    {
        ComponenteQuest->AlCambiarMisiones.RemoveDynamic(
            this,
            &UReclaimColmilloComponent::AlCambiarMisiones
        );
    }

    ComponenteQuest = nullptr;

    Super::EndPlay(EndPlayReason);
}


void UReclaimColmilloComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(
        UReclaimColmilloComponent,
        bColmilloComprado
    );
}


bool UReclaimColmilloComponent::EstaDisponibleEnTienda() const
{
    if (bColmilloComprado || !IsValid(ComponenteQuest))
    {
        return false;
    }

    const TArray<FEstadoQuest> Misiones =
        ComponenteQuest->ObtenerMisiones();

    for (const FEstadoQuest& Estado : Misiones)
    {
        if (IsValid(Estado.DatosMision) &&
            Estado.DatosMision->IdMision == FName(TEXT("BuscarHabilidad")) &&
            !Estado.bCompletada)
        {
            return true;
        }
    }

    return false;
}


bool UReclaimColmilloComponent::YaFueComprado() const
{
    return bColmilloComprado;
}


void UReclaimColmilloComponent::OnRep_ColmilloComprado()
{
    AlCambiarColmillo.Broadcast();
}


void UReclaimColmilloComponent::AlCambiarMisiones()
{
    AlCambiarColmillo.Broadcast();
}

