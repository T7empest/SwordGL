//
// Created by Lukáš Blažek on 05.09.2025.
//

#include "graphics/UI.h"

#include "imgui_impl_sdl3.h"

void UI::init(SDL_Window* window, SDL_GPUDevice* device)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplSDL3_InitForSDLGPU(window);
	ImGui_ImplSDLGPU3_InitInfo init_info = {
		.Device = device,
		.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(device, window),
		.PresentMode = SDL_GPU_PRESENTMODE_VSYNC
	};
	ImGui_ImplSDLGPU3_Init(&init_info);
}

bool UI::process_event(const SDL_Event& event)
{
	return ImGui_ImplSDL3_ProcessEvent(&event);
}

void UI::render(ImguiPassContext& ctx)
{
	ImGui_ImplSDLGPU3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	main_window();

	ImGui::Render();
	ImguiPass::execute(ctx);
}

void UI::shutdown()
{
	ImGui_ImplSDL3_Shutdown();
	ImGui_ImplSDLGPU3_Shutdown();
	ImGui::DestroyContext();
}

void UI::main_window()
{

	static SDL_FColor clear_color = {0.0f, 0.0f, 0.0f, 0.5f};
	static bool show_test_window = false;

	ImGui::Begin("todo: configurable params of particles"); // Create a window called "Hello, world!" and append into it.

	ImGui::Text("This is some useful text.");
	// Display some text (you can use a format strings too)
	ImGui::Checkbox("Demo Window", &show_test_window);
	// Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &show_test_window);

	ImGui::SliderFloat("size", &size_, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	ImGui::End();
}
