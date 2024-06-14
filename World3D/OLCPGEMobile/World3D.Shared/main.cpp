
//////////////////////////////////////////////////////////////////
// Pixel Game Engine Mobile Release 2.2.7,                      //
// John Galvin aka Johnngy63: 03-Jun-2024                       //
// iOS Sensor NOT supported, coming soon                        //
// Please report all bugs to https://discord.com/invite/WhwHUMV //
// Or on Github: https://github.com/Johnnyg63					//
//////////////////////////////////////////////////////////////////

//
// Base Project
//


// Set up headers for the different platforms
#if defined (__ANDROID__)

#include "../World3D/pch.h"
//#include "pch.h"

#endif

#if defined (__APPLE__)

#include "ios_native_app_glue.h"
#include <memory>

#endif

//#define STBI_NO_SIMD // Removes SIMD Support
// SIMD greatly improves the speed of your game
#if defined(__arm__)||(__aarch64__)

// Use Advance SIMD NEON when loading images for STB Default is SSE2 (x86)
#define STBI_NEON

#endif

#define OLC_PGE_APPLICATION
#define OLC_IMAGE_STB
#include "olcPixelGameEngine_Mobile.h"

#define OLC_PGEX_GRAPHICS3D
#include "olcPGEX_Graphics3D.h" 

#define OLC_PGEX_MINIAUDIO
#include "olcPGEX_MiniAudio.h"  // Checkout https://github.com/Moros1138/olcPGEX_MiniAudio Thanks Moros1138

#include <fstream> // Used for saving the savestate to a file

/// <summary>
/// To ensure proper cross platform support keep the class name as PGE_Mobile
/// This will ensure the iOS can launch the engine correctly
/// If you change it make the required changes in GameViewController.mm in the iOS app to suit
/// </summary>
class PGE_Mobile : public olc::PixelGameEngine {


public:

	olc::GFX3D::mesh cube;
	olc::GFX3D::PipeLine renderer;

	olc::GFX3D::vec3d vUp = { 0,1,0 };
	olc::GFX3D::vec3d vCamera = { 0,0,-4 };
	olc::GFX3D::vec3d vLookDir = { 0,0,1 };
	olc::GFX3D::vec3d vSun = { 4, 4 ,4 };

	float fYaw = 0.0f;		// FPS Camera rotation in XZ plane
	float fTheta = 0.0f;	// Spins World transform

	olc::Sprite* cubeTex;

public:

	PGE_Mobile() {
		sAppName = "Android/iOS Demo";
	}

	/* Vectors */
	std::vector<std::string> vecMessages;
	/* END Vectors*/

	int nFrameCount = 0;
	int nStep = 20;

	// The instance of the audio engine, no fancy config required.
	olc::MiniAudio ma;


public:
	//Example Save State Struct and Vector for when your app is paused
	struct MySaveState {
		std::string key;
		int value;
	};

	std::vector<MySaveState> vecLastState;

	std::string sampleAFullPath; // Holds the full path to sampleA.wav

public:
	bool OnUserCreate() override {
		//NOTE: To access the features with your phone device use:

		std::string fileMountains;

#if defined(__ANDROID__)
		// You can save files in the android Internal app storage

		fileMountains = (std::string)app_GetInternalAppStorage() + "/objectfiles/mountains.obj";
		olc::filehandler->ExtractFileFromAssets("objectfiles/mountains.obj", fileMountains);
#endif
#if defined(__APPLE__)
		// For iOS the internal app storage is read only, therefore we use External App Storage
		const char* internalPath = app_GetExternalAppStorage(); // iOS protected storage AKA /Library
		fileMountains = (std::string)app_GetInternalAppStorage() + "/objectfiles/mountains.obj";
#endif

		cube.LoadOBJFile(fileMountains);
		//cube.LoadOBJFile("/unitcube.obj", olc::FileHandler::INTERNAL);
		olc::GFX3D::ConfigureDisplay();

		cubeTex = new olc::Sprite("images/dirtblock.png");

		renderer.SetProjection(270.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.1f, 1000.0f, 0.0f, 0.0f, ScreenWidth(), ScreenHeight());

		std::string sAssetFile = "images/car_top.png";
		std::string sStorageFoler(app_GetPublicAppStorage()); /* Store it in --YOURPHONE--/Phone/Android/obb/com.olcPEGMob3DGraphicTest */
		std::string sStorageFile = sStorageFoler + "/car_top.png";

		if (app_ExtractFileFromAssets(sAssetFile, sStorageFile) == olc::rcode::OK)
		{
			// Test point
			int testPoint = 1;
		}

		return true;
	}

	// <summary>
	/// Draws a Target Pointer at the center position of Center Point
	/// </summary>
	/// <param name="vCenterPoint">Center Position of the target</param>
	/// <param name="nLineLenght">Length of lines</param>
	/// <param name="nCircleRadus">Center Circle radius</param>
	void DrawTargetPointer(olc::vi2d vCenterPoint, int32_t nLineLenght, int32_t nCircleRadus, olc::Pixel p = olc::WHITE)
	{
		/*
						|
						|
					----O----
						|
						|


		*/
		FillCircle(vCenterPoint, nCircleRadus, p);
		DrawLine(vCenterPoint, { vCenterPoint.x, vCenterPoint.y + nLineLenght }, p);
		DrawLine(vCenterPoint, { vCenterPoint.x, vCenterPoint.y - nLineLenght }, p);
		DrawLine(vCenterPoint, { vCenterPoint.x + nLineLenght, vCenterPoint.y }, p);
		DrawLine(vCenterPoint, { vCenterPoint.x - nLineLenght, vCenterPoint.y }, p);

	}

	bool OnUserUpdate(float fElapsedTime) override {

		SetDrawTarget(nullptr);

		nFrameCount = GetFPS();

		std::string sLineBreak = "-------------------------";

		std::string sMessage = "OneLoneCoder.com";
		vecMessages.push_back(sMessage);

		sMessage = "PGE Mobile Release 2.2.7";
		vecMessages.push_back(sMessage);

		sMessage = "Now With iOS Support";
		vecMessages.push_back(sMessage);

		sMessage = "NOTE: Android FPS = CPU FPS, iOS = GPU FPS";
		vecMessages.push_back(sMessage);

		sMessage = sAppName + " - FPS: " + std::to_string(nFrameCount);
		vecMessages.push_back(sMessage);

		sMessage = "---";
		vecMessages.push_back(sMessage);


		std::string sTouchScreen = "Touch the screen with two fingers";
		vecMessages.push_back(sTouchScreen);

		vecMessages.push_back(sLineBreak);


		// Start of 3D Stuff

		olc::GFX3D::mat4x4 matRotateX = olc::GFX3D::Math::Mat_MakeRotationX(fTheta);
		olc::GFX3D::mat4x4 matRotateZ = olc::GFX3D::Math::Mat_MakeRotationZ(fTheta * 0.5f);
		//olc::GFX3D::mat4x4 matWorld = olc::GFX3D::Math::Mat_MultiplyMatrix(matRotateX, matRotateZ);

		olc::GFX3D::mat4x4 matTrans;
		matTrans = olc::GFX3D::Math::Mat_MakeTranslation(0.0f, 0.0f, 5.0f);

		olc::GFX3D::mat4x4 matWorld;
		matWorld = olc::GFX3D::Math::Mat_MakeIdentity();;	// Form World Matrix
		matWorld = olc::GFX3D::Math::Mat_MultiplyMatrix(matRotateZ, matRotateX); // Transform by rotation
		matWorld = olc::GFX3D::Math::Mat_MultiplyMatrix(matWorld, matTrans); // Transform by translation

		// Create a "Point At"
		olc::GFX3D::vec3d vTarget = { 0,0,1 };
		olc::GFX3D::mat4x4 matCameraRot = olc::GFX3D::Math::Mat_MakeRotationY(fYaw);
		vLookDir = olc::GFX3D::Math::Mat_MultiplyVector(matCameraRot, vTarget);
		vTarget = olc::GFX3D::Math::Vec_Add(vCamera, vLookDir);
		olc::GFX3D::mat4x4 matCamera = olc::GFX3D::Math::Mat_PointAt(vCamera, vTarget, vUp);
		renderer.SetCamera(vCamera, vTarget, vUp);


		olc::GFX3D::vec3d vForward = olc::GFX3D::Math::Vec_Mul(vLookDir, 8.0f * fElapsedTime);

		renderer.SetCamera(vCamera, vTarget, vUp);

		Clear(olc::BLUE);

		olc::GFX3D::ClearDepth();

		renderer.SetTransform(matWorld);

		//renderer.SetTexture(cubeTex);
		//renderer.Render(cube.tris, olc::GFX3D::RENDERFLAGS::RENDER_TEXTURED);

		renderer.SetLightSource(5, 5, olc::DARK_GREEN, vCamera);

		renderer.SetTexture(cubeTex);
		renderer.Render(cube.tris);

		//renderer.Render(cube.tris);

		sMessage = sAppName + " - Camera x: " + std::to_string(vCamera.x);
		vecMessages.push_back(sMessage);

		//vLookTarget

		sMessage = sAppName + " - vLookTarget x: " + std::to_string(vTarget.x);
		vecMessages.push_back(sMessage);

		nStep = 10;
		for (auto& s : vecMessages)
		{
			DrawString(20, nStep, s);
			nStep += 10;
		}
		vecMessages.clear();


		olc::vi2d centreScreenPos = GetScreenSize();
		centreScreenPos.x = centreScreenPos.x / 2;
		centreScreenPos.y = centreScreenPos.y / 2;
		DrawTargetPointer(centreScreenPos, 50, 10);

		olc::vi2d leftCenterScreenPos = GetScreenSize();
		leftCenterScreenPos.x = leftCenterScreenPos.x / 100 * 25;
		leftCenterScreenPos.y = leftCenterScreenPos.y / 2;
		DrawTargetPointer(leftCenterScreenPos, 50, 10, olc::GREEN);

		olc::vi2d rightCenterScreenPos = GetScreenSize();
		rightCenterScreenPos.x = rightCenterScreenPos.x / 100 * 75;
		rightCenterScreenPos.y = rightCenterScreenPos.y / 2;
		DrawTargetPointer(rightCenterScreenPos, 50, 10, olc::RED);


		// Camera
		if (GetTouch().bHeld)
		{
			DrawLine(leftCenterScreenPos, GetTouchPos(), olc::GREEN, 0xF0F0F0F0);
			DrawTargetPointer(GetTouchPos(), 50, 10, olc::GREEN);

			// Moving Left
			if (GetTouchX(0) > leftCenterScreenPos.x)
			{
				fYaw += 1.0f * fElapsedTime;
			}

			// Moving Right
			if (GetTouchX(0) < leftCenterScreenPos.x)
			{
				fYaw -= 1.0f * fElapsedTime;
			}

			// Moving Forwards
			if (GetTouchY(0) < leftCenterScreenPos.y)
			{
				vCamera = olc::GFX3D::Math::Vec_Add(vCamera, vForward);
			}

			// Moving Backwards
			if (GetTouchY(0) > leftCenterScreenPos.y)
			{
				vCamera = olc::GFX3D::Math::Vec_Sub(vCamera, vForward);
			}



		}


		// Direction
		if (GetTouch(1).bHeld)
		{
			DrawLine(rightCenterScreenPos, GetTouchPos(1), olc::RED, 0xF0F0F0F0);
			DrawTargetPointer(GetTouchPos(1), 50, 10, olc::RED);

			// We know the Right Center point we need to compare our positions

			// Moving Left
			if (GetTouchX(1) > rightCenterScreenPos.x)
			{
				vCamera.x -= 8.0f * fElapsedTime;	// Travel Along X-Axis
			}

			// Moving Right
			if (GetTouchX(1) < rightCenterScreenPos.x)
			{
				vCamera.x += 8.0f * fElapsedTime;	// Travel Along X-Axis
			}

			// Moving Up
			if (GetTouchY(1) < rightCenterScreenPos.y)
			{
				vCamera.y += 8.0f * fElapsedTime;	// Travel Upwards
			}

			// Moving Down
			if (GetTouchY(1) > rightCenterScreenPos.y)
			{
				vCamera.y -= 8.0f * fElapsedTime;	// Travel Downwards
			}

		}

		//fTheta += fElapsedTime;

		return true;
	}

	bool OnUserDestroy() override {
		return true;
	}

	void OnSaveStateRequested() override
	{
		// Fires when the OS is about to put your game into pause mode
		// You have, at best 30 Seconds before your game will be fully shutdown
		// It depends on why the OS is pausing your game tho, Phone call, etc
		// It is best to save a simple Struct of your settings, i.e. current level, player position etc
		// NOTE: The OS can terminate all of your data, pointers, sprites, layers can be freed
		// Therefore do not save sprites, pointers etc 

		// Example 1: vector
		vecLastState.clear();
		vecLastState.push_back({ "MouseX", 55 });
		vecLastState.push_back({ "MouseY", 25 });
		vecLastState.push_back({ "GameLevel", 5 });

#if defined(__ANDROID__)
		// You can save files in the android Internal app storage
		const char* internalPath = app_GetInternalAppStorage(); //Android protected storage
#endif
#if defined(__APPLE__)
		// For iOS the internal app storage is read only, therefore we use External App Storage
		const char* internalPath = app_GetExternalAppStorage(); // iOS protected storage AKA /Library
#endif

		std::string dataPath(internalPath);

		// internalDataPath points directly to the files/ directory                                  
		std::string lastStateFile = dataPath + "/lastStateFile.bin";

		std::ofstream file(lastStateFile, std::ios::out | std::ios::binary);

		if (file)
		{
			float fVecSize = vecLastState.size();
			file.write((char*)&fVecSize, sizeof(long));
			for (auto& vSS : vecLastState)
			{
				file.write((char*)&vSS, sizeof(MySaveState));
			}

			file.close();
		}


	}

	void OnRestoreStateRequested() override
	{
		// This will fire every time your game launches 
		// OnUserCreate will be fired again as the OS may have terminated all your data

#if defined(__ANDROID__)
		// You can save files in the android Internal app storage
		const char* internalPath = app_GetInternalAppStorage(); //Android protected storage
#endif
#if defined(__APPLE__)
		// For iOS the internal app storage is read only, therefore we use External App Storage
		const char* internalPath = app_GetExternalAppStorage(); // iOS protected storage AKA /Library
#endif

		std::string dataPath(internalPath);
		std::string lastStateFile = dataPath + "/lastStateFile.bin";

		vecLastState.clear();

		std::ifstream file(lastStateFile, std::ios::in | std::ios::binary);

		MySaveState saveState;

		if (file)
		{
			float fVecSize = 0.0f;
			file.read((char*)&fVecSize, sizeof(long));
			for (long i = 0; i < fVecSize; i++)
			{
				file.read((char*)&saveState, sizeof(MySaveState));
				vecLastState.push_back(saveState);
			}

			file.close();
			// Note this is a temp file, we must delete it
			std::remove(lastStateFile.c_str());

		}


	}

};


#if defined (__ANDROID__)
/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
* This is now what drives the engine, the thread is controlled from the OS
*/
void android_main(struct android_app* initialstate) {

	/*
		initalstate allows you to make some more edits
		to your app before the PGE Engine starts
		Recommended just to leave it at its defaults
		but change it at your own risk
		to access the Android/iOS directly in your code
		android_app* pMyAndroid = this->pOsEngine.app;;

	*/

	PGE_Mobile demo;

	/*
		Note it is best to use HD(1280, 720, ? X ? pixel, Fullscreen = true) the engine can scale this best for all screen sizes,
		without affecting performance... well it will have a very small affect, it will depend on your pixel size
		Note: cohesion is currently not working
	*/
	demo.Construct(1280, 720, 2, 2, true, false, false);

	demo.Start(); // Lets get the party started


}

#endif

#if defined(__APPLE__)

/*
* The is the calling point from the iOS Objective C, called during the startup of your application
* Use the objects definded in IOSNativeApp to pass data to the Objective C
* By Default you must at minmum pass the game construct vars, pIOSNatvieApp->SetPGEConstruct
*
* iOS runs in its own threads, with its own
* event loop for receiving input events and doing other things.
* This is now what drives the engine, the thread is controlled from the OS
*/
int ios_main(IOSNativeApp* pIOSNatvieApp)
{
	// The iOS will instance your app differnetly to how Android does it
	// In the iOS it will automatically create the required classes and pointers
	// to get the PGE up and running successfull.

	// IMPORTANT: You must set your class name to PGE_Mobile (see above) always for iOS
	// Don't worry it will not conflict with any other apps that use the same base class name of PGE_Mobile
	// I got your back

	// Finally just like the Android you can access any avialble OS options using pIOSNatvieApp
	// Please note options will NOT be the same across both platforms
	// It is best to use the build in functions for File handling, Mouse/Touch events, Key events, Joypad etc

	//
	// To access the iOS directly in your code
	// auto* pMyApple = this->pOsEngine.app;
	//

	/*
		Note it is best to use HD(0, 0, ? X ? pixel, Fullscreen = true) the engine can scale this best for all screen sizes,
		without affecting performance... well it will have a very small affect, it will depend on your pixel size
		Note: cohesion is currently not working
		Note: It is best to set maintain_aspect_ratio to false, Fullscreen to true and use the olcPGEX_TransformView.h to manage your world-view
		in short iOS does not want to play nice, the screen ratios and renta displays make maintaining a full screen with aspect radio a pain to manage
	*/
	pIOSNatvieApp->SetPGEConstruct(0, 0, 2, 2, true, true, false);


	// We now need to return SUCCESS or FAILURE to get the party stated!!!!
	return EXIT_SUCCESS;
}

#endif


