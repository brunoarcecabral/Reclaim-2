#include "MisClases/TPKent/APasswordDoor.h"

AAPasswordDoor::AAPasswordDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	PuzzleData.ObjectName = "Puerta";
	PuzzleData.ObjectType = EPuzzleObjectType::PasswordDoor;
	PuzzleData.Description = "Puerta que se abre al completar el puzzle";
}

void AAPasswordDoor::BeginPlay()
{
	Super::BeginPlay();
}

void AAPasswordDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAPasswordDoor::OnInteract_Implementation(AActor* Interactor)
{
	if (bIsOpen) return;

	// Movemos la puerta hacia arriba (o en la direccion configurada)
	FVector NewLocation = GetActorLocation() + OpenOffset;
	SetActorLocation(NewLocation);

	bIsOpen = true;

	UE_LOG(LogTemp, Log, TEXT("[PUERTA] Abierta!"));
}

FPuzzleObjectData AAPasswordDoor::GetPuzzleData_Implementation()
{
	return PuzzleData;
}

bool AAPasswordDoor::CanInteract_Implementation()
{
	return !bIsOpen;
}