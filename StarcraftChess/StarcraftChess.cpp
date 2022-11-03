#include <iostream>
#include "include/raylib-cpp.hpp"
#include <map>
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#pragma comment (lib, "lib/raylibdll.lib")

#pragma region Resources Class
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

    Texture2D GetTexture(std::string name)
    {
        return LoadTexture((assetPath + "/textures/" + name + ".png").c_str());
    }

    Font GetFont(std::string name)
    {
        return LoadFont((assetPath + "/fonts/" + name + ".ttf").c_str());
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
#pragma endregion Resources Class

Resources resourceInstance;

#pragma region Chess Piece Classes

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
        return { 2 - (10.0f * colY), 0, -72 + (10.0f * colX) };
    }

    static bool Tile_IsHovered(float x, float y, Camera c, bool isFlipped = false)
    {
        Vector2 mPos = GetMousePosition();
        Vector3 piecePos = ChessHelper::GridPos(x, y);
        Vector3 oPos = piecePos;
        piecePos.x += 8;
        piecePos.z -= 8;
        oPos.x -= 2;
        oPos.z += 2;
        Vector2 w = GetWorldToScreen(piecePos, c);
        Vector2 w2 = GetWorldToScreen(oPos, c);
        if (!isFlipped)
        {
            if (mPos.x > w2.x && mPos.y > w2.y && mPos.x < w.x && mPos.y < w.y)
                return true;
        }
        else
            if (mPos.x < w2.x && mPos.y < w2.y && mPos.x > w.x && mPos.y > w.y)
                return true;
        return false;
    }

    static bool IsPiece(float x, float y, std::map<int, std::map<int, bool>> pieces)
    {
        int xx = std::roundf(x);
        int yy = std::roundf(y);

        return pieces[xx][yy];
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
private:
    float toX;
    float toY;

    float lastX;
    float lastY;

    float lastMoveTime = 0;
public:
    bool isMoving = false;

    PieceType type;
    float gX;
    float gY;

    int moveLifeTime = 0;

    bool hasMoved = false;

    Color c;

    Piece(float gridX, float gridY, PieceType _type, Color _c)
    {
        gX = gridX;
        gY = gridY;

        lastX = gX;
        lastY = gY;
        toX = gX;
        toY = gY;

        lastMoveTime = 1;

        type = _type;
        c = _c;
    }


    std::vector<Vector2> GetMoves(std::map<int, std::map<int, bool>> pieces)
    {
        std::vector<Vector2> moves;
        std::vector<bool> blocked;
        switch (type)
        {
        case PieceType::Pawn:
            if (c.r == 255) // white
            {
                if (moveLifeTime < 1) // 2 possible moves rather than 1
                {
                    for(int i = 1; i < 3; i++)
                        if (!ChessHelper::IsPiece(gX, gY + i, pieces))
                            moves.push_back({ gX,gY + i });
                }
                else
                {
                    if (!ChessHelper::IsPiece(gX, gY + 1, pieces))
                        moves.push_back({gX,gY + 1.0f});
                }
            }
            else
            {
                if (moveLifeTime < 1) // 2 possible moves rather than 1
                {
                    for (int i = 1; i < 3; i++)
                        if (!ChessHelper::IsPiece(gX, gY + i, pieces))
                            moves.push_back({ gX,gY - i });
                }
                else
                {
                    if (!ChessHelper::IsPiece(gX, gY + 1, pieces))
                        moves.push_back({ gX,gY - 1.0f });
                }
            }
            break;
        case PieceType::Knight:
            moves.push_back({ gX - 1,gY + 2 });
            moves.push_back({ gX - 2,gY + 1 });
            moves.push_back({ gX - 1,gY - 2 });
            moves.push_back({ gX - 2,gY - 1 });
            moves.push_back({ gX + 1,gY + 2 });
            moves.push_back({ gX + 2,gY + 1 });
            moves.push_back({ gX + 1,gY - 2 });
            moves.push_back({ gX + 2,gY - 1 });
            break;
        case PieceType::Bishop:
            blocked = { false, false, false, false };
            for (int i = 1; i < 9; i++)
            {
                if (!blocked[0])
                {
                    if (ChessHelper::IsPiece(gX + i, gY + i, pieces))
                        blocked[0] = true;
                    moves.push_back({ gX + i,gY + i });
                }
                if (!blocked[1])
                {
                    if (ChessHelper::IsPiece(gX - i, gY + i, pieces))
                        blocked[1] = true;
                    moves.push_back({ gX - i,gY + i });
                }
                if (!blocked[2])
                {
                    if (ChessHelper::IsPiece(gX + i, gY - i, pieces))
                        blocked[2] = true;
                    moves.push_back({ gX + i,gY - i });
                }
                if (!blocked[3])
                {
                    if (ChessHelper::IsPiece(gX - i, gY - i, pieces))
                        blocked[3] = true;
                    moves.push_back({ gX - i,gY - i });
                }
            }
            break;
        case PieceType::Queen:
            blocked = { false, false, false, false, false, false, false, false };
            for (int i = 1; i < 9; i++)
            {
                if (!blocked[0])
                {
                    if (ChessHelper::IsPiece(gX + i, gY + i, pieces))
                        blocked[0] = true;
                    moves.push_back({ gX + i,gY + i });
                }
                if (!blocked[1])
                {
                    if (ChessHelper::IsPiece(gX - i, gY + i, pieces))
                        blocked[1] = true;
                    moves.push_back({ gX - i,gY + i });
                }
                if (!blocked[2])
                {
                    if (ChessHelper::IsPiece(gX + i, gY - i, pieces))
                        blocked[2] = true;
                    moves.push_back({ gX + i,gY - i });
                }
                if (!blocked[3])
                {
                    if (ChessHelper::IsPiece(gX - i, gY - i, pieces))
                        blocked[3] = true;
                    moves.push_back({ gX - i,gY - i });
                }
                if (!blocked[4])
                {
                    if (ChessHelper::IsPiece(gX + i, gY, pieces))
                        blocked[4] = true;
                    moves.push_back({ gX + i,gY });
                }
                if (!blocked[5])
                {
                    if (ChessHelper::IsPiece(gX - i, gY, pieces))
                        blocked[5] = true;
                    moves.push_back({ gX - i,gY });
                }
                if (!blocked[6])
                {
                    if (ChessHelper::IsPiece(gX, gY + i, pieces))
                        blocked[6] = true;
                    moves.push_back({ gX,gY + i });
                }
                if (!blocked[7])
                {
                    if (ChessHelper::IsPiece(gX, gY - i, pieces))
                        blocked[7] = true;
                    moves.push_back({ gX,gY - i});
                }
            }
            break;
        }

        // Clean moves to check for off board stuff

        std::vector<Vector2> copied = moves;

        int i = 0;
        for (Vector2 move : copied)
        {
            bool remove = false;

            if (move.x < 1)
                remove = true;
            if (move.x > 8)
                remove = true;

            if (move.y < 1)
                remove = true;
            if (move.y > 8)
                remove = true;

            if (remove)
            {
                moves.erase(moves.begin() + i);
                // shift back array
                i--;
            }

            i++;
        }

        return moves;
    }

    void Move(int x, int y)
    {
        // We take ints for easy stuff but they're actually floats
        moveLifeTime++;
        lastX = std::roundf(gX);
        lastY = std::roundf(gY);
        toX = x;
        toY = y;
        hasMoved = true;

        lastMoveTime = 0;
    }

    void Draw()
    {
        isMoving = lastMoveTime < 1;
        if (lastMoveTime < 1)
        {
            gX = Lerp(lastX, toX, lastMoveTime / 1);
            gY = Lerp(lastY, toY, lastMoveTime / 1);
            lastMoveTime += GetFrameTime() * 6;
        }
        else
        {
            gX = toX;
            gY = toY;
        }

        DrawModelEx(ChessHelper::TypeToModel(type), ChessHelper::GridPos(gX, gY), {1.0f, 0.0f, 0.0f}, -90.0f, {1,1,1}, c);
    }
};

std::map<int, std::map<int, bool>> convertBoardIntoBools(std::vector<Piece> pieces)
{
    std::map<int, std::map<int, bool>> board;
    for (Piece p : pieces)
        board[p.gX][p.gY] = true;

    return board;
}

#pragma endregion Chess Piece Classes


int main()
{
    // Create a new window
    raylib::Window w = raylib::Window(1280,720, "Starcraft Chess");

    // Set anti-aliasing and FPS cap.

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    SetTargetFPS(120);

    // Create a new instance of our resource class with the "assets" folder.

    resourceInstance = Resources("assets");

    Model board = resourceInstance.GetModel("board");

    Model select = resourceInstance.GetModel("selected");

    Font menuFont = resourceInstance.GetFont("Arial Bold");
    Texture2D menuBG = resourceInstance.GetTexture("menuBG");

    raylib::Camera3D c = raylib::Camera3D({ 25,60,-30}, {-40,-18,-30}, {0,1,0}, 45, CAMERA_PERSPECTIVE);

    // Animation value

    float turnLerp = 0;

    std::vector<Piece> pieces = {};

    std::vector<Vector2> highlights = {};

    // Gameplay Variables

    bool turn = false; // what side of the board to show.
    bool side = false; // false for white, true for black

    bool ai = false; // You can control both sets of pieces OR an AI controls black and you can control white

    bool menu = true; // Whether or not if we should show the menu

    Piece* selected = NULL;

    // Menu variables

    int menuSelected = 0;

    float menuLerp = 0;
    float menuTime = 0;

    Color clearColor = { 67,67,67,255 };

    while (!w.ShouldClose())
    {
        UpdateCamera(&c);
        if (!menu)
        {
#pragma region Game
            bool currentTurn = !turn; // white or blacks turn

#pragma region End Turn Animation
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

#pragma endregion End Turn Animation

#pragma region Gameplay

            bool mDown = IsMouseButtonDown(0);
            bool mReleased = IsMouseButtonReleased(0);

            if (IsKeyPressed(KEY_SPACE))
            {
                std::cout << "end turn" << std::endl;
                turn = !turn;
                turnLerp = 0;
                for (Piece& p : pieces)
                    p.hasMoved = false;
            }

#pragma endregion Gameplay

#pragma region Draw

            BeginDrawing();

            w.ClearBackground(clearColor);

            Vector2 mousePos = GetMousePosition();

            DrawFPS(0, 0);

            BeginMode3D(c);

            DrawModelEx(board, { 0,0,0 }, { 1.0f, 0.0f, 0.0f }, 90.0f, { 1,1,1 }, WHITE);

            // Selection box


            for (int x = 1; x < 9; x++)
            {
                for (int y = 1; y < 9; y++)
                {
                    if (ChessHelper::Tile_IsHovered(x, y, c, turn))
                    {
                        Vector3 pos = ChessHelper::GridPos(x, y);
                        // center
                        pos.x += 8;
                        pos.z -= 8;
                        DrawModelEx(select, pos, { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, WHITE);
                    }
                }
            }

            if (selected)
            {
                Vector3 pos = ChessHelper::GridPos(selected->gX, selected->gY);
                // center
                pos.x += 8;
                pos.y += 0.1;
                pos.z -= 8;
                DrawModelEx(select, pos, { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, GREEN);
            }

            bool wipeHighlights = true;

            std::map<int, std::map<int, bool>> boardBool = convertBoardIntoBools(pieces);

            bool selectedPiece = false;


            for (Piece& p : pieces)
            {
                p.Draw();

                // Check if you own the piece
                // Kinda jank

                if (ai)
                    if ((p.c.r == 255 && turn) || p.c.r == 0)
                        continue;
                // Check if you clicked on em

                bool canMove = ((p.c.r == 255 && !turn) || (p.c.r == 0 && turn)) && !p.hasMoved;

                if (ChessHelper::Tile_IsHovered(p.gX, p.gY, c, turn) && selected == NULL && canMove)
                {
                    wipeHighlights = false;
                    // would be o^2 if this wasn't just one piece. Luckily it is only one piece
                    highlights = p.GetMoves(boardBool);

                    if (mReleased)
                    {
                        selected = &p;
                        selectedPiece = true;
                    }
                }
            }

            for (Vector2& highlight : highlights)
            {
                Vector3 pos = ChessHelper::GridPos(highlight.x, highlight.y);
                // center
                pos.x += 8;
                pos.z -= 8;
                pos.y += 0.1;
                Color cc = YELLOW;
                cc.a = 125;

                bool stop = false;

                int pId = 0;
                int takeId = -1;
                for (Piece& p : pieces)
                {
                    if (p.gX == highlight.x && p.gY == highlight.y)
                    {
                        if (currentTurn && p.c.r == 255)
                        {
                            stop = true;
                            break;
                        }
                        else if (!currentTurn && p.c.r == 0)
                        {
                            stop = true;
                            break;
                        }
                        else
                            takeId = pId;
                    }
                    pId++;
                }

                if (stop)
                    continue;

                DrawModelEx(select, pos, { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, cc);

                // detect if you click here

                if (ChessHelper::Tile_IsHovered(highlight.x, highlight.y, c, turn))
                {
                    if (mDown && selected)
                    {
                        selected->Move(highlight.x, highlight.y);
                        if (takeId != -1)
                            pieces.erase(pieces.begin() + takeId);

                        selected = NULL;
                    }
                }
            }


            if (mDown && !selectedPiece)
                selected = NULL;

            if (wipeHighlights && selected == NULL)
                highlights.clear();

            EndMode3D();
            DrawText(("Mouse Pos: " + std::to_string((int)mousePos.x) + "," + std::to_string((int)mousePos.y)).c_str(), 0, 24, 18, WHITE);
            DrawText(("Turn Lerp: " + std::to_string(turnLerp)).c_str(), 0, 48, 18, WHITE);
            DrawText(("Highlights: " + std::to_string(highlights.size())).c_str(), 0, 72, 18, WHITE);

            EndDrawing();
#pragma endregion Draw

#pragma endregion Game
        }
        else
        {
#pragma region Menu

        if (IsKeyPressed(KEY_DOWN))
            menuSelected++;
        if (IsKeyPressed(KEY_UP))
            menuSelected--;

        if (menuSelected > 1)
            menuSelected = 0;
        if (menuSelected < 0)
            menuSelected = 1;

        // camera movement

        menuTime += GetFrameTime() * 0.08;

        if (menuTime > 1)
            menuTime = 0;

        menuLerp = menuTime / 1;

        if (menuLerp < 0.25)
        {
            c.position.x = Lerp(25, -100, menuLerp / 0.25);
            c.position.z = Lerp(-30, 0, menuLerp / 0.25);
        }
        else if (menuLerp < 0.5)
        {
            c.position.x = Lerp(-100, -80, (menuLerp - 0.25) / 0.25);
            c.position.z = Lerp(0, -60, (menuLerp - 0.25) / 0.25);
        }
        else if (menuLerp > 0.5)
        {
            c.position.x = Lerp(-80, 25, (menuLerp - 0.5) / 0.5);
            c.position.z = Lerp(-60, -30, (menuLerp - 0.5) / 0.5);
        }

        BeginDrawing();

        w.ClearBackground(clearColor);

        BeginMode3D(c);

        DrawModelEx(board, { 0,0,0 }, { 1.0f, 0.0f, 0.0f }, 90.0f, { 1,1,1 }, WHITE);

        EndMode3D();

        DrawTexture(menuBG, -360, 0, WHITE);

        DrawTextEx(menuFont, "Starcraft Chess", { 25,25 }, 64, 1, WHITE);
        switch (menuSelected)
        {
        case 0:
            DrawTextEx(menuFont, "PLAY", { 25,104 }, 42, 1, WHITE);
            DrawTextEx(menuFont, "QUIT", { 25,158 }, 42, 1, DARKGRAY);
            break;
        case 1:
            DrawTextEx(menuFont, "PLAY", { 25,104 }, 42, 1, DARKGRAY);
            DrawTextEx(menuFont, "QUIT", { 25,158 }, 42, 1, WHITE);
            break;
        }

        EndDrawing();

#pragma endregion Menu
        }
    }



    return 0;
}
