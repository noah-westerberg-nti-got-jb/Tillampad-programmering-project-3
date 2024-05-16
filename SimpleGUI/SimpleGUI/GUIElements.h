#include <iostream>
#include <raylib.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#define Horizontal 0
#define Vertical 1

template<typename Type>
struct Variable {
	const char* name;
	Type* value;
};

class Variables {
public:
	std::vector<Variable<int>> variablesInt;
	std::vector<Variable<float>> variablesFloat;
	std::vector<Variable<std::string>> variablesString;

	template <typename Type>
	Variable<Type>* FindVariable(std::string name, std::string type) {
		if (type == "int") {
			return FindVariableInt(name);
		}
		else if (type == "float") {
			return FindVariableFloat(name);
		}
		else if (type == "string") {
			return FindVariableString(name);
		}
	}

	Variable<int>* FindVariableInt(std::string name) {
		for (int i = 0; i < variablesInt.size(); i++) {
			if (variablesInt[i].name == name)
				return &variablesInt[i];
		}
	}

	Variable<float>* FindVariableFloat(std::string name) {
		for (int i = 0; i < variablesFloat.size(); i++) {
			if (variablesFloat[i].name == name)
				return &variablesFloat[i];
		}
	}

	Variable<std::string>* FindVariableString(std::string name) {
		for (int i = 0; i < variablesString.size(); i++) {
			if (variablesString[i].name == name)
				return &variablesString[i];
		}
	}

};

template<typename Type>
void InheritValue(Type* variable, Type inheritanceVariable, nlohmann::json json, std::string key) {
	if (json[key] == nullptr)
		*variable = inheritanceVariable;
	else
		*variable = json[key];
}

void InheritColor(Color* color, Color inheritanceColor, nlohmann::json json, std::string key) {
	if (json[key] == nullptr)
		*color = inheritanceColor;
	else
		*color = { json[key][0], json[key][1], json[key][2], json[key][3] };
}

class Text {
	int width, height;
	int cornerXPosition, cornerYPostiion;

	const char* text;
	float fontSize;
	Color textColor;
	Font font;
	Rectangle rec;

	nlohmann::json json;
	Variables* variables;

public:
	Text(int width, int height, int cornerXPosition, int cornerYPostiion, float fontSize, Color textColor, nlohmann::json textJson, Variables* variables) {
		this->width = width;
		this->height = height;
		this->cornerXPosition = cornerXPosition;
		this->cornerYPostiion = cornerYPostiion;
		this->json = textJson;
		this->variables = variables;
		InheritValue<float>(&this->fontSize, fontSize, textJson, "font_size");
		InheritColor(&this->textColor, textColor, textJson, "text_color");
		font = GetFontDefault();
		rec = { (float)cornerXPosition + 4, (float)cornerYPostiion + 4, (float)width - 4, (float)height - 4 };
	}

	void UpdateText() {
		text = "";

		for (int i = 0; i < json["number_of_text_segments"]; i++) {
			std::string combinedString = std::string(text) + (std::string)(json["text_segments"][i]["text"]);
			if (json["text_segments"][i]["variable"] != nullptr && variables != nullptr) {
				auto variable = json["text_segments"][i]["variable"];
				std::string variableName = variable["name"], variableType = variable["type"];
				if (variableType == "int") {
					Variable<int>* variablePointer = variables->FindVariableInt(variableName);
					text = TextFormat(combinedString.c_str(), *(variablePointer->value));
				}
				else if (variableType == "float") {
					Variable<float>* variablePointer = variables->FindVariableFloat(variableName);
					text = TextFormat(combinedString.c_str(), *(variablePointer->value));
				}
				else if (variableType == "string") {
					Variable<std::string>* variablePointer = variables->FindVariableString(variableName);
					text = TextFormat(combinedString.c_str(), (variablePointer->value)->c_str());
				}
				else
					text = TextFormat(combinedString.c_str());
			}
			else
				text = TextFormat(combinedString.c_str());
		}
	}

	// https://github.com/raysan5/raylib/blob/master/examples/text/text_rectangle_bounds.c
	// Draw text using font inside rectangle limits with support for text selection
	void Draw() {
		UpdateText();

		int selectStart = 0, selectLength = 0;
		float spacing = 2.0;
		bool wordWrap = true;
		Color selectTint = WHITE, selectBackTint = WHITE;

		int length = TextLength(text);  // Total length in bytes of the text, scanned by codepoints in loop

		float textOffsetY = 0;          // Offset between lines (on line break '\n')
		float textOffsetX = 0.0f;       // Offset X to next character to draw

		float scaleFactor = fontSize / (float)font.baseSize;     // Character rectangle scaling factor

		// Word/character wrapping mechanism variables
		enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
		bool state = wordWrap ? MEASURE_STATE : DRAW_STATE;

		int startLine = -1;         // Index where to begin drawing (where a line begins)
		int endLine = -1;           // Index where to stop drawing (where a line ends)
		int lastk = -1;             // Holds last value of the character position

		for (int i = 0, k = 0; i < length; i++, k++)
		{
			// Get next codepoint from byte string and glyph index in font
			int codepointByteCount = 0;
			int codepoint = GetCodepoint(&text[i], &codepointByteCount);
			int index = GetGlyphIndex(font, codepoint);

			// NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
			// but we need to draw all of the bad bytes using the '?' symbol moving one byte
			if (codepoint == 0x3f) codepointByteCount = 1;
			i += (codepointByteCount - 1);

			float glyphWidth = 0;
			if (codepoint != '\n')
			{
				glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width * scaleFactor : font.glyphs[index].advanceX * scaleFactor;

				if (i + 1 < length) glyphWidth = glyphWidth + spacing;
			}

			// NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
			// We store this info in startLine and endLine, then we change states, draw the text between those two variables
			// and change states again and again recursively until the end of the text (or until we get outside of the container).
			// When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
			// and begin drawing on the next line before we can get outside the container.
			if (state == MEASURE_STATE)
			{
				// TODO: There are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
				// Ref: http://jkorpela.fi/chars/spaces.html
				if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) endLine = i;

				if ((textOffsetX + glyphWidth) > rec.width)
				{
					endLine = (endLine < 1) ? i : endLine;
					if (i == endLine) endLine -= codepointByteCount;
					if ((startLine + codepointByteCount) == endLine) endLine = (i - codepointByteCount);

					state = !state;
				}
				else if ((i + 1) == length)
				{
					endLine = i;
					state = !state;
				}
				else if (codepoint == '\n') state = !state;

				if (state == DRAW_STATE)
				{
					textOffsetX = 0;
					i = startLine;
					glyphWidth = 0;

					// Save character position when we switch states
					int tmp = lastk;
					lastk = k - 1;
					k = tmp;
				}
			}
			else
			{
				if (codepoint == '\n')
				{
					if (!wordWrap)
					{
						textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
						textOffsetX = 0;
					}
				}
				else
				{
					if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
					{
						textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
						textOffsetX = 0;
					}

					// When text overflows rectangle height limit, just stop drawing
					if ((textOffsetY + font.baseSize * scaleFactor) > rec.height) break;

					// Draw selection background
					bool isGlyphSelected = false;
					if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
					{
						DrawRectangleRec({ rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize * scaleFactor }, selectBackTint);
						isGlyphSelected = true;
					}

					// Draw current character glyph
					if ((codepoint != ' ') && (codepoint != '\t'))
					{
						DrawTextCodepoint(font, codepoint, { rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, isGlyphSelected ? selectTint : textColor);
					}
				}

				if (wordWrap && (i == endLine))
				{
					textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
					textOffsetX = 0;
					startLine = endLine;
					endLine = -1;
					glyphWidth = 0;
					selectStart += lastk - k;
					k = lastk;

					state = !state;
				}
			}

			if ((textOffsetX != 0) || (codepoint != ' ')) textOffsetX += glyphWidth;  // avoid leading spaces
		}
	}
};

class Window {
	int width;
	int height;
	int padding;
	// position for top left corner
	int cornerXPosition;
	int cornerYPostiion;

	Window* parentWindow;
	std::vector<Window*> subWindows;
	int numberOfWindows = 0;

	bool selected = false;

	int cursorPosition = 0;

	Color borderColor, selectionColor, backgroundColor, textColor;

	Text* text;
	float fontSize;

	void DrawBorder() {
		DrawRectangleLines(cornerXPosition + padding, cornerYPostiion + padding, width - (2 * padding), height - (2 * padding), borderColor);
	}

	void DrawBackground() {
		DrawRectangle(cornerXPosition + 1, cornerYPostiion + 1, width - 2, height - 2, backgroundColor);
	}

	void DrawSelectionBorder() {
		DrawRectangleLines(cornerXPosition + (padding / 2), cornerYPostiion + (padding / 2), width - padding, height - padding, selectionColor);
	}

	void CreateSubWindows(int width, int height, int cornerXPosition, int cornerYPosition, nlohmann::json parrentWindowJson, Variables* variables) {
		int subLayerCornerXPosition = cornerXPosition + this->padding, SubLayerCornerYPosition = cornerYPostiion + this->padding;
		int* shiftingCorner = &subLayerCornerXPosition;
		int subLayerWidth = width - (2 * this->padding), subLayerHeight = height - (2 * this->padding);
		int* variableSide = &subLayerWidth;
		switch ((int)parrentWindowJson["direction"])
		{
		case Horizontal:
			subLayerWidth = (width / parrentWindowJson["number_of_sections"]) - this->padding;
			variableSide = &subLayerWidth;
			shiftingCorner = &subLayerCornerXPosition;
			break;
		case Vertical:
			subLayerHeight = (height / parrentWindowJson["number_of_sections"]) - this->padding;
			variableSide = &subLayerHeight;
			shiftingCorner = &SubLayerCornerYPosition;
			break;
		}
		for (int i = 0; i < numberOfWindows; i++) {
			auto type = parrentWindowJson["sections"][i]["type"];

			if (type == "window")
				subWindows.emplace_back(new Window(subLayerWidth, subLayerHeight, subLayerCornerXPosition, SubLayerCornerYPosition, this, parrentWindowJson["sections"][i], variables));
			else if (type == "text")
				text = new Text(subLayerWidth, subLayerHeight, subLayerCornerXPosition, SubLayerCornerYPosition, fontSize, textColor, parrentWindowJson["sections"][i], variables);

			*shiftingCorner += *variableSide;
		}
	}
public:
	Window(nlohmann::json json, Variables* variables) {
		this->parentWindow = nullptr;
		this->width = json["window_size"][0];
		this->height = json["window_size"][1];;
		this->cornerXPosition = 0;
		this->cornerYPostiion = 0;
		this->padding = json["padding"];
		this->fontSize = json["font_size"];
		this->borderColor = { json["border_color"][0], json["border_color"][1], json["border_color"][2], json["border_color"][3] };
		this->selectionColor = { json["selection_color"][0], json["selection_color"][1], json["selection_color"][2], json["selection_color"][3] };
		this->backgroundColor = { json["background_color"][0], json["background_color"][1], json["background_color"][2], json["background_color"][3] };
		this->textColor = { json["text_color"][0], json["text_color"][1], json["text_color"][2], json["text_color"][3] };
		numberOfWindows = json["number_of_sections"];
		if (numberOfWindows == 0) return;

		CreateSubWindows(this->width, this->height, this->cornerXPosition, this->cornerYPostiion, json, variables);
	}
	Window(int width, int height, int cornerXPosition, int cornerYPostiion, Window* parentWindow, nlohmann::json windowJson, Variables* variables) {
		this->parentWindow = parentWindow;
		this->width = width;
		this->height = height;
		this->cornerXPosition = cornerXPosition;
		this->cornerYPostiion = cornerYPostiion;
		if (parentWindow != nullptr) {
			InheritValue<int>(&this->padding, parentWindow->padding, windowJson, "padding");
			InheritValue<float>(&this->fontSize, parentWindow->fontSize, windowJson, "font_size");
			InheritColor(&this->borderColor, parentWindow->borderColor, windowJson, "border_color");
			InheritColor(&this->selectionColor, parentWindow->selectionColor, windowJson, "selection_color");
			InheritColor(&this->backgroundColor, parentWindow->backgroundColor, windowJson, "background_color");
			InheritColor(&this->textColor, parentWindow->textColor, windowJson, "text_color");
		}
		else {
			this->padding = windowJson["padding"];
			this->fontSize = windowJson["font_size"];
			this->borderColor = { windowJson["border_color"][0], windowJson["border_color"][1], windowJson["border_color"][2], windowJson["border_color"][3] };
			this->selectionColor = { windowJson["selection_color"][0], windowJson["selection_color"][1], windowJson["selection_color"][2], windowJson["selection_color"][3] };
			this->backgroundColor = { windowJson["background_color"][0], windowJson["background_color"][1], windowJson["background_color"][2], windowJson["background_color"][3] };
			this->textColor = { windowJson["text_color"][0], windowJson["text_color"][1], windowJson["text_color"][2], windowJson["text_color"][3] };
		}
		numberOfWindows = windowJson["number_of_sections"];
		if (numberOfWindows == 0) return;

		CreateSubWindows(this->width, this->height, this->cornerXPosition, this->cornerYPostiion, windowJson, variables);
	}

	void Draw() {
		DrawBackground();
		DrawBorder();
		if (selected)
			DrawSelectionBorder();
		if (text != nullptr)
			text->Draw();


		for (int i = 0; i < subWindows.size(); i++) {
			subWindows[i]->Draw();
		}
	}

	void SetSelected(bool value) {
		this->selected = value;
	}

	void RemoveSelection() {
		if (subWindows.size() >= 1)
			subWindows[cursorPosition]->SetSelected(false);
	}

	void MoveCursor(int direction) {
		if (subWindows.size() >= 1)
		{
			subWindows[cursorPosition]->SetSelected(false);
			cursorPosition += direction;
			if (cursorPosition >= numberOfWindows)
				cursorPosition = numberOfWindows - 1;
			else if (cursorPosition <= 0)
				cursorPosition = 0;
			subWindows[cursorPosition]->SetSelected(true);
		}
	}

	Window* EnterSelf() {
		if (subWindows.size() >= 1)
			return this;
		else 
			return parentWindow;
	}

	Window* EnterSelection() {
		if (subWindows.size() >= 1) {
			subWindows[cursorPosition]->RemoveSelection();
			subWindows[cursorPosition]->MoveCursor(0);
			return subWindows[cursorPosition]->EnterSelf();
		}
		return this;
	}

	Window* ExitWindow() {
		if (parentWindow != nullptr) {
			RemoveSelection();
			parentWindow->MoveCursor(0);
			return parentWindow;
		}
		return this;
	}
};