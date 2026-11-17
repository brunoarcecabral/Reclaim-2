#include "Habilidades/ReclaimHabilidadWallRunComponent.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ActorComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Habilidades/ReclaimIconoWallRunWidget.h"
#include "Inventario/ReclaimInventarioColmillo.h"
#include "Net/UnrealNetwork.h"
#include "SistemaNivel/ReclaimNivelComponent.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"


UReclaimHabilidadWallRunComponent::UReclaimHabilidadWallRunComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.2f;

    SetIsReplicatedByDefault(true);
}


void UReclaimHabilidadWallRunComponent::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(
        UReclaimHabilidadWallRunComponent,
        bWallRunDesbloqueado
    );

    DOREPLIFETIME(
        UReclaimHabilidadWallRunComponent,
        EstadoWallRun
    );
}


bool UReclaimHabilidadWallRunComponent::EstaDesbloqueado() const
{
    return bWallRunDesbloqueado;
}


EEstadoHabilidadWallRun
UReclaimHabilidadWallRunComponent::ObtenerEstadoWallRun() const
{
    return EstadoWallRun;
}


void UReclaimHabilidadWallRunComponent::SolicitarConsumirColmillo()
{
    APawn* Personaje = Cast<APawn>(GetOwner());

    if (!IsValid(Personaje) || !Personaje->IsLocallyControlled())
    {
        return;
    }

    ServidorConsumirColmillo();
}


void UReclaimHabilidadWallRunComponent::ServidorConsumirColmillo_Implementation()
{
    APawn* Personaje = Cast<APawn>(GetOwner());

    if (!IsValid(Personaje) || !Personaje->IsPlayerControlled())
    {
        return;
    }

    if (bWallRunDesbloqueado)
    {
        ClienteRecibirResultadoConsumo(
            EResultadoConsumirColmillo::YaDesbloqueado
        );

        return;
    }

    UReclaimNivelComponent* Nivel =
        Personaje->FindComponentByClass<UReclaimNivelComponent>();

    if (!IsValid(Nivel) || Nivel->ObtenerNivel() < 2)
    {
        ClienteRecibirResultadoConsumo(
            EResultadoConsumirColmillo::NivelInsuficiente
        );

        return;
    }

    UActorComponent* Inventario = BuscarInventario();

    if (!IsValid(Inventario))
    {
        ClienteRecibirResultadoConsumo(
            EResultadoConsumirColmillo::InventarioNoEncontrado
        );

        return;
    }

    const bool bQuitado =
        IReclaimInventarioColmillo::Execute_QuitarColmillo(Inventario);

    if (!bQuitado)
    {
        ClienteRecibirResultadoConsumo(
            EResultadoConsumirColmillo::SinColmillo
        );

        return;
    }

    bWallRunDesbloqueado = true;
    
    if (IsValid(MisionAprenderWallRun))
    {
        UWorld* Mundo = GetWorld();

        AGameStateBase* EstadoJuego =
            IsValid(Mundo) ? Mundo->GetGameState() : nullptr;

        UReclaimQuestComponent* Misiones =
            IsValid(EstadoJuego)
                ? EstadoJuego->FindComponentByClass<UReclaimQuestComponent>()
                : nullptr;

        if (IsValid(Misiones))
        {
            Misiones->IniciarMision(MisionAprenderWallRun);
        }
        else
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("WALL RUN: No se encontro el componente de misiones.")
            );
        }
    }
    else
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("WALL RUN: Falta asignar Mision Aprender Wall Run.")
        );
    }
    
    AlCambiarDesbloqueo.Broadcast();

    Personaje->ForceNetUpdate();

    ClienteRecibirResultadoConsumo(
        EResultadoConsumirColmillo::Exito
    );
}


UActorComponent* UReclaimHabilidadWallRunComponent::BuscarInventario() const
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario))
    {
        return nullptr;
    }

    TInlineComponentArray<UActorComponent*> Componentes;
    Propietario->GetComponents(Componentes);

    for (UActorComponent* Componente : Componentes)
    {
        if (
            IsValid(Componente) &&
            Componente->GetClass()->ImplementsInterface(
                UReclaimInventarioColmillo::StaticClass()
            )
        )
        {
            return Componente;
        }
    }

    return nullptr;
}


void UReclaimHabilidadWallRunComponent::OnRep_WallRunDesbloqueado()
{
    AlCambiarDesbloqueo.Broadcast();
}


void UReclaimHabilidadWallRunComponent::ClienteRecibirResultadoConsumo_Implementation(
    EResultadoConsumirColmillo Resultado
)
{
    AlRecibirResultadoConsumo.Broadcast(Resultado);
}


void UReclaimHabilidadWallRunComponent::SolicitarActivarWallRun()
{
    const APawn* Personaje = Cast<APawn>(GetOwner());

    if (!IsValid(Personaje) || !Personaje->IsLocallyControlled())
    {
        return;
    }

    ServidorActivarWallRun();
}


void UReclaimHabilidadWallRunComponent::ServidorActivarWallRun_Implementation()
{
    APawn* Personaje = Cast<APawn>(GetOwner());
    UWorld* Mundo = GetWorld();

    if (
        !IsValid(Personaje) ||
        !Personaje->IsPlayerControlled() ||
        !IsValid(Mundo)
    )
    {
        return;
    }

    if (
        !bWallRunDesbloqueado ||
        EstadoWallRun != EEstadoHabilidadWallRun::Lista
    )
    {
        return;
    }

    EstadoWallRun = EEstadoHabilidadWallRun::Activa;
    Personaje->ForceNetUpdate();

    Mundo->GetTimerManager().SetTimer(
        TimerDuracionWallRun,
        this,
        &UReclaimHabilidadWallRunComponent::IniciarRecargaWallRun,
        FMath::Max(DuracionWallRun, 0.1f),
        false
    );
}


void UReclaimHabilidadWallRunComponent::IniciarRecargaWallRun()
{
    UWorld* Mundo = GetWorld();
    AActor* Propietario = GetOwner();

    if (
        !IsValid(Mundo) ||
        !IsValid(Propietario) ||
        !Propietario->HasAuthority()
    )
    {
        return;
    }

    EstadoWallRun = EEstadoHabilidadWallRun::Recargando;
    Propietario->ForceNetUpdate();

    Mundo->GetTimerManager().SetTimer(
        TimerRecargaWallRun,
        this,
        &UReclaimHabilidadWallRunComponent::FinalizarRecargaWallRun,
        FMath::Max(TiempoRecargaWallRun, 0.1f),
        false
    );
}


void UReclaimHabilidadWallRunComponent::FinalizarRecargaWallRun()
{
    AActor* Propietario = GetOwner();

    if (!IsValid(Propietario) || !Propietario->HasAuthority())
    {
        return;
    }

    EstadoWallRun = EEstadoHabilidadWallRun::Lista;
    Propietario->ForceNetUpdate();
}


void UReclaimHabilidadWallRunComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const APawn* Personaje = Cast<APawn>(GetOwner());

    if (!IsValid(Personaje) || !Personaje->IsLocallyControlled())
    {
        return;
    }

    if (bWallRunDesbloqueado && !IsValid(WidgetIconoWallRun))
    {
        CrearIconoWallRun();
    }

    if (bWallRunDesbloqueado && IsValid(WidgetIconoWallRun))
    {
        ActualizarColorIconoWallRun();
    }
}


void UReclaimHabilidadWallRunComponent::CrearIconoWallRun()
{
    APawn* Personaje = Cast<APawn>(GetOwner());

    if (!IsValid(Personaje) || !IsValid(IconoWallRun))
    {
        return;
    }

    APlayerController* Controlador =
        Cast<APlayerController>(Personaje->GetController());

    if (!IsValid(Controlador) || !Controlador->IsLocalController())
    {
        return;
    }

    UReclaimIconoWallRunWidget* NuevoWidget =
        CreateWidget<UReclaimIconoWallRunWidget>(
            Controlador,
            UReclaimIconoWallRunWidget::StaticClass()
        );

    if (!IsValid(NuevoWidget) || !IsValid(NuevoWidget->WidgetTree))
    {
        return;
    }

    UCanvasPanel* Panel =
        NuevoWidget->WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("PanelWallRun")
        );

    UImage* Imagen =
        NuevoWidget->WidgetTree->ConstructWidget<UImage>(
            UImage::StaticClass(),
            TEXT("ImagenWallRun")
        );

    UTextBlock* TextoTecla =
        NuevoWidget->WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoTeclaWallRun")
        );

    if (!IsValid(Panel) || !IsValid(Imagen) || !IsValid(TextoTecla))
    {
        return;
    }

    NuevoWidget->WidgetTree->RootWidget = Panel;
    
    Imagen->SetBrushFromTexture(IconoWallRun);

    UCanvasPanelSlot* EspacioImagen = Panel->AddChildToCanvas(Imagen);

    if (!IsValid(EspacioImagen))
    {
        return;
    }

    EspacioImagen->SetAnchors(FAnchors(0.0f, 1.0f));
    EspacioImagen->SetAlignment(FVector2D(0.0f, 1.0f));
    EspacioImagen->SetPosition(FVector2D(30.0f, -30.0f));
    EspacioImagen->SetSize(FVector2D(80.0f, 80.0f));
    
    TextoTecla->SetText(FText::FromString(TEXT("1")));
    TextoTecla->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    TextoTecla->SetShadowColorAndOpacity(
        FLinearColor(0.0f, 0.0f, 0.0f, 1.0f)
    );
    TextoTecla->SetShadowOffset(FVector2D(1.0f, 1.0f));

    FSlateFontInfo Fuente = TextoTecla->GetFont();
    Fuente.Size = 18;
    TextoTecla->SetFont(Fuente);

    UCanvasPanelSlot* EspacioTexto = Panel->AddChildToCanvas(TextoTecla);

    if (!IsValid(EspacioTexto))
    {
        return;
    }
    
    EspacioTexto->SetAnchors(FAnchors(0.0f, 1.0f));
    EspacioTexto->SetAlignment(FVector2D(0.0f, 1.0f));
    EspacioTexto->SetPosition(FVector2D(35.0f, -83.0f));
    EspacioTexto->SetSize(FVector2D(25.0f, 25.0f));
    
    NuevoWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

    WidgetIconoWallRun = NuevoWidget;
    WidgetIconoWallRun->AddToViewport();
}




void UReclaimHabilidadWallRunComponent::ActualizarColorIconoWallRun()
{
    if (
        !IsValid(WidgetIconoWallRun) ||
        !IsValid(WidgetIconoWallRun->WidgetTree)
    )
    {
        return;
    }

    UImage* Imagen = Cast<UImage>(
        WidgetIconoWallRun->WidgetTree->FindWidget(
            FName(TEXT("ImagenWallRun"))
        )
    );

    if (!IsValid(Imagen))
    {
        return;
    }

    switch (EstadoWallRun)
    {
    case EEstadoHabilidadWallRun::Lista:
        Imagen->SetColorAndOpacity(FLinearColor::White);
        break;

    case EEstadoHabilidadWallRun::Activa:
        Imagen->SetColorAndOpacity(
            FLinearColor(1.0f, 1.0f, 0.35f, 1.0f)
        );
        break;

    case EEstadoHabilidadWallRun::Recargando:
        Imagen->SetColorAndOpacity(
            FLinearColor(0.35f, 0.35f, 0.35f, 0.8f)
        );
        break;
    }
}


void UReclaimHabilidadWallRunComponent::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerDuracionWallRun);
        Mundo->GetTimerManager().ClearTimer(TimerRecargaWallRun);
    }

    if (IsValid(WidgetIconoWallRun))
    {
        WidgetIconoWallRun->RemoveFromParent();
        WidgetIconoWallRun = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

