#include "stdafx.h"
#include "main.h"
#include "ServerDevice.h"
#include "Packet.h"
#include "PieceManager.h"

#ifdef UNICODE
     #pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
     #pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#define MAX_LOADSTRING 100
#define D_MAPXSIZE 8
#define D_MAPYSIZE 8

// 전역 변수:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
ServerDevice device;
HWND winHandle;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
static int heroX, heroY, heroID;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_CLIENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	winHandle = hWnd;
	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static std::vector<std::vector<int>> board;
	static HBITMAP pawnImage;

	switch (message) {
	case WM_CREATE: {
		board.resize(D_MAPYSIZE);
		for (int y = 0; y < D_MAPYSIZE; ++y)
			for (int x = 0; x < D_MAPXSIZE; ++x)
				board[y].emplace_back(static_cast<int>(E_CHESS_TYPE::E_NONE));

		std::string texturePath = { "Data/Pawn.bmp" };
		pawnImage = (HBITMAP)LoadImage(NULL, texturePath.data(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		
		std::cout << "IP 를 입력해주세요" << std::endl;
		std::string ip;
		std::cin >> ip;

		if (!device.initialize(ip.c_str(), 9000, winHandle))
			std::cout << "접속 실패" << std::endl;
		else {
			PlayerInfo info{ device.recvInfo() } ;
			heroX = info.pos.xPos;
			heroY = info.pos.yPos;
			heroID = info.PlayerID;
			std::cout << "아이디 : " << info.PlayerID << " x, y : " <<
				heroX << "   " << heroY << std::endl;
			PieceManager::getInstance().setHeroID(heroID);
			PieceManager::getInstance().initPiece(E_CHESS_TYPE::E_PAWN,
				heroID, heroX, heroY);
			
		}
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		for (int y = 0; y < D_MAPYSIZE; ++y)
			for (int x = 0; x < D_MAPXSIZE; ++x) {
				int offsetX = x * 64;
				int offsetY = y * 64;
				Rectangle(hdc, offsetX, offsetY, offsetX + 64, offsetY + 64);
			}
		HDC memDC = CreateCompatibleDC(hdc);
		SelectObject(memDC, pawnImage);
		auto [start, end] = PieceManager::getInstance().getAllPiece();
		for (start; start != end; ++start) {
			auto [x, y] = (*start).second.getPosition();
			BitBlt(hdc, x * 64, y * 64, 64, 64, memDC, 0, 0, SRCCOPY);
		}
		
		DeleteDC(memDC);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			device.sendKey(E_PACKET_TYPE::E_LEFT, heroID);
			break;
		case VK_RIGHT:
			device.sendKey(E_PACKET_TYPE::E_RIGHT, heroID);
			break;
		case VK_UP:
			device.sendKey(E_PACKET_TYPE::E_UP, heroID);
			break;
		case VK_DOWN:
			device.sendKey(E_PACKET_TYPE::E_DOWN, heroID);
			break;
		}
		break;
	case WM_DESTROY:
		device.destroy();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

