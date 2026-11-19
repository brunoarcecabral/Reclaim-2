#include "UI/ReclaimRecoleccionComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "UI/ReclaimRecoleccionWidget.h"

UReclaimRecoleccionComponent::UReclaimRecoleccionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UReclaimRecoleccionComponent::NotificarRecoleccion(
    FText NombreRecurso,
    int32 Cantidad
)
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario) ||
        !Propietario->HasAuthority() ||
        NombreRecurso.IsEmpty() ||
        Cantidad <= 0)
    {
        return;
    }

    ClienteMostrarRecoleccion(NombreRecurso, Cantidad);
}

void UReclaimRecoleccionComponent::ClienteMostrarRecoleccion_Implementation(
    const FText& NombreRecurso,
    int32 Cantidad
)
{
    APawn* Personaje = Cast<APawn>(GetOwner());
    UWorld* Mundo = GetWorld();

    if (!IsValid(Personaje) ||
        !Personaje->IsLocallyControlled() ||
        !Mundo ||
        NombreRecurso.IsEmpty() ||
        Cantidad <= 0)
    {
        return;
    }

    APlayerController* Controlador =
        Cast<APlayerController>(Personaje->GetController());

    if (!IsValid(Controlador))
    {
        return;
    }

    if (!IsValid(VentanaRecoleccion))
    {
        VentanaRecoleccion =
            CreateWidget<UReclaimRecoleccionWidget>(
                Controlador,
                UReclaimRecoleccionWidget::StaticClass()
            );

        if (!IsValid(VentanaRecoleccion))
        {
            return;
        }

        if (!VentanaRecoleccion->AddToPlayerScreen(100))
        {
            VentanaRecoleccion = nullptr;
            return;
        }
    }

    const FString Clave = NombreRecurso.ToString();

    FReclaimAvisoRecurso& Aviso =
        AvisosActivos.FindOrAdd(Clave);

    Aviso.Cantidad += Cantidad;

    VentanaRecoleccion->ActualizarRecurso(
        Clave,
        NombreRecurso,
        Aviso.Cantidad
    );

    Mundo->GetTimerManager().ClearTimer(Aviso.Timer);

    TWeakObjectPtr<UReclaimRecoleccionComponent> ComponenteDebil(this);

    FTimerDelegate AlTerminar = FTimerDelegate::CreateLambda(
        [ComponenteDebil, Clave]()
        {
            if (ComponenteDebil.IsValid())
            {
                ComponenteDebil->OcultarRecurso(Clave);
            }
        }
    );

    Mundo->GetTimerManager().SetTimer(
        Aviso.Timer,
        AlTerminar,
        FMath::Max(0.1f, DuracionMensaje),
        false
    );
}

void UReclaimRecoleccionComponent::OcultarRecurso(
    const FString& Clave
)
{
    if (IsValid(VentanaRecoleccion))
    {
        VentanaRecoleccion->QuitarRecurso(Clave);
    }

    AvisosActivos.Remove(Clave);

    if (AvisosActivos.IsEmpty() && IsValid(VentanaRecoleccion))
    {
        VentanaRecoleccion->RemoveFromParent();
        VentanaRecoleccion = nullptr;
    }
}

void UReclaimRecoleccionComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (UWorld* Mundo = GetWorld())
    {
        for (auto& Entrada : AvisosActivos)
        {
            Mundo->GetTimerManager().ClearTimer(Entrada.Value.Timer);
        }
    }

    AvisosActivos.Empty();

    if (IsValid(VentanaRecoleccion))
    {
        VentanaRecoleccion->RemoveFromParent();
        VentanaRecoleccion = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

