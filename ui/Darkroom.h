#pragma once

#include "imgui.h"

namespace Darkroom
{
inline ImVec4 Color(unsigned rgb, float alpha = 1.0f)
{
    return ImVec4(((rgb >> 16) & 255) / 255.f, ((rgb >> 8) & 255) / 255.f,
                  (rgb & 255) / 255.f, alpha);
}
inline const ImVec4 Paper = Color(0x171b20);
inline const ImVec4 Surface = Color(0x1e242b);
inline const ImVec4 Canvas = Color(0x101418);
inline const ImVec4 Wash = Color(0x252c34);
inline const ImVec4 Ink = Color(0xedf0f4);
inline const ImVec4 Muted = Color(0xb4bdc8);
inline const ImVec4 Line = Color(0x39424e);
inline const ImVec4 Amber = Color(0xe7b665);
inline const ImVec4 AmberSoft = Color(0x383127);
inline const ImVec4 Red = Color(0xef998b);
inline const ImVec4 Green = Color(0xa5cbbf);

extern ImFont* Mono;
extern ImFont* Heading;
void Apply(float scale);
void Hint(const char* text);
void Label(const char* text);
void Mark(ImVec2 position, float size);
enum class Icon { Play, Pause, Stop, Previous, Next, Up, Down, Remove, Fit };
bool IconButton(const char* id, Icon icon, const char* hint, float scale, bool accent = false);
bool AccentButton(const char* label, ImVec2 size = ImVec2(0, 0));
}
