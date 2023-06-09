/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** CText.hpp
*/

#pragma once

#include "engine/components/ComponentBase.hpp"

class CText : public Engine::ComponentBase {
  public:
    /**
     * @brief Construct a new CText object
     */
    CText();

    /**
     * @brief Destroy the CText object
     */
    ~CText() override = default;

    /**
     * @brief Get the text of the component
     * @return The text of the component
     */
    [[nodiscard]] const std::string &getText() const;

    /**
     * @brief Set the text of the component
     * @param text The text to set
     */
    void setText(const std::string &text);

    /**
     * @brief Get the font size of the component
     * @return The font size of the component
     */
    [[nodiscard]] float getFontSize() const;

    /**
     * @brief Set the font size of the component
     * @param fontSize The font size to set
     */
    void setFontSize(float fontSize);

    void setOffset(Vector3 offset);

    [[nodiscard]] Vector3 getOffset();

  private:
    std::string _text;
    float _fontSize;
    Vector3 _offset;
};
