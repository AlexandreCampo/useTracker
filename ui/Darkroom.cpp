#include "Darkroom.h"

extern const unsigned char DarkroomSansData[];
extern const int DarkroomSansSize;
extern const unsigned char DarkroomMonoData[];
extern const int DarkroomMonoSize;

namespace Darkroom
{
ImFont* Mono = nullptr;
ImFont* Heading = nullptr;

void Apply(float scale)
{
    auto& io = ImGui::GetIO();
    io.Fonts->Clear();
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.OversampleH = 2;
    config.OversampleV = 2;
    io.FontDefault = io.Fonts->AddFontFromMemoryTTF(
        const_cast<unsigned char*>(DarkroomSansData), DarkroomSansSize, 16.f * scale, &config);
    Mono = io.Fonts->AddFontFromMemoryTTF(
        const_cast<unsigned char*>(DarkroomMonoData), DarkroomMonoSize, 14.f * scale, &config);
    Heading = io.Fonts->AddFontFromMemoryTTF(
        const_cast<unsigned char*>(DarkroomMonoData), DarkroomMonoSize, 25.f * scale, &config);
    io.FontGlobalScale = 1.f;
    io.Fonts->Build();

    auto& s = ImGui::GetStyle();
    s = ImGuiStyle();
    ImGui::StyleColorsDark(&s);
    s.WindowPadding = ImVec2(12, 10);
    s.FramePadding = ImVec2(9, 6);
    s.ItemSpacing = ImVec2(7, 6);
    s.ItemInnerSpacing = ImVec2(6, 4);
    s.ScrollbarSize = 11;
    s.GrabMinSize = 10;
    s.WindowRounding = 5;
    s.ChildRounding = 3;
    s.FrameRounding = 3;
    s.PopupRounding = 4;
    s.GrabRounding = 2;
    s.TabRounding = 3;
    s.WindowBorderSize = 1;
    s.ChildBorderSize = 1;
    s.FrameBorderSize = 0;
    s.TabBarBorderSize = 1;
    auto* c = s.Colors;
    c[ImGuiCol_Text] = Ink;
    c[ImGuiCol_TextDisabled] = Muted;
    c[ImGuiCol_WindowBg] = Paper;
    c[ImGuiCol_ChildBg] = Surface;
    c[ImGuiCol_PopupBg] = Surface;
    c[ImGuiCol_Border] = Line;
    c[ImGuiCol_BorderShadow] = Color(0, 0);
    c[ImGuiCol_FrameBg] = Wash;
    c[ImGuiCol_FrameBgHovered] = Color(0x343e49);
    c[ImGuiCol_FrameBgActive] = Color(0x414b56);
    c[ImGuiCol_TitleBg] = Paper;
    c[ImGuiCol_TitleBgActive] = AmberSoft;
    c[ImGuiCol_TitleBgCollapsed] = Paper;
    c[ImGuiCol_MenuBarBg] = Paper;
    c[ImGuiCol_ScrollbarBg] = Color(0, 0);
    c[ImGuiCol_ScrollbarGrab] = Color(0x46505c);
    c[ImGuiCol_ScrollbarGrabHovered] = Color(0x626e7c);
    c[ImGuiCol_ScrollbarGrabActive] = Amber;
    c[ImGuiCol_CheckMark] = Amber;
    c[ImGuiCol_SliderGrab] = Amber;
    c[ImGuiCol_SliderGrabActive] = Color(0xf3ce91);
    c[ImGuiCol_Button] = Wash;
    c[ImGuiCol_ButtonHovered] = Color(0x3b434c);
    c[ImGuiCol_ButtonActive] = Color(0x515258);
    c[ImGuiCol_Header] = AmberSoft;
    c[ImGuiCol_HeaderHovered] = Color(0x3c3b35);
    c[ImGuiCol_HeaderActive] = Color(0x514736);
    c[ImGuiCol_Separator] = Line;
    c[ImGuiCol_SeparatorHovered] = Amber;
    c[ImGuiCol_SeparatorActive] = Amber;
    c[ImGuiCol_ResizeGrip] = Color(0x626e7c, .3f);
    c[ImGuiCol_ResizeGripHovered] = Color(0xe7b665, .6f);
    c[ImGuiCol_ResizeGripActive] = Amber;
    c[ImGuiCol_Tab] = Surface;
    c[ImGuiCol_TabHovered] = AmberSoft;
    c[ImGuiCol_TabSelected] = Wash;
    c[ImGuiCol_TabSelectedOverline] = Amber;
    c[ImGuiCol_TextSelectedBg] = Color(0xe7b665, .3f);
    c[ImGuiCol_NavCursor] = Amber;
    c[ImGuiCol_PlotLines] = Amber;
    c[ImGuiCol_PlotHistogram] = Amber;
    c[ImGuiCol_ModalWindowDimBg] = Color(0x080b10, .7f);
    s.ScaleAllSizes(scale);
}

void Hint(const char* text)
{
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("%s", text);
}

void Label(const char* text)
{
    ImGui::PushFont(Mono);
    ImGui::TextColored(Muted, "%s", text);
    ImGui::PopFont();
}

void Mark(ImVec2 p, float size)
{
    auto* d = ImGui::GetWindowDrawList();
    ImU32 color = ImGui::GetColorU32(Amber);
    float arm = size * .25f, weight = size * .065f;
    for (int x = 0; x < 2; ++x)
        for (int y = 0; y < 2; ++y)
        {
            ImVec2 corner(p.x + x * size, p.y + y * size);
            d->AddLine(corner, ImVec2(corner.x + (x ? -arm : arm), corner.y), color, weight);
            d->AddLine(corner, ImVec2(corner.x, corner.y + (y ? -arm : arm)), color, weight);
        }
    d->AddCircleFilled(ImVec2(p.x + size * .5f, p.y + size * .5f), size * .085f, color);
}

bool AccentButton(const char* label, ImVec2 size)
{
    ImGui::PushStyleColor(ImGuiCol_Button, Amber);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Color(0xf3ce91));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Color(0xcfa057));
    ImGui::PushStyleColor(ImGuiCol_Text, Canvas);
    bool pressed = ImGui::Button(label, size);
    ImGui::PopStyleColor(4);
    return pressed;
}

bool IconButton(const char* id, Icon icon, const char* hint, float scale, bool accent)
{
    ImVec2 size(32 * scale, ImGui::GetFrameHeight());
    bool pressed = accent ? AccentButton(id, size) : ImGui::Button(id, size);
    auto a = ImGui::GetItemRectMin(), b = ImGui::GetItemRectMax();
    ImVec2 p((a.x + b.x) / 2, (a.y + b.y) / 2);
    float r = 5 * scale, thick = 1.6f * scale;
    auto* d = ImGui::GetWindowDrawList();
    ImU32 ink = ImGui::GetColorU32(accent ? Canvas : Ink);
    if (icon == Icon::Play || icon == Icon::Next)
        d->AddTriangleFilled(ImVec2(p.x-r,p.y-r),ImVec2(p.x-r,p.y+r),ImVec2(p.x+r,p.y),ink);
    if (icon == Icon::Previous)
        d->AddTriangleFilled(ImVec2(p.x+r,p.y-r),ImVec2(p.x+r,p.y+r),ImVec2(p.x-r,p.y),ink);
    if (icon == Icon::Next || icon == Icon::Previous)
    {
        float x = p.x + (icon == Icon::Next ? r + 2*scale : -r - 2*scale);
        d->AddLine(ImVec2(x,p.y-r),ImVec2(x,p.y+r),ink,thick);
    }
    if (icon == Icon::Pause)
    {
        d->AddRectFilled(ImVec2(p.x-r,p.y-r),ImVec2(p.x-2*scale,p.y+r),ink);
        d->AddRectFilled(ImVec2(p.x+2*scale,p.y-r),ImVec2(p.x+r,p.y+r),ink);
    }
    if (icon == Icon::Stop)
        d->AddRectFilled(ImVec2(p.x-r,p.y-r),ImVec2(p.x+r,p.y+r),ink,scale);
    if (icon == Icon::Up || icon == Icon::Down)
    {
        float y = icon == Icon::Up ? -r : r;
        d->AddLine(ImVec2(p.x,p.y-y),ImVec2(p.x,p.y+y),ink,thick);
        d->AddLine(ImVec2(p.x-r,p.y),ImVec2(p.x,p.y+y),ink,thick);
        d->AddLine(ImVec2(p.x+r,p.y),ImVec2(p.x,p.y+y),ink,thick);
    }
    if (icon == Icon::Remove)
    {
        d->AddRect(ImVec2(p.x-r+scale,p.y-r+2*scale),ImVec2(p.x+r-scale,p.y+r),ink,scale,0,thick);
        d->AddLine(ImVec2(p.x-r-1,p.y-r),ImVec2(p.x+r+1,p.y-r),ink,thick);
        d->AddLine(ImVec2(p.x-2*scale,p.y-r-2*scale),ImVec2(p.x+2*scale,p.y-r-2*scale),ink,thick);
    }
    if (icon == Icon::Fit)
        d->AddRect(ImVec2(p.x-r-2*scale,p.y-r),ImVec2(p.x+r+2*scale,p.y+r),ink,0,0,thick);
    Hint(hint);
    return pressed;
}
}
