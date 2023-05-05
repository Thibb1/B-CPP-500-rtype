/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** player.hpp by thibb1
*/

#pragma once

#include "utils.hpp"
/**
 * @brief Class used to create player in the server
 * 
 */
class Player {
  private:
    Vector2 _position;
    BoundingBox _bounds{};
    int _hp;
    bool _isAlive;
    int _id;
    std::string _name;
    int _score;

  public:
    /**
     * @brief Construct a new Player object
     * 
     */
    Player() : _position({0, 0}), _hp(3), _isAlive(true), _id(0), _name(""), _score(0) { _bounds = GetBoundingBoxAroundPoint(_position, 0.5); }
    /**
     * @brief Construct a new Player object
     * 
     * @param id 
     */
    explicit Player(int id) : Player() { _id = id; }
    Vector2 getPosition();
    /**
     * @brief Get the Hp object
     * 
     * @return int 
     */
    int getHp() const;
    /**
     * @brief Get the Id object
     * 
     * @return int 
     */
    int getId() const;
    /**
     * @brief Get the Bounds object
     * 
     * @return BoundingBox 
     */
    BoundingBox getBounds() const;
    /**
     * @brief check if player is alive
     * 
     * @return true 
     * @return false 
     */
    [[nodiscard]] bool isAlive() const;
    /**
     * @brief Set the Position object
     * 
     * @param pos 
     */
    void setPosition(Vector2 pos);
    /**
     * @brief Set the Hp object
     * 
     * @param hp 
     */
    void setHp(int hp);
    /**
     * @brief reduce player hp
     * 
     */
    void takeDamage(int damage);
    /**
     * @brief Set the Alive object
     * 
     * @param alive 
     */
    void setAlive(bool alive);
    /**
     * @brief move the player
     * 
     * @param direction 
     * @param position 
     * @return true 
     * @return false 
     */
    bool move(const std::string &direction, const Vector2 &position);
    /**
     * @brief Set the Name object
     * 
     * @param name 
     */
    void setName(const std::string &name);
    /**
     * @brief Get the Name object
     * 
     * @return std::string 
     */
    std::string getName();
    /**
     * @brief Set the Score object
     * 
     * @param score 
     */
    void setScore(int score);
    /**
     * @brief Get the Score object
     * 
     * @return int 
     */
    int getScore();
    /**
     * @brief add score to the totalscore
     * 
     * @param score 
     */
    void addScore(int score);
};
