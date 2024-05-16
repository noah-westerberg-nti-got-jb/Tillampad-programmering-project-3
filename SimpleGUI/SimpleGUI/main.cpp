#include <iostream>
#include <iomanip>
#include <raylib.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include "GUIElements.h"

#define MOVE_UP_RIGHT (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A))
#define MOVE_BOTTOM_LEFT (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_D))

int main(int argc, char* argv[]) {
	//Initialization 

	std::ifstream file("app1.json");
	nlohmann::json json;
	file >> json;

	const int screenWidth = json["window_size"][0];
	const int screenHeight = json["window_size"][1];
	InitWindow(screenWidth, screenHeight, "Simple GUI");

	int framesPassed = 0;
	float framesPerSecond = 0;
	std::string isFrameEven;

	Variables variables;
	variables.variablesInt.push_back({ "frames_passed", &framesPassed });
	variables.variablesFloat.push_back({ "frames_per_second", &framesPerSecond });
	variables.variablesString.push_back({ "even_or_odd_frame", &isFrameEven });

	Window baseLayer(json, &variables);
	Window* currentSelection = &baseLayer;

	SetTargetFPS(60); // Set our game to run at 60 frames-per-second when possible

	// `WindowShouldClose` detects window close
	// Main game loop
	while (!WindowShouldClose()) {
		// Update;
		
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
		
		// Draw
		BeginDrawing();
		
		//ClearBackground(BLACK);

		baseLayer.Draw();

		EndDrawing();
		 
		framesPassed++;
		if ((framesPassed + 1) % 2 == 0)
			isFrameEven = "Yes";
		else
			isFrameEven = "No";
		framesPerSecond = 1 / GetFrameTime();
		std::cout << "fps: " << framesPerSecond << std::endl;
	}

	// De-Initialization

	CloseWindow(); // Close window and OpenGL context

	return 0;
}
