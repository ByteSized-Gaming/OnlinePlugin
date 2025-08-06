// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "eos_sdk.h"
#include "MyOnlinePlugin_EASDeviceLogin.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnlineEOSLoginResult, bool, bSuccess, const FString&, EOSUserIdStr);

/**
 * 
 */
UCLASS()
class MYONLINEPLUGIN_API UMyOnlinePlugin_EASDeviceLogin : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintAssignable)
	FOnlineEOSLoginResult OnCompleted;

	/** Blueprint node to initiate login */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "EOS", DisplayName = "Online Plugin Device Login EAS")
	static UMyOnlinePlugin_EASDeviceLogin* LoginWithEOSDeviceID();

	virtual void Activate() override;

private:
	void StartEOSLogin();
	void HandleLoginCallback(const EOS_Connect_LoginCallbackInfo* Data);
	void HandleCreateUserCallback(const EOS_Connect_CreateUserCallbackInfo* Data);

	EOS_HConnect ConnectHandle = nullptr;
};
	
