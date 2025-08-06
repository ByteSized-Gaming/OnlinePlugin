// Fill out your copyright notice in the Description page of Project Settings.


#include "MyOnlinePlugin_LoginStatus.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
//#include "eos_sdk.h"

//#include "eos_connect.h"
//#include "eos_connect_types.h"
//#include "IOnlineSubsystemEOS.h"
//#include "EOSSharedTypes.h"
//#include "IEOSSDKManager.h"

bool UMyOnlinePlugin_LoginStatus::MyOnlineLoginStatus(const FString& KnownProductUserIdStr, UObject* WorldContextObject)
{
		UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
		if (!World) return false;

		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(TEXT("EOS"));
		if (!Subsystem) return false;

		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
		if (!Identity.IsValid()) return false;

		TSharedPtr<const FUniqueNetId> LocalUserId = Identity->GetUniquePlayerId(0);
		if (!LocalUserId.IsValid()) return false;

		// Compare stored ID string to current logged-in ID string
		return LocalUserId->ToString() == KnownProductUserIdStr;
	
}
