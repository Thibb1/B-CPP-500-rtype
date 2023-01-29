/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** main.hpp by thibb1
*/

#ifndef RTYPE_RTYPE_CLIENT_HPP
#define RTYPE_RTYPE_CLIENT_HPP
#define MAX_LIGHTS 100

#include "bullet.hpp"
#include "button.hpp"
#include "deps.hpp"
#include "game-object.hpp"
#include "light.hpp"
#include "particle2d.hpp"
#include "slider.hpp"
#include "structs.hpp"
// should always be included last
#include "engine.hpp"

// BoundingBox GetMyObjectBoundingBox(const GameObject &myObject, Vector3 scale = Vector3Zero());
BoundingBox GetBoundingBoxAroundPoint(Vector3 position, float scale);
void moveSpaceship(GameObject *spaceship);
#endif // RTYPE_RTYPE_CLIENT_HPP
