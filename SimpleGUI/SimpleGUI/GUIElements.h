#include <iostream>
#include <raylib.h>
#include <nlohmann/json.hpp>
#include <vector>

#define Horizontal 0
#define Vertical 1

class Layer {
	int width;
	int height;
	int padding;
	// position for top left corner
	int cornerXPosition;
	int cornerYPostiion;


	std::vector<Layer*> subLayers;
	int numberOfWindows;
	bool selected = true;

	int markerPosition = 0;


	Color borderColor, selectionColor;


	void DrawBorder() {
		BeginDrawing();
		DrawRectangleLines(cornerXPosition + padding, cornerYPostiion + padding, width - (2 * padding), height - (2 * padding), borderColor);
		EndDrawing();
	}

	void DrawSelectionBorder() {
		BeginDrawing();
		DrawRectangleLines(cornerXPosition + (padding / 2), cornerYPostiion + (padding / 2), width - padding, height - padding, selectionColor);
		EndDrawing();
	}
public:
	Layer(int width, int height, int cornerXPosition, int cornerYPostiion, int padding, Color borderColor, Color selectionColor, nlohmann::json layerJson) {
		this->width = width;
		this->height = height;
		this->cornerXPosition = cornerXPosition;
		this->cornerYPostiion = cornerYPostiion;
		if (layerJson["padding"] == nullptr)
			this->padding = padding;
		else
			this->padding = layerJson["padding"];
		if (layerJson["color"] == nullptr)
			this->borderColor = borderColor;
		else
			this->borderColor = { layerJson["color"][0], layerJson["color"][1], layerJson["color"][2], layerJson["color"][3] };
		if (layerJson["selection_color"] == nullptr)
			this->selectionColor = selectionColor;
		else
			this->selectionColor = { layerJson["selection_color"][0], layerJson["selection_color"][1], layerJson["selection_color"][2], layerJson["selection_color"][3] };
		numberOfWindows = layerJson["number_of_sections"];
		if (numberOfWindows == 0) return;


		int subLayerCornerXPosition = cornerXPosition + this->padding, SubLayerCornerYPosition = cornerYPostiion + this->padding;
		int* shiftingCorner = &subLayerCornerXPosition;
		int subLayerWidth = width - (2 * this->padding), subLayerHeight = height - (2 * this->padding);
		int* variableSide = &subLayerWidth;
		switch ((int)layerJson["direction"])
		{
		case Horizontal:
			// ooptimalt att sätta samma värde flera gånger, men koden blir mer "clean"
			subLayerWidth = (width / layerJson["number_of_sections"]) - this->padding;
			variableSide = &subLayerWidth;
			shiftingCorner = &subLayerCornerXPosition;
			break;
		case Vertical:
			subLayerHeight = (height / layerJson["number_of_sections"]) - this->padding;
			variableSide = &subLayerHeight;
			shiftingCorner = &SubLayerCornerYPosition;
			break;
		}
		for (int i = 0; i < numberOfWindows; i++) {
			subLayers.emplace_back(new Layer(subLayerWidth, subLayerHeight, subLayerCornerXPosition, SubLayerCornerYPosition, this->padding, this->borderColor, this->selectionColor, layerJson["sections"][i]));
			*shiftingCorner += *variableSide;
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