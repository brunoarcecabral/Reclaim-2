#include "SistemaNivel/ReclaimTutorialNivelComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "SistemaQuest/ReclaimNotaQuestData.h"
#include "TimerManager.h"


UReclaimTutorialNivelComponent::UReclaimTutorialNivelComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}


void UReclaimTutorialNivelComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerCerrarTutorial);
    }

    Super::EndPlay(EndPlayReason);
}


void UReclaimTutorialNivelComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(
        UReclaimTutorialNivelComponent,
        TutorialActual
    );
}


bool UReclaimTutorialNivelComponent::EstaAbierto() const
{
    return TutorialActual != ETutorialReclaim::Ninguno;
}


UReclaimNotaQuestData*
UReclaimTutorialNivelComponent::ObtenerDatosTutorial() const
{
    switch (TutorialActual)
    {
        case ETutorialReclaim::Nivel:
            return DatosTutorial;

        case ETutorialReclaim::WallRun:
            return DatosTutorialWallRun;
        
    case ETutorialReclaim::Arma:
        return DatosTutorialArma;
    
        default:
            return nullptr;
    }
}


bool UReclaimTutorialNivelComponent::AbrirTutorial()
{
    return SolicitarTutorial(ETutorialReclaim::Nivel);
}


bool UReclaimTutorialNivelComponent::AbrirTutorialWallRun()
{
    return SolicitarTutorial(ETutorialReclaim::WallRun);
}

bool UReclaimTutorialNivelComponent::AbrirTutorialArma()
{
    return SolicitarTutorial(ETutorialReclaim::Arma);
}

bool UReclaimTutorialNivelComponent::SolicitarTutorial(
    ETutorialReclaim Tutorial
)
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        Tutorial == ETutorialReclaim::Ninguno)
    {
        return false;
    }

    if (Tutorial == ETutorialReclaim::Nivel &&
        !IsValid(DatosTutorial))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("TUTORIAL: Falta asignar DA_TutorialNivel.")
        );

        return false;
    }

    if (Tutorial == ETutorialReclaim::WallRun &&
        !IsValid(DatosTutorialWallRun))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("TUTORIAL: Falta asignar DA_TutorialWallRun.")
        );

        return false;
    }
    
    if (Tutorial == ETutorialReclaim::Arma &&
    !IsValid(DatosTutorialArma))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("TUTORIAL: Falta asignar DA_TutorialArma.")
        );

        return false;
    }
    
    if (TutorialActual == Tutorial ||
        TutorialesPendientes.Contains(Tutorial))
    {
        return false;
    }

    if (TutorialActual == ETutorialReclaim::Ninguno)
    {
        TutorialActual = Tutorial;

        IniciarTemporizador();
        NotificarCambio();
    }
    else
    {
        TutorialesPendientes.Add(Tutorial);
    }

    return true;
}


void UReclaimTutorialNivelComponent::IniciarTemporizador()
{
    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return;
    }

    Mundo->GetTimerManager().SetTimer(
        TimerCerrarTutorial,
        this,
        &UReclaimTutorialNivelComponent::CerrarPorTiempo,
        FMath::Max(1.0f, DuracionTutorial),
        false
    );
}


void UReclaimTutorialNivelComponent::CerrarPorTiempo()
{
    CerrarTutorial();
}


bool UReclaimTutorialNivelComponent::CerrarTutorial()
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        TutorialActual == ETutorialReclaim::Ninguno)
    {
        return false;
    }

    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerCerrarTutorial);
    }

    if (!TutorialesPendientes.IsEmpty())
    {
        TutorialActual = TutorialesPendientes[0];
        TutorialesPendientes.RemoveAt(0);
        
        IniciarTemporizador();
    }
    else
    {
        TutorialActual = ETutorialReclaim::Ninguno;
    }

    NotificarCambio();
    return true;
}


void UReclaimTutorialNivelComponent::OnRep_TutorialActual()
{
    AlCambiarTutorial.Broadcast();
}


void UReclaimTutorialNivelComponent::NotificarCambio()
{
    AlCambiarTutorial.Broadcast();

    if (AActor* Propietario = GetOwner())
    {
        Propietario->ForceNetUpdate();
    }
}

