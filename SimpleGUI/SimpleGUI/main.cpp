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

	Layer baseLayer(json);
	Layer* currentLayer = &baseLayer;

	InitWindow(screenWidth, screenHeight, "Simple GUI");
	SetTargetFPS(60); // Set our game to run at 60 frames-per-second when possible

	// `WindowShouldClose` detects window close
	// Main game loop
	while (!WindowShouldClose()) {
		// Update;
		
		if (IsKeyPressed(KEY_SPACE)) {
			currentLayer = currentLayer->EnterSubLayer();
		}
		else if (IsKeyPressed(KEY_BACKSPACE)) {
			currentLayer = currentLayer->ExitLayer();
		}
		else if (MOVE_UP_RIGHT) {
			currentLayer->MoveCursor(-1);
		}
		else if (MOVE_BOTTOM_LEFT) {
			currentLayer->MoveCursor(1);
		}
		
		// Draw
		BeginDrawing();
		
		//ClearBackground(BLACK);

		baseLayer.Draw();

		EndDrawing();
		
		std::cout << "fps: " << 1 / GetFrameTime() << std::endl;
	}

	// De-Initialization

	CloseWindow(); // Close window and OpenGL context

	return 0;
}