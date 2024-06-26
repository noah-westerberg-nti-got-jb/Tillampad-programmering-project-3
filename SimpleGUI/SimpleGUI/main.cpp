#include <iostream>
#include <raylib.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include "GUIElements.h"

#define MOVE_UP_RIGHT (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A))
#define MOVE_BOTTOM_LEFT (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_D))

int main(int argc, char* argv[]) {
	// //Initialization

	std::ifstream file(argv[1]);
	nlohmann::json json;
	file >> json;

	const int screenWidth = json["window_size"][0];
	const int screenHeight = json["window_size"][1];
	InitWindow(screenWidth, screenHeight, ((std::string)json["window_name"]).c_str());

	// Initialisering av variabler
	float framesPerSecond = 0;

	int framesPassed = 0;

	std::string isFrameEven;

	int currentLevel = 1;
	int framesPassedCurrentLevel = 0;
	std::string framesPassedSuffix = "";

	// Initialisering av variabler behållaren
	Variables variables;
	variables.variablesInt.push_back({ "frames_passed", &framesPassed });
	variables.variablesFloat.push_back({ "frames_per_second", &framesPerSecond });
	variables.variablesString.push_back({ "even_or_odd_frame", &isFrameEven });
	variables.variablesInt.push_back({ "frames_passed_prefix", &framesPassedCurrentLevel });
	variables.variablesString.push_back({ "frames_passed_suffix", &framesPassedSuffix });

	// Initialisering av GUI:n
	Window baseLayer(json, &variables);
	Window* currentSelection = &baseLayer;

	SetTargetFPS(60); // Set our game to run at 60 frames-per-second when possible

	// // `WindowShouldClose` detects window close
	// // Main game loop
	while (!WindowShouldClose()) {
		// // Update;

		if ((framesPassed) % 2 == 0)
			isFrameEven = "Yes";
		else
			isFrameEven = "No";

		if (framesPassed % currentLevel == 0)
			framesPassedCurrentLevel++;

		if (framesPassed != 0) {
			if (framesPassed % 100 == 0 && currentLevel < 100) {
				framesPassedCurrentLevel = 1;
				currentLevel = 100;
				framesPassedSuffix = "hundred";
			}
			else if (framesPassed % 1000 == 0 && currentLevel < 1000) {
				framesPassedCurrentLevel = 1;
				currentLevel = 1000;
				framesPassedSuffix = "thousand";
			}
			else if (framesPassed % 1000000 == 0 && currentLevel < 1000000) {
				framesPassedCurrentLevel = 1;
				currentLevel = 1000000;
				framesPassedSuffix = "million";
			}
		}

		// Input
		if (IsKeyPressed(KEY_SPACE)) {
			currentSelection = currentSelection->EnterSelection();
		}
		else if (IsKeyPressed(KEY_BACKSPACE)) {
			currentSelection = currentSelection->ExitWindow();
		}
		else if (MOVE_UP_RIGHT) {
			currentSelection->MoveCursor(-1);
		}
		else if (MOVE_BOTTOM_LEFT) {
			currentSelection->MoveCursor(1);
		}

		// // Draw
		BeginDrawing();

		baseLayer.Draw();

		EndDrawing();

		framesPassed++;

		framesPerSecond = 1 / GetFrameTime();
	}

	// // De-Initialization

	CloseWindow(); // // Close window and OpenGL context

	return 0;
}
