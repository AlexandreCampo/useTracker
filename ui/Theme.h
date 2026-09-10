#pragma once

#include "imgui.h"

namespace TrackerUI
{
inline ImVec4 Color(unsigned rgb, float alpha = 1.0f)
{
    return ImVec4(((rgb >> 16) & 255) / 255.f, ((rgb >> 8) & 255) / 255.f,
                  (rgb & 255) / 255.f, alpha);
}
enum class Mode { Dark, Light };
struct Palette
{
    ImVec4 Paper, Surface, Canvas, Wash, Ink, Muted, Line;
    ImVec4 Accent, AccentSoft, OnAccent, Red, Green, Bookmark;
};
extern Palette Colors;

extern ImFont* Mono;
extern ImFont* Heading;
void Apply(float scale, Mode mode);
void ApplyColors(Mode mode);
void Hint(const char* text);
void Label(const char* text);
void Mark(ImVec2 position, float size);
enum class Icon { Play, Pause, Stop, Previous, Next, Up, Down, Remove, Fit };
bool IconButton(const char* id, Icon icon, const char* hint, float scale, bool accent = false);
bool AccentButton(const char* label, ImVec2 size = ImVec2(0, 0));
}
