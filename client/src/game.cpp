/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** game.cpp by thibb1
*/

#include "rtype-client.hpp"

std::string floatToString(float number, int precision = 1) {
    std::ostringstream out;
    out.precision(precision);
    out << std::fixed << number;
    return out.str();
}

std::string getPositionString(Vector3 position) { return floatToString(position.x) + "," + floatToString(position.y); }

void moveSpaceship(GameObject *spaceship, UdpClient *client) {
    float speed = 0.1;
    Vector3 position = spaceship->GetPosition();
    Vector3 rotationGoal = spaceship->GetRotationGoal();
    if (IsKeyDown(KEY_LEFT) && position.x > -8) {
        position.x -= speed;
        client->request((std::string)"move:left," + getPositionString(position));
    }
    if (IsKeyDown(KEY_RIGHT) && position.x < 8) {
        position.x += speed;
        client->request((std::string)"move:right," + getPositionString(position));
    }
    if (IsKeyDown(KEY_UP) && position.y < 4.4) {
        position.y += speed;
        rotationGoal.x = -.35;
        client->request((std::string)"move:up," + getPositionString(position));
    }
    if (IsKeyDown(KEY_DOWN) && position.y > -4.4) {
        position.y -= speed;
        rotationGoal.x = .35;
        client->request((std::string)"move:down," + getPositionString(position));
    }
    if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
        rotationGoal.x = 0;
    }
    spaceship->SetPosition(position);
    spaceship->SetRotationGoal(rotationGoal);
}
