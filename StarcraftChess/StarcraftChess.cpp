#include <iostream>
#include "include/raylib-cpp.hpp"
#include <map>
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include <functional>

#pragma comment (lib, "lib/raylibdll.lib")

#pragma region Resources Class
class Resources {
private:
    std::map<std::string, Model> cachedModels;
    std::string assetPath;
    Shader outlineShader;
public:
    Resources()
    {

    }
    Resources(std::string path)
    {
        assetPath = path;
    }

    void setModelOutlineShader(Shader s)
    {
        outlineShader = s;
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

    Shader GetShader(std::string vs, std::string fs)
    {
        return LoadShader((vs.size() > 0 ? (assetPath + "/shaders/" + vs + ".vs").c_str() : ""),
            fs.size() > 0 ? (assetPath + "/shaders/" + fs + ".fs").c_str() : "");
    }

    Model& GetModel(std::string name)
    {
        if (cachedModels[name].materialCount > 0)
            return cachedModels[name];

        Model m = cachedModels[name] = LoadModel((assetPath + "/models/" + name + ".obj").c_str());
        /*for (int i = 0; i < m.materialCount; i++)
            cachedModels[name].materials[i].shader = outlineShader;*/
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

struct DataPiece {
    int x, y;
    PieceType type;
    bool enPassant;
    Color c;
};

class ChessHelper {
public:
    // Helper function to convert a 2d array into 3d cords
    static Vector3 GridPos(float colX, float colY)
    {
        return { 2 - (10.0f * colY), 0, -72 + (10.0f * colX) };
    }

    // Helper function to check if a 2d array set of cords is hovered by the mouse
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

    // Helper function to check if a piece is in a spot
    static DataPiece IsPiece(float x, float y, std::list<DataPiece> pieces)
    {
        DataPiece pp;
        pp.enPassant = false;
        pp.type = PieceType::King;
        pp.x = -1;
        pp.y = -1;
        for (DataPiece p : pieces)
        {
            int xx = std::roundf(x);
            int yy = std::roundf(y);

            if (std::roundf(p.x) == xx && std::roundf(p.y) == yy)
                pp = p;
        }
        return pp;
    }

    // Helper function to convert the board state into a smaller data state
    static std::list<DataPiece> ConvertToDataState(std::list<Piece> pieces)
    {
        
    }

    // Helper function to convert a piece type to it's model.
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

    bool enPassantable = false;

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

    // Function to obtain the moves of the current piece.
    std::vector<Vector2> GetMoves(std::list<DataPiece> pieces)
    {
        std::vector<Vector2> moves;
        std::vector<bool> blocked;
        switch (type)
        {
        case PieceType::Pawn:
            
            break;
        case PieceType::Knight:

            break;
        case PieceType::Bishop:
            
            break;
        case PieceType::Queen:
            
        case PieceType::King:
            // I don't wanna talk about this code... lol
            
        case PieceType::Rook:

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
        int diff = std::abs(lastY - y);
        if (diff == 2)
            enPassantable = true;
        else
            enPassantable = false;
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

#pragma region UI Classes

struct UIItem {
    Texture2D texture;
    std::string detail;

    std::function<void(std::type_identity_t<UIItem*>)> callback;
    bool isSelected = false;
    Vector2 pos;
};

class ChessUI {
public:
    Texture2D actionBar;
    Texture2D actionBarItemBorder;
    Texture2D actionBarItemBorderSelected;
    Texture2D actionBarItemBG;
    std::vector<UIItem> items;

    bool isHovered = false;

    Vector2 anchorPos;

    ChessUI(Resources& resourceInstance)
    {
        actionBar = resourceInstance.GetTexture("Action_Bar_UI");
        actionBarItemBorder = resourceInstance.GetTexture("Action_Item_Border_UI");
        actionBarItemBorderSelected = resourceInstance.GetTexture("Action_Item_Border_UI_Selected");
        actionBarItemBG = resourceInstance.GetTexture("Action_Item_Background_UI");
        anchorPos = { 0, 720.0f - (actionBar.height - 42) };
    }

    void SetSelected(bool selected, std::string detail)
    {
        for (UIItem& item : items)
        {
            if (item.detail == detail)
                item.isSelected = selected;
        }
    }

    void Click(std::string detail)
    {
        for (UIItem& item : items)
        {
            if (item.detail == detail)
                item.callback(&item);
        }
    }

    void Click(Vector2 mouse)
    {
        for (UIItem& item : items)
        {
            // AABB Collision to check for a click
            if (item.pos.x < mouse.x &&
                item.pos.x + actionBarItemBorder.width > mouse.x &&
                item.pos.y < mouse.y &&
                item.pos.y + actionBarItemBorder.height > mouse.y)
                item.callback(&item);
        }
    }

    void CreateItem(std::string detail, std::string image, std::function<void(std::type_identity_t<UIItem*>)> callback, Resources& resourceInstance)
    {
        UIItem nI;
        nI.callback = callback;
        nI.detail = detail;
        nI.texture = resourceInstance.GetTexture(image);

        // Calculate some arbitary stuff because columns and rows and I love ui design
        float max = (actionBar.width - actionBarItemBorder.width) / (actionBarItemBorder.width);
        int pos = items.size() + 1;
        int col = std::floor(pos / max);
        // if col > 1, move it down twice the width (width and height should be the same), and contiune the higher it goes.
        // Max to check if its below 1 and return 1 as you dont want to do anything less than that cuz it's cringe.
        int row = std::max(std::floor((actionBarItemBorder.width + 12) * col), 1.0);

        int colX = (anchorPos.x + (actionBarItemBorder.width / 3)) + ((actionBarItemBorder.width + 12) * (items.size() - ((max - 1) * col)));

        // Create a vector and add the offset to the anchor pos.

        nI.pos = Vector2();
        nI.pos.x = anchorPos.x + (actionBarItemBorder.width / 2) + colX;
        nI.pos.y = anchorPos.y + (actionBarItemBorder.height / 2) + row;
        items.push_back(nI);
    }

    void DeleteItem(std::string detail)
    {
        for (int i = 0; i < items.size(); i++)
        {
            if (detail == items[i].detail) // If it's detail is the specified one
            {
                items.erase(items.begin() + i); // Erase it
                break; // Break so we stop looping because we shift the index stuff. it'll cause a out of bounds null ref if we dont.
            }
        }
    }

    void Draw(Vector2 mouse)
    {
        // AABB Collision to check for hover
        if (anchorPos.x < mouse.x &&
            anchorPos.x + actionBar.width > mouse.x &&
            anchorPos.y < mouse.y &&
            anchorPos.y + actionBar.height > mouse.y)
            isHovered = true;
        else
            isHovered = false;

        Color aColor = WHITE;

        DrawTextureEx(actionBar, anchorPos, 0, 1, aColor);

        // Draw items

        for (UIItem& item : items)
        {
            if (item.pos.x < mouse.x &&
                item.pos.x + actionBarItemBorder.width > mouse.x &&
                item.pos.y < mouse.y &&
                item.pos.y + actionBarItemBorder.height > mouse.y)
                aColor.a = 180;
            else
                aColor.a = 255;
            DrawTextureEx(actionBarItemBG, item.pos, 0, 1, aColor);
            DrawTextureEx(item.texture, item.pos, 0, 1, aColor);
            if (!item.isSelected)
                DrawTextureEx(actionBarItemBorder, item.pos, 0, 1, aColor);
            else
                DrawTextureEx(actionBarItemBorderSelected, item.pos, 0, 1, aColor);
        }
    }
};

#pragma endregion


int main()
{
    // Create a new window
    raylib::Window w = raylib::Window(1280,720, "Starcraft Chess");

    // Set anti-aliasing and FPS cap.

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    SetTargetFPS(120);

    // Create a new instance of our resource class with the "assets" folder.

    resourceInstance = Resources("assets");

    //Shader outline = resourceInstance.GetShader("", "outline");

    //resourceInstance.setModelOutlineShader(outline);

    Model board = resourceInstance.GetModel("board");

    Model select = resourceInstance.GetModel("selected");

    Font menuFont = resourceInstance.GetFont("Arial Bold");
    Texture2D menuBG = resourceInstance.GetTexture("menuBG");
    Texture2D aiCheckbox = resourceInstance.GetTexture("AI_Checkbox");

    raylib::Camera3D c = raylib::Camera3D({ 25,60,-30}, {-40,-18,-30}, {0,1,0}, 75, CAMERA_PERSPECTIVE);

    // Animation value

    float turnLerp = 0;

    std::vector<Piece> pieces = {};

    std::vector<Vector2> highlights = {};

    // Gameplay Variables

    bool turn = false; // what side of the board to show.
    bool side = false; // false for white, true for black

    bool ai = false; // You can control both sets of pieces OR an AI controls black and you can control white

    bool menu = true; // Whether or not if we should show the menu

    bool isMoving = false;

    Piece* selected = NULL;

    // Menu variables

    int menuSelected = 0;

    float menuLerp = 0;
    float menuTime = 0;

    Color clearColor = { 67,67,67,255 };

    float startLerpX = 25;
    float startLerpY = 60;
    float startLerpZ = -30;

    float speedModifier = 2;

    bool shouldClose = false;

    ChessUI cUi = ChessUI(resourceInstance);

    while (!shouldClose)
    {
        UpdateCamera(&c);
        if (!menu)
        {
#pragma region Game
            bool currentTurn = !turn; // white or blacks turn

#pragma region End Turn Animation
            if (turn && c.position.x > -100)
            {
                startLerpX = -100;
                startLerpZ = -60;
                startLerpY = 90;
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
                c.position.x = Lerp(startLerpX, 25, turnLerp);
                if (turnLerp < 0.5)
                {
                    c.position.z = Lerp(-30, -60, turnLerp / 0.5);
                    c.position.y = Lerp(60, 90, (turnLerp / 0.5));
                }
                else
                {
                    c.position.y = Lerp(startLerpY, 60, (turnLerp - 0.5) / 0.5);
                    c.position.z = Lerp(startLerpZ, -30, (turnLerp - 0.5) / 0.5);
                }
            }

            if (turnLerp < 1)
                turnLerp += GetFrameTime() * speedModifier;

            if (speedModifier > 2 || speedModifier < 2)
                speedModifier = Lerp(speedModifier, 2, 0.01);

#pragma endregion End Turn Animation

#pragma region Gameplay

            bool mDown = IsMouseButtonDown(0);
            bool mReleased = IsMouseButtonReleased(0);

            Vector2 mousePos = GetMousePosition();


            if (mReleased)
                cUi.Click(mousePos);

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

            DrawFPS(0, 0);

            BeginMode3D(c);

#pragma region 3D

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

            bool wipeHighlights = false;
            if (isMoving)
                wipeHighlights = false;


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
                    if (mReleased)
                    {
                        wipeHighlights = false;
                        // would be o^2 if this wasn't just one piece. Luckily it is only one piece
                        
                        selected = &p;
                        cUi.items.clear();
                        cUi.CreateItem("move", "Move_Icon", [&](UIItem* item) {
                            item->isSelected = true;
                            isMoving = true;
                        }, resourceInstance);
                        cUi.Click("move");
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
                    Color cc = WHITE;
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

                    if (takeId != -1)
                    {
                        cc = RED;
                        cc.a = 125;
                    }
                    DrawModelEx(select, pos, { 1.0f,0.0f,0.0f }, -90, { 1,1,1 }, cc);
                    if (stop)
                        continue;


                    // detect if you click here

                    if (ChessHelper::Tile_IsHovered(highlight.x, highlight.y, c, turn))
                    {
                        if (mDown && selected && isMoving)
                        {
                            selected->Move(highlight.x, highlight.y);
                            if (takeId != -1)
                                pieces.erase(pieces.begin() + takeId);
                            isMoving = false;
                            selected = NULL;
                        }
                    }
                }
           


            if (mDown && !selectedPiece && !cUi.isHovered)
            {
                cUi.items.clear();
                selected = NULL;
                highlights.clear();
            }

            EndMode3D();

#pragma endregion 3D

#pragma region 2D


            cUi.Draw(mousePos);

            EndDrawing();
#pragma endregion 2D
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

        bool enter = false;

        if (IsKeyPressed(KEY_ENTER))
            enter = true;

        Vector2 mPos = GetMousePosition();

        if ((mPos.x > 164 && mPos.y > 108 && mPos.x < 228 && mPos.y < 172) && IsMouseButtonReleased(0))
            ai = !ai;

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

        if (!ai)
            DrawTextureEx(aiCheckbox, { 164, 108 }, 0, 0.25, RED);
        else
            DrawTextureEx(aiCheckbox, { 164, 108 }, 0, 0.25, GREEN);

        DrawTextEx(menuFont, "Starcraft Chess", { 25,25 }, 64, 1, WHITE);
        switch (menuSelected)
        {
        case 0:
            DrawTextEx(menuFont, "PLAY", { 25,104 }, 42, 1, WHITE);
            DrawTextEx(menuFont, "QUIT", { 25,158 }, 42, 1, DARKGRAY);
            if (enter)
            {
                menu = false;
                turn = false;
                turnLerp = 0.5;
                menuLerp = 0;
                speedModifier = 0.4;
                startLerpX = c.position.x;
                startLerpY = c.position.y;
                startLerpZ = c.position.z;

                cUi.items.clear();

                // Start Pieces
                pieces.clear();
                /*pieces.push_back(Piece(4, 1, PieceType::King, WHITE));
                pieces.push_back(Piece(4, 2, PieceType::Pawn, WHITE));
                pieces.push_back(Piece(5, 2, PieceType::Pawn, WHITE));
                pieces.push_back(Piece(4, 8, PieceType::King, BLACK));
                pieces.push_back(Piece(4, 7, PieceType::Pawn, BLACK));
                pieces.push_back(Piece(5, 7, PieceType::Pawn, BLACK));*/
                // normal chess
                for (int i = 1; i < 9; i++)
                {
                    pieces.push_back(Piece(i, 2, PieceType::Pawn, WHITE));
                    pieces.push_back(Piece(i, 7, PieceType::Pawn, BLACK));
                }
            }
            break;
        case 1:
            DrawTextEx(menuFont, "PLAY", { 25,104 }, 42, 1, DARKGRAY);
            DrawTextEx(menuFont, "QUIT", { 25,158 }, 42, 1, WHITE);
            if (enter)
                shouldClose = true;
            break;
        }

        EndDrawing();

#pragma endregion Menu
        }
    }

    CloseWindow();



    return 0;
}
