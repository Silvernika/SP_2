#ifndef UNICODE 
#define UNICODE 
#endif // UNICODE 

#include <Windows.h> 
#include<strsafe.h> 
#include <vector> 
#include <string> 
#include <fstream> 

#define MAX_STRING_LEN 255 

using namespace std;

const int columnNumber = 3;
const  int stringNumber = 3;
const int MAX_STRINGS = columnNumber * stringNumber;

HWND edits[MAX_STRINGS];

int fontHeight = 20;
char** dynamicText;
string TextParts[MAX_STRINGS];

HWND hWnd;
bool shouldUpdate = false;


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrawTable(HDC hDC, int windowWidth, int windowHeight);
void InitDC(HWND hWnd, int windowWidth, int windowHeight);
void Redraw(HWND hWnd, int windowWidth, int windowHeight);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;
    //HWND hWnd; 
    MSG msg;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_VREDRAW | CS_HREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"SecondLab";
    wcex.hIconSm = wcex.hIcon;

    if (!RegisterClassEx(&wcex))
    {
        return 0;
    }

    hWnd = CreateWindowEx(0, L"SecondLab", L"Lab2", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    // Creating dynamic array of strings 
    ifstream in("C:/Users/User/Desktop/Lab2 — копия/x64/Debug/ff.txt");
    string buffer;
    int i = 0;
    if (in.is_open()) {
        while (!in.eof()) {
            char symbol = in.get();
            if (symbol != ' ')
                buffer += symbol;
            else
            {
                TextParts[i % MAX_STRINGS] += buffer;
                i++;
                buffer = "";
            }

        }
    }
 

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static int width, height;

    switch (uMsg)
    {
    case WM_SIZE:
        width = LOWORD(lParam);
        height = HIWORD(lParam);
        break;
    case WM_PAINT:
        InitDC(hWnd, width, height);
        break;
    case WM_DESTROY:
        free(dynamicText);
        PostQuitMessage(0);
    case WM_COMMAND:
        if (HIWORD(wParam) == EN_SETFOCUS && shouldUpdate)
        {
            shouldUpdate = false;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        if (HIWORD(wParam) == EN_CHANGE)
        {
            //UpdateEditsText(LOWORD(wParam)); 
            shouldUpdate = true;
        }
        break;
    case WM_KEYUP:
    {
        switch (wParam)
        {
        case VK_UP:
            fontHeight++;
            break;
        case VK_DOWN:
            fontHeight--;
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void DrawVerticalBorders(HDC hDC, int windowWidth, int windowHeight)
{
    int columnWidth = windowWidth / columnNumber;

    for (int i = 0; i < columnNumber; i++)
    {
        MoveToEx(hDC, i * columnWidth, 0, NULL);
        LineTo(hDC, i * columnWidth, windowHeight);
    }
    MoveToEx(hDC, windowWidth - 1, 0, NULL);
    LineTo(hDC, windowWidth - 1, windowHeight);
}

void DrawHorizontalBorders(HDC hDC, int windowWidth, int rowHeight)
{
    MoveToEx(hDC, 0, rowHeight, NULL);
    LineTo(hDC, windowWidth, rowHeight);
}


void DrawTable(HDC currDC, int windowWidth, int windowHeight) {
    int columnWidth = windowWidth / columnNumber;
    DrawHorizontalBorders(currDC, windowWidth, 1);
    int i = 0;
    int editInd = 0; int textHeight = 0;
    int fontSize = 20; RECT rect = { 0, 0, 0, windowHeight };
    while (i < MAX_STRINGS) {
        rect.top = textHeight;
        //rect.bottom = rect.top+f;  

        int maxTextHeight = 0;  for (int j = 0; (j < columnNumber) && (i < MAX_STRINGS); j++)
        {
            const char* str = TextParts[i].c_str();
            rect.left = j * columnWidth + 3;
            rect.right = (j + 1) * columnWidth;   int currentTextHeight = DrawTextA(currDC, str, strlen(str), &rect, DT_EDITCONTROL | DT_WORDBREAK);
            if (maxTextHeight < currentTextHeight) {
                maxTextHeight = currentTextHeight;
            }
            i++;
        }
  
        textHeight += maxTextHeight;
        DrawHorizontalBorders(currDC, windowWidth, textHeight);
    }
    DrawVerticalBorders(currDC, windowWidth, textHeight);
}

void InitDC(HWND hWnd, int windowWidth, int windowHeight) {
    RECT rect; HDC memDC;
    HBITMAP hBmp, hOldBmp; PAINTSTRUCT ps;
    HDC currDC = BeginPaint(hWnd, &ps);
    //DeleteEdits();
    GetClientRect(hWnd, &rect);

    memDC = CreateCompatibleDC(currDC); hBmp = CreateCompatibleBitmap(currDC, rect.right - rect.left, rect.bottom - rect.top);
    hOldBmp = (HBITMAP)SelectObject(memDC, hBmp);
    HBRUSH hBkgrndBrush = CreateSolidBrush(RGB(255, 255, 255)); FillRect(currDC, &rect, hBkgrndBrush);
    DeleteObject(hBkgrndBrush);
    SetBkMode(currDC, TRANSPARENT); HFONT hFont = CreateFont(fontHeight, 0, 0, 0, FW_BOLD, false, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, NULL); SelectObject(currDC, hFont);
    DrawTable(currDC, rect.right - rect.left, rect.bottom - rect.top);
    DeleteDC(memDC);
    EndPaint(hWnd, &ps);
}