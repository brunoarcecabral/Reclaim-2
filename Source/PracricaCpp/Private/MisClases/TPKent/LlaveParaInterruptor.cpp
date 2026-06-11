#include "MisClases/TPKent/LlaveParaInterruptor.h"
#include "MisClases/TPKent/PuzzleManagerComponent.h"
#include "Components/BoxComponent.h"

ALlaveParaInterruptor::ALlaveParaInterruptor()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
    OverlapBox->SetupAttachment(RootComponent);
    OverlapBox->SetBoxExtent(FVector(50.f, 50.f, 50.f));

    PuzzleData.ObjectName = "Pila";
    PuzzleData.ObjectType = EPuzzleObjectType::Battery;
    PuzzleData.Description = "Una pila que alimenta la maquina";
}

void ALlaveParaInterruptor::BeginPlay()
{
    Super::BeginPlay();
    OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &ALlaveParaInterruptor::OnOverlapBegin);
}

void ALlaveParaInterruptor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Si el jugador esta cargando la pila, la mantenemos flotando frente a el
    if (bIsHeld && HoldingActor)
    {
        FVector NewLocation = HoldingActor->GetActorLocation()
            + HoldingActor->GetActorForwardVector() * 80.f
            + FVector(0.f, 0.f, 30.f);

        SetActorLocation(NewLocation);
    }
}

void ALlaveParaInterruptor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || bIsHeld) return;

    UPuzzleManagerComponent* PuzzleManager =
        OtherActor->FindComponentByClass<UPuzzleManagerComponent>();

    if (PuzzleManager && !PuzzleManager->bIsHoldingBattery)
    {
        bIsHeld = true;
        HoldingActor = OtherActor;
        PuzzleManager->bIsHoldingBattery = true;
        PuzzleManager->HeldBattery = this;

        Mesh->SetSimulatePhysics(false);
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        UE_LOG(LogTemp, Log, TEXT("[PILA] Agarrada"));
    }
}

void ALlaveParaInterruptor::OnInteract_Implementation(AActor* Interactor)
{
    if (!bIsHeld) return;

    bIsHeld = false;

    UPuzzleManagerComponent* PuzzleManager =
        Interactor->FindComponentByClass<UPuzzleManagerComponent>();

    if (PuzzleManager)
    {
        PuzzleManager->bIsHoldingBattery = false;
        PuzzleManager->HeldBattery = nullptr;
    }

    HoldingActor = nullptr;
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    UE_LOG(LogTemp, Log, TEXT("[PILA] Soltada"));
}

FPuzzleObjectData ALlaveParaInterruptor::GetPuzzleData_Implementation()
{
    return PuzzleData;
}

bool ALlaveParaInterruptor::CanInteract_Implementation()
{
    return !bIsHeld;
}