// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "MyOnlinePlugin_LoginStatus.generated.h"

/**
 * 
 */
UCLASS()
class MYONLINEPLUGIN_API UMyOnlinePlugin_LoginStatus : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "My Plugin|Online", meta = (WorldContext = "WorldContextObject"), DisplayName = "Is Plugin Logged In")
	static bool MyOnlineLoginStatus(const FString& KnownProductUserIdStr, UObject* WorldContextObject);

};
