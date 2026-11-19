#include "UI/ReclaimMenuPausaWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UReclaimMenuPausaWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree)
    {
        return;
    }
    
    UCanvasPanel* PanelPrincipal =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("PanelPrincipal")
        );

    if (!PanelPrincipal)
    {
        return;
    }

    WidgetTree->RootWidget = PanelPrincipal;
    
    UBorder* Fondo =
        WidgetTree->ConstructWidget<UBorder>(
            UBorder::StaticClass(),
            TEXT("FondoPausa")
        );

    Fondo->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.65f));

    UCanvasPanelSlot* SlotFondo =
        PanelPrincipal->AddChildToCanvas(Fondo);

    SlotFondo->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
    SlotFondo->SetOffsets(FMargin(0.0f));
    
    UVerticalBox* Contenedor =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass(),
            TEXT("ContenedorPausa")
        );

    UCanvasPanelSlot* SlotContenedor =
        PanelPrincipal->AddChildToCanvas(Contenedor);

    SlotContenedor->SetAnchors(FAnchors(0.5f, 0.5f));
    SlotContenedor->SetAlignment(FVector2D(0.5f, 0.5f));
    SlotContenedor->SetPosition(FVector2D(0.0f, 0.0f));
    SlotContenedor->SetAutoSize(true);

    UTextBlock* Titulo =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TituloPausa")
        );

    Titulo->SetText(FText::FromString(TEXT("PAUSA")));
    Titulo->SetJustification(ETextJustify::Center);
    Titulo->SetColorAndOpacity(FSlateColor(FLinearColor::White));

    FSlateFontInfo FuenteTitulo = Titulo->GetFont();
    FuenteTitulo.Size = 32;
    Titulo->SetFont(FuenteTitulo);

    UVerticalBoxSlot* SlotTitulo =
        Contenedor->AddChildToVerticalBox(Titulo);

    SlotTitulo->SetHorizontalAlignment(HAlign_Fill);
    SlotTitulo->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 20.0f));
    
    auto CrearBoton = [this, Contenedor](
        const FName NombreBoton,
        const FName NombreTexto,
        const FString& Etiqueta
    ) -> UButton*
    {
        USizeBox* Caja =
            WidgetTree->ConstructWidget<USizeBox>();

        Caja->SetWidthOverride(300.0f);
        Caja->SetHeightOverride(50.0f);

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
        Fuente.Size = 20;
        Texto->SetFont(Fuente);

        UButtonSlot* SlotTexto =
            Cast<UButtonSlot>(Boton->AddChild(Texto));

        if (SlotTexto)
        {
            SlotTexto->SetHorizontalAlignment(HAlign_Center);
            SlotTexto->SetVerticalAlignment(VAlign_Center);
        }

        Caja->SetContent(Boton);

        UVerticalBoxSlot* SlotCaja =
            Contenedor->AddChildToVerticalBox(Caja);

        SlotCaja->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));

        return Boton;
    };

    BotonContinuar = CrearBoton(
        TEXT("BotonContinuar"),
        TEXT("TextoContinuar"),
        TEXT("Continuar")
    );

    BotonGuardar = CrearBoton(
        TEXT("BotonGuardar"),
        TEXT("TextoGuardar"),
        TEXT("Guardar")
    );

    BotonSalir = CrearBoton(
        TEXT("BotonSalir"),
        TEXT("TextoSalir"),
        TEXT("Salir")
        
    );
    TextoEstadoGuardado =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoEstadoGuardado")
        );

    TextoEstadoGuardado->SetText(FText::GetEmpty());
    TextoEstadoGuardado->SetJustification(ETextJustify::Center);
    TextoEstadoGuardado->SetColorAndOpacity(
        FSlateColor(FLinearColor::White)
    );

    UVerticalBoxSlot* SlotEstado =
        Contenedor->AddChildToVerticalBox(TextoEstadoGuardado);

    SlotEstado->SetPadding(FMargin(0.0f, 8.0f, 0.0f, 0.0f));
    
    BotonContinuar->OnClicked.AddUniqueDynamic(
        this,
        &UReclaimMenuPausaWidget::PresionarContinuar
    );

    BotonSalir->OnClicked.AddUniqueDynamic(
        this,
        &UReclaimMenuPausaWidget::PresionarSalir
    );
    
    BotonGuardar->OnClicked.AddUniqueDynamic(
     this,
     &UReclaimMenuPausaWidget::PresionarGuardar
 );
    
}

void UReclaimMenuPausaWidget::PresionarContinuar()
{
    AlContinuar.Broadcast();
}

void UReclaimMenuPausaWidget::PresionarSalir()
{
    AlSalir.Broadcast();
}
void UReclaimMenuPausaWidget::PresionarGuardar()
{
    AlGuardar.Broadcast();
}
void UReclaimMenuPausaWidget::HabilitarGuardado(bool bHabilitado)
{
    if (IsValid(BotonGuardar))
    {
        BotonGuardar->SetIsEnabled(bHabilitado);
    }
}

void UReclaimMenuPausaWidget::MostrarEstadoGuardado(
    const FText& Mensaje
)
{
    if (IsValid(TextoEstadoGuardado))
    {
        TextoEstadoGuardado->SetText(Mensaje);
    }
}

