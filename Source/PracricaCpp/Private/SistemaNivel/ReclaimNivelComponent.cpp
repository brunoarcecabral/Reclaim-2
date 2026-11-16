#include "SistemaNivel/ReclaimNivelComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UI/ReclaimNivelWidget.h"


UReclaimNivelComponent::UReclaimNivelComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}


void UReclaimNivelComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario))
    {
        return;
    }

    // El servidor inicia el nivel de este personaje.
    if (Propietario->HasAuthority())
    {
        EstadoNivel.Nivel = 1;
        EstadoNivel.ExperienciaActual = 0;
        EstadoNivel.ExperienciaRequerida =
            FMath::Max(1, ExperienciaParaNivel2);

        NotificarCambio();
    }

    UWorld* Mundo = GetWorld();

    if (!Mundo || Mundo->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    // Puede que el controlador local todavía no esté listo en BeginPlay.
    Mundo->GetTimerManager().SetTimer(
        TimerCrearInterfaz,
        this,
        &UReclaimNivelComponent::IntentarCrearInterfaz,
        0.5f,
        true
    );

    IntentarCrearInterfaz();
}


void UReclaimNivelComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerCrearInterfaz);
    }

    if (IsValid(InterfazNivel))
    {
        InterfazNivel->RemoveFromParent();
        InterfazNivel = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}


void UReclaimNivelComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(
        UReclaimNivelComponent,
        EstadoNivel
    );
}


FEstadoNivelReclaim UReclaimNivelComponent::ObtenerEstadoNivel() const
{
    return EstadoNivel;
}


int32 UReclaimNivelComponent::ObtenerNivel() const
{
    return EstadoNivel.Nivel;
}


bool UReclaimNivelComponent::AgregarExperiencia(int32 Cantidad)
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        Cantidad <= 0 ||
        EstadoNivel.Nivel >= NivelMaximo)
    {
        return false;
    }

    int64 ExperienciaTotal =
        static_cast<int64>(EstadoNivel.ExperienciaActual) +
        static_cast<int64>(Cantidad);

    while (
        EstadoNivel.Nivel < NivelMaximo &&
        ExperienciaTotal >= EstadoNivel.ExperienciaRequerida
    )
    {
        ExperienciaTotal -= EstadoNivel.ExperienciaRequerida;
        EstadoNivel.Nivel++;

        if (EstadoNivel.Nivel >= NivelMaximo)
        {
            ExperienciaTotal = 0;
            EstadoNivel.ExperienciaRequerida = 0;
            break;
        }

        const float Multiplicador =
            FMath::Max(1.0f, MultiplicadorExperiencia);

        const double NuevaExperiencia =
            static_cast<double>(EstadoNivel.ExperienciaRequerida) *
            static_cast<double>(Multiplicador);

        const double ValorLimitado =
            FMath::Min(NuevaExperiencia, 2000000000.0);

        EstadoNivel.ExperienciaRequerida =
            FMath::Max(
                1,
                FMath::CeilToInt(ValorLimitado)
            );
    }

    EstadoNivel.ExperienciaActual =
        static_cast<int32>(
            FMath::Min<int64>(ExperienciaTotal, 2000000000)
        );

    NotificarCambio();

    return true;
}


void UReclaimNivelComponent::OnRep_EstadoNivel()
{
    AlCambiarNivel.Broadcast();
}


void UReclaimNivelComponent::NotificarCambio()
{
    AlCambiarNivel.Broadcast();

    if (AActor* Propietario = GetOwner())
    {
        Propietario->ForceNetUpdate();
    }
}


void UReclaimNivelComponent::IntentarCrearInterfaz()
{
    UWorld* Mundo = GetWorld();
    APawn* Personaje = Cast<APawn>(GetOwner());

    if (!Mundo || !IsValid(Personaje))
    {
        return;
    }

    if (IsValid(InterfazNivel))
    {
        Mundo->GetTimerManager().ClearTimer(TimerCrearInterfaz);
        return;
    }

    if (!Personaje->IsLocallyControlled())
    {
        return;
    }

    APlayerController* Controlador =
        Cast<APlayerController>(Personaje->GetController());

    if (!IsValid(Controlador) || !Controlador->IsLocalController())
    {
        return;
    }

    UReclaimNivelWidget* NuevaInterfaz =
        CreateWidget<UReclaimNivelWidget>(
            Controlador,
            UReclaimNivelWidget::StaticClass()
        );

    if (!IsValid(NuevaInterfaz))
    {
        return;
    }

    NuevaInterfaz->ConectarNivel(this);

    if (!NuevaInterfaz->AddToPlayerScreen())
    {
        return;
    }

    InterfazNivel = NuevaInterfaz;

    // Ya no necesitamos seguir intentando crearla.
    Mundo->GetTimerManager().ClearTimer(TimerCrearInterfaz);
}

