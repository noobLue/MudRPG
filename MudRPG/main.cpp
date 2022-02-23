/*******************************************************************************************
*
*   raylib [core] example - Keyboard input
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "RaylibOperators.h"
#include <iostream>
#include <string>
#include <map>

#include "yojimbo.h"
#include "shared.h"

#include "GameController.h"
#include <fstream>
#include "json.hpp"


struct SheetSizes
{
    int64_t sprite_width;  // : 26,
    int64_t sprite_height;  // : 36,
    int64_t sprite_count_x; // : 3,
    int64_t sprite_count_y; // : 4

    SheetSizes() {}
    SheetSizes(int64_t x, int64_t y, int64_t size_x, int64_t size_y)
    {
        sprite_width = x;
        sprite_height = y;
        sprite_count_x = size_x;
        sprite_count_y = size_y;
    }
};

struct SpritePartition
{
    std::string id;
    std::string type;
    int64_t start_x;
    int64_t start_y;
};

struct SpriteFile
{
    Texture2D texture;
    std::map<std::string, SpritePartition> partitions;

    SpriteFile(){}
    SpriteFile(std::string file_path)
    {
        texture = LoadTexture(file_path.c_str());
        //std::map<std::string, SpritePartition>();
    }
};


void DrawSpritePart(std::string file, std::string partition, std::string tile_x, std::string tile_y)
{
    // ... 

}


struct ResourceFile
{
    std::map<std::string, SheetSizes> sheet_types;
    std::map<std::string, SpriteFile> files;

    void LoadFromJson(std::string file_name)
    {
        std::ifstream i("resources/resources.json");
        nlohmann::json j;
        i >> j;

        for (auto& [key, value]: j["sprite_sheet_types"].items())
        {
            SheetSizes sizes = {
                value["sprite_size_x"].get<int64_t>(),
                value["sprite_size_y"].get<int64_t>(),
                value["sprite_count_x"].get<int64_t>(),
                value["sprite_count_y"].get<int64_t>()
            };
            sheet_types.emplace(key, sizes);
        }

        for (auto& [key, value] : j["files"].items())
        {
            files[key] = SpriteFile("resources/" + key);

            for (auto& obj: value)
            {
                SpritePartition part; 
                part.id = obj["id"].get<std::string>();
                part.type = obj["type"].get<std::string>();
                part.start_x = obj["start_x"].get<int64_t>();
                part.start_y = obj["start_y"].get<int64_t>();


                files[key].partitions.emplace(part.id, part);
            }
        }
    }
};

struct SpriteSheet
{
    Texture2D texture;
    int sprites_x;
    int sprites_y;

    int GetSpriteWidth()
    {
        return texture.width / sprites_x;
    }

    int GetSpriteHeight()
    {
        return texture.height / sprites_y;
    }
};


void LoadResourceJson()
{
    std::ifstream i("resources/resources.json");
    nlohmann::json j;
    i >> j;

    std::cout << std::setw(4) << j << std::endl;
}

void init_client()
{
    double time = 100.0;

    uint64_t clientId = 0;
    random_bytes((uint8_t*)&clientId, 8);
    printf("client id is %.16" PRIx64 "\n", clientId);

    ClientServerConfig config;

    Client client(GetDefaultAllocator(), Address("0.0.0.0"), config, adapter, time);

    Address serverAddress("127.0.0.1", ServerPort);
}


ResourceFile resources;
void RenderSprite(int64_t x, int64_t y, const std::pair<std::string, std::string>& FileId)
{
    auto spriteFile = resources.files[FileId.first];
    auto tiled2 = spriteFile.partitions[FileId.second];
    auto tile_type = resources.sheet_types[tiled2.type];

    Rectangle tile_source = { tiled2.start_x, tiled2.start_y, tile_type.sprite_width, tile_type.sprite_height };

    // TODO: more efficient method of drawing duplicate tiles
    // Combine all of these into one image_texture and draw the whole texture after.
    DrawTextureRec(spriteFile.texture, tile_source, { (float)x * tile_type.sprite_width, (float)y * tile_type.sprite_height }, WHITE);
}


// TODO: make general animatio system for players / entities
int frame = 0;
float frame_time_temp = 0.0f;
bool frame_forward = true;
int last_dir = 0;
Rectangle frameRect = { 0.0f, 0.0f, 0.0f, 0.0f };

void RenderPlayer(PlayableCharacter* player, const std::pair<std::string, std::string>& FileId, float delta)
{
    auto lichie = resources.files[FileId.first];
    auto partition = lichie.partitions[FileId.second];
    auto sp_type = resources.sheet_types[partition.type];

    frameRect.width = sp_type.sprite_width;
    frameRect.height = sp_type.sprite_height;

    // TODO: Refactor to DrawPlayableCharacter(PlayableCharacter* character);
    float animation_speed = 0.15f; // frames per second
    if (player->moving)
    {
        if (frame_time_temp >= animation_speed)
        {
            if (frame_forward)
            {
                if (++frame >= 2)
                {
                    frame = 2;
                    frame_forward = false;
                }
            }
            else
            {
                if (--frame <= 0)
                {
                    frame = 0;
                    frame_forward = true;
                }
            }
            frame_time_temp -= animation_speed;
        }
    }
    else {
        frame = 1;
        frame_time_temp = animation_speed * 0.95f;
    }

    if (!player->moving) {
        last_dir = -1;
    }
    else
    {
        // TODO:
        // If only one button pressed:
        //      always face that direction
        // Else:
        //      go by direction where we are moving


        // something abou directions are wrong here. I'm pretty sure the last_dir != ... part.

        if (player->DIRECTIONS[player->direction].x > 0 && (last_dir != 3 && last_dir != 5 || player->DIRECTIONS[player->direction].y == 0))
        {
            // right
            frameRect.y = partition.start_y + (float)2 * (float)sp_type.sprite_height;
            last_dir = 1;
        }
        else if (player->DIRECTIONS[player->direction].x < 0 && (last_dir != 3 && last_dir != 5 || player->DIRECTIONS[player->direction].y == 0))
        {
            // left
            frameRect.y = partition.start_y + (float)1 * (float)sp_type.sprite_height;
            last_dir = 2;
        }
        else if (player->DIRECTIONS[player->direction].y > 0 && (last_dir != 1 && last_dir != 2 || player->DIRECTIONS[player->direction].x == 0))
        {
            // down
            frameRect.y = partition.start_y + (float)0 * (float)sp_type.sprite_height;
            last_dir = 5;
        }
        else if (player->DIRECTIONS[player->direction].y < 0 && (last_dir != 1 && last_dir != 2 || player->DIRECTIONS[player->direction].x == 0))
        {
            // up
            frameRect.y = partition.start_y + (float)3 * (float)sp_type.sprite_height;
            last_dir = 3;
        }
    }

    frameRect.x = partition.start_x + (float)frame * (float)sp_type.sprite_width;

    Vector2 s_offset = { 0 - (float)sp_type.sprite_width / 2 + 0.5f, 0 - 33.0f };
    Vector2 sprite_offsetted = player->get_position() + s_offset;

    // DEBUG: draw hitbox
    //auto inner_hitbox = player->collider.get_rect();
    //DrawRectangleRec(inner_hitbox, BLACK);
    DrawTextureRec(lichie.texture, frameRect, sprite_offsetted, WHITE);
    frame_time_temp += delta;
}

int main(void)
{
    //LoadResourceJson();
    // TODO: networking
    //init_client();


    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 1200;

    // Set VSYNC on, so far this gives the best performance (and kinda the best fps)
    SetConfigFlags(FLAG_VSYNC_HINT);

    InitWindow(screenWidth, screenHeight, "raylib [core] example - keyboard input");

    GameController controller = GameController();

    resources.LoadFromJson("resources/resources.json");

    std::map<std::string, SpriteSheet> sheets;

    SpriteSheet tiles;
    tiles.texture = LoadTexture("resources/tf_ff_tileA2.png");
    tiles.sprites_x = 16;
    tiles.sprites_y = 12;

    SpriteSheet lich;
    lich.texture = LoadTexture("resources/lich.png");
    lich.sprites_x = 12;
    lich.sprites_y = 8;
    //int last_dir = 0;
    //Rectangle frameRect = { 0.0f, 0.0f, (float)lich.GetSpriteWidth(), (float)lich.GetSpriteHeight() };
    //Rectangle dest = { 250.0f, 250.0f, lich.GetSpriteWidth(), lich.GetSpriteHeight() };


    int frame = 0;
    float frame_time = 0.0f;
    bool frame_forward = true;

    //PlayableCharacter lich_player;
    //lich_player.position = { 100.0f, 100.0f };
    //lich_player.moving = false;
    //lich_player.up = false;
    //lich_player.down = false;
    //lich_player.left = false;
    //lich_player.right = false;


    std::cout << "Sprite dimensions: " << (float)lich.GetSpriteWidth() << ", " << (float)lich.GetSpriteHeight() << std::endl;


    //Vector2 ballPosition = { (float)screenWidth / 2, (float)screenHeight / 2 };

    //SetTargetFPS(200);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------


    auto player = controller.GetPlayer();

    Camera2D camera = { 0 };
    camera.target = player->get_position();
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

    float fps_second = 0.0f;
    int tick_count = 0;
    int last_fps = 0;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        auto delta = GetFrameTime();
        controller.GameTick(delta);
        camera.target = player->get_position(); //{ player->position.x, player->position.y };

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        {
            BeginMode2D(camera); // To give perspective of moving
            DrawCircleV({ 100,100 }, 25, MAROON);


            Vector2 player_tile = player->get_position() / controller.GetTiles()->tile_size;

            auto tiles_st = controller.GetTiles();
            for (int y = player_tile.y - 50; y < player_tile.y + 50; y++)
            {
                for (int x = player_tile.x - 50; x < player_tile.x + 50; x++)
                {
                    if (y >= 0 && y < controller.GetTiles()->tile_y
                        && x >= 0 && x < controller.GetTiles()->tile_x)
                    {
                        RenderSprite(x, y, tiles_st->TileId(x, y));
                    }
                    else
                    {
                        RenderSprite(x, y, {"tf_ff_tileA2.png", "tile/grass_out_of_bounds_1"});
                    }
                }
            }

            RenderPlayer(controller.GetPlayer(), { "lich.png", "character/skeleton_blue_big_1" }, delta);
            EndMode2D();
        }



        auto s = std::string("FPS ");
        s += std::to_string(last_fps);

        DrawText(s.c_str(), 10, 10, 20, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------

        frame_time += delta;
        tick_count++;

        fps_second += delta;
        if (fps_second >= 1.0f)
        {
            //std::cout << "fps: " << tick_count << std::endl;
            last_fps = tick_count;
            fps_second -= 1.0f;
            tick_count = 0;
        }
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(lich.texture);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}