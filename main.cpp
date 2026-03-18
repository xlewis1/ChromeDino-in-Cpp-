#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// --- Include all PNG headers (generated via xxd -i) ---
#include "dino-run1.h"
#include "dino-run2.h"
#include "dino-jump.h"
#include "cactus1.h"
#include "big-cactus1.h"
#include "bird1.h"
#include "bird2.h"
#include "game-over.h"
#include "reset.h"

// --- Obstacle structure ---
struct Obstacle {
    Texture2D texture1;
    Texture2D texture2;
    Rectangle rect;
    float speed;
    int type;      // 0 = cactus, 1 = bird
    float animTimer;
    int animFrame;
};

// --- Helper: load texture from memory ---
Texture2D LoadTextureFromHeader(const unsigned char* data, int dataSize) {
    Image img = LoadImageFromMemory(".png", data, dataSize);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 400;
    const int groundY = screenHeight - 60;

    InitWindow(screenWidth, screenHeight, "Chrome Dino in C++ with Raylib");
    SetTargetFPS(60);

    // --- Load textures ---
    Texture2D dinoRun1    = LoadTextureFromHeader(dino_run1_png, dino_run1_png_len);
    Texture2D dinoRun2    = LoadTextureFromHeader(dino_run2_png, dino_run2_png_len);
    Texture2D dinoJump    = LoadTextureFromHeader(dino_jump_png, dino_jump_png_len);
    Texture2D cactus1     = LoadTextureFromHeader(cactus1_png, cactus1_png_len);
    Texture2D bigCactus1  = LoadTextureFromHeader(big_cactus1_png, big_cactus1_png_len);
    Texture2D bird1Tex    = LoadTextureFromHeader(bird1_png, bird1_png_len);
    Texture2D bird2Tex    = LoadTextureFromHeader(bird2_png, bird2_png_len);
    Texture2D gameOverTex = LoadTextureFromHeader(game_over_png, game_over_png_len);
    Texture2D resetTex    = LoadTextureFromHeader(reset_png, reset_png_len);

    // --- Player ---
    Vector2 playerPos = {50, (float)groundY - dinoRun1.height};
    float playerSpeedY = 0;
    int jumpCount = 0;
    const int maxJumps = 2;
    float animTimer = 0;
    int animFrame = 0;

    // --- World variables ---
    float gameSpeed = 300.0f;
    float spawnTimer = 0.0f;
    float nextSpawnTime = 1.0f;
    float floorOffset = 0.0f;
    int score = 0;
    int hiScore = 0;
    bool gameOver = false;

    std::vector<Obstacle> obstacles;
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (!gameOver) {
            gameSpeed += 1.5f * deltaTime;
            score += (int)(deltaTime * 60);

            // --- Jump ---
            if (IsKeyPressed(KEY_SPACE) && jumpCount < maxJumps) {
                playerSpeedY = (jumpCount == 0) ? -700 : -600;
                jumpCount++;
            }

            // --- Gravity ---
            float gravity = 1400.0f;
            if (playerSpeedY < 0 && !IsKeyDown(KEY_SPACE)) gravity = 2800.0f;
            playerSpeedY += gravity * deltaTime;
            playerPos.y += playerSpeedY * deltaTime;

            if (playerPos.y >= groundY - dinoRun1.height) {
                playerPos.y = groundY - dinoRun1.height;
                playerSpeedY = 0;
                jumpCount = 0;
            }

            // --- Dino Animation ---
            animTimer += deltaTime;
            if (jumpCount == 0 && animTimer > 0.12f) {
                animFrame = (animFrame + 1) % 2;
                animTimer = 0;
            }

            // --- Floor Scrolling ---
            floorOffset -= gameSpeed * deltaTime;
            if (floorOffset <= -screenWidth) floorOffset = 0;

            // --- Spawn Obstacles (original fast style) ---
            spawnTimer += deltaTime;
            if (spawnTimer >= nextSpawnTime) {
                Obstacle obs;
                obs.speed = gameSpeed;
                obs.animTimer = 0;
                obs.animFrame = 0;

                int type = std::rand() % 4; // 0-2 cactus, 3 bird
                if (type < 3) {
                    obs.type = 0;
                    obs.texture1 = (std::rand() % 2 == 0) ? cactus1 : bigCactus1;
                    obs.texture2 = obs.texture1;
                    obs.rect = {
                        (float)screenWidth,
                        (float)groundY - obs.texture1.height,
                        (float)obs.texture1.width,
                        (float)obs.texture1.height
                    };
                } else {
                    obs.type = 1;
                    obs.texture1 = bird1Tex;
                    obs.texture2 = bird2Tex;
                    float minY = groundY - dinoRun1.height - 140;
                    float maxY = groundY - dinoRun1.height - 80;
                    obs.rect = {
                        (float)screenWidth,
                        (float)(minY + rand() % (int)(maxY - minY + 1)),
                        (float)bird1Tex.width,
                        (float)bird1Tex.height
                    };
                }

                obstacles.push_back(obs);
                spawnTimer = 0.0f;
                nextSpawnTime = ((std::rand() % 80) / 100.0f) + 0.8f; // random 0.8-1.6 sec
            }

            // --- Movement & Collision ---
            Rectangle playerHitbox = { playerPos.x + 8, playerPos.y + 4, (float)dinoRun1.width - 16, (float)dinoRun1.height - 8 };
            for (auto it = obstacles.begin(); it != obstacles.end();) {
                it->rect.x -= it->speed * deltaTime;

                if (it->type == 1) {
                    it->animTimer += deltaTime;
                    if (it->animTimer > 0.15f) {
                        it->animFrame = (it->animFrame + 1) % 2;
                        it->animTimer = 0;
                    }
                }

                if (CheckCollisionRecs(playerHitbox, it->rect)) gameOver = true;

                if (it->rect.x < -100) it = obstacles.erase(it);
                else ++it;
            }
        } else {
            if (score > hiScore) hiScore = score;
            Rectangle resetButton = { (float)screenWidth/2 - resetTex.width/2, 180.0f, (float)resetTex.width, (float)resetTex.height };
            bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec({(float)GetMouseX(), (float)GetMouseY()}, resetButton);

            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || clicked) {
                obstacles.clear();
                playerPos.y = groundY - dinoRun1.height;
                playerSpeedY = 0;
                jumpCount = 0;
                gameSpeed = 300.0f;
                score = 0;
                gameOver = false;
            }
        }

        bool isNight = (score / 1000) % 2 != 0;
        Color themeColor = isNight ? RAYWHITE : BLACK;
        Color bg = isNight ? (Color){33,33,33,255} : RAYWHITE;

        BeginDrawing();
        ClearBackground(bg);

        DrawLineEx({0, (float)groundY}, {(float)screenWidth, (float)groundY}, 2, themeColor);
        for (int i = 0; i < 10; i++) {
            DrawCircle(floorOffset + i * 100, groundY + 10, 2, themeColor);
            DrawCircle(floorOffset + i * 100 + 400, groundY + 15, 1, themeColor);
        }

        for (auto& obs : obstacles) {
            Texture2D tex = (obs.type == 1 && obs.animFrame == 1) ? obs.texture2 : obs.texture1;
            DrawTexture(tex, (int)obs.rect.x, (int)obs.rect.y, WHITE);
        }

        Texture2D currentDino = (jumpCount > 0) ? dinoJump : (animFrame == 0 ? dinoRun1 : dinoRun2);
        DrawTexture(currentDino, (int)playerPos.x, (int)playerPos.y, WHITE);

        DrawText(TextFormat("HI %05d  %05d", hiScore, score), screenWidth - 220, 20, 20, themeColor);

        if (gameOver) {
            DrawTexture(gameOverTex, screenWidth/2 - gameOverTex.width/2, 120, WHITE);
            DrawTexture(resetTex, screenWidth/2 - resetTex.width/2, 180, WHITE);
            DrawText("PRESS SPACE TO RUN", screenWidth/2 - 95, 260, 20, themeColor);
        }

        EndDrawing();
    }

    // --- Cleanup ---
    UnloadTexture(dinoRun1); UnloadTexture(dinoRun2); UnloadTexture(dinoJump);
    UnloadTexture(cactus1); UnloadTexture(bigCactus1);
    UnloadTexture(bird1Tex); UnloadTexture(bird2Tex);
    UnloadTexture(gameOverTex); UnloadTexture(resetTex);

    CloseWindow();
    return 0;
}