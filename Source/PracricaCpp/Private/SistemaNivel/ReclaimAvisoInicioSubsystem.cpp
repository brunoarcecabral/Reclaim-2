#include "SistemaNivel/ReclaimAvisoInicioSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "SistemaQuest/ReclaimNotaQuestData.h"
#include "SistemaQuest/ReclaimNotaQuestWidget.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"
#include "SistemaGuardado/ReclaimGuardadoSubsystem.h"


void UReclaimAvisoInicioSubsystem::Deinitialize()
{
    if (APlayerController* Controlador = ControladorAviso.Get())
    {
        if (UWorld* Mundo = Controlador->GetWorld())
        {
            Mundo->GetTimerManager().ClearTimer(TimerAviso);
        }
    }

    CerrarAviso();

    Super::Deinitialize();
}


void UReclaimAvisoInicioSubsystem::ProgramarAviso(
    APlayerController* Controlador
)
{
    if (!IsValid(Controlador) ||
        !Controlador->IsLocalController() ||
        bAvisoProgramado ||
        bAvisoMostrado)
    {
        return;
    }

    UWorld* Mundo = Controlador->GetWorld();

    if (!Mundo)
    {
        return;
    }
    UGameInstance* Instancia = Controlador->GetGameInstance();

    if (IsValid(Instancia))
    {
        UReclaimGuardadoSubsystem* SistemaGuardado =
            Instancia->GetSubsystem<UReclaimGuardadoSubsystem>();

        if (IsValid(SistemaGuardado) &&
            SistemaGuardado->ConsumirOmisionAvisoInicial())
        {
            return;
        }
    }
    
    ControladorAviso = Controlador;
    bAvisoProgramado = true;

    Mundo->GetTimerManager().SetTimer(
        TimerAviso,
        this,
        &UReclaimAvisoInicioSubsystem::MostrarAviso,
        1.0f,
        false
    );
}


void UReclaimAvisoInicioSubsystem::MostrarAviso()
{
    bAvisoProgramado = false;

    APlayerController* Controlador = ControladorAviso.Get();

    if (!IsValid(Controlador) ||
        !Controlador->IsLocalController() ||
        bAvisoMostrado)
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
    
    UReclaimNotaQuestData* DatosAviso =
        NewObject<UReclaimNotaQuestData>(this);

    if (!IsValid(DatosAviso))
    {
        return;
    }

    DatosAviso->Titulo =
        FText::FromString(TEXT("Base de datos de tutoriales"));

    DatosAviso->Contenido =
        FText::FromString(
            TEXT("Tienes disponible una base de datos de tutoriales.\n\n")
            TEXT("Pulsa T para abrirla y volver a pulsar T para cerrarla.")
            TEXT(" A medida que avances, encontraras nuevos tutoriales.")
        );

    NuevaVentana->MostrarNota(DatosAviso);

    NuevaVentana->AlContinuarNota.AddDynamic(
        this,
        &UReclaimAvisoInicioSubsystem::CerrarAviso
    );

    if (!NuevaVentana->AddToPlayerScreen(130))
    {
        NuevaVentana->AlContinuarNota.RemoveDynamic(
            this,
            &UReclaimAvisoInicioSubsystem::CerrarAviso
        );

        return;
    }

    VentanaAviso = NuevaVentana;
    bAvisoMostrado = true;

    bCursorEstabaVisible = Controlador->bShowMouseCursor;

    FInputModeUIOnly ModoEntrada;
    ModoEntrada.SetWidgetToFocus(VentanaAviso->TakeWidget());

    Controlador->SetInputMode(ModoEntrada);
    VentanaAviso->SetUserFocus(Controlador);
}


void UReclaimAvisoInicioSubsystem::CerrarAviso()
{
    if (!IsValid(VentanaAviso))
    {
        VentanaAviso = nullptr;
        return;
    }

    VentanaAviso->AlContinuarNota.RemoveDynamic(
        this,
        &UReclaimAvisoInicioSubsystem::CerrarAviso
    );

    VentanaAviso->RemoveFromParent();
    VentanaAviso = nullptr;

    APlayerController* Controlador = ControladorAviso.Get();

    if (!IsValid(Controlador) || !Controlador->IsLocalController())
    {
        return;
    }

    Controlador->bShowMouseCursor = bCursorEstabaVisible;

    if (bCursorEstabaVisible)
    {
        FInputModeGameAndUI ModoEntrada;
        Controlador->SetInputMode(ModoEntrada);
    }
    else
    {
        FInputModeGameOnly ModoEntrada;
        Controlador->SetInputMode(ModoEntrada);
    }
}

