//-----------------------------------------------------------------------------
// COMPILE-TIME OPTIONS FOR DEAR IMGUI
// Runtime options (clipboard callbacks, enabling various features, etc.) are typically set in ImGui::GetIO() or ImGui::GetStyle().
// Those are often the only settings you need.
// This file is for compile-time options. Changing those settings requires recompiling the library.
// - A #define followed by 'IMGUI_' in this file is a compile-time option available to user.
// - Other #defines in this file are internal implementation details we expose for you to debug or test.
//-----------------------------------------------------------------------------

#pragma once

//---- Define assertion handler. Defaults to logging() and abort().
//#define IM_ASSERT(_EXPR)  MyAssertion(_EXPR)

//---- Define attributes of IMGUI_API. Necessary for DLL/Shared library builds.
//#define IMGUI_API __declspec(dllexport)
//#define IMGUI_API __declspec(dllimport)

//---- Don't define obsolete functions/enums names. Consider switching from 'Obsoleted' to 'Deprecated'.
//#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
//#define IMGUI_DISABLE_OBSOLETE_KEYIO

//---- Remove all API names that end in "Void". (There's no reason to use this, but if you must use it you can)
//#define IMGUI_DISABLE_OBSOLETE_NAMES

//---- Disable all of Dear ImGui or don't implement standard windows/tools.
//#define IMGUI_DISABLE                                     // Disable everything (all imgui headers, your project must not rely on imgui)
//#define IMGUI_DISABLE_DEMO_WINDOWS                        // Disable demo windows (ShowDemoWindow(), ShowUserGuide(), etc.)
//#define IMGUI_DISABLE_DEBUG_TOOLS                         // Disable Style Editor, Metrics, Debug Tools, Stack Tool, etc.
//#define IMGUI_DISABLE_METRICS_WINDOW                      // Disable metrics/debugger window and ShowMetricsWindow(). ShowStyleEditor() will still work.

//---- Don't implement some demo windows (you can still use ShowDemoWindow(), it'll just skip the disabled windows)
//#define IMGUI_DISABLE_DEMO_WINDOWS_FILE_EXPLORER          // Disable File Explorer demo window
//#define IMGUI_DISABLE_DEMO_WINDOWS_LOG_VIEWER             // Disable Log Viewer demo window
//#define IMGUI_DISABLE_DEMO_WINDOWS_STACK_TOOL             // Disable Stack Tool demo window
//#define IMGUI_DISABLE_DEMO_WINDOWS_NODE_EDITOR            // Disable Node Editor demo window

//---- Disable default behaviors of OS windows (mouse auto-activation, gaming input focus, etc.)
//#define IMGUI_DISABLE_OS_WINDOWS                     // Disables Win32 platform features: clipboard, IME, gamepad, mouse cursor change, etc.
//#define IMGUI_DISABLE_OS_MACOS                      // Disables macOS platform features: clipboard, IME, gamepad, mouse cursor change, etc.
//#define IMGUI_DISABLE_OS_UNIX                       // Disables Unix/Linux platform features: clipboard, IME, gamepad, mouse cursor change, etc.
//#define IMGUI_DISABLE_OS_ANDROID                    // Disables Android platform features: clipboard, gamepad, mouse cursor change, etc.
//#define IMGUI_DISABLE_OS_IOS                        // Disables iOS platform features: clipboard, gamepad, mouse cursor change, etc.
//#define IMGUI_DISABLE_OS_WINRT                      // Disables WinRT (UWP) platform features: clipboard, IME, gamepad, mouse cursor change, etc.

//---- Enable helper functions to automatically create ImGuiTestEngine hooks for testing and debugging purposes
//#define IMGUI_ENABLE_TEST_ENGINE                    // Automatically create hooks using IMGUI_API enums. An ImGuiTestEngine instance must be available somewhere.

//---- Include imgui_user.h at the end of imgui.h as a convenience for user headers. May facilitate forward declarations.
//#define IMGUI_INCLUDE_IMGUI_USER_H

//---- Provide select backend+helper files if imgui_sources/ path is set or if available in standard include paths.
//#define IMGUI_USE glfw3
//#define IMGUI_USE sdl3
//#define IMGUI_USE vulkan
//#define IMGUI_USE opengl3
//#define IMGUI_USE dx11
//#define IMGUI_USE dx12
//#define IMGUI_USE osx_metal
//#define IMGUI_USE android_gl3
//#define IMGUI_USE win32

//---- Define constructor and implicit cast operators to convert back and forth between your math types and ImVec2/ImVec4.
// This requires that your own math types have accessible x/y/w/z fields and that they can be cast to float.
// This will be necessary for many features (most UI functions use ImVec2/ImVec4 as parameters).
//#define IMGUI_VEC2_CLASS_EXTRA \
//    ImVec2(const MyVec2& f) { x = f.x; y = f.y; } \
//    operator MyVec2() const { return MyVec2(x, y); }
//#define IMGUI_VEC4_CLASS_EXTRA \
//    ImVec4(const MyVec4& f) { x = f.x; y = f.y; z = f.z; w = f.w; } \
//    operator MyVec4() const { return MyVec4(x, y, z, w); }

//---- Use 32-bit for indices. Default is 16-bit for optimal performance and size.
// If you want to use 32-bit indices (to handle very large meshes, for example), uncomment this line:
//#define ImDrawIdx unsigned int

//---- Use 64-bit for ImTextureID. Default is the size of a void* pointer.
// You may need this if you're binding your texture IDs to 64-bit pointers or handles.
//#define ImTextureID ImU64

//---- Use 32-bit pointer for ImTextureID. Default is the size of a void* pointer.
// This is useful if your texture IDs are 32-bit handles or values instead of pointers.
//#define ImTextureID unsigned int

//---- Use 8-bit indices for ImDrawCmd. Default is none (empty base class, no indices).
// This may be useful if you have memory constraints or you know that your draw data will have fewer than 256 draw calls.
//#define ImDrawCmd_Header  int HeaderCount;      // Optional. For example, 1
//#define ImDrawCmd_Header  int HeaderCount; unsigned char Indices;  // Optional. For example, 1
//#define ImDrawCmd_Header  int HeaderCount; unsigned short Indices; // Optional. For example, 1

//---- Override ImDrawCallback signature (default is ~void (*)(ImDrawCmd*, const ImDrawCmd*))
// This is useful if you want to pass custom data to your draw callback.
//#define ImDrawCallback CustomDrawCallbackSignature

//---- Override ImDrawCmd callback type signature
// This can be used to add custom data to ImDrawCmd and pass it to the callback function.
// Default is a simple function pointer. Uncomment and customize to your needs.
//#define ImDrawCmd_UserCallback(name) void (*name)(ImDrawCmd*, const ImDrawCmd*, void*)

//---- Add padding to ImDrawCmd to align it.
//#define ImDrawCmd_UserData  void* user_data;

//---- Debug options: enable assertions, then enable printfs in some widgets.
//#define IMGUI_ENABLE_ASSERTS
//#define IMGUI_ENABLE_PARANOIA_PRINTS

//---- Enable gamma correction for color operations. This can make text and borders look smoother/better in some scenarios.
//#define IMGUI_USE_COLOR_BT709

//---- Use FreeType to render text (instead of stb_truetype)
//#define IMGUI_ENABLE_FREETYPE

//---- Use FreeType+lunasvg library to render OpenType SVG fonts (requires IMGUI_ENABLE_FREETYPE)
//#define IMGUI_ENABLE_FREETYPE_LUNASVG

//---- Use stb_truetype to render text (default)
//#define IMGUI_ENABLE_STB_TRUETYPE

//---- Use LZW compression. Needed for some old imgui_tool. It used to be implemented by default.
//#define IMGUI_ENABLE_LZW_COMPRESSION

//---- Miscellaneous options
//#define IMGUI_USE_WCHAR32             // Use wchar_t 32-bit (instead of 16-bit) to represent unicode characters. This requires compiler support for wchar_t 32-bit (e.g. on Windows).
//#define IMGUI_DISABLE_DEFAULT_ALLOCATORS// Don't use default malloc/free. Provide your own, then define IMGUI_NEW/IMGUI_DELETE in imconfig.h and add code in your custom allocator.
//#define IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS // Don't provide helpers: FormatString, FormatStringV, ImTextSnprintf, etc.
//#define IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS  // Don't provide helpers: ImFileOpen, ImFileClose, ImFileReadLine, etc.
//#define IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS    // Don't provide helper: ImLoadFile, ImSaveFile (file to string/buffer)
//#define IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS    // Don't provide helpers: ImMin, ImMax, ImLerp, ImClamp, ImSqrt, ImPow, ImFloor, ImCeil, ImTrunc, ImRound, ImSign, ImSwap
//#define IMGUI_DISABLE_DEFAULT_COLORS            // Don't provide default values for ImGui::GetStyle().Colors[].
//#define IMGUI_DISABLE_DEFAULT_VISUALS           // Don't provide default visuals such as ImGui::GetStyle().WindowRounding.
//#define IMGUI_DISABLE_TODO_ALGORITHM           // Don't implement userland versions of std::bit_count, std::has_single_bit, etc.
//#define IMGUI_DISABLE_DEBUG_LOG                // Don't implement ImGui::DebugLog(), ImGui::DebugLogV(), DebugLogFlags, etc.
//#define IMGUI_DISABLE_DEBUG_ALLOCATIONS        // Don't implement ImGui::DebugAlloc(), ImGui::DebugDumpAllocations(), etc.
//#define IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION // Disable STB rect pack implementation of ImDrawRectInterpolator.
//#define IMGUI_DISABLE_STB_SPRINTF_IMPLEMENTATION  // Disable STB sprintf implementation. If disabled, you need to link with standard sprintf or provide your own.
//#define IMGUI_DISABLE_STB_SPRINTF             // Disable all stb_sprintf implementation.
//#define IMGUI_USE_STB_SPRINTF                 // Use stb_sprintf for string formatting. (Note: you also need to define IMGUI_DISABLE_STB_SPRINTF_IMPLEMENTATION if you want to link with stb_sprintf)
//#define IMGUI_DISABLE_TOOLS                    // Disable Debug Tools (ShowAboutWindow, ShowStyleEditor, ShowUserGuide, ShowDebugLogWindow, ShowIDStackToolWindow, ShowFontAtlasToolWindow, etc.)
//#define IMGUI_ENABLE_STB_PERFANIUME_IMPLEMENTATION  // Enable implementation of stb_perlin_noise in static functions.
//#define IMGUI_ENABLE_STB_IMAGE_IMPLEMENTATION     // Enable implementation of stb_image in static functions.
//#define IMGUI_ENABLE_STB_RECT_PACK_IMPLEMENTATION   // Enable implementation of stb_rect_pack in static functions.
//#define IMGUI_ENABLE_STB_SPRINTF_IMPLEMENTATION    // Enable implementation of stb_sprintf in static functions.
//#define IMGUI_ENABLE_STB_RECT_PACK_H_IMPLEMENTATION // Enable implementation of stb_rect_pack.h in static functions.
//#define IMGUI_ENABLE_STB_TRUETYPE_IMPLEMENTATION   // Enable implementation of stb_truetype in static functions.

//---- Define the following to disable default behavior and define your own.
//#define IMGUI_NEW(X)        MyMalloc(X)
//#define IMGUI_DELETE(X)     MyFree(X)

//---- Include imgui_internal.h in imgui.h
//#define IMGUI_INCLUDE_IMGUI_INTERNAL_H

//---- Define these in imconfig.h if needed
// If you are implementing bindings for a custom engine, you may need to define these:
//#define IMGUI_ALLOCATOR          YourCustomAllocator
//#define IMGUI_ALLOCATOR_FREE     YourCustomFree
//#define IMGUI_ALLOCATOR_REALLOC  YourCustomRealloc

//---- Define this to 1 if you're running on a platform where the function prototype for printf doesn't support the 'z' length modifier.
//#define IMGUI_DISABLE_DEBUG_PRINTF_FORMAT_SPECIFIER

//---- Enable Automation API
//#define IMGUI_ENABLE_TEST_ENGINE

//---- Define constructor to allow implicit conversion
//#define IMGUI_DISABLE_MATH_FORMATTING

//---- Define if you want to disable the default font atlas builder
//#define IMGUI_DISABLE_DEFAULT_FONT_ATLAS

//---- Use this if you want to set your own icon to max_title
//#define IMGUI_MAX_TITLE 256

//---- Use this if you want to set your own icon to max_tooltip
//#define IMGUI_MAX_TOOLTIP 4096

//---- Default to STB_Truetype
#define IMGUI_ENABLE_STB_TRUETYPE

//---- Disable obscure features
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_DISABLE_OBSOLETE_KEYIO
