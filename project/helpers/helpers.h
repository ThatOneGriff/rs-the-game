#pragma once
#ifndef HELPERS_H
#define HELPERS_H

/// For text colouring.
#include <windows.h> /// Text colouring.
#define WHITE  7
#define GRAY   8
#define RED    4
#define GREEN  2
#define YELLOW 6


void textcolor(const short int color_code)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_code);
}

#endif /// HELPERS_H