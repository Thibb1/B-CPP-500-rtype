/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** Button.cpp by thibb1
*/

#include "button.hpp"

Button::Button(const std::string &text, Rectangle bounds, bool enabled) {
    _text = text;
    _bounds = bounds;
    _pressed = false;
    _hovered = false;
    _enabled = enabled;
    _baseColor = LIGHTGRAY;
    _selectedColor = DARKGRAY;
    _hoveredColor = SKYBLUE;
    _borderColor = GRAY;
    _selectedBorderColor = BLUE;
    _textPosition.x = _bounds.x + _bounds.width / 2 - float(MeasureText(_text.c_str(), 20)) / 2;
    _textPosition.y = _bounds.y + _bounds.height / 2 - 10;
    _callback = nullptr;
}

bool Button::UpdateButton() {
    if (!_enabled)
        return false;
    Vector2 mousePosition = GetMousePosition();
    if (CheckCollisionPointRec(mousePosition, _bounds)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            _pressed = true;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && _pressed) {
            _pressed = false;
            if (_callback != nullptr)
                _callback();
            return true;
        }
        _hovered = true;
    } else {
        _pressed = false;
        _hovered = false;
    }
    return false;
}

void Button::Draw() {
    if (!_enabled)
        return;
    else if (_pressed) {
        DrawRectangleRec(_bounds, _selectedColor);
        DrawRectangleLinesEx(_bounds, 2, _selectedBorderColor);
    } else if (_hovered) {
        DrawRectangleRec(_bounds, _hoveredColor);
        DrawRectangleLinesEx(_bounds, 2, _borderColor);
    } else {
        DrawRectangleRec(_bounds, _baseColor);
        DrawRectangleLinesEx(_bounds, 2, _borderColor);
    }
    DrawText(_text.c_str(), int(_textPosition.x), int(_textPosition.y), 20, BLACK);
}

void Button::SetEnabled(bool enabled) { _enabled = enabled; }
