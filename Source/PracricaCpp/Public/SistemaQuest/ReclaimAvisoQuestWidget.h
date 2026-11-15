#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReclaimAvisoQuestWidget.generated.h"


UCLASS()
class PRACRICACPP_API UReclaimAvisoQuestWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;
};

