//-----------------------------------------------------------------------------
// imgui_internal.h - This file is not part of the public API
// This is a minimal version for compilation purposes
//-----------------------------------------------------------------------------

#pragma once

// Minimal stub implementation for compilation
// Most of the internal functionality is not needed for basic rendering

namespace ImGui
{
// Forward declarations for internal structures
struct ImGuiContext;
struct ImGuiWindowTempData;
struct ImGuiWindow;
struct ImGuiTabBar;
struct ImGuiDockContext;
struct ImGuiViewport;
struct ImGuiStackToolInfo;
struct ImGuiStyleMod;
struct ImGuiGroupStack;
struct ImGuiPopupData;
struct ImGuiColumnData;
struct ImGuiDrawChannel;
struct ImGuiDimmeds;
struct ImGuiTable;
struct ImGuiTableInstanceData;
struct ImGuiTableColumn;
struct ImGuiTableTempData;
struct ImGuiMenuItemData;
struct ImGuiMenuColumns;
struct ImGuiInputTextState;
struct ImGuiNextWindowData;
struct ImGuiNextItemData;
struct ImGuiColorMod;
struct ImGuiFocusScopeData;
struct ImGuiOldColumns;
struct ImGuiBoxSelectState;
struct ImGuiKeyOwnerData;

// Stub functions that might be called
inline void ErrorCheckEndFrameSanityChecks() {}
inline void ErrorCheckNewFrameSanityChecks() {}
inline void DebugDrawCursorPos() {}
inline void DebugLocateItem() {}
inline void DebugLocateItemHovered() {}
inline void DebugLog(const char* fmt, ...) {}
}
