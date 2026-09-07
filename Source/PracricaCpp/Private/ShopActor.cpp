#include "ShopActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Components/InputComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

AShopActor::AShopActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ShopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShopMesh"));
	RootComponent = ShopMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));

	if (CubeMesh.Succeeded())
	{
		ShopMesh->SetStaticMesh(CubeMesh.Object);
	}

	ShopMesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	ShopMesh->SetRelativeScale3D(FVector(2.f, 1.f, 1.2f));
	ShopMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionZone"));
	InteractionZone->SetupAttachment(RootComponent);
	InteractionZone->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	InteractionZone->SetBoxExtent(FVector(250.f, 250.f, 200.f));
	InteractionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionZone->SetGenerateOverlapEvents(true);

	StatusText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StatusText"));
	StatusText->SetupAttachment(RootComponent);
	StatusText->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	StatusText->SetHorizontalAlignment(EHTA_Center);
	StatusText->SetWorldSize(28.f);
	StatusText->SetTextRenderColor(FColor::Yellow);
	StatusText->SetText(FText::FromString(TEXT("TIENDA")));
}

void AShopActor::BeginPlay()
{
	Super::BeginPlay();

	InteractionZone->OnComponentBeginOverlap.AddDynamic(
		this, &AShopActor::OnPlayerEnteredShop);

	InteractionZone->OnComponentEndOverlap.AddDynamic(
		this, &AShopActor::OnPlayerLeftShop);
}

void AShopActor::OnPlayerEnteredShop(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (!PlayerPawn || !PlayerPawn->IsPlayerControlled())
	{
		return;
	}

	APlayerController* PlayerController =
		Cast<APlayerController>(PlayerPawn->GetController());

	if (!PlayerController)
	{
		return;
	}

	bPlayerInRange = true;
	StatusText->SetText(FText::FromString(TEXT("E - Abrir tienda")));

	EnableInput(PlayerController);

	if (InputComponent && !bInputBound)
	{
		FInputKeyBinding& Binding = InputComponent->BindKey(
			EKeys::E,
			IE_Pressed,
			this,
			&AShopActor::HandleShopInput);

		Binding.bConsumeInput = false;
		bInputBound = true;
	}
}

void AShopActor::OnPlayerLeftShop(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (!PlayerPawn || !PlayerPawn->IsPlayerControlled())
	{
		return;
	}

	bPlayerInRange = false;
	StatusText->SetText(FText::FromString(TEXT("TIENDA")));

	if (APlayerController* PlayerController =
		Cast<APlayerController>(PlayerPawn->GetController()))
	{
		DisableInput(PlayerController);
		
		if (ShopWidget && ShopWidget->IsInViewport())
		{
			ShopWidget->RemoveFromParent();
			ShopWidget = nullptr;
		}
		
		PlayerController->bShowMouseCursor = false;
		
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}
}

void AShopActor::HandleShopInput()
{
	if (bPlayerInRange)
	{
		Interact();
	}
}

void AShopActor::Interact()
{
	if (!ShopWidgetClass)
	{
		StatusText->SetText(
			FText::FromString(TEXT("Falta asignar WBP_Shop")));
		return;
	}

	// No abre más de una copia del menú al mismo tiempo.
	if (ShopWidget && ShopWidget->IsInViewport())
	{
		return;
	}

	APlayerController* PlayerController =
		GetWorld()->GetFirstPlayerController();

	if (!PlayerController)
	{
		return;
	}

	ShopWidget = CreateWidget<UUserWidget>(
		PlayerController,
		ShopWidgetClass);

	if (!ShopWidget)
	{
		return;
	}

	ShopWidget->AddToViewport();

	PlayerController->bShowMouseCursor = true;

	FInputModeGameAndUI InputMode;
	PlayerController->SetInputMode(InputMode);

	StatusText->SetText(FText::FromString(TEXT("TIENDA ABIERTA")));
}

