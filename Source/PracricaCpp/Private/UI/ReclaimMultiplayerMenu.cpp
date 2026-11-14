#include "UI/ReclaimMultiplayerMenu.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OnlineSessionSettings.h"
#include "Components/ButtonSlot.h"

#include "ReclaimSessionSubsystem.h"

void UReclaimMultiplayerMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree || WidgetTree->RootWidget)
    {
        return;
    }

    // Panel principal que ocupa toda la pantalla.
    UCanvasPanel* PanelPrincipal =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("PanelPrincipal")
        );

    WidgetTree->RootWidget = PanelPrincipal;

    // Imagen de fondo.
    UImage* ImagenFondo =
        WidgetTree->ConstructWidget<UImage>(
            UImage::StaticClass(),
            TEXT("ImagenFondo")
        );

    UTexture2D* TexturaFondo = LoadObject<UTexture2D>(
        nullptr,
        TEXT("/Game/UI/Images/Menu_principal.Menu_principal")
    );

    if (TexturaFondo)
    {
        ImagenFondo->SetBrushFromTexture(TexturaFondo);
    }

    // La imagen es decorativa: no debe bloquear los clics.
    ImagenFondo->SetVisibility(ESlateVisibility::HitTestInvisible);

    UCanvasPanelSlot* SlotFondo =
        PanelPrincipal->AddChildToCanvas(ImagenFondo);

    SlotFondo->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
    SlotFondo->SetOffsets(FMargin(0.0f));

    // Contenedor de botones, anclado abajo a la izquierda.
    UVerticalBox* Contenedor =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass(),
            TEXT("ContenedorMenu")
        );

    UCanvasPanelSlot* SlotMenu =
        PanelPrincipal->AddChildToCanvas(Contenedor);

    SlotMenu->SetAnchors(FAnchors(0.0f, 1.0f));
    SlotMenu->SetAlignment(FVector2D(0.0f, 1.0f));
    SlotMenu->SetPosition(FVector2D(70.0f, -55.0f));
    SlotMenu->SetSize(FVector2D(300.0f, 420.0f));

    // Función local para crear botones del mismo tamaño.
    auto CrearBoton = [this, Contenedor](
        const FName NombreBoton,
        const FName NombreTexto,
        const FString& Etiqueta
    ) -> UButton*
    {
        USizeBox* Caja =
            WidgetTree->ConstructWidget<USizeBox>();

        Caja->SetWidthOverride(300.0f);
        Caja->SetHeightOverride(44.0f);

        UButton* Boton =
            WidgetTree->ConstructWidget<UButton>(
                UButton::StaticClass(),
                NombreBoton
            );

        UTextBlock* Texto =
            WidgetTree->ConstructWidget<UTextBlock>(
                UTextBlock::StaticClass(),
                NombreTexto
            );

        Texto->SetText(FText::FromString(Etiqueta));
        Texto->SetJustification(ETextJustify::Center);

        FSlateFontInfo Fuente = Texto->GetFont();
        Fuente.Size = 18;
        Texto->SetFont(Fuente);

        UButtonSlot* SlotTexto = Cast<UButtonSlot>(Boton->AddChild(Texto));

        if (SlotTexto)
        {
            SlotTexto->SetHorizontalAlignment(HAlign_Center);
            SlotTexto->SetVerticalAlignment(VAlign_Center);
        }


        Caja->AddChild(Boton);

        UVerticalBoxSlot* Slot =
            Contenedor->AddChildToVerticalBox(Caja);

        Slot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

        return Boton;
    };

    BotonCrearPartida = CrearBoton(
        TEXT("BotonCrearPartida"),
        TEXT("TextoCrear"),
        TEXT("Crear partida")
    );

    BotonBuscarPartidas = CrearBoton(
        TEXT("BotonBuscarPartidas"),
        TEXT("TextoBuscar"),
        TEXT("Buscar partidas")
    );

    TextoEstado =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoEstado")
        );

    TextoEstado->SetText(FText::GetEmpty());
    TextoEstado->SetColorAndOpacity(
        FSlateColor(FLinearColor::White)
    );

    UVerticalBoxSlot* SlotEstado =
        Contenedor->AddChildToVerticalBox(TextoEstado);

    SlotEstado->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));

    // Área para mostrar las partidas encontradas.
    USizeBox* CajaLista =
        WidgetTree->ConstructWidget<USizeBox>();

    CajaLista->SetWidthOverride(300.0f);
    CajaLista->SetHeightOverride(55.0f);

    ListaPartidas =
        WidgetTree->ConstructWidget<UScrollBox>(
            UScrollBox::StaticClass(),
            TEXT("ListaPartidas")
        );

    CajaLista->AddChild(ListaPartidas);

    UVerticalBoxSlot* SlotLista =
        Contenedor->AddChildToVerticalBox(CajaLista);

    SlotLista->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));

    // Campo para escribir el número de la partida.
    USizeBox* CajaEntrada =
        WidgetTree->ConstructWidget<USizeBox>();

    CajaEntrada->SetWidthOverride(300.0f);
    CajaEntrada->SetHeightOverride(40.0f);

    EntradaNumeroPartida =
        WidgetTree->ConstructWidget<UEditableTextBox>(
            UEditableTextBox::StaticClass(),
            TEXT("EntradaNumeroPartida")
        );

    EntradaNumeroPartida->SetHintText(
        FText::FromString(TEXT("Número de partida"))
    );

    EntradaNumeroPartida->SetForegroundColor(FLinearColor::Black);

    FEditableTextBoxStyle EstiloEntrada =
        EntradaNumeroPartida->GetWidgetStyle();

    EstiloEntrada.SetBackgroundColor(FSlateColor(FLinearColor::White));
    EntradaNumeroPartida->SetWidgetStyle(EstiloEntrada);


    CajaEntrada->AddChild(EntradaNumeroPartida);

    UVerticalBoxSlot* SlotEntrada =
        Contenedor->AddChildToVerticalBox(CajaEntrada);

    SlotEntrada->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

    BotonUnirse = CrearBoton(
        TEXT("BotonUnirse"),
        TEXT("TextoUnirse"),
        TEXT("Unirse")
    );

    BotonSalir = CrearBoton(
        TEXT("BotonSalir"),
        TEXT("TextoSalir"),
        TEXT("Salir")
    );

    BotonCrearPartida->OnClicked.AddUniqueDynamic(
        this,
        &UReclaimMultiplayerMenu::CrearPartida
    );

    BotonBuscarPartidas->OnClicked.AddUniqueDynamic(
        this,
        &UReclaimMultiplayerMenu::BuscarPartidas
    );

    BotonUnirse->OnClicked.AddUniqueDynamic(
        this,
        &UReclaimMultiplayerMenu::UnirseAPartida
    );

    BotonSalir->OnClicked.AddUniqueDynamic(
        this,
        &UReclaimMultiplayerMenu::SalirDelJuego
    );

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UReclaimSessionSubsystem* SessionSubsystem =
            GameInstance->GetSubsystem<UReclaimSessionSubsystem>())
        {
            SessionSubsystem->OnSessionCreated.AddUniqueDynamic(
                this,
                &UReclaimMultiplayerMenu::AlCrearPartida
            );

            SessionSubsystem->OnSessionsFound.AddUniqueDynamic(
                this,
                &UReclaimMultiplayerMenu::AlEncontrarPartidas
            );

            SessionSubsystem->OnSessionJoined.AddUniqueDynamic(
                this,
                &UReclaimMultiplayerMenu::AlUnirseAPartida
            );
        }
    }
}

void UReclaimMultiplayerMenu::NativeDestruct()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UReclaimSessionSubsystem* SessionSubsystem =
            GameInstance->GetSubsystem<UReclaimSessionSubsystem>())
        {
            SessionSubsystem->OnSessionCreated.RemoveDynamic(
                this,
                &UReclaimMultiplayerMenu::AlCrearPartida
            );

            SessionSubsystem->OnSessionsFound.RemoveDynamic(
                this,
                &UReclaimMultiplayerMenu::AlEncontrarPartidas
            );

            SessionSubsystem->OnSessionJoined.RemoveDynamic(
                this,
                &UReclaimMultiplayerMenu::AlUnirseAPartida
            );
        }
    }

    Super::NativeDestruct();
}

void UReclaimMultiplayerMenu::CrearPartida()
{
    UGameInstance* GameInstance = GetGameInstance();

    if (!GameInstance)
    {
        if (TextoEstado)
        {
            TextoEstado->SetText(
                FText::FromString(TEXT("No se pudo acceder al juego."))
            );
        }

        return;
    }

    UReclaimSessionSubsystem* SessionSubsystem =
        GameInstance->GetSubsystem<UReclaimSessionSubsystem>();

    if (!SessionSubsystem)
    {
        if (TextoEstado)
        {
            TextoEstado->SetText(
                FText::FromString(
                    TEXT("No se encontró el sistema de sesiones.")
                )
            );
        }

        return;
    }

    if (TextoEstado)
    {
        TextoEstado->SetText(
            FText::FromString(TEXT("Creando partida..."))
        );
    }

    SessionSubsystem->HostSession(
        TEXT("/Game/ThirdPerson/Lvl_ThirdPerson"),
        4
    );
}

void UReclaimMultiplayerMenu::AlCrearPartida(bool bWasSuccessful)
{
    if (!bWasSuccessful && TextoEstado)
    {
        TextoEstado->SetText(
            FText::FromString(TEXT("No se pudo crear la partida."))
        );
    }
}

void UReclaimMultiplayerMenu::BuscarPartidas()
{
    UGameInstance* GameInstance = GetGameInstance();

    if (!GameInstance)
    {
        if (TextoEstado)
        {
            TextoEstado->SetText(
                FText::FromString(TEXT("No se pudo acceder al juego."))
            );
        }

        return;
    }

    UReclaimSessionSubsystem* SessionSubsystem =
        GameInstance->GetSubsystem<UReclaimSessionSubsystem>();

    if (!SessionSubsystem)
    {
        if (TextoEstado)
        {
            TextoEstado->SetText(
                FText::FromString(
                    TEXT("No se encontró el sistema de sesiones.")
                )
            );
        }

        return;
    }

    PartidasEncontradas.Empty();

    if (ListaPartidas)
    {
        ListaPartidas->ClearChildren();
    }

    if (TextoEstado)
    {
        TextoEstado->SetText(
            FText::FromString(TEXT("Buscando partidas..."))
        );
    }

    SessionSubsystem->FindSessions(100);
}

void UReclaimMultiplayerMenu::AlEncontrarPartidas(
    const TArray<FBlueprintSessionResult>& SessionResults,
    bool bWasSuccessful
)
{
    if (!bWasSuccessful)
    {
        if (TextoEstado)
        {
            TextoEstado->SetText(
                FText::FromString(TEXT("No se pudieron buscar partidas."))
            );
        }

        return;
    }

    PartidasEncontradas.Empty();

    if (ListaPartidas)
    {
        ListaPartidas->ClearChildren();
    }

    for (const FBlueprintSessionResult& Resultado : SessionResults)
    {
        FString Identificador;

        const bool bTieneIdentificador =
            Resultado.OnlineResult.Session.SessionSettings.Get(
                FName("SessionName"),
                Identificador
            );

        if (!bTieneIdentificador || Identificador != TEXT("ReclaimViejo"))
        {
            continue;
        }

        PartidasEncontradas.Add(Resultado);

        if (ListaPartidas && WidgetTree)
        {
            UTextBlock* TextoPartida =
                WidgetTree->ConstructWidget<UTextBlock>();

            const int32 Numero = PartidasEncontradas.Num();

            FString NombreAnfitrion =
                Resultado.OnlineResult.Session.OwningUserName;

            if (NombreAnfitrion.IsEmpty())
            {
                NombreAnfitrion = TEXT("Anfitrión sin nombre");
            }

            TextoPartida->SetText(
                FText::FromString(
                    FString::Printf(
                        TEXT("%d - %s"),
                        Numero,
                        *NombreAnfitrion
                    )
                )
            );

            TextoPartida->SetColorAndOpacity(
                FSlateColor(FLinearColor::White)
            );

            ListaPartidas->AddChild(TextoPartida);
        }
    }

    if (TextoEstado)
    {
        if (PartidasEncontradas.IsEmpty())
        {
            TextoEstado->SetText(
                FText::FromString(
                    TEXT("No se encontraron partidas de Reclaim.")
                )
            );
        }
        else
        {
            TextoEstado->SetText(
                FText::FromString(
                    FString::Printf(
                        TEXT("Partidas encontradas: %d"),
                        PartidasEncontradas.Num()
                    )
                )
            );
        }
    }
}

void UReclaimMultiplayerMenu::UnirseAPartida()
{
    if (!EntradaNumeroPartida)
    {
        return;
    }

    const FString TextoNumero =
        EntradaNumeroPartida->GetText().ToString().TrimStartAndEnd();

    if (TextoNumero.IsEmpty() || !TextoNumero.IsNumeric())
    {
        if (TextoEstado)
        {
            TextoEstado->SetText(
                FText::FromString(
                    TEXT("Escribí un número de partida válido.")
                )
            );
        }

        return;
    }

    const int32 Indice = FCString::Atoi(*TextoNumero) - 1;

    if (!PartidasEncontradas.IsValidIndex(Indice))
    {
        if (TextoEstado)
        {
            TextoEstado->SetText(
                FText::FromString(TEXT("Ese número no está en la lista."))
            );
        }

        return;
    }

    UGameInstance* GameInstance = GetGameInstance();

    if (!GameInstance)
    {
        return;
    }

    UReclaimSessionSubsystem* SessionSubsystem =
        GameInstance->GetSubsystem<UReclaimSessionSubsystem>();

    if (!SessionSubsystem)
    {
        if (TextoEstado)
        {
            TextoEstado->SetText(
                FText::FromString(
                    TEXT("No se encontró el sistema de sesiones.")
                )
            );
        }

        return;
    }

    if (TextoEstado)
    {
        TextoEstado->SetText(
            FText::FromString(TEXT("Uniéndose a la partida..."))
        );
    }

    SessionSubsystem->JoinSession(PartidasEncontradas[Indice]);
}

void UReclaimMultiplayerMenu::AlUnirseAPartida(bool bWasSuccessful)
{
    if (!bWasSuccessful && TextoEstado)
    {
        TextoEstado->SetText(
            FText::FromString(TEXT("No se pudo unir a la partida."))
        );
    }
}

void UReclaimMultiplayerMenu::SalirDelJuego()
{
    UKismetSystemLibrary::QuitGame(
        this,
        GetOwningPlayer(),
        EQuitPreference::Quit,
        false
    );
}

