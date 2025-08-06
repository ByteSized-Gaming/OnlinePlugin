// Fill out your copyright notice in the Description page of Project Settings.


#include "MyOnlinePlugin_LoginAsyncFunction.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "eos_sdk.h"

UMyOnlinePlugin_LoginAsyncFunction* UMyOnlinePlugin_LoginAsyncFunction::LoginEASOnline()
{
	UMyOnlinePlugin_LoginAsyncFunction* UCamelot_LoginObject = NewObject<UMyOnlinePlugin_LoginAsyncFunction>();
	return UCamelot_LoginObject;
}

void UMyOnlinePlugin_LoginAsyncFunction::Activate()
{
	Super::Activate();
	if (const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(GetWorld()))
	{
		//UE_LOG(LogTemp, Log, TEXT("Subsystem %s"), *SubsystemRef->GetSubsystemName().ToString());
		UE_LOG(LogTemp, Warning, TEXT("LoginUsingAuthInterface: Subsystem and Identity Interface is valid and proceeding with login."));
		if (const IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface())
		{
			Online::GetSubsystem(GetWorld())->GetIdentityInterface()->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::LoginCallback));
			PromptLogin();
		}
	}
}

void UMyOnlinePlugin_LoginAsyncFunction::PromptLogin()
{
	if (const IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(GetWorld()))
	{
		if (const IOnlineIdentityPtr IdentityPointerRef = SubsystemRef->GetIdentityInterface())
		{
			UE_LOG(LogTemp, Verbose, TEXT("LoginUsingAuthInterface: Subsystem and Identity Interface is valid and proceeding with login."));

			// Prompt the user to login using the EAS interfaceUE_LOG(LogTemp, Log, TEXT("Login using Account Portal."));

			FOnlineAccountCredentials AccountDetails;
			AccountDetails.Id = "";
			AccountDetails.Token = "";
			AccountDetails.Type = TEXT("accountportal");

			//IdentityPointerRef->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::LoginCallback));
			IdentityPointerRef->Login(0, AccountDetails);
		}
	}
}

void UMyOnlinePlugin_LoginAsyncFunction::LoginCallback(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Message)
{
	UE_LOG(LogTemp, Log, TEXT("EAS LoginCallback called with bWasSuccessful: %s, UserId: %s, Message: %s"),
		bWasSuccessful ? TEXT("true") : TEXT("false"),
		*UserId.ToString(),
		*Message);
	if (bWasSuccessful)
	{
		LoginSuccess.Broadcast(true, UserId.ToString());
	}
	else
	{
		LoginFailure.Broadcast(false, Message);
	}
	SetReadyToDestroy();
	MarkAsGarbage();
}