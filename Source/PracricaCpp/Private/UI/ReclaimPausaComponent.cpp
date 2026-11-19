#include "UI/ReclaimPausaComponent.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SistemaQuest/ReclaimNotaQuestWidget.h"
#include "UI/ReclaimBibliotecaNotasWidget.h"
#include "UI/ReclaimBibliotecaTutorialWidget.h"
#include "UI/ReclaimMenuPausaWidget.h"
#include "SistemaGuardado/ReclaimGuardadoJugadorComponent.h"

UReclaimPausaComponent::UReclaimPausaComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UReclaimPausaComponent::HayVentanaBloqueante() const
{
    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return true;
    }

    TArray<UUserWidget*> Ventanas;
    
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
        Mundo,
        Ventanas,
        UReclaimNotaQuestWidget::StaticClass(),
        true
    );

    if (!Ventanas.IsEmpty())
    {
        return true;
    }

    Ventanas.Empty();
    
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
        Mundo,
        Ventanas,
        UReclaimBibliotecaTutorialWidget::StaticClass(),
        true
    );

    if (!Ventanas.IsEmpty())
    {
        return true;
    }

    Ventanas.Empty();
    
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
        Mundo,
        Ventanas,
        UReclaimBibliotecaNotasWidget::StaticClass(),
        true
    );

    return !Ventanas.IsEmpty();
}

void UReclaimPausaComponent::AbrirMenuPausa()
{
    ACharacter* Personaje = Cast<ACharacter>(GetOwner());

    if (!IsValid(Personaje) || !Personaje->IsLocallyControlled())
    {
        return;
    }

    APlayerController* Controlador =
        Cast<APlayerController>(Personaje->GetController());

    if (!IsValid(Controlador) ||
        IsValid(MenuPausa) ||
        HayVentanaBloqueante())
    {
        return;
    }

    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return;
    }

    UReclaimMenuPausaWidget* NuevoMenu =
        CreateWidget<UReclaimMenuPausaWidget>(
            Controlador,
            UReclaimMenuPausaWidget::StaticClass()
        );

    if (!IsValid(NuevoMenu))
    {
        return;
    }

    NuevoMenu->AlContinuar.AddUniqueDynamic(
        this,
        &UReclaimPausaComponent::Continuar
    );

    NuevoMenu->AlSalir.AddUniqueDynamic(
        this,
        &UReclaimPausaComponent::Salir
    );
    
    NuevoMenu->AlGuardar.AddUniqueDynamic(
    this,
    &UReclaimPausaComponent::GuardarPartida
);

    NuevoMenu->HabilitarGuardado(true);
    
    if (!NuevoMenu->AddToPlayerScreen(200))
    {
        NuevoMenu->AlContinuar.RemoveDynamic(
            this,
            &UReclaimPausaComponent::Continuar
        );

        NuevoMenu->AlSalir.RemoveDynamic(
            this,
            &UReclaimPausaComponent::Salir
        );

        return;
    }

    MenuPausa = NuevoMenu;
    ControladorMenu = Controlador;
    bCursorEstabaVisible = Controlador->bShowMouseCursor;

    Controlador->bShowMouseCursor = true;

    FInputModeUIOnly ModoEntrada;
    ModoEntrada.SetLockMouseToViewportBehavior(
        EMouseLockMode::DoNotLock
    );

    Controlador->SetInputMode(ModoEntrada);
    
    bJuegoPausadoPorEsteMenu = false;

    if (Mundo->GetNetMode() == NM_Standalone &&
        !UGameplayStatics::IsGamePaused(this))
    {
        bJuegoPausadoPorEsteMenu =
            UGameplayStatics::SetGamePaused(this, true);
    }
}

void UReclaimPausaComponent::Continuar()
{
    if (bJuegoPausadoPorEsteMenu)
    {
        UGameplayStatics::SetGamePaused(this, false);
        bJuegoPausadoPorEsteMenu = false;
    }

    if (IsValid(MenuPausa))
    {
        MenuPausa->AlContinuar.RemoveDynamic(
            this,
            &UReclaimPausaComponent::Continuar
        );

        MenuPausa->AlSalir.RemoveDynamic(
            this,
            &UReclaimPausaComponent::Salir
        );
        MenuPausa->AlGuardar.RemoveDynamic(
            this,
            &UReclaimPausaComponent::GuardarPartida
        );
        
        MenuPausa->RemoveFromParent();
        MenuPausa = nullptr;
    }

    if (APlayerController* Controlador = ControladorMenu.Get())
    {
        Controlador->bShowMouseCursor = bCursorEstabaVisible;

        FInputModeGameOnly ModoEntrada;
        Controlador->SetInputMode(ModoEntrada);
    }

    ControladorMenu.Reset();
}

void UReclaimPausaComponent::Salir()
{
    APlayerController* Controlador = ControladorMenu.Get();
    
    if (bJuegoPausadoPorEsteMenu)
    {
        UGameplayStatics::SetGamePaused(this, false);
        bJuegoPausadoPorEsteMenu = false;
    }

    UKismetSystemLibrary::QuitGame(
        this,
        Controlador,
        EQuitPreference::Quit,
        false
    );
}

void UReclaimPausaComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    Continuar();

    Super::EndPlay(EndPlayReason);
}
void UReclaimPausaComponent::GuardarPartida()
{
    if (!IsValid(MenuPausa))
    {
        return;
    }
    
    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return;
    }

    if (Mundo->GetNetMode() != NM_Standalone)
    {
        MenuPausa->MostrarEstadoGuardado(
            FText::FromString(
                TEXT("Esta opcion no esta disponible en multijugador.")
            )
        );

        return;
    }
    
    AActor* Personaje = GetOwner();

    UReclaimGuardadoJugadorComponent* GuardadoJugador =
        IsValid(Personaje)
            ? Personaje->FindComponentByClass<
                UReclaimGuardadoJugadorComponent
              >()
            : nullptr;

    if (!IsValid(GuardadoJugador))
    {
        MenuPausa->MostrarEstadoGuardado(
            FText::FromString(
                TEXT("No se encontro el componente de guardado.")
            )
        );

        return;
    }

    if (GuardadoJugador->GuardarJugador())
    {
        MenuPausa->MostrarEstadoGuardado(
            FText::FromString(TEXT("Partida guardada."))
        );
    }
    else
    {
        MenuPausa->MostrarEstadoGuardado(
            FText::FromString(TEXT("No se pudo guardar la partida."))
        );
    }
}

