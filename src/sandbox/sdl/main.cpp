#include "SDL.h"

#include <Orbit.h>
#include <SDL_opengl.h>
#include <bindings/imgui_impl_opengl3.h>
#include <bindings/imgui_impl_sdl.h>
#include <cmath>
#include <imgui.h>
#include <iostream>


double pi()
{
    // return std::atan(1) * 4;
    return 3.141;
}
struct Point
{
    int x;
    int y;
};

void drawCircle(SDL_Renderer* renderer, float radius, const Point& mid)
{
    for (int i = 0; i < 360; ++i)
    {
        int  j      = i - 1;
        auto theta  = (i * pi() / 180.0);
        auto theta1 = (j * pi() / 180.0);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        int x  = radius * cos(theta) + mid.x;
        int y  = radius * sin(theta) + mid.y;
        int x1 = radius * cos(theta1) + mid.x;
        int y1 = radius * sin(theta1) + mid.y;

        SDL_RenderDrawLine(renderer, x, y, x1, y1);
    }
}
void drawEllipse(SDL_Renderer* renderer, float a, float b, const Point& mid)
{
    for (int i = 0; i < 360; ++i)
    {
        int  j      = i - 1;
        auto theta  = (i * pi() / 180.0);
        auto theta1 = (j * pi() / 180.0);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        int x  = a * cos(theta) + mid.x;
        int y  = b * sin(theta) + mid.y;
        int x1 = a * cos(theta1) + mid.x;
        int y1 = b * sin(theta1) + mid.y;

        SDL_RenderDrawLine(renderer, x, y, x1, y1);
    }
}

void drawAtEllipse(SDL_Renderer* renderer, float a, float b, float theta, Point mid)
{
    int      x = a * cos(theta) + mid.x;
    int      y = b * sin(theta) + mid.y;
    SDL_Rect r{x, y, 3, 3};
    SDL_RenderFillRect(renderer, &r);
}

struct Dimension
{
    int width;
    int height;
};


OrbitalParameters op{
    .eccentricity   = 0.70,
    .semimajor_axis = 100,
    .incliniation   = 0.0,
    .longAN         = 0.0,
    .longPA         = 0.0,


};
SDL_Point windowSize{1024, 768};

void drawScene(SDL_Renderer* renderer, double t)
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    Point mid{static_cast<int>(windowSize.x * 0.5), static_cast<int>(windowSize.y * 0.5)};

    drawCircle(renderer, 100, mid);
    drawEllipse(renderer, 100, 150, mid);

    for (int i = 360; i-- > 0;)
    {
        auto p = OrbitalMechanics::getEulerAnglesFromEccentricAnomaly(op, i / 360.0 * 2 * M_PI).toCartesian();
        SDL_RenderDrawPoint(renderer, static_cast<int>(p[0] + mid.x - 1), static_cast<int>(p[1] + mid.y - 1));
    }
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

    auto     p = OrbitalMechanics::getEulerAngelsAtJulianDay(op, t).toCartesian();
    SDL_Rect r{static_cast<int>(p[0] + mid.x), static_cast<int>(p[1] + mid.y), 3, 3};
    SDL_RenderFillRect(renderer, &r);
}

void initialize(SDL_Window** window, SDL_Renderer** renderer, SDL_GLContext* gl_context)
{
    // Seems necessary to draw SDL images but broke rendering of external window
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to the latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {

        throw std::runtime_error("SDL_Init Error");
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    *window                      = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    *renderer                    = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    *gl_context                  = SDL_GL_CreateContext(*window);
    SDL_GL_MakeCurrent(*window, *gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(*window, *gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
}
namespace ImGui
{
bool SilderDouble(const char* label, double* v, double v_min, double v_max, const char* format, ImGuiSliderFlags flags = 0)
{
    return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, flags);
}

bool SilderDoubleAngular(const char* label, double* v, const char* format, ImGuiSliderFlags flags = 0)
{
    static double v_min = 0.0;
    static double v_max = 2 * M_PI;
    return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, flags);
}
} // namespace ImGui

int main(int, char**)
{
    SDL_Window*   window     = nullptr;
    SDL_Renderer* renderer   = nullptr;
    SDL_GLContext gl_context = nullptr;
    initialize(&window, &renderer, &gl_context);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    SDL_Surface* surface = SDL_LoadBMP("E:/test.bmp");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    auto         t       = 0.0;
    // Main loop
    bool done = false;
    while (!done)
    {
        t += 0.1;
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        const double
                     f64_one  = 1.0;
        const double f64_zero = 0.0;

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::SilderDouble("Eccentricity low", &op.eccentricity, 0.0, 0.9999999, "%.4f ecc");
            ImGui::SilderDoubleAngular("Semimajor axis", &op.semimajor_axis, "%.4f semi major");

            ImGui::SilderDoubleAngular("Inclination", &op.incliniation, "%.4f ecc");
            ImGui::SilderDoubleAngular("Longitude of ascending node", &op.longAN, "%.4f LAN");

            ImGui::SilderDoubleAngular("Argument of periapsis", &op.longPA, "%.4f PA");
            ImGui::SilderDoubleAngular("Mean anomaly", &op.meanLongitude, "%.4f Mean Long");
            ImGui::SilderDouble("Revolution number at epoch", &op.epoch, 0, 365.0, "%.4f epoch");


            static int    counter = 0;
            static double f       = 0.7;

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)

            // ImGui::SliderFloat("float", ImGui::InputDouble(&f), 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        ImGuiIO& io = ImGui::GetIO();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // SDL_renderer
        SDL_Rect rct2;
        rct2.x = rct2.y = 200;
        rct2.w = rct2.h = 400;

        // SDL_RenderCopy(renderer, texture, 0, &rct2);
        drawScene(renderer, t);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window*   backup_current_window  = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}