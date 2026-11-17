#include "SistemaNivel/ReclaimTutorialJugadorComponent.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "SistemaNivel/ReclaimTutorialNivelComponent.h"
#include "SistemaQuest/ReclaimNotaQuestData.h"
#include "SistemaQuest/ReclaimNotaQuestWidget.h"
#include "TimerManager.h"


UReclaimTutorialJugadorComponent::UReclaimTutorialJugadorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}


void UReclaimTutorialJugadorComponent::BeginPlay()
{
    Super::BeginPlay();

    UWorld* Mundo = GetWorld();

    if (!Mundo || Mundo->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    // Espera a que el jugador local y el GameState estén disponibles.
    Mundo->GetTimerManager().SetTimer(
        TimerBuscarTutorial,
        this,
        &UReclaimTutorialJugadorComponent::BuscarTutorialCompartido,
        0.5f,
        true
    );

    BuscarTutorialCompartido();
}


void UReclaimTutorialJugadorComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerBuscarTutorial);
    }

    if (IsValid(TutorialCompartido))
    {
        TutorialCompartido->AlCambiarTutorial.RemoveDynamic(
            this,
            &UReclaimTutorialJugadorComponent::ActualizarTutorial
        );
    }

    CerrarVentanaLocal();
    TutorialCompartido = nullptr;

    Super::EndPlay(EndPlayReason);
}


void UReclaimTutorialJugadorComponent::BuscarTutorialCompartido()
{
    APawn* Personaje = Cast<APawn>(GetOwner());
    UWorld* Mundo = GetWorld();

    if (!IsValid(Personaje) ||
        !Personaje->IsLocallyControlled() ||
        !Mundo)
    {
        return;
    }

    AGameStateBase* EstadoJuego = Mundo->GetGameState();

    if (!IsValid(EstadoJuego))
    {
        return;
    }

    UReclaimTutorialNivelComponent* Encontrado =
        EstadoJuego->FindComponentByClass<UReclaimTutorialNivelComponent>();

    if (!IsValid(Encontrado))
    {
        return;
    }

    if (TutorialCompartido != Encontrado)
    {
        if (IsValid(TutorialCompartido))
        {
            TutorialCompartido->AlCambiarTutorial.RemoveDynamic(
                this,
                &UReclaimTutorialJugadorComponent::ActualizarTutorial
            );
        }

        TutorialCompartido = Encontrado;

        TutorialCompartido->AlCambiarTutorial.AddDynamic(
            this,
            &UReclaimTutorialJugadorComponent::ActualizarTutorial
        );
    }

    ActualizarTutorial();
}


void UReclaimTutorialJugadorComponent::ActualizarTutorial()
{
    APawn* Personaje = Cast<APawn>(GetOwner());

    if (!IsValid(Personaje) ||
        !Personaje->IsLocallyControlled() ||
        !IsValid(TutorialCompartido))
    {
        return;
    }

    if (!TutorialCompartido->EstaAbierto())
    {
        CerrarVentanaLocal();
        return;
    }

    UReclaimNotaQuestData* Datos =
        TutorialCompartido->ObtenerDatosTutorial();

    if (!IsValid(Datos))
    {
        return;
    }

    // Si hay otro tutorial en cola, usamos la misma ventana.
    if (IsValid(VentanaTutorial))
    {
        VentanaTutorial->MostrarNota(Datos);
        return;
    }

    APlayerController* Controlador =
        Cast<APlayerController>(Personaje->GetController());

    if (!IsValid(Controlador) || !Controlador->IsLocalController())
    {
        return;
    }

    UReclaimNotaQuestWidget* NuevaVentana =
        CreateWidget<UReclaimNotaQuestWidget>(
            Controlador,
            UReclaimNotaQuestWidget::StaticClass()
        );

    if (!IsValid(NuevaVentana))
    {
        return;
    }

    NuevaVentana->MostrarNota(Datos);
    NuevaVentana->UsarCierreAutomatico();

    NuevaVentana->AlContinuarNota.AddDynamic(
        this,
        &UReclaimTutorialJugadorComponent::SolicitarCerrarTutorial
    );

    if (!NuevaVentana->AddToPlayerScreen(150))
    {
        NuevaVentana->AlContinuarNota.RemoveDynamic(
            this,
            &UReclaimTutorialJugadorComponent::SolicitarCerrarTutorial
        );

        return;
    }

    VentanaTutorial = NuevaVentana;

    // El tutorial es informativo: no captura el mouse ni cambia
    // el modo de entrada del juego o de la tienda.
    VentanaTutorial->SetVisibility(ESlateVisibility::HitTestInvisible);
}


void UReclaimTutorialJugadorComponent::ReenfocarVentana()
{
    APawn* Personaje = Cast<APawn>(GetOwner());

    if (!IsValid(Personaje) ||
        !Personaje->IsLocallyControlled() ||
        !IsValid(VentanaTutorial))
    {
        return;
    }

    APlayerController* Controlador =
        Cast<APlayerController>(Personaje->GetController());

    if (!IsValid(Controlador) || !Controlador->IsLocalController())
    {
        return;
    }

    FInputModeGameAndUI ModoEntrada;
    ModoEntrada.SetWidgetToFocus(VentanaTutorial->TakeWidget());

    Controlador->SetInputMode(ModoEntrada);
    VentanaTutorial->SetUserFocus(Controlador);
}


void UReclaimTutorialJugadorComponent::SolicitarCerrarTutorial()
{
    APawn* Personaje = Cast<APawn>(GetOwner());

    if (!IsValid(Personaje) || !Personaje->IsLocallyControlled())
    {
        return;
    }

    ServidorCerrarTutorial();
}


void UReclaimTutorialJugadorComponent::ServidorCerrarTutorial_Implementation()
{
    APawn* Personaje = Cast<APawn>(GetOwner());
    UWorld* Mundo = GetWorld();

    if (!IsValid(Personaje) ||
        !Personaje->IsPlayerControlled() ||
        !Mundo)
    {
        return;
    }

    AGameStateBase* EstadoJuego = Mundo->GetGameState();

    if (!IsValid(EstadoJuego))
    {
        return;
    }

    UReclaimTutorialNivelComponent* Tutorial =
        EstadoJuego->FindComponentByClass<UReclaimTutorialNivelComponent>();

    if (IsValid(Tutorial))
    {
        Tutorial->CerrarTutorial();
    }
}


void UReclaimTutorialJugadorComponent::CerrarVentanaLocal()
{
    if (!IsValid(VentanaTutorial))
    {
        VentanaTutorial = nullptr;
        return;
    }

    VentanaTutorial->AlContinuarNota.RemoveDynamic(
        this,
        &UReclaimTutorialJugadorComponent::SolicitarCerrarTutorial
    );

    VentanaTutorial->RemoveFromParent();
    VentanaTutorial = nullptr;
}

