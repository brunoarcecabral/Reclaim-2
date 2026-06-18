// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MisClases/TPKent/ComunicacionPuzzle.h"
#include "MisClases/TPKent/InterfaceColores.h"
#include "PracricaCppCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class APracricaCppCharacter : public ACharacter, public IComunicacionPuzzle, public IInterfaceColores
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

protected:

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* MouseLookAction;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* InteractAction;

public:
    APracricaCppCharacter();

protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Interact();

public:
    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoMove(float Right, float Forward);

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoLook(float Yaw, float Pitch);

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoJumpStart();

    UFUNCTION(BlueprintCallable, Category="Input")
    virtual void DoJumpEnd();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progresion")
    float CExperienciaActual = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progresion")
    float CExperienciaRequerida = 40.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progresion")
    float CNivelActual = 1;

    UFUNCTION(BlueprintCallable, Category = "Progresion")
    void CGanarExperiencia(float CExpObtenida);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
    bool TengoLaLlave = false;

    virtual void ComunicarPuzzle_Implementation(bool llave) override;
    virtual void InformacionColor_Implementation(const FString& Color, bool ColorActivado) override;

    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};