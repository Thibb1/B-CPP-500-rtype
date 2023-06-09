/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** CRotation.hpp
*/

#pragma once

#include "engine/components/ComponentBase.hpp"

namespace Engine {
  /**
 * @brief Class used to handle CRotation
 * 
 */
    class CRotation : public ComponentBase {
      public:
        /**
         * @brief Construct a new CRotation object
         */
        CRotation();

        /**
         * @brief Construct a new CRotation object
         * @param Vector3 rotation
         */
        explicit CRotation(const Vector3 &rotation);

        /**
         * @brief Destroy the CRotation object
         */
        ~CRotation() override;

        /**
         * @brief Get the rotation
         * @return Vector3
         */
        [[nodiscard]] Vector3 getRotation() const;

        /**
         * @brief Set the rotation
         * @param Vector3 rotation
         */
        void setRotation(const Vector3 &rotation);

        /**
         * @brief Get the rotation goal
         * @return Vector3
         */
        [[nodiscard]] Vector3 getRotationGoal() const;

        /**
         * @brief Set the rotation goal
         * @param Vector3 rotation
         */
        void setRotationGoal(const Vector3 &rotation);

        /**
         * @brief Update the rotation
         */
        void doUpdate() override;

      private:
        Vector3 _rotation;
        Vector3 _rotationGoal{};
    };
}
