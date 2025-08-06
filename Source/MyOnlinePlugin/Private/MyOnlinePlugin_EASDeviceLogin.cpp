// Fill out your copyright notice in the Description page of Project Settings.


#include "MyOnlinePlugin_EASDeviceLogin.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "eos_sdk.h"

#include "eos_connect.h"
#include "eos_connect_types.h"
#include "IOnlineSubsystemEOS.h"
#include "EOSSharedTypes.h"
#include "IEOSSDKManager.h"





UMyOnlinePlugin_EASDeviceLogin* UMyOnlinePlugin_EASDeviceLogin::LoginWithEOSDeviceID()
{
	return NewObject<UMyOnlinePlugin_EASDeviceLogin>();
}

void UMyOnlinePlugin_EASDeviceLogin::Activate()
{
	StartEOSLogin();
}

void UMyOnlinePlugin_EASDeviceLogin::StartEOSLogin()
{
	IOnlineSubsystemEOS* EOSSubsystem = static_cast<IOnlineSubsystemEOS*>(IOnlineSubsystem::Get(TEXT("EOS")));
	if (!EOSSubsystem)
	{
		OnCompleted.Broadcast(false, TEXT("EOS Subsystem not found"));
		return;
	}

	IEOSPlatformHandlePtr PlatformHandle = EOSSubsystem->GetEOSPlatformHandle();
	if (!PlatformHandle.IsValid())
	{
		OnCompleted.Broadcast(false, TEXT("Invalid EOS Platform Handle"));
		return;
	}

	ConnectHandle = EOS_Platform_GetConnectInterface(*PlatformHandle);
	if (!ConnectHandle)
	{
		OnCompleted.Broadcast(false, TEXT("Invalid EOS Connect Handle"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Login With Device started"));
	
	// Step 1: Create Device ID
	FString PlatformName = FPlatformProperties::PlatformName();
	FString DeviceMakeModel = FPlatformMisc::GetDeviceMakeAndModel();

	FString CombinedDeviceModel = FString::Printf(TEXT("%s - %s"), *PlatformName, *DeviceMakeModel);
	static FTCHARToUTF8 DeviceModelUtf8(*CombinedDeviceModel);
	const char* DeviceModelAnsi = DeviceModelUtf8.Get();

	EOS_Connect_CreateDeviceIdOptions CreateDeviceIdOptions = {};
	CreateDeviceIdOptions.ApiVersion = EOS_CONNECT_CREATEDEVICEID_API_LATEST;
	CreateDeviceIdOptions.DeviceModel = DeviceModelAnsi;
	UE_LOG(LogTemp, Log, TEXT("CreateDeviceIdOptions.ApiVersion: %d"), CreateDeviceIdOptions.ApiVersion);
	UE_LOG(LogTemp, Log, TEXT("CreateDeviceIdOptions.DeviceModel: %s"), UTF8_TO_TCHAR(CreateDeviceIdOptions.DeviceModel));

	EOS_Connect_CreateDeviceId(ConnectHandle, &CreateDeviceIdOptions, this, [](const EOS_Connect_CreateDeviceIdCallbackInfo* Data)
		{

			UE_LOG(LogTemp, Warning, TEXT("Creating Device ID Started"));
			auto* Self = static_cast<UMyOnlinePlugin_EASDeviceLogin*>(Data->ClientData);
			if (!Self) return;

			if (Data->ResultCode == EOS_EResult::EOS_Success || Data->ResultCode == EOS_EResult::EOS_DuplicateNotAllowed)
			{
				// Step 2: Login
				EOS_Connect_Credentials Credentials = {};
				Credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
				Credentials.Type = EOS_EExternalCredentialType::EOS_ECT_DEVICEID_ACCESS_TOKEN;
				Credentials.Token = nullptr;

				EOS_Connect_UserLoginInfo UserLoginInfo = {};
				UserLoginInfo.ApiVersion = EOS_CONNECT_USERLOGININFO_API_LATEST;
				UserLoginInfo.DisplayName = "LocalUser";

				EOS_Connect_LoginOptions LoginOptions = {};
				LoginOptions.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
				LoginOptions.Credentials = &Credentials;
				LoginOptions.UserLoginInfo = &UserLoginInfo;
		


				EOS_Connect_Login(Self->ConnectHandle, &LoginOptions, Self, [](const EOS_Connect_LoginCallbackInfo* LoginData)
					{
						UE_LOG(LogTemp, Warning, TEXT("Connect Login Started"));
						auto* Self = static_cast<UMyOnlinePlugin_EASDeviceLogin*>(LoginData->ClientData);
						if (!Self) return;

						if (LoginData->ResultCode == EOS_EResult::EOS_Success)
						{
							
							char Buffer[EOS_PRODUCTUSERID_MAX_LENGTH + 1] = {};
							int32_t BufferLen = sizeof(Buffer);
							EOS_ProductUserId_ToString(LoginData->LocalUserId, Buffer, &BufferLen);
							FString UserIdStr(Buffer);

							Self->OnCompleted.Broadcast(true, UserIdStr);
							UE_LOG(LogTemp, Log, TEXT("EOS Device ID Exist and Login Success. ProductUserId: %s"), *UserIdStr);
						}
						else if (LoginData->ResultCode == EOS_EResult::EOS_InvalidUser && LoginData->ContinuanceToken)
						{
							// Need to create a new user
							EOS_Connect_CreateUserOptions CreateUserOptions = {};
							CreateUserOptions.ApiVersion = EOS_CONNECT_CREATEUSER_API_LATEST;
							CreateUserOptions.ContinuanceToken = LoginData->ContinuanceToken;

							EOS_Connect_CreateUser(Self->ConnectHandle, &CreateUserOptions, Self, [](const EOS_Connect_CreateUserCallbackInfo* CreateData)
								{
									UE_LOG(LogTemp, Warning, TEXT("EOS Connect Create User"));
									auto* Self = static_cast<UMyOnlinePlugin_EASDeviceLogin*>(CreateData->ClientData);
									if (!Self) return;

									if (CreateData->ResultCode == EOS_EResult::EOS_Success)
									{
										char Buffer[EOS_PRODUCTUSERID_MAX_LENGTH + 1] = {};
										int32_t BufferLen = sizeof(Buffer);
										EOS_ProductUserId_ToString(CreateData->LocalUserId, Buffer, &BufferLen);
										FString UserIdStr(Buffer);

										Self->OnCompleted.Broadcast(true, UserIdStr);
										UE_LOG(LogTemp, Log, TEXT("EOS Device ID Created and Login Success. ProductUserId: %s"), *UserIdStr);
									}
									else
									{
										Self->OnCompleted.Broadcast(false, FString::Printf(TEXT("CreateUser Failed: %s"), *FString(EOS_EResult_ToString(CreateData->ResultCode))));
									}
								});
						}
						else
						{
							Self->OnCompleted.Broadcast(false, FString::Printf(TEXT("Login Failed: %s"), *FString(EOS_EResult_ToString(LoginData->ResultCode))));
						}
					});
			}
			else
			{
				Self->OnCompleted.Broadcast(false, FString::Printf(TEXT("CreateDeviceId Failed: %s"), *FString(EOS_EResult_ToString(Data->ResultCode))));
			}
		});
}

