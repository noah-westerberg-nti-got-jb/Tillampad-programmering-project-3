#include <raylib.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#define Horizontal 0
#define Vertical 1

class Layer {
	int width;
	int height;
	int padding;
	// position for top left corner
	int cornerXPosition;
	int cornerYPostiion;

	Layer* parentLayer;
	std::vector<Layer*> subLayers;
	int numberOfWindows;
	bool selected = false;

	int cursorPosition = 0;

	Color borderColor, selectionColor, backgroundColor, textColor;


	void DrawBorder() {
		DrawRectangleLines(cornerXPosition + padding, cornerYPostiion + padding, width - (2 * padding), height - (2 * padding), borderColor);
	}

	void DrawBackground() {
		DrawRectangle(cornerXPosition + 1, cornerYPostiion + 1, width - 2, height - 2, backgroundColor);
	}

	void DrawSelectionBorder() {
		DrawRectangleLines(cornerXPosition + (padding / 2), cornerYPostiion + (padding / 2), width - padding, height - padding, selectionColor);
	}

	template<typename Type>
	void InheritValue(Type* variable, Type backupValue, nlohmann::json layerJson, std::string key) {
		if (layerJson[key] == nullptr)
			*variable = backupValue;
		else
			*variable = layerJson[key];
	}

	void InheritColor(Color* color, Color backupColor, nlohmann::json layerJson, std::string key) {
		if (layerJson[key] == nullptr)
			*color = backupColor;
		else
			*color = { layerJson[key][0], layerJson[key][1], layerJson[key][2], layerJson[key][3] };
	}
public:
	Layer(int width, int height, int cornerXPosition, int cornerYPostiion, Layer* parentLayer, nlohmann::json layerJson) {
		this->parentLayer = parentLayer;
		this->width = width;
		this->height = height;
		this->cornerXPosition = cornerXPosition;
		this->cornerYPostiion = cornerYPostiion;
		if (parentLayer != nullptr) {
			InheritValue<int>(&this->padding, parentLayer->padding, layerJson, "padding");
			InheritColor(&this->borderColor, parentLayer->borderColor, layerJson, "border_color");
			InheritColor(&this->selectionColor, parentLayer->selectionColor, layerJson, "selection_color");
			InheritColor(&this->backgroundColor, parentLayer->backgroundColor, layerJson, "background_color");
			InheritColor(&this->textColor, parentLayer->textColor, layerJson, "text_color");
		}
		else {
			this->padding = layerJson["padding"];
			this->borderColor = { layerJson["border_color"][0], layerJson["border_color"][1], layerJson["border_color"][2], layerJson["border_color"][3] };
			this->selectionColor = { layerJson["selection_color"][0], layerJson["selection_color"][1], layerJson["selection_color"][2], layerJson["selection_color"][3] };
			this->backgroundColor = { layerJson["background_color"][0], layerJson["background_color"][1], layerJson["background_color"][2], layerJson["background_color"][3] };
			this->textColor = { layerJson["text_color"][0], layerJson["text_color"][1], layerJson["text_color"][2], layerJson["text_color"][3] };
		}
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
			subLayers.emplace_back(new Layer(subLayerWidth, subLayerHeight, subLayerCornerXPosition, SubLayerCornerYPosition, this, layerJson["sections"][i]));
			*shiftingCorner += *variableSide;
		}
	}

	void Draw() {
		DrawBackground();
		DrawBorder();
		if (selected)
			DrawSelectionBorder();

		for (int i = 0; i < numberOfWindows; i++) {
			subLayers[i]->Draw();
		}
	}

	void SetSelected(bool value) {
		this->selected = value;
	}

	void RemoveSelection() {
		if (numberOfWindows >= 1)
			subLayers[cursorPosition]->SetSelected(false);
	}

	void MoveCursor(int direction) {
		if (numberOfWindows >= 1)
		{
			subLayers[cursorPosition]->SetSelected(false);
			cursorPosition += direction;
			if (cursorPosition >= numberOfWindows)
				cursorPosition = numberOfWindows - 1;
			else if (cursorPosition <= 0)
				cursorPosition = 0;
			subLayers[cursorPosition]->SetSelected(true);
		}
	}

	Layer* EnterSubLayer() {
		if (numberOfWindows >= 1) {
			subLayers[cursorPosition]->RemoveSelection();
			subLayers[cursorPosition]->MoveCursor(0);
			return subLayers[cursorPosition];
		}
		return this;
	}

	Layer* ExitLayer() {
		if (parentLayer != nullptr) {
			RemoveSelection();
			parentLayer->MoveCursor(0);
			return parentLayer;
		}
		return this;
	}
};