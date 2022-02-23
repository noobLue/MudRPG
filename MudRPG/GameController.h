#pragma once
#include <memory>
#include <vector>
#include <iostream>

#include "raylib.h"
#include <cassert>


struct TileWalkable
{
	const int tile_size = 16;
	//const int default_tile_id = 0;
	//const int out_of_bounds_tile_id = 9;

	int tile_x = 100; // rename to size
	int tile_y = 100;
	std::vector<uint8_t> walkable; // 0 = unwalkable
    std::vector<std::pair<std::string, std::string>> tiles;

    TileWalkable();
    bool IsWalkable(int x, int y);
    std::pair<std::string, std::string> TileId(int x, int y);
    bool IsTileWalkable(Rectangle hitbox);
};



class Component
{
    virtual std::string get_name() = 0;
};

/*class Collider : public Component
{
    Rectangle hitbox;
    virtual void on_enter_collision(const std::unique_ptr<Component>& e) = 0;
    virtual void while_collision(const std::unique_ptr<Component>& e) = 0;
    virtual void on_exit_collision(const std::unique_ptr<Component>& e) = 0;
};*/

enum class CollisionLayer
{
    PLAYER
};

struct BoxCollider
{
    Rectangle rect;

public:
    BoxCollider() {}
    BoxCollider(Rectangle _rect)
    {
        rect = _rect;
    }

    Rectangle get_rect()
    {
        return rect;
    }

    bool is_colliding(BoxCollider other) 
    {
        return rect.x < (other.rect.x + other.rect.width)
            && (rect.x + rect.width) > other.rect.x
            && rect.y < (other.rect.y + other.rect.height)
            && (rect.y + rect.height) > other.rect.y;
    }

    void update(Vector2 center)
    {
        rect.x = center.x - rect.width / 2.0f;
        rect.y = center.y - rect.height / 2.0f;
    }
};




class Entity
{
//protected:
public:
    int64_t id;
    std::string model_id;
    std::vector<std::unique_ptr<Component>> components;

    Vector2 position;
    CollisionLayer collision_type;
    // collision_box;
//public:
    BoxCollider collider;
    Entity(std::string model_id, int64_t _id, Vector2 _pos, CollisionLayer _col_layer, BoxCollider _collider)
    {
        id = _id;
        position = _pos;
        collision_type = _col_layer;
        collider = _collider;
    }
    virtual void on_enter_collision(const std::unique_ptr<Entity>& ent) = 0;
    virtual void while_collision(const std::unique_ptr<Entity>& ent) = 0;
    virtual void on_exit_collision(const std::unique_ptr<Entity>& ent) = 0;
    
    Vector2 get_position()
    {
        return position;
    }

    void update_pos(Vector2 _position)
    {
        position = _position;
        collider.update(position);
    }
};


struct PlayableCharacter : public Entity
{
    const std::vector<Vector2> DIRECTIONS = {
                                // hv       // xy
         {1.0f, 0.0f},          // 10
         {-1.0f, 0.0f},         // 20
         {0.0f, -1.0f},          // 01
         {0.707f, -0.707f},      // 11

         {-0.707f, -0.707f},     // 21
         {0.0f, 1.0f},         // 02
         {0.707f, 0.707f},     // 12
         {-0.707f, 0.707f},    // 22
    };
    int direction;
    bool moving;

    PlayableCharacter(
        std::string _model_id,
        int64_t _id, 
        Vector2 _pos, 
        CollisionLayer _col_layer, 
        BoxCollider _collider
    ) : Entity(_model_id, _id, _pos, _col_layer, _collider)
    {
        direction = 4;
        moving = false;
    }

    // TODO: blocking and non-blocking collision should work a little differently?
    void on_enter_collision(const std::unique_ptr<Entity>& ent) override
    {

    }

    void while_collision(const std::unique_ptr<Entity>& ent) override
    {

    }

    void on_exit_collision(const std::unique_ptr<Entity>& ent) override
    {

    }

    /*Rectangle get_outer_hitbox()
    {
        return { position.x - player_hitbox_width / 2.0f, position.y - player_hitbox_width / 2.0f , player_hitbox_width, player_hitbox_width };
    }

    Rectangle get_inner_hitbox()
    {
        return { position.x - p_inner_hitbox_width / 2.0f, position.y - p_inner_hitbox_width / 2.0f , p_inner_hitbox_width, p_inner_hitbox_width };
    }*/

    /*
    Vector2 accurate_hitbox = { 15.0f, 6.0f };

    Vector2 sprite_offsetted = { player->position.x - (float)lich.GetSpriteWidth() / 2 + 0.5f, player->position.y - 33.0f};

    //DrawCircleV(lich_player.position, player_hitbox_width, DARKBLUE);
    DrawRectangleV({ player->position.x - player_hitbox_width / 2.0f, player->position.y - player_hitbox_width / 2.0f}, { player_hitbox_width, player_hitbox_width }, DARKBLUE);
    DrawRectangleV({ player->position.x - p_inner_hitbox_width / 2.0f, player->position.y - p_inner_hitbox_width / 2.0f }, { p_inner_hitbox_width, p_inner_hitbox_width }, DARKGREEN);
    
    */
};

class GameController
{
    int64_t entity_counter = 0;
	std::unique_ptr<TileWalkable> tile_walkable;

	std::unique_ptr<PlayableCharacter> lich_player;
    std::vector<std::unique_ptr<Entity>> entities;

public:
	GameController()
	{
		tile_walkable = std::make_unique<TileWalkable>();
		//player_position = { 100, 100 };

        // accurate hitbox size = 9.0f, smaller = 7.0f
        const Rectangle p_rect = { 100.0f, 100.0f, 7.0f, 7.0f };
        const Vector2 pos = { 100.0f, 100.0f };
        lich_player = std::make_unique<PlayableCharacter>("lich",  entity_counter++, pos, CollisionLayer::PLAYER, BoxCollider(p_rect));

	}

	TileWalkable* GetTiles()
	{
		return tile_walkable.get();
	}

	// return copy of player? (useful if we do game tick in separate thread)
	PlayableCharacter* GetPlayer() 
	{
        return lich_player.get();
	}

    void GameTick(float delta);
};

