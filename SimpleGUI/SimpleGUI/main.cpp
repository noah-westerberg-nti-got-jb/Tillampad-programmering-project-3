#include <iostream>
#include <iomanip>
#include <raylib.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include "GUIElements.h"

int main(int argc, char* argv[]) {
	//Initialization 

	std::ifstream file("app1.json");
	nlohmann::json json;
	file >> json;

	const int screenWidth = 960;
	const int screenHeight = 720;

	Layer baseLayer(screenWidth, screenHeight, 0, 0, json["padding"], { json["color"][0], json["color"][1], json["color"][2], json["color"][3] }, { json["selection_color"][0], json["selection_color"][1], json["selection_color"][2], json["selection_color"][3] }, json);

	InitWindow(screenWidth, screenHeight, "Simple GUI");
	SetTargetFPS(60); // Set our game to run at 60 frames-per-second when possible

	// `WindowShouldClose` detects window close
	// Main game loop
	while (!WindowShouldClose()) {
		// Update;

		// Draw

		BeginDrawing();

		//std::cout << "frame start" << "\n";

		baseLayer.Draw();

		EndDrawing();
	}

	// De-Initialization

	CloseWindow(); // Close window and OpenGL context

	return 0;
}