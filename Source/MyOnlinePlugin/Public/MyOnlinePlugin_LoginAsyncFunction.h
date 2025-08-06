// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "MyOnlinePlugin_LoginAsyncFunction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginCallback, bool, bSuccess, const FString&, Message);

/**
 * 
 */
UCLASS()
class MYONLINEPLUGIN_API UMyOnlinePlugin_LoginAsyncFunction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "EOS Integration Kit | Login", DisplayName = "Online Login EAS")
	static UMyOnlinePlugin_LoginAsyncFunction* LoginEASOnline();

	UPROPERTY(BlueprintAssignable)
	FOnLoginCallback LoginSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnLoginCallback LoginFailure;

private:
	virtual void Activate() override;
	void PromptLogin();
	void LoginCallback(int32 LocalUserNum, bool bWasSuccess, const FUniqueNetId& UserId, const FString& Message);

};
