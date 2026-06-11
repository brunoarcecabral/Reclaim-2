#include "MisClases/TPKent/PasswordPanel.h"
#include "MisClases/TPKent/PuzzleManagerComponent.h"
#include "MisClases/TPKent/IPuzzleInteractable.h"
#include "Components/BoxComponent.h"

APasswordPanel::APasswordPanel()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
    OverlapBox->SetupAttachment(RootComponent);
    // Caja grande para que el jugador lo active al acercarse
    OverlapBox->SetBoxExtent(FVector(150.f, 150.f, 100.f));

    PuzzleData.ObjectName = "Panel de Contrasena";
    PuzzleData.ObjectType = EPuzzleObjectType::PasswordDoor;
    PuzzleData.Description = "Panel donde se ingresa la contrasena";

    CorrectPassword = "RGBA"; // Valor por defecto, cambiar en el editor
}

void APasswordPanel::BeginPlay()
{
    Super::BeginPlay();
    OverlapBox->OnComponentBeginOverlap.AddDynamic(
        this, &APasswordPanel::OnOverlapBegin);
    OverlapBox->OnComponentEndOverlap.AddDynamic(
        this, &APasswordPanel::OnOverlapEnd);
}

void APasswordPanel::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APasswordPanel::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UPuzzleManagerComponent* PuzzleManager =
        OtherActor->FindComponentByClass<UPuzzleManagerComponent>();

    // Solo reaccionamos si es el jugador y la pila fue insertada
    if (!PuzzleManager || !PuzzleManager->bPuzzleActive) return;

    bPlayerInRange = true;

    // Notificamos al componente que el jugador esta cerca del panel
    // El componente se encarga de mostrar el prompt en pantalla
    PuzzleManager->OnPasswordPanelEnter(this);
}

void APasswordPanel::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    UPuzzleManagerComponent* PuzzleManager =
        OtherActor->FindComponentByClass<UPuzzleManagerComponent>();

    if (!PuzzleManager) return;

    bPlayerInRange = false;

    // Avisamos al componente que el jugador se fue
    PuzzleManager->OnPasswordPanelExit();
}

void APasswordPanel::OnInteract_Implementation(AActor* Interactor)
{
    // Al presionar interactuar cerca del panel se abre el widget de contrasena
    UPuzzleManagerComponent* PuzzleManager =
        Interactor->FindComponentByClass<UPuzzleManagerComponent>();

    if (PuzzleManager && bPlayerInRange)
    {
        PuzzleManager->OpenPasswordWidget(this);
    }
}

void APasswordPanel::TryPassword(FString EnteredPassword, AActor* Interactor)
{
    if (EnteredPassword == CorrectPassword)
    {
        UE_LOG(LogTemp, Log, TEXT("[PANEL] Contrasena correcta!"));

        // Avisamos a la puerta via interface, sin castear
        if (LinkedDoor && LinkedDoor->Implements<UIPuzzleInteractable>())
        {
            IIPuzzleInteractable::Execute_OnInteract(LinkedDoor, Interactor);
        }

        // Registramos en el codice
        UPuzzleManagerComponent* PuzzleManager =
            Interactor->FindComponentByClass<UPuzzleManagerComponent>();
        if (PuzzleManager)
        {
            PuzzleManager->RegisterCompletedObject(PuzzleData);
            PuzzleManager->bPuzzleActive = false; // Desactivamos el LineTrace
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[PANEL] Contrasena incorrecta: %s"),
            *EnteredPassword);
    }
}

FPuzzleObjectData APasswordPanel::GetPuzzleData_Implementation()
{
    return PuzzleData;
}

bool APasswordPanel::CanInteract_Implementation()
{
    return bPlayerInRange && bIsActive;
}
