#include "MisClases/TPKent/AColorPanel.h"
#include "MisClases/TPKent/PuzzleManagerComponent.h"
#include "Components/BoxComponent.h"

AAColorPanel::AAColorPanel()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
    OverlapBox->SetupAttachment(RootComponent);
    // Caja fina y ancha para detectar cuando el jugador pisa el tile
    OverlapBox->SetBoxExtent(FVector(60.f, 60.f, 20.f));

    // Valores por defecto, se configuran por instancia en el editor
    PuzzleData.ObjectName = "Tile";
    PuzzleData.ObjectType = EPuzzleObjectType::ColorButton;
    PuzzleData.Description = "Cuadrado de color del puzzle";
    PuzzleData.ObjectColor = FLinearColor::White;
    PuzzleData.ColorInitial = "?";
}

void AAColorPanel::BeginPlay()
{
    Super::BeginPlay();
    OverlapBox->OnComponentBeginOverlap.AddDynamic(
        this, &AAColorPanel::OnOverlapBegin);
}

void AAColorPanel::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AAColorPanel::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Buscamos el PuzzleManager en el actor que pisó el tile
    UPuzzleManagerComponent* PuzzleManager =
        OtherActor->FindComponentByClass<UPuzzleManagerComponent>();

    // Solo reaccionamos si es el jugador y el puzzle esta activo
    if (!PuzzleManager || !PuzzleManager->bPuzzleActive) return;

    // Notificamos al componente que se pisó este tile
    // El componente decide si es correcto o no
    PuzzleManager->OnTileStep(PuzzleData);
}

void AAColorPanel::OnInteract_Implementation(AActor* Interactor)
{
    // El tile no necesita interaccion directa, solo overlap
}

FPuzzleObjectData AAColorPanel::GetPuzzleData_Implementation()
{
    return PuzzleData;
}

bool AAColorPanel::CanInteract_Implementation()
{
    return bIsActive;
}