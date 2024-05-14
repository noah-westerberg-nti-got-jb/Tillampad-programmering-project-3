#include <raylib.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#define Horizontal 0
#define Vertical 1

class Layer {
	int width;
	int height;
	// position for top left corner
	int cornerXPosition;
	int cornerYPostiion;

	std::vector<Layer*> subLayers;
	int numberOfWindows;
	bool selected = true;

	int markerPosition = 0;

	Color color;

	void DrawBorder() {
		BeginDrawing();
		DrawRectangleLines(cornerXPosition + 10, cornerYPostiion + 10, width - 20, height - 20, color);
		EndDrawing();
	}

	void DrawSelectionBorder() {
		BeginDrawing();
		DrawRectangleLines(cornerXPosition, cornerYPostiion, width, height, color);
		EndDrawing();
	}
public:
	Layer(int width, int height, int cornerXPosition, int cornerYPostiion, nlohmann::json layerJson) {
		this->width = width;
		this->height = height;
		this->cornerXPosition = cornerXPosition;
		this->cornerYPostiion = cornerYPostiion;
		this->color = WHITE; // temp
		this->numberOfWindows = layerJson["number_of_sections"];
		if (this->numberOfWindows <= 1)
			this->numberOfWindows = 0;
		
			int subLayerCornerXPosition = cornerXPosition, SubLayerCornerYPosition = cornerYPostiion;
			int subLayerWidth = width, subLayerHeight = height;
			for (int i = 0; i < this->numberOfWindows; i++) {

				switch ((int)layerJson["direction"])
				{
				case Horizontal:
					// ooptimalt att sätta samma värde flera gånger, men koden blir mer "clean"
					subLayerWidth = width / layerJson["number_of_sections"];
					subLayerCornerXPosition += subLayerWidth;
					break;
				case Vertical:
					subLayerHeight = height / layerJson["number_of_sections"];
					SubLayerCornerYPosition += subLayerHeight;
					break;
				}
				
				subLayers.emplace_back(new Layer(subLayerWidth, subLayerHeight, subLayerCornerXPosition, SubLayerCornerYPosition, layerJson["sections"][i]));
			}
		 
	}

	void Draw() {
		DrawBorder();
		if (selected)
			DrawSelectionBorder();
		
		for (int i = 0; i < numberOfWindows; i++) {
			subLayers[i]->Draw();
		}
	}
};

int main(int argc, char* argv[]) {
	//Initialization

	std::ifstream file("app1.json");
	nlohmann::json json;
	file >> json;

	const int screenWidth = 960;
	const int screenHeight = 720;

	Layer baseLayer(screenWidth, screenHeight, 0, 0, json);

	InitWindow(screenWidth, screenHeight, "Simple GUI");
	SetTargetFPS(60); // Set our game to run at 60 frames-per-second when possible

	// `WindowShouldClose` detects window close
	// Main game loop
	while (!WindowShouldClose()) {
		// Update;

		// Draw

		BeginDrawing();
		
		baseLayer.Draw();

		EndDrawing();
	}

	// De-Initialization

	CloseWindow(); // Close window and OpenGL context

	return 0;
}