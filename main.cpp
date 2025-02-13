#include "includes.hpp"
#include <iostream>
#include <thread>
#include <thread>
#include <cstdlib>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void displayBaseAddress() {
	uintptr_t Base = Driver::GetBase(); 
}
void SetConsoleColor(WORD color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}
void SetConsoleTransparency(int transparency) {
	HWND hwnd = GetConsoleWindow();
	LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, (BYTE)transparency, LWA_ALPHA);
}
void FadeColors() {
	WORD colors[] = { FOREGROUND_RED, FOREGROUND_BLUE, FOREGROUND_GREEN,
					 FOREGROUND_RED | FOREGROUND_GREEN,
					 FOREGROUND_BLUE | FOREGROUND_GREEN,
					 FOREGROUND_RED | FOREGROUND_BLUE,
					 FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE };

	int colorCount = sizeof(colors) / sizeof(colors[0]);

	while (true) {
		for (int i = 0; i < colorCount; i++) {
			SetConsoleColor(colors[i]);
			std::cout << "\rColor fading effect...";  // Overwrites text to prevent spamming new lines
			std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Adjust for faster/slower fading
		}
	}
}

void main()
{
	SetConsoleTitleA("Sofmain Recode | Landen1337                                                        //////  .gg/visualstudio  //////");
	SetConsoleTransparency(200);
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	SetConsoleColor(FOREGROUND_BLUE);
	printf("Waiting for Fortnite...");

	while (game_wnd == 0)
	{
		Sleep(1);
		
		processID = Driver::FindProcess(L"FortniteClient-Win64-Shipping.exe"); 
		game_wnd = get_process_wnd(processID); 
		Sleep(1);
	}

	system("cls");
	SetConsoleTitleA("Sofmain Recode | Landen1337                                                        //////  .gg/visualstudio  //////");
	SetConsoleTransparency(200);
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	SetConsoleColor(FOREGROUND_BLUE);
	printf("Sofmain Rework by Landen1337\n");

	if (!input::init())
	{
		printf("The input was not initialized :("); 
		Sleep(3000);
		exit(0);
	}

	if (!gui::init())
	{
		printf("The gui was not initialized :("); 
		Sleep(3000);
		exit(0);
	}
	if (!Driver::Init())
	{
		printf("The driver was not initialized :("); 
		Sleep(3000);
		exit(0);
	}
	if (!Driver::CR3())
	{
		//ts is annoying so i stopped tryna get this to work 
	}
	Base = Driver::GetBase();

	if (!Base) 
	{
		printf("The driver couldn't get the base address"); 
		exit(0);
	}
	
	create_overlay(); 
	directx_init();
	render_loop();

}


