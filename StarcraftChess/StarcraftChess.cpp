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
    Resources()
    {

    }
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

        return m;
    }
};

Resources resourceInstance;

enum class PieceType {
    Pawn = 0,
    Rook = 1,
    Knight = 2,
    Bishop = 3,
    Queen = 4,
    King = 5
};

class ChessHelper {
public:

    static Vector3 GridPos(float colX, float colY)
    {
        return {2 - (10.0f * colY), 0, -72 + (10.0f * colX)};
    }

    static Vector2 WorldToScreenModel(Vector3 w, Camera c)
    {
        Vector2 s = GetWorldToScreen(w, c);

        return s;
    }

    static Model TypeToModel(PieceType t)
    {
        switch (t)
        {
        case PieceType::Pawn:
            return resourceInstance.GetModel("pawn");
            break;
        case PieceType::Rook:
            return resourceInstance.GetModel("rook");
            break;
        case PieceType::Bishop:
            return resourceInstance.GetModel("bishop");
            break;
        case PieceType::Knight:
            return resourceInstance.GetModel("knight");
            break;
        case PieceType::Queen:
            return resourceInstance.GetModel("queen");
            break;
        case PieceType::King:
            return resourceInstance.GetModel("king");
            break;
        }
    }
};

class Piece {
public:
    PieceType type;
    float gX;
    float gY;

    Color c;

    Piece(float gridX, float gridY, PieceType _type, Color _c) 
    {
        gX = gridX;
        gY = gridY;
        type = _type;
        c = _c;
    }


    void Draw()
    {
        DrawModelEx(ChessHelper::TypeToModel(type), ChessHelper::GridPos(gX, gY), {1.0f, 0.0f, 0.0f}, -90.0f, {1,1,1}, c);
    }
};

int main()
{
    raylib::Window w = raylib::Window(1280,720, "Starcraft Chess");

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    SetTargetFPS(120);

    resourceInstance = Resources("assets");

    Shader ls = resourceInstance.GetShader("lighting");

    int ambientLoc = GetShaderLocation(ls, "ambient");
    float f[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    SetShaderValue(ls, ambientLoc, f, SHADER_UNIFORM_VEC4);

    Model board = resourceInstance.GetModel("board");

    Model select = resourceInstance.GetModel("selected");

    bool turn = false;
    float turnLerp = 0;

    raylib::Camera3D c = raylib::Camera3D({ 25,60,-30}, {-40,-18,-30}, {0,1,0}, 45, CAMERA_PERSPECTIVE);


    Piece p = Piece(5, 2, PieceType::Pawn, WHITE);
    Piece p2 = Piece(4, 7, PieceType::Pawn, BLACK);

    float scale = 1;
    while (!w.ShouldClose())
    {
        if (turn && c.position.x > -100)
        {
            c.position.x = Lerp(25, -100, turnLerp);
            if (turnLerp < 0.5)
            {
                c.position.z = Lerp(-30, 30, turnLerp / 0.5);
                c.position.y = Lerp(60, 90, (turnLerp / 0.5));
            }
            else
            {
                c.position.y = Lerp(90, 60, (turnLerp - 0.5) / 0.5);
                c.position.z = Lerp(30, -30, (turnLerp - 0.5) / 0.5);
            }
        }
        else if (!turn && c.position.x < 25)
        {
            c.position.x = Lerp(-100, 25, turnLerp);
            if (turnLerp < 0.5)
            {
                c.position.z = Lerp(-30, -60, turnLerp / 0.5);
                c.position.y = Lerp(60, 90, (turnLerp / 0.5));
            }
            else
            {
                c.position.y = Lerp(90, 60, (turnLerp - 0.5) / 0.5);
                c.position.z = Lerp(-60, -30, (turnLerp - 0.5) / 0.5);
            }
        }

        if (turnLerp < 1)
            turnLerp += GetFrameTime() * 2;


        UpdateCamera(&c);

        if (IsKeyPressed(KEY_SPACE))
        {
            std::cout << "end turn" << std::endl;
            turn = !turn;
            turnLerp = 0;
        }
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
        BeginDrawing();

        w.ClearBackground(DARKGRAY);

        Vector2 mousePos = GetMousePosition();

        DrawFPS(0, 0);

        BeginMode3D(c);

        DrawModelEx(board, { 0,0,0 }, { 1.0f, 0.0f, 0.0f }, 90.0f, { 1,1,1 }, WHITE);
        
        for (int x = 1; x < 9; x++)
        {
            for (int y = 1; y < 9; y++)
            {
                Vector3 piecePos = ChessHelper::GridPos(x, y);
                Vector3 oPos = piecePos;
                piecePos.x += 8;
                piecePos.z -= 8;
                oPos.x -= 2;
                oPos.z += 2;
                Vector2 w = ChessHelper::WorldToScreenModel(piecePos, c);
                Vector2 w2 = ChessHelper::WorldToScreenModel(oPos, c);
                if (mousePos.x > w2.x && mousePos.y > w2.y && mousePos.x < w.x && mousePos.y < w.y)
                {
                    DrawModelEx(select, piecePos, { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, WHITE);
                }
            }
        }

        p.Draw();
        p2.Draw();

        EndMode3D();
        DrawText(("Mouse Pos: " + std::to_string((int)mousePos.x) + "," + std::to_string((int)mousePos.y)).c_str(), 0, 24, 18, WHITE);
        DrawText(("Turn Lerp: " + std::to_string(turnLerp)).c_str(), 0, 48, 18, WHITE);

        EndDrawing();
    }

    return 0;
}
