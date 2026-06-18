#include "MisClases/TPKent/TilesDeColores.h"
#include "Kismet/GameplayStatics.h"

ATilesDeColores::ATilesDeColores()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATilesDeColores::BeginPlay()
{
	Super::BeginPlay();
}

void ATilesDeColores::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PuzzleActivad) return;

	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0.f, 0.f, 100.f);

	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;

	UKismetSystemLibrary::LineTraceSingle(
		this,
		Start,
		End,
		ETraceTypeQuery::TraceTypeQuery2,
		true,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true
	);
	

	if (!HitResult.GetActor()) return;

	AActor* HitActor = HitResult.GetActor();

	// Ahora usa IInterfaceColores con MyData.MyString y ColorActivado = false
	if (HitActor->GetClass()->ImplementsInterface(UInterfaceColores::StaticClass()))
	{
		IInterfaceColores::Execute_InformacionColor(HitActor, MyData.MyString, false);

		PuzzleActivad = false;

		GetWorldTimerManager().SetTimer(CooldownTimer, [this]()
		{
			PuzzleActivad = true;
		}, 3.f, false);
	}
}

void ATilesDeColores::ComunicarPuzzle_Implementation(bool llave)
{
	MyData.bMyBool = llave;
	PuzzleActivad = true;
}