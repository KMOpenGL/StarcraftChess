#include <iostream>
#include "include/raylib-cpp.hpp"
#include <map>
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#pragma comment (lib, "lib/raylibdll.lib")

class Resources {
private:
    std::map<std::string, Model> cachedModels;
    std::string assetPath;
    Shader lightingShader;
public:
    Resources(std::string path)
    {
        assetPath = path;
    }

    void setModelLightingShader(Shader s)
    {
        lightingShader = s;
    }

    ~Resources()
    {
        for (auto pair : cachedModels)
        {
            UnloadModel(pair.second);
        }
        cachedModels.clear();
    }
    Shader GetShader(std::string name)
    {
        return LoadShader((assetPath + "/shaders/" + name + ".vs").c_str(),
            (assetPath + "/shaders/" + name + ".fs").c_str());
    }

    Model GetModel(std::string name)
    {
        if (cachedModels[name].materialCount > 0)
            return cachedModels[name];

        Model m = cachedModels[name] = LoadModel((assetPath + "/models/" + name + ".obj").c_str());
        for (int i = 0; i < m.materialCount; i++)
            cachedModels[name].materials[i].shader = lightingShader;
        return m;
    }
};

class ChessHelper {
public:

    static Vector3 GridPos(int colX, int colY)
    {
        return {29.5f - (9.5f * colY), 0, -8.0f + (10.0f * colX)};
    }

    static Vector2 WorldToScreenModel(Vector3 w, Camera c)
    {
        w.x += 52;
        w.z -= 32;

        Vector2 s = GetWorldToScreen(w, c);

        return s;
    }
};

int main()
{
    raylib::Window w = raylib::Window(1280,720, "Starcraft Chess");

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    SetTargetFPS(120);

    Resources r = Resources("assets");

    Shader ls = r.GetShader("lighting");

    ls.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(ls, "viewPos");

    int ambientLoc = GetShaderLocation(ls, "ambient");
    float f[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    SetShaderValue(ls, ambientLoc, f, SHADER_UNIFORM_VEC4);

    r.setModelLightingShader(ls);

    Model board = r.GetModel("board");
    Model pawn = r.GetModel("pawn");
    Model rook = r.GetModel("rook");
    Model bishop = r.GetModel("bishop");
    Model knight = r.GetModel("knight");

    Model select = r.GetModel("selected");

    raylib::Camera3D c = raylib::Camera3D({ 126,50,0}, {0,-25,0}, {0,1,0}, 45, CAMERA_PERSPECTIVE);

    Vector3 lPos = { 50,25,2 };
    Light l = CreateLight(LIGHT_POINT, lPos, Vector3Zero(), WHITE, ls);
    l.enabled = true;

    Vector3 p = { 0,25,50};


    float scale = 1;
    while (!w.ShouldClose())
    {
        UpdateCamera(&c);
        float cameraPos[3] = { c.position.x, c.position.y, c.position.z };
        SetShaderValue(ls, ls.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);


                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
        BeginDrawing();

        w.ClearBackground(DARKGRAY);


        Vector2 mousePos = GetMousePosition();

        DrawFPS(0, 0);

        DrawText(("Mouse Pos: " + std::to_string((int)mousePos.x) + "," + std::to_string((int)mousePos.y)).c_str(), 0, 24, 18, WHITE);

        BeginMode3D(c);

        DrawSphereWires(l.position, 2.0f, 2, 2, l.color);

        DrawLine3D(lPos, { 50,0,2 }, YELLOW);

        DrawModelEx(board, {0,0,-58}, {1.0f, 0.0f, 0.0f}, 90.0f, {1,1,1}, WHITE);
        

        for (int i = 1; i < 9; i++)
        {
            DrawModelEx(pawn, ChessHelper::GridPos(i,1), {1.0f,0.0f,0.0f}, -90, {1,1,1}, WHITE);
        }

        DrawModelEx(rook, ChessHelper::GridPos(1, 0), { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, WHITE);
        DrawModelEx(rook, ChessHelper::GridPos(8, 0), { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, WHITE);

        DrawModelEx(knight, ChessHelper::GridPos(2, 0), { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, WHITE);
        DrawModelEx(knight, ChessHelper::GridPos(7, 0), { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, WHITE);

        DrawModelEx(bishop, ChessHelper::GridPos(3, 0), { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, WHITE);
        DrawModelEx(bishop, ChessHelper::GridPos(6, 0), { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, WHITE);

        EndMode3D();

        for (int x = 1; x < 9; x++)
        {
            for (int y = 1; y < 9; y++)
            {
                Vector3 piecePos = ChessHelper::GridPos(x, y);
                Vector2 w = ChessHelper::WorldToScreenModel(piecePos, c);
                DrawText((std::to_string((int)piecePos.x) + "," + std::to_string((int)piecePos.z)).c_str(), w.x, w.y, 20, WHITE);

            }
        }


        EndDrawing();
    }

    return 0;
}
