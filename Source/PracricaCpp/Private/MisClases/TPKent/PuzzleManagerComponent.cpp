#include "MisClases/TPKent/PuzzleManagerComponent.h"
#include "MisClases/TPKent/IPuzzleInteractable.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h" // Para dibujar el LineTrace en pantalla (debug)

UPuzzleManagerComponent::UPuzzleManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPuzzleManagerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UPuzzleManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // El LineTrace solo se ejecuta cuando el puzzle esta activo
    if (bPuzzleActive)
    {
        GetLookedAtActor(); // Por ahora solo dibuja la linea, despues detecta tiles
    }
}

AActor* UPuzzleManagerComponent::GetLookedAtActor()
{
    // GetOwner() devuelve AActor*, no necesitamos saber que es un ACharacter
    AActor* Owner = GetOwner();
    if (!Owner) return nullptr;

    // Buscamos la camara directamente en el actor dueño
    // FindComponentByClass funciona en cualquier AActor, sin castear
    UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>();

    // Si tiene camara usamos su posicion, sino usamos la del actor
    FVector Start = Camera ? Camera->GetComponentLocation()
                           : Owner->GetActorLocation();

    FVector Direction = Camera ? Camera->GetForwardVector()
                               : Owner->GetActorForwardVector();

    FVector End = Start + Direction * InteractDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, Start, End, ECC_Visibility, Params);

    DrawDebugLine(GetWorld(), Start, End,
        bHit ? FColor::Green : FColor::Red,
        false, -1.f, 0, 1.f);

    return bHit ? Hit.GetActor() : nullptr;
}

void UPuzzleManagerComponent::TryInteract()
{
    // Obtenemos el actor que estamos mirando via LineTrace
    AActor* LookedAt = GetLookedAtActor();
    if (!LookedAt) return;

    // Verificamos si el actor implementa nuestra interface
    // Esto evita hacer Cast directo a una clase concreta
    if (LookedAt->Implements<UIPuzzleInteractable>())
    {
        // Preguntamos si el objeto puede ser interactuado ahora
        bool bCan = IIPuzzleInteractable::Execute_CanInteract(LookedAt);
        if (bCan)
        {
            // Obtenemos los datos del objeto y notificamos la deteccion
            FPuzzleObjectData Data =
                IIPuzzleInteractable::Execute_GetPuzzleData(LookedAt);
            OnObjectDetected.Broadcast(Data);

            // Le decimos al objeto que fue interactuado
            IIPuzzleInteractable::Execute_OnInteract(LookedAt, GetOwner());
        }
    }
}

void UPuzzleManagerComponent::RegisterCompletedObject(FPuzzleObjectData ObjectData)
{
    // Marcamos el objeto como completado
    ObjectData.bIsCompleted = true;

    // Lo agregamos al codice
    CompletedObjects.Add(ObjectData);

    // Notificamos a quien escuche que se completo un objeto
    OnObjectCompleted.Broadcast(ObjectData);

    // Log en consola para debug
    UE_LOG(LogTemp, Log, TEXT("[CODICE] Objeto completado: %s"), *ObjectData.ObjectName);
}

void UPuzzleManagerComponent::OnTileStep(FPuzzleObjectData TileData)
{
    // Agregamos la inicial del color pisado a la secuencia actual
    CurrentSequence.Add(TileData.ColorInitial);

    // Registramos en el log
    UE_LOG(LogTemp, Log, TEXT("[TILE] Pisado: %s (%s)"),
        *TileData.ObjectName, *TileData.ColorInitial);

    // Notificamos el delegate para que el widget lo muestre
    OnObjectDetected.Broadcast(TileData);

    // Verificamos si la secuencia actual coincide con la correcta
    if (CurrentSequence == CorrectSequence)
    {
        UE_LOG(LogTemp, Log, TEXT("[PUZZLE] Secuencia correcta!"));
        OnPuzzleCompleted.Broadcast();
    }
}

void UPuzzleManagerComponent::OnPasswordPanelEnter(AActor* Panel)
{
    ActivePasswordPanel = Panel;
    UE_LOG(LogTemp, Log, TEXT("[PANEL] Jugador cerca del panel"));
    // Aqui despues mostraremos el prompt en el widget
}

void UPuzzleManagerComponent::OnPasswordPanelExit()
{
    ActivePasswordPanel = nullptr;
    UE_LOG(LogTemp, Log, TEXT("[PANEL] Jugador se alejo del panel"));
}

void UPuzzleManagerComponent::OpenPasswordWidget(AActor* Panel)
{
    // Aqui despues abriremos el widget de contrasena
    UE_LOG(LogTemp, Log, TEXT("[PANEL] Abriendo widget de contrasena"));
}