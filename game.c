#include <stdio.h>
#include <conio.h>
// #include <graphics.h>
#include <string.h>
#include <stdlib.h>
int board[8][8] = {
    {-1, 1, -1, 1, -1, 1, -1, 1},
    {1, -1, 1, -1, 1, -1, 1, -1},
    {-1, 1, -1, 1, -1, 1, -1, 1},
    {0, -1, 0, -1, 0, -1, 0, -1},
    {-1, 0, -1, 0, -1, 0, -1, 0},
    {2, -1, 2, -1, 2, -1, 2, -1},
    {-1, 2, -1, 2, -1, 2, -1, 2},
    {2, -1, 2, -1, 2, -1, 2, -1}
};
// 0=empty, 1=red, 2=black, -1: not movable
char statusMsg[50] = "";
int currentPlayer = 1; // red starts
int forcedRow = -1;
int forcedCol = -1;

// a,b,c,d,e,f,g,h,i
// 1,2,3,4,5,6,7,8
int cursorRow = 0, cursorCol = 0;       // currently highlighted square
int selectedRow = -1, selectedCol = -1; // -1 means no piece selected
int prevCursorRow = 0, prevCursorCol = 0;

void drawBoard()
{
    cleardevice(); // clear screen every redraw

    int xOffset = 20;
    int yOffset = 20;
    int cellWidth = 75;
    int cellHeight = 50;
    int i, j;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            int x1 = xOffset + j * cellWidth;
            int y1 = yOffset + i * cellHeight; // 20 + 1 * 50
            int x2 = x1 + cellWidth;
            int y2 = y1 + cellHeight;

            // Draw board cell
            if ((i + j) % 2 == 0)
                setfillstyle(SOLID_FILL, WHITE);
            else
                setfillstyle(SOLID_FILL, LIGHTGRAY);
            bar(x1, y1, x2, y2);

            // Draw piece if exists
            int piece = board[i][j];
            if (piece > 0)
            {
                int pieceColor = (piece == 1 || piece == 3) ? RED : BLACK;
                setcolor(pieceColor);
                setfillstyle(SOLID_FILL, pieceColor);

                int centerX = x1 + cellWidth / 2;
                int centerY = y1 + cellHeight / 2;
                int radius = (cellHeight < cellWidth ? cellHeight : cellWidth) / 2 - 5;

                circle(centerX, centerY, radius);        // draw border
                floodfill(centerX, centerY, pieceColor); // fill inside border

                // King marker
                if (piece == 3 || piece == 4)
                {
                    setcolor(YELLOW);
                    circle(centerX, centerY, radius / 2);
                    setcolor(pieceColor); // reset
                }
            }

            // Highlight selected piece (red border)
            if (i == selectedRow && j == selectedCol)
            {
                setcolor(LIGHTRED);
                rectangle(x1 + 2, y1 + 2, x2 - 2, y2 - 2);
                setcolor(WHITE);
            }

            // Highlight cursor (blue border) — drawn last so it’s on top
            if (i == cursorRow && j == cursorCol)
            {
                setcolor(LIGHTBLUE);
                rectangle(x1 + 2, y1 + 2, x2 - 2, y2 - 2);
                setcolor(WHITE);
            }
        }
    }

    // Draw status message at bottom
    setcolor(WHITE);
}
void redrawCell(int r, int c)
{
    int xOffset = 20, yOffset = 20;
    int cellW = 75, cellH = 50;

    int x1 = xOffset + c * cellW;
    int y1 = yOffset + r * cellH;
    int x2 = x1 + cellW;
    int y2 = y1 + cellH;

    // restore square
    if ((r + c) % 2 == 0)
        setfillstyle(SOLID_FILL, WHITE);
    else
        setfillstyle(SOLID_FILL, LIGHTGRAY);

    bar(x1, y1, x2, y2);

    // redraw piece if exists
    int piece = board[r][c];
    if (piece > 0)
    {
        int color = (piece == 1 || piece == 3) ? RED : BLACK;
        setcolor(color);
        setfillstyle(SOLID_FILL, color);

        int cx = x1 + cellW / 2;
        int cy = y1 + cellH / 2;
        int rad = 18;

        circle(cx, cy, rad);
        floodfill(cx, cy, color);

        if (piece == 3 || piece == 4)
        {
            setcolor(YELLOW);
            circle(cx, cy, rad / 2);
        }
    }
}
void drawUI()
{
    int xOffset = 20;        // same as board
    int boardWidth = 8 * 75; // 8 columns * 75 px
    int y = getmaxy() - 60;  // bar height

    // Background bar (same width as board)
    setfillstyle(SOLID_FILL, DARKGRAY);
    bar(xOffset, y, xOffset + boardWidth, getmaxy());

    setcolor(WHITE);

    int padding = 5; // padding inside rectangles
    int boxHeight = 20;

    // Calculate box widths proportionally
    int turnBoxW = 120;
    int selectBoxW = 140;
    // int statusBoxW = boardWidth - turnBoxW - selectBoxW - 3 * padding; // remaining space

    // ---- Turn box ----
    rectangle(xOffset + padding, y + padding, xOffset + padding + turnBoxW, y + padding + boxHeight);
    if (currentPlayer == 1)
        outtextxy(xOffset + 2 * padding, y + 11, "Turn: RED");
    else
        outtextxy(xOffset + 2 * padding, y + 11, "Turn: BLACK");

    // ---- Selection box ----
    int selX = xOffset + padding + turnBoxW + padding;
    rectangle(selX, y + padding, selX + selectBoxW, y + padding + boxHeight);
    if (selectedRow != -1)
        outtextxy(selX + 3, y + 11, "Piece selected");
    else
        outtextxy(selX + 3, y + 11, "No piece selected");

    // ---- Status message box ----
    int statusX = selX + selectBoxW + padding;
    rectangle(statusX, y + padding, xOffset + boardWidth - padding, y + padding + boxHeight);
    outtextxy(statusX + 5, y + 8, statusMsg);

    // ---- Controls below bar ----
    outtextxy(xOffset + 10, y + boxHeight + 3 * padding, "ENTER=Select/Move   ESC=Cancel   Q=Quit");
}

// void firstpiece()
// {
//     board[2][1] = 0;
//     board[3][0] = 1;
// }
int colFromChar(char c)
{
    return c - 'a'; // 'a'→0, 'b'→1, ...
}

int rowFromChar(char c)
{
    return c - '1'; // '1'→0, '2'→1, ...
}
// Helper function to check if target is opponent
int isOpponentPiece(int playerPiece, int targetPiece)
{
    if (playerPiece == 1 || playerPiece == 3) // Red
        return (targetPiece == 2 || targetPiece == 4);
    else // Black
        return (targetPiece == 1 || targetPiece == 3);
}

int pieceCanCapture(int r, int c)
{
    int piece = board[r][c];

    if (piece <= 0)
        return 0;

    int dirs[4][2] = {{2, 2}, {2, -2}, {-2, 2}, {-2, -2}};

    for (int i = 0; i < 4; i++)
    {
        int tr = r + dirs[i][0];
        int tc = c + dirs[i][1];
        int mr = r + dirs[i][0] / 2;
        int mc = c + dirs[i][1] / 2;

        if (tr < 0 || tr > 7 || tc < 0 || tc > 7)
            continue;

        if (board[tr][tc] == 0 && isOpponentPiece(piece, board[mr][mc]))
            return 1;
    }
    return 0;
}

int hasAnyCapture(int player)
{
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            int piece = board[r][c];

            // Only consider current player's pieces
            if (piece != player && piece != player + 2)
                continue;

            if (pieceCanCapture(r, c))
                return 1; // at least one piece can capture
        }
    }
    return 0;
}

int countPieces(int player)
{
    int i, j, count = 0;

    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            if (board[i][j] == player || board[i][j] == player + 2)
                count++;

    return count;
}
int playerHasMove(int player)
{
    int i, j;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            int piece = board[i][j];
            if (piece != player && piece != player + 2)
                continue;

            // Try all diagonal directions
            int dr, dc;
            for (dr = -2; dr <= 2; dr += 4)
            {
                for (dc = -2; dc <= 2; dc += 4)
                {
                    int tr = i + dr;
                    int tc = j + dc;

                    if (tr < 0 || tr > 7 || tc < 0 || tc > 7)
                        continue;

                    if (board[tr][tc] == 0)
                        return 1;
                }
            }
        }
    }
    return 0;
}

int movePiece(int fr, int fc, int tr, int tc)
{
    int dr = tr - fr;
    int dc = tc - fc;
    int piece = board[fr][fc];

    int captureExists = hasAnyCapture(currentPlayer);
    int selectedCanCapture = pieceCanCapture(fr, fc);

    if (captureExists && !selectedCanCapture)
    {
        strcpy(statusMsg, "You must move a capturing piece");
        return 0;
    }

    // --- Out of bounds ---
    if (fr < 0 || fr > 7 || fc < 0 || fc > 7 || tr < 0 || tr > 7 || tc < 0 || tc > 7)
    {
        strcpy(statusMsg, "Out of bounds");
        return 0;
    }

    // --- Forced jump check ---
    if (forcedRow != -1)
    {
        if (fr != forcedRow || fc != forcedCol)
        {
            strcpy(statusMsg, "Must continue jump with same piece");
            return 0;
        }
    }

    // --- Must be current player's piece ---
    if (piece != currentPlayer && piece != currentPlayer + 2)
    {
        strcpy(statusMsg, "Not your turn or no piece");
        return 0;
    }

    // --- Destination must be empty ---
    if (board[tr][tc] != 0)
    {
        strcpy(statusMsg, "Destination blocked");
        return 0;
    }

    // --- Must move diagonally ---
    if (!((dr == dc) || (dr == -dc)))
    {
        strcpy(statusMsg, "Must move diagonally");
        return 0;
    }

    // --- Mandatory capture rule ---
    if (hasAnyCapture(currentPlayer) && !pieceCanCapture(fr, fc))
    {
        strcpy(statusMsg, "You must capture with a capturing piece");
        return 0;
    }

    // ================= NORMAL MOVE (1-step) =================
    if ((piece == 1 && dr == 1 && (dc == 1 || dc == -1)) ||
        (piece == 2 && dr == -1 && (dc == 1 || dc == -1)) ||
        ((piece == 3 || piece == 4) && (abs(dr) == 1 && abs(dc) == 1)))
    {
        if (forcedRow != -1)
        {
            strcpy(statusMsg, "Must capture again");
            return 0;
        }

        board[tr][tc] = piece;
        board[fr][fc] = 0;

        // --- King promotion ---
        if (piece == 1 && tr == 7)
            piece = 3;
        if (piece == 2 && tr == 0)
            piece = 4;
        board[tr][tc] = piece;

        // Switch turn
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
        strcpy(statusMsg, "Move OK");
        return 1;
    }

    // ================= CAPTURE MOVE (2-step) =================
    if ((piece == 1 && dr == 2 && (dc == 2 || dc == -2)) ||
        (piece == 2 && dr == -2 && (dc == 2 || dc == -2)) ||
        ((piece == 3 || piece == 4) && (abs(dr) == 2 && abs(dc) == 2)))
    {
        int midR = fr + dr / 2;
        int midC = fc + dc / 2;

        if (!isOpponentPiece(piece, board[midR][midC]))
        {
            strcpy(statusMsg, "No piece to capture");
            return 0;
        }

        // --- Perform capture ---
        board[tr][tc] = piece;
        board[fr][fc] = 0;
        board[midR][midC] = 0;
        redrawCell(midR, midC);

        // --- King promotion ---
        if (piece == 1 && tr == 7)
            piece = 3;
        if (piece == 2 && tr == 0)
            piece = 4;
        board[tr][tc] = piece;

        // --- Check for another capture ---
        int canJumpAgain = 0;
        int dR[4], dC[4], dirCount = 0;

        if (piece == 1)
        {
            dR[0] = 2;
            dC[0] = 2;
            dR[1] = 2;
            dC[1] = -2;
            dirCount = 2;
        }
        else if (piece == 2)
        {
            dR[0] = -2;
            dC[0] = 2;
            dR[1] = -2;
            dC[1] = -2;
            dirCount = 2;
        }
        else
        { // King
            dR[0] = 2;
            dC[0] = 2;
            dR[1] = 2;
            dC[1] = -2;
            dR[2] = -2;
            dC[2] = 2;
            dR[3] = -2;
            dC[3] = -2;
            dirCount = 4;
        }

        for (int i = 0; i < dirCount; i++)
        {
            int r = tr + dR[i];
            int c = tc + dC[i];
            int mr = tr + dR[i] / 2;
            int mc = tc + dC[i] / 2;

            if (r >= 0 && r < 8 && c >= 0 && c < 8 && board[r][c] == 0 && isOpponentPiece(piece, board[mr][mc]))
            {
                canJumpAgain = 1;
                break;
            }
        }

        if (canJumpAgain)
        {
            forcedRow = tr;
            forcedCol = tc;
            strcpy(statusMsg, "Captured! Continue jumping");
            return 1;
        }

        // --- No more jumps → switch turn ---
        forcedRow = -1;
        forcedCol = -1;
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
        strcpy(statusMsg, "Capture complete");

        // --- Check win condition ---
        if (countPieces(1) == 0 || !playerHasMove(1))
        {
            strcpy(statusMsg, "BLACK WINS!");
            drawUI();
        }
        else if (countPieces(2) == 0 || !playerHasMove(2))
        {
            strcpy(statusMsg, "RED WINS!");
            drawUI();
        }

        return 1;
    }

    // --- Invalid move ---
    strcpy(statusMsg, "Invalid move");
    return 0;
}

void keyboardInput()
{
    int ch;
    while (1)
    {
        ch = getch();
        prevCursorRow = cursorRow;
        prevCursorCol = cursorCol;

        // Arrow keys are detected as two-part codes
        if (ch == 0 || ch == 0xE0)
        {
            ch = getch();
            switch (ch)
            {
            case 72: // UP
                if (cursorRow > 0)
                    cursorRow--;

                break;
            case 80: // DOWN
                if (cursorRow < 7)
                    cursorRow++;

                break;
            case 75: // LEFT
                if (cursorCol > 0)
                    cursorCol--;

                break;
            case 77: // RIGHT
                if (cursorCol < 7)
                    cursorCol++;

                break;
            }
        }
        else if (ch == 13) // ENTER
        {
            if (selectedRow == -1) // no piece selected yet
            {
                int piece = board[cursorRow][cursorCol];
                if (piece == currentPlayer || piece == currentPlayer + 2)
                {
                    selectedRow = cursorRow;
                    selectedCol = cursorCol;
                    strcpy(statusMsg, "Piece selected");
                    drawUI();
                }
                else
                {
                    strcpy(statusMsg, "Select your own piece");
                    drawUI();
                }
            }
            else // piece already selected → try move
            {
                if (movePiece(selectedRow, selectedCol, cursorRow, cursorCol))
                {
                    // erase old position
                    redrawCell(selectedRow, selectedCol);

                    // redraw destination
                    redrawCell(cursorRow, cursorCol);

                    // clear selection highlight
                    selectedRow = -1;
                    selectedCol = -1;
                    drawUI();
                }

                else
                {
                    // Move failed, keep piece selected
                    drawUI();
                }
            }
        }

        else if (ch == 27) // ESC to cancel selection
        {
            selectedRow = -1;
            selectedCol = -1;
            strcpy(statusMsg, "Selection canceled");
            drawUI();
        }
        else if (ch == 'q' || ch == 'Q') // Quit
        {
            closegraph();
            exit(0); // Immediately exit the program
        }

        redrawCell(prevCursorRow, prevCursorCol);
        redrawCell(cursorRow, cursorCol);

        // draw cursor highlight
        setcolor(LIGHTBLUE);
        rectangle(
            20 + cursorCol * 75 + 2,
            20 + cursorRow * 50 + 2,
            20 + cursorCol * 75 + 73,
            20 + cursorRow * 50 + 48);

        // redraw after each key press
    }
}

int main()
{
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "c:\Users\computer clininc\Desktop\");

    drawBoard();
    drawUI();
    keyboardInput();

    closegraph();
    return 0;
}

// centerX = j * cellSize + cellSize / 2;
// centerY = i * cellSize + cellSize / 2;

