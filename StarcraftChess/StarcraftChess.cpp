#include <iostream>
#include "include/raylib-cpp.hpp"

#pragma comment (lib, "lib/raylibdll.lib")

int main()
{
    raylib::Window w = raylib::Window(1280,720, "Starcraft Chess");

    SetTargetFPS(120);

    Model pawn = LoadModel("assets/models/pawn.obj");
    Model rook = LoadModel("assets/models/rook.obj");
    Model board = LoadModel("assets/models/board.obj");


    raylib::Camera3D c = raylib::Camera3D({ 125,50,0}, {42,4,54}, {0,1,0}, 45, CAMERA_PERSPECTIVE);

    c.SetMode(CAMERA_PERSPECTIVE);
    
    Vector3 p = { 0,25,50};


    float scale = 1;
    while (!w.ShouldClose())
    {
        UpdateCamera(c);
        BeginDrawing();

        w.ClearBackground(DARKGRAY);

        DrawFPS(0, 0);

        BeginMode3D(c);

        DrawModelEx(board, {0,0,0}, {1.0f, 0.0f, 0.0f}, 90.0f, {1,1,1}, WHITE);

        for (int i = 0; i < 8; i++)
        {
            DrawModelEx(pawn, {64, 2, 52.0f + (10 * i)}, {1.0f,0.0f,0.0f}, -90, {1,1,1}, WHITE);
        }

        DrawModelEx(rook, { 73, 2, 116 }, { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, WHITE);
        DrawModelEx(rook, { 73, 2, 49 }, { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, WHITE);

        EndMode3D();

        EndDrawing();
    }

    return 0;
}
