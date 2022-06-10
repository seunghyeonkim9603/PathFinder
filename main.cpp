#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "framework.h"
#include "PathFinder.h"

#include <queue>
#include <unordered_map>
#include <functional>

#include "eAttribute.h"
#include "Node.h"
#include "Candidate.h"
#include "AStar.h"

#define NUM_ROWS (50)
#define NUM_COLUMNS (100)
#define GRID_SIZE (16)


AStar gPathFinder;

HINSTANCE hInst;
HPEN hGridPen;
HBRUSH hTileBrush;


char gTile[NUM_ROWS][NUM_COLUMNS];

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void RenderGrid(HDC hdc);
void RenderObstacle(HDC hdc);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PATHFINDER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PATHFINDER);
	wcex.lpszClassName = L"PathFinder";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowW(L"PathFinder", L"PathFinder", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (hWnd == nullptr)
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PATHFINDER));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

void RenderGrid(HDC hdc)
{
	constexpr int height = GRID_SIZE * NUM_ROWS;
	constexpr int width = GRID_SIZE * NUM_COLUMNS;
	int x = 0;
	int y = 0;

	HPEN hOldPen = (HPEN)SelectObject(hdc, hGridPen);
	{
		for (int i = 0; i <= NUM_COLUMNS; ++i)
		{
			MoveToEx(hdc, x, 0, nullptr);
			LineTo(hdc, x, height);
			x += GRID_SIZE;
		}
		for (int i = 0; i <= NUM_ROWS; ++i)
		{
			MoveToEx(hdc, 0, y, nullptr);
			LineTo(hdc, width, y);
			y += GRID_SIZE;
		}
	}
	SelectObject(hdc, hOldPen);
}

void RenderObstacle(HDC hdc)
{
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hTileBrush);
	{
		SelectObject(hdc, GetStockObject(NULL_PEN));
		for (int row = 0; row < NUM_ROWS; ++row)
		{
			char* line = gTile[row];
			for (int column = 0; column < NUM_COLUMNS; ++column)
			{
				if (line[column])
				{
					int x = column * GRID_SIZE;
					int y = row * GRID_SIZE;

					Rectangle(hdc, x, y, x + GRID_SIZE + 2, y + GRID_SIZE + 2);
				}
			}
		}
	}
	SelectObject(hdc, hOldBrush);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool isDrag = false;
	static bool isErase = false;

	switch (message)
	{
	case WM_CREATE:
	{
		hGridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
		hTileBrush = CreateSolidBrush(RGB(100, 100, 100));
	}
	break;
	case WM_LBUTTONDOWN:
	{
		isDrag = true;
		int x = GET_X_LPARAM(lParam) / GRID_SIZE;
		int y = GET_Y_LPARAM(lParam) / GRID_SIZE;

		if (NUM_ROWS <= y || NUM_COLUMNS <= x)
		{
			break;
		}
		if (gTile[y][x] == 1)
		{
			isErase = true;
		}
		else
		{
			isErase = false;
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		isDrag = false;
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (isDrag)
		{
			int x = GET_X_LPARAM(lParam) / GRID_SIZE;
			int y = GET_Y_LPARAM(lParam) / GRID_SIZE;

			if (NUM_ROWS <= y || NUM_COLUMNS <= x)
			{
				break;
			}
			gTile[y][x] = !isErase;
			InvalidateRect(hWnd, nullptr, false);
		}
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		{
			RECT rect;
			GetWindowRect(hWnd, &rect);

			HDC hBuffer = CreateCompatibleDC(hdc);
			HBITMAP hBmpBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			HBITMAP hBmpOldBuffer = (HBITMAP)SelectObject(hBuffer, hBmpBuffer);
			PatBlt(hBuffer, 0, 0, rect.right, rect.bottom, WHITENESS);

			RenderGrid(hBuffer);
			RenderObstacle(hBuffer);
			BitBlt(hdc, 0, 0, rect.right, rect.bottom, hBuffer, 0, 0, SRCCOPY);

			SelectObject(hBuffer, hBmpOldBuffer);
			DeleteObject(hBuffer);
			DeleteObject(hBmpBuffer);
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		DeleteObject(hGridPen);
		DeleteObject(hTileBrush);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
