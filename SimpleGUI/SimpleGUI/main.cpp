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
	int direction;

	Color color;

public:
	Layer(int width, int height, int direction, int cornerXPosition, int cornerYPostiion, Color color) {
		this->width = width;
		this->height = height;
		this->direction = direction;
		this->cornerXPosition = cornerXPosition;
		this->cornerYPostiion = cornerYPostiion;
		this->color = color;

		bool reverse = false;
		/*switch (direction) {
		case Horizontal:
			float windowSizeRatio = (width % windowSize);
			numberOfWindows = windowSizeRatio - (windowSizeRatio % 1);
			break;;
		case Vertical:
			float windowSizeRatio = (height % windowSize);
			numberOfWindows = windowSizeRatio - (windowSizeRatio % 1);
			break;
		};*/

	}

	void DrawBorder() {
		BeginDrawing();
		DrawRectangleLines(cornerXPosition, cornerYPostiion, width, height, color);
		EndDrawing();
	}
};

int main(int argc, char* argv[]) {
	 //Initialization

	std::ifstream file("app1.json");
	nlohmann::json json;
	file >> json;

	const int screenWidth = 960;
	const int screenHeight = 720;

	InitWindow(screenWidth, screenHeight, "Simple GUI");
	SetTargetFPS(60); // Set our game to run at 60 frames-per-second when possible

	Color color;
	int colorSwitch = 0;
	float time = 0;
	// `WindowShouldClose` detects window close
	// Main game loop
	while (!WindowShouldClose()) {
		// Update;
		if (IsKeyDown(KEY_SPACE)) {
			time += GetFrameTime();
			if (time > 0.1) {
				time = 0;
				colorSwitch++;
				if (colorSwitch >= 8) colorSwitch = 0;
			}
		}
		switch (colorSwitch)
		{
		case 0:
			color = WHITE;
			break;
		case 1:
			color = GRAY;
			break;
		case 2:
			color = RED;
			break;
		case 3:
			color = BLUE;
			break;
		case 4:
			color = DARKBLUE;
			break;
		case 5:
			color = GREEN;
			break;
		case 6:
			color = DARKGREEN;
			break;
		case 7:
			color = ORANGE;
			break;
		}

		// Draw

		BeginDrawing();
		ClearBackground(BLACK);

		DrawText(TextFormat("Delta time: %f", time), 10, 10, 30, WHITE);

		const int fontSize = 50;
		DrawText("Hello, world!", ((float)screenWidth / 2) - (((float)fontSize * 6) / 2), (float)screenHeight / 2, fontSize, color);

		EndDrawing();
	}

	// De-Initialization

	CloseWindow(); // Close window and OpenGL context

	return 0;
}