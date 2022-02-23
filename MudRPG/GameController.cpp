#include "GameController.h"
#include "RaylibOperators.h"


Rectangle operator+(const Rectangle& rect, const Vector2& v2)
{
    Rectangle r;
    r.x = rect.x + v2.x;
    r.y = rect.y + v2.y;
    r.width = rect.width;
    r.height = rect.height;
    return r;
}

struct Vector2i
{
    int x;
    int y;

    Vector2i(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    Vector2i(Vector2 v2)
    {
        x = (int)v2.x;
        y = (int)v2.y;
    }
};


struct Rectangle2i
{
    int x;
    int y;
    int width;
    int height;


    //Rectangle2i()
    //{
    //    x = 0;
    //    y = 0;
    //    width = 0;
    //    height = 0;
    //}

    Rectangle2i(int _x, int _y, int _width, int _height)
    {
        x = _x;
        y = _y;
        width = _width;
        height = _height;
    }

    Rectangle2i(Rectangle v2)
    {
        x = (int)v2.x;
        y = (int)v2.y;
        width = (int)v2.width;
        height = (int)v2.height;
    }

    //Rectangle ToRectangle()
    //{
    //    return { (float)x, (float)y, (float)width, (float)height };
    //}
};
/*---------------------------------------------------------------------------------------------------------*/

TileWalkable::TileWalkable()
{
    walkable = std::vector<uint8_t>((uint64_t)tile_x * tile_y, 1);
    tiles = std::vector<std::pair<std::string, std::string>>((uint64_t)tile_x * tile_y, 
        { "tf_ff_tileA2.png","tile/grass_1" });


    walkable[(uint64_t)10 * tile_x + 10] = 0;
    tiles[(uint64_t)10 * tile_x + 10] = { "tf_ff_tileA2.png","tile/grass_out_of_bounds_2" };
}

bool TileWalkable::IsWalkable(int x, int y)
{
    return x >= 0 && x < tile_x
        && y >= 0 && y < tile_y
        && walkable[(uint64_t)y * tile_x + x] != 0;
}

std::pair<std::string, std::string> TileWalkable::TileId(int x, int y)
{
    return tiles[(uint64_t)y * tile_x + x];
}


bool TileWalkable::IsTileWalkable(Rectangle hitbox)
{
    // Have to do this because negative fractions are rounded upwards to zero 
    if (hitbox.x < 0 || hitbox.y < 0)
        return false;

    for (int y = (int)(hitbox.y / tile_size); y <= (int)((hitbox.y + hitbox.height) / tile_size); y++)
        for (int x = (int)(hitbox.x / tile_size); x <= (int)((hitbox.x + hitbox.width) / tile_size); x++)
            if (!IsWalkable(x, y))
                return false;

    return true;
}




void GameController::GameTick(float delta)
{
    // Player movement controller ---------------------------------------------------------------------
    int h = 0;
    int v = 0;
    if (IsKeyDown(KEY_D)) h = 1;
    if (IsKeyDown(KEY_A)) h = 2;
    if (IsKeyDown(KEY_W)) v = 1;
    if (IsKeyDown(KEY_S)) v = 2;

    int direction_index = h + 3 * v - 1;
    lich_player->moving = direction_index >= 0;


    auto old_pos = lich_player->get_position();
    lich_player->update_pos(old_pos + lich_player->DIRECTIONS[direction_index] * 200.0f * delta);

    // Check diagonal
    if (!tile_walkable->IsTileWalkable(lich_player->collider.get_rect()))
    {
        lich_player->update_pos(old_pos);

        // Check horizontal-only
        if (h != 0)
        {
            lich_player->update_pos(old_pos + lich_player->DIRECTIONS[h - 1] * 200.0f * delta);

            if (!tile_walkable->IsTileWalkable(lich_player->collider.get_rect()))
            {
                lich_player->update_pos(old_pos);

                // Check vertical-only
                if (v != 0)
                {
                    lich_player->update_pos(old_pos + lich_player->DIRECTIONS[3 * v - 1] * 200.0f * delta);

                    if (!tile_walkable->IsTileWalkable(lich_player->collider.get_rect()))
                    {
                        lich_player->update_pos(old_pos);
                    }
                    else
                    {
                        lich_player->direction = 3 * v - 1;
                    }
                }
            }
            else
            {
                lich_player->direction = h - 1;
            }
        }
    } 
    else
    {
        lich_player->direction = direction_index;
    }
    // ------------------------------------------------------------------------------------------------
}
