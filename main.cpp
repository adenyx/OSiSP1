#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <winuser.h>
#include <string.h>

#define RECT_WIDTH 50
#define RECT_HEIGHT 50
#define STEP 5

HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
        _In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPSTR     lpCmdLine,
        _In_ int       nCmdShow
)
{
    WNDCLASSEX wcex;
    const wchar_t CLASS_NAME[] = L"Desktop App";
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = CLASS_NAME;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
            return 1;

    hInst = hInstance;

    HWND hWnd = CreateWindowEx(
            WS_EX_OVERLAPPEDWINDOW,
            CLASS_NAME,
            L"OSiSP2_lab1",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            500, 400,
            NULL,
            NULL,
            hInstance,
            NULL
    );

    ShowWindow(hWnd,
               nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rcSprite;
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    static long coor_x = (long) (rcClient.right / 3);
    static long coor_y = (long)(rcClient.bottom / 3);
    static bool is_bmp = false, moving = false;
    static HBITMAP hBitmap;
    static int vel_x = STEP, vel_y = STEP;
    BITMAP bitmap;

    switch (message)
    {
        case WM_CREATE:
            hBitmap = (HBITMAP) LoadImage(hInst, L"../picture.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            SetTimer(hWnd, 1, 3000, (TIMERPROC) NULL);
            if (moving)
                SetTimer(hWnd, 2, 30, (TIMERPROC) NULL);

            if (!is_bmp){
                SetRect(&rcSprite, coor_x, coor_y, coor_x + RECT_WIDTH, coor_y + RECT_HEIGHT);
                SelectObject(hdc, CreateSolidBrush(RGB(123, 104, 238)));
                Rectangle(ps.hdc, rcSprite.left, rcSprite.top, rcSprite.right, rcSprite.bottom);
            }
            else {
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
                GetObject(hBitmap, sizeof(BITMAP), &bitmap);
                StretchBlt(hdc, coor_x, coor_y, RECT_WIDTH, RECT_HEIGHT, hdcMem, 0, 0, bitmap.bmWidth,
                           bitmap.bmHeight, SRCCOPY);
                SelectObject(hdcMem, oldBitmap);
                DeleteDC(hdcMem);
            }
            EndPaint(hWnd, &ps);
            break;
        case WM_TIMER: {
            switch(wParam){
                case 1:
                    moving = true;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                case 2:
                    if (moving) {
                        coor_x += vel_x;
                        coor_y += vel_y;
                        if (coor_x + RECT_WIDTH >= rcClient.right)
                            vel_x = -abs(vel_x);
                        if (coor_x <= rcClient.left)
                            vel_x = abs(vel_x);
                        if (coor_y + RECT_HEIGHT >= rcClient.bottom)
                            vel_y = -abs(vel_y);
                        if (coor_y <= rcClient.top)
                            vel_y = abs(vel_y);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    break;
            }
            break;
        }
        case WM_MOUSEMOVE: {
            moving = false;
            coor_x = LOWORD(lParam);
            coor_y = HIWORD(lParam);
            if (coor_x + RECT_WIDTH > rcClient.right)
                coor_x = rcClient.right - RECT_WIDTH;
            if (coor_y + RECT_HEIGHT > rcClient.bottom)
                coor_y = rcClient.bottom - RECT_HEIGHT;
            KillTimer(hWnd, 1);
            KillTimer(hWnd, 2);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        case WM_MOUSEWHEEL: {
            moving = false;
            int wheel_offset = GET_WHEEL_DELTA_WPARAM(wParam);
            if (LOWORD(wParam) == MK_SHIFT) {
                if (wheel_offset > 0) {
                    if ((coor_x + RECT_WIDTH) < rcClient.right)
                        coor_x += STEP;
                }
                else {
                    if (coor_x > rcClient.left)
                        coor_x -= STEP;
                }
            }
            else {
                if (wheel_offset > 0) {
                    if (coor_y > rcClient.top)
                        coor_y -= STEP;
                }
                else {
                    if ((coor_y + RECT_HEIGHT) < rcClient.bottom)
                        coor_y += STEP;
                }
            }
            KillTimer(hWnd, 1);
            KillTimer(hWnd, 2);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        case WM_KEYDOWN: {
            moving = false;
            switch (wParam) {
                case VK_LEFT: {
                    if (coor_x > rcClient.left)
                        coor_x -= STEP;
                    break;
                }
                case VK_RIGHT: {
                    if (coor_x + RECT_WIDTH + STEP < rcClient.right)
                        coor_x += STEP;
                    break;
                }
                case VK_UP: {
                    if (coor_y > rcClient.top)
                        coor_y -= STEP;
                    break;
                }

                case VK_DOWN: {
                    if (coor_y + RECT_HEIGHT < rcClient.bottom)
                        coor_y += STEP;
                    break;
                }
                case VK_SPACE: {
                    is_bmp = !is_bmp;
                    break;
                }
            }
            KillTimer(hWnd, 1);
            KillTimer(hWnd, 2);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        case WM_DESTROY:
            DeleteObject(hBitmap);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}