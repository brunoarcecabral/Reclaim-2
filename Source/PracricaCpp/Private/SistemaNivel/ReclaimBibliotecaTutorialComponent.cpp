#include "SistemaNivel/ReclaimBibliotecaTutorialComponent.h"

#include "Components/InputComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "SistemaNivel/ReclaimAvisoInicioSubsystem.h"
#include "SistemaQuest/ReclaimColmilloComponent.h"
#include "SistemaQuest/ReclaimNotaQuestData.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"
#include "TimerManager.h"
#include "UI/ReclaimBibliotecaTutorialWidget.h"
#include "UI/ReclaimIndicadorTutorialWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SistemaQuest/ReclaimNotaQuestWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SistemaQuest/ReclaimNotaQuestWidget.h"
#include "UI/ReclaimBibliotecaNotasWidget.h"


UReclaimBibliotecaTutorialComponent::UReclaimBibliotecaTutorialComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}


void UReclaimBibliotecaTutorialComponent::BeginPlay()
{
    Super::BeginPlay();

    UWorld* Mundo = GetWorld();

    if (!Mundo || Mundo->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }
    
    Mundo->GetTimerManager().SetTimer(
        TimerInicializacion,
        this,
        &UReclaimBibliotecaTutorialComponent::IntentarInicializar,
        0.5f,
        true
    );

    IntentarInicializar();
}


void UReclaimBibliotecaTutorialComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerInicializacion);
    }

    CerrarBiblioteca();

    if (IsValid(Indicador))
    {
        Indicador->RemoveFromParent();
        Indicador = nullptr;
    }

    if (IsValid(ControladorLocal) && IsValid(EntradaTutorial))
    {
        ControladorLocal->PopInputComponent(EntradaTutorial);
        EntradaTutorial->DestroyComponent();
    }

    EntradaTutorial = nullptr;
    ControladorLocal = nullptr;

    Super::EndPlay(EndPlayReason);
}


void UReclaimBibliotecaTutorialComponent::IntentarInicializar()
{
    UWorld* Mundo = GetWorld();
    APawn* Personaje = Cast<APawn>(GetOwner());

    if (!Mundo ||
        !IsValid(Personaje) ||
        !Personaje->IsLocallyControlled())
    {
        return;
    }

    APlayerController* Controlador =
        Cast<APlayerController>(Personaje->GetController());

    if (!IsValid(Controlador) || !Controlador->IsLocalController())
    {
        return;
    }

    ControladorLocal = Controlador;
    
    if (!IsValid(EntradaTutorial))
    {
        EntradaTutorial = NewObject<UInputComponent>(
            Controlador,
            TEXT("EntradaBibliotecaTutorial")
        );

        if (IsValid(EntradaTutorial))
        {
            EntradaTutorial->RegisterComponent();

            FInputKeyBinding& Enlace = EntradaTutorial->BindKey(
                EKeys::T,
                IE_Pressed,
                this,
                &UReclaimBibliotecaTutorialComponent::AlternarBiblioteca
            );

            Enlace.bConsumeInput = true;
            
            FInputKeyBinding& EnlaceNotas = EntradaTutorial->BindKey(
    EKeys::N,
    IE_Pressed,
    this,
    &UReclaimBibliotecaTutorialComponent::AlternarNotas
);

            EnlaceNotas.bConsumeInput = true;
            
            Controlador->PushInputComponent(EntradaTutorial);
        }
    }
    
    if (!IsValid(Indicador))
    {
        UReclaimIndicadorTutorialWidget* NuevoIndicador =
            CreateWidget<UReclaimIndicadorTutorialWidget>(
                Controlador,
                UReclaimIndicadorTutorialWidget::StaticClass()
            );

        if (IsValid(NuevoIndicador))
        {
            NuevoIndicador->SetVisibility(
                ESlateVisibility::HitTestInvisible
            );

            if (NuevoIndicador->AddToPlayerScreen())
            {
                Indicador = NuevoIndicador;
            }
        }
    }
    
    if (IsValid(EntradaTutorial) && IsValid(Indicador))
    {
        if (ULocalPlayer* JugadorLocal = Controlador->GetLocalPlayer())
        {
            UReclaimAvisoInicioSubsystem* Aviso =
                JugadorLocal->GetSubsystem<UReclaimAvisoInicioSubsystem>();

            if (IsValid(Aviso))
            {
                Aviso->ProgramarAviso(Controlador);
            }
            else
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("TUTORIALES: No se encontro el subsistema del aviso inicial.")
                );
            }
        }

        Mundo->GetTimerManager().ClearTimer(TimerInicializacion);
    }
}

bool UReclaimBibliotecaTutorialComponent::HayNotaEmergenteAbierta() const
{
    if (!IsValid(ControladorLocal))
    {
        return false;
    }

    TArray<UUserWidget*> VentanasDeNota;

    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
        this,
        VentanasDeNota,
        UReclaimNotaQuestWidget::StaticClass(),
        true
    );

    for (UUserWidget* Ventana : VentanasDeNota)
    {
        if (IsValid(Ventana) &&
            Ventana->IsInViewport() &&
            Ventana->GetOwningPlayer() == ControladorLocal)
        {
            return true;
        }
    }

    return false;
}

void UReclaimBibliotecaTutorialComponent::AlternarNotas()
{
    if (IsValid(VentanaNotas))
    {
        CerrarNotas();
        return;
    }
    
    if (IsValid(VentanaBiblioteca) || HayNotaEmergenteAbierta())
    {
        return;
    }

    AbrirNotas();
}

void UReclaimBibliotecaTutorialComponent::AbrirNotas()
{
    if (!IsValid(ControladorLocal) ||
        !ControladorLocal->IsLocalController() ||
        IsValid(VentanaNotas))
    {
        return;
    }

    UReclaimBibliotecaNotasWidget* NuevaVentana =
        CreateWidget<UReclaimBibliotecaNotasWidget>(
            ControladorLocal,
            UReclaimBibliotecaNotasWidget::StaticClass()
        );

    if (!IsValid(NuevaVentana))
    {
        return;
    }

    NuevaVentana->ActualizarLista();

    NuevaVentana->AlCerrarBiblioteca.AddDynamic(
        this,
        &UReclaimBibliotecaTutorialComponent::CerrarNotas
    );

    if (!NuevaVentana->AddToPlayerScreen(120))
    {
        NuevaVentana->AlCerrarBiblioteca.RemoveDynamic(
            this,
            &UReclaimBibliotecaTutorialComponent::CerrarNotas
        );

        return;
    }

    VentanaNotas = NuevaVentana;

    bCursorAntesDeNotas = ControladorLocal->bShowMouseCursor;
    ControladorLocal->bShowMouseCursor = true;

    FInputModeGameAndUI ModoEntrada;
    ModoEntrada.SetWidgetToFocus(VentanaNotas->TakeWidget());

    ControladorLocal->SetInputMode(ModoEntrada);
    VentanaNotas->SetUserFocus(ControladorLocal);
}

void UReclaimBibliotecaTutorialComponent::CerrarNotas()
{
    if (!IsValid(VentanaNotas))
    {
        VentanaNotas = nullptr;
        return;
    }

    VentanaNotas->AlCerrarBiblioteca.RemoveDynamic(
        this,
        &UReclaimBibliotecaTutorialComponent::CerrarNotas
    );

    VentanaNotas->RemoveFromParent();
    VentanaNotas = nullptr;

    if (!IsValid(ControladorLocal))
    {
        return;
    }

    ControladorLocal->bShowMouseCursor = bCursorAntesDeNotas;

    if (bCursorAntesDeNotas)
    {
        FInputModeGameAndUI ModoEntrada;
        ControladorLocal->SetInputMode(ModoEntrada);
    }
    else
    {
        FInputModeGameOnly ModoEntrada;
        ControladorLocal->SetInputMode(ModoEntrada);
    }
}

void UReclaimBibliotecaTutorialComponent::AlternarBiblioteca()
{
    if (IsValid(VentanaBiblioteca))
    {
        CerrarBiblioteca();
        CerrarNotas();
        return;
    }
    
    if (IsValid(VentanaNotas) || HayNotaEmergenteAbierta())
    {
        return;
    }

    AbrirBiblioteca();
}

void UReclaimBibliotecaTutorialComponent::AbrirBiblioteca()
{
    if (!IsValid(ControladorLocal) ||
        !ControladorLocal->IsLocalController() ||
        IsValid(VentanaBiblioteca))
    {
        return;
    }

    UReclaimBibliotecaTutorialWidget* NuevaVentana =
        CreateWidget<UReclaimBibliotecaTutorialWidget>(
            ControladorLocal,
            UReclaimBibliotecaTutorialWidget::StaticClass()
        );

    if (!IsValid(NuevaVentana))
    {
        return;
    }

    NuevaVentana->Configurar(this);

    NuevaVentana->AlCerrarBiblioteca.AddDynamic(
        this,
        &UReclaimBibliotecaTutorialComponent::CerrarBiblioteca
    );

    if (!NuevaVentana->AddToPlayerScreen(120))
    {
        NuevaVentana->AlCerrarBiblioteca.RemoveDynamic(
            this,
            &UReclaimBibliotecaTutorialComponent::CerrarBiblioteca
        );

        return;
    }

    VentanaBiblioteca = NuevaVentana;

    bCursorEstabaVisible = ControladorLocal->bShowMouseCursor;

    ControladorLocal->bShowMouseCursor = true;

    FInputModeGameAndUI ModoEntrada;
    ModoEntrada.SetWidgetToFocus(VentanaBiblioteca->TakeWidget());

    ControladorLocal->SetInputMode(ModoEntrada);
    VentanaBiblioteca->SetUserFocus(ControladorLocal);
}


void UReclaimBibliotecaTutorialComponent::CerrarBiblioteca()
{
    if (!IsValid(VentanaBiblioteca))
    {
        VentanaBiblioteca = nullptr;
        return;
    }

    VentanaBiblioteca->AlCerrarBiblioteca.RemoveDynamic(
        this,
        &UReclaimBibliotecaTutorialComponent::CerrarBiblioteca
    );

    VentanaBiblioteca->RemoveFromParent();
    VentanaBiblioteca = nullptr;

    if (!IsValid(ControladorLocal))
    {
        return;
    }

    ControladorLocal->bShowMouseCursor = bCursorEstabaVisible;

    if (bCursorEstabaVisible)
    {
        FInputModeGameAndUI ModoEntrada;
        ControladorLocal->SetInputMode(ModoEntrada);
    }
    else
    {
        FInputModeGameOnly ModoEntrada;
        ControladorLocal->SetInputMode(ModoEntrada);
    }
}


TArray<UReclaimNotaQuestData*>
UReclaimBibliotecaTutorialComponent::ObtenerTutorialesDisponibles() const
{
    TArray<UReclaimNotaQuestData*> Tutoriales;
    
    if (IsValid(TutorialControles))
    {
        Tutoriales.Add(TutorialControles);
    }

    const UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return Tutoriales;
    }

    AGameStateBase* EstadoJuego = Mundo->GetGameState();

    if (!IsValid(EstadoJuego))
    {
        return Tutoriales;
    }

    UReclaimQuestComponent* ComponenteQuest =
        EstadoJuego->FindComponentByClass<UReclaimQuestComponent>();

    if (IsValid(ComponenteQuest) && IsValid(TutorialNivel))
    {
        const TArray<FEstadoQuest> Misiones =
            ComponenteQuest->ObtenerMisiones();

        for (const FEstadoQuest& Estado : Misiones)
        {
            if (IsValid(Estado.DatosMision) &&
                Estado.DatosMision->IdMision ==
                    FName(TEXT("SubirNivelDos")))
            {
                Tutoriales.Add(TutorialNivel);
                break;
            }
        }
    }

    UReclaimColmilloComponent* ComponenteColmillo =
        EstadoJuego->FindComponentByClass<UReclaimColmilloComponent>();

    if (IsValid(ComponenteColmillo) &&
        ComponenteColmillo->YaFueComprado() &&
        IsValid(TutorialWallRun))
    {
        Tutoriales.Add(TutorialWallRun);
    }

    return Tutoriales;
}

