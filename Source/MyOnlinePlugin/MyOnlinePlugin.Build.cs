// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class MyOnlinePlugin : ModuleRules
{
	public MyOnlinePlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDefinitions.Add("WITH_EOS_SDK=1");
		Console.WriteLine("MyOnlinePlugin to integrate EOS and EAS");
		PublicDefinitions.Add("EOSSDK_USE_PROJECT_BINARY=1");
		
		// PublicIncludePaths.AddRange(
		// 	new string[] {
		// 		// ... add public include paths required here ...
		// 	}
		// 	);
				
		
		// PrivateIncludePaths.AddRange(
		// 	new string[] {
		// 		// ... add other private include paths required here ...
		// 	}
		// 	);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"OnlineSubsystem",
				"OnlineSubsystemEOS",
				"OnlineSubsystemUtils",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        string PluginDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));

        if (Target.Platform == UnrealTargetPlatform.Win64)
			{

            string Win64BinPath = Path.Combine(PluginDir, "Source", "ThirdParty", "EOSSDK", "Bin", "Win64");
            string DLLFileName = "EOSSDK-Win64-Shipping.dll";
            string LIBFileName = "EOSSDK-Win64-Shipping.lib";

            string DLLSourcePath = Path.Combine(Win64BinPath, DLLFileName);
            string DLLTargetPath = "$(BinaryOutputDir)/" + DLLFileName;

            string LIBSourcePath = Path.Combine(Win64BinPath, LIBFileName);
            string LIBTargetPath = "$(BinaryOutputDir)/" + LIBFileName;

            //Console.WriteLine("My Online EOS SDK Compilining Windows");
            Console.WriteLine("My Online EOS SDK Compiling for Windows");

			// Add the import library
			PublicAdditionalLibraries.Add(LIBSourcePath);

			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add(DLLFileName);

			// Ensure that the DLL and LIB are staged along with the executable
			RuntimeDependencies.Add(DLLTargetPath, DLLSourcePath, StagedFileType.NonUFS);
			RuntimeDependencies.Add(LIBTargetPath, LIBSourcePath, StagedFileType.NonUFS);
			}

			else if (Target.Platform == UnrealTargetPlatform.Android)
			{
				string EOSLibPath = Path.Combine(PluginDir,  "Source","ThirdParty", "EOSSDK", "Bin", "Android");

				Console.WriteLine("My Online EOS SDK: Compiling for Android (UE 5.3+)");

				if (Target.Architectures.Contains(UnrealArch.Arm64))
				{
					Console.WriteLine("Adding EOS SDK for arm64-v8a");
					string SoPath = Path.Combine(EOSLibPath, "arm64-v8a", "libEOSSDK.so");
					PublicAdditionalLibraries.Add(SoPath);
					RuntimeDependencies.Add(SoPath);
				}

				// Optional: remove this if you're *not* targeting 32-bit Android (ArmV7)
				if (Target.Architectures.Contains(UnrealArch.Arm64ec)) // UnrealArch does not directly expose ArmV7, so this may not match
				{
					Console.WriteLine("Adding EOS SDK for armeabi-v7a");
					string SoPath = Path.Combine(EOSLibPath, "armeabi-v7a", "libEOSSDK.so");
					PublicAdditionalLibraries.Add(SoPath);
					RuntimeDependencies.Add(SoPath);
				}

				if (Target.Architectures.Contains(UnrealArch.X64))
				{
					Console.WriteLine("Adding EOS SDK for x86_64");
					string SoPath = Path.Combine(EOSLibPath, "x86_64", "libEOSSDK.so");
					PublicAdditionalLibraries.Add(SoPath);
					RuntimeDependencies.Add(SoPath);
				}

            // Attach the Android Plugin (UPL)
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginDir, "Source", "ThirdParty", "EOSSDK", "LibAndroid_UPL.xml"));
	
        }


    }
}
