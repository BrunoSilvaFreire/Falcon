//
// Created by Novak on 22/03/2020.
//

#include <eagle/editor/EditorMaster.h>

EG_EDITOR_BEGIN

std::vector<Reference<EditorWindow>> EditorMaster::m_windows;

EditorMaster::EditorMaster() {

}

EditorMaster::~EditorMaster() {

}

void EditorMaster::init() {
    m_listener.attach(&DesktopApplication::instance().event_bus());
    m_listener.subscribe<Engine::OnRenderContextInit>(this);
    m_listener.subscribe<Engine::OnRenderCommandBufferMainRenderPass>(this);
    init_imgui();
}

void EditorMaster::deinit() {
    m_listener.detach();
    ImGui::DestroyContext();
}

void EditorMaster::init_imgui() {
    ImGui::CreateContext();

    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.25f, 0.26f, 0.25f, 0.7f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.46f, 0.4f, 0.46f, 0.8f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.16f, 0.15f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.3f, 0.32f, 0.3f, 0.4f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.125f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.215f, 0.215f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.2f, 0.215f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.4f, 0.475f, 0.45f, 0.8f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.65f, 0.6f, 0.65f, 0.8f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.5f, 0.55f, 0.8f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.4f, 0.45f, 0.6f, 0.8f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.55f, 0.55f, 0.7f, 0.8f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.35f, 0.35f, 0.365f, 0.8f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.45f, 0.46f, 0.8f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.45f, 0.4f, 0.425f, 0.8f);
    style.ChildRounding = 2;
    style.FrameRounding = 2;
    style.GrabRounding = 2;
    style.PopupRounding = 1;
    style.WindowRounding = 2;
    style.WindowPadding = ImVec2(2, 2);

    ImGuiIO &io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)

    io.KeyMap[ImGuiKey_Tab] = EG_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = EG_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = EG_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = EG_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = EG_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = EG_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = EG_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = EG_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = EG_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = EG_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = EG_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = EG_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = EG_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = EG_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = EG_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = EG_KEY_A;
    io.KeyMap[ImGuiKey_C] = EG_KEY_C;
    io.KeyMap[ImGuiKey_V] = EG_KEY_V;
    io.KeyMap[ImGuiKey_X] = EG_KEY_X;
    io.KeyMap[ImGuiKey_Y] = EG_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = EG_KEY_Z;

    auto& window = DesktopApplication::instance().window();
    float width = static_cast<float>(window.width());
    float height = static_cast<float>(window.height());

    io.DisplaySize = ImVec2(width, height);
    io.DisplayFramebufferScale = ImVec2( window.framebuffer_width_scale(), window.framebuffer_height_scale());
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

void EditorMaster::update(float dt) {
    ImGuiIO &io = ImGui::GetIO();
    io.DeltaTime = dt + 0.0001f;

    ImGui::NewFrame();
    for (auto &window : m_windows) {
        window->handle_update();
    }
    ImGui::EndFrame();
    ImGui::Render();
    update_mouse_cursor();
    update_buffers();
}

void EditorMaster::init_render_state(const Reference <Shader> &shader, CommandBuffer *commandBuffer,
                                     ImDrawData *drawData) {
    ImGuiIO &io = ImGui::GetIO();
    commandBuffer->set_viewport(io.DisplaySize.x * io.DisplayFramebufferScale.x, io.DisplaySize.y * io.DisplayFramebufferScale.y, 0, 0, 0, 1);
    commandBuffer->bind_shader(shader);
    pushConstBlock.scale = glm::vec2(2.0f / drawData->DisplaySize.x, 2.0f / drawData->DisplaySize.y);
    pushConstBlock.translate = glm::vec2(-1.0f) - glm::vec2(drawData->DisplayPos.x, drawData->DisplayPos.y) * pushConstBlock.scale;
    commandBuffer->push_constants(ShaderStage::VERTEX, 0, sizeof(pushConstBlock), &pushConstBlock);
}

bool EditorMaster::handle_window_resized(const OnWindowResized &e) {

    auto& window = DesktopApplication::instance().window();

    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(e.width, e.height);
    io.DisplayFramebufferScale = ImVec2( window.framebuffer_width_scale(), window.framebuffer_height_scale());
    return false;
}

bool EditorMaster::handle_mouse_moved(const OnMouseMove &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(e.x, e.y);
    return false;
}

bool EditorMaster::handle_mouse_button(const OnMouseButton &e) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[e.key] = e.action == EG_PRESS || e.action == EG_REPEAT;
    return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

bool EditorMaster::handle_mouse_scrolled(const OnMouseScrolled &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheel = e.y;
    io.MouseWheelH = e.x;
    return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}

bool EditorMaster::handle_key(const OnKey &e) {
    ImGuiIO &io = ImGui::GetIO();

    io.KeysDown[e.key] = e.action == EG_PRESS || e.action == EG_REPEAT;

    // Modifiers are not reliable across m_systems
    io.KeyCtrl = io.KeysDown[EG_KEY_LEFT_CONTROL] || io.KeysDown[EG_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[EG_KEY_LEFT_SHIFT] || io.KeysDown[EG_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[EG_KEY_LEFT_ALT] || io.KeysDown[EG_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[EG_KEY_LEFT_SUPER] || io.KeysDown[EG_KEY_RIGHT_SUPER];
    return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}

bool EditorMaster::handle_key_typed(const OnKeyTyped &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(e.key);
    return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}

void EditorMaster::add_window(const Reference<EditorWindow> &window) {
    m_windows.emplace_back(window);
}

void EditorMaster::remove_window(const Reference<EditorWindow> &window) {
    m_windows.erase(std::find(m_windows.begin(), m_windows.end(), window));
}

void EditorMaster::update_buffers() {
    ImDrawData *imDrawData = ImGui::GetDrawData();

    if ((imDrawData->TotalVtxCount == 0) || (imDrawData->TotalIdxCount == 0)) {
        return;
    }

    ImDrawVert *vtxDst = new ImDrawVert[imDrawData->TotalVtxCount];
    ImDrawIdx *idxDst = new ImDrawIdx[imDrawData->TotalIdxCount];

    ImDrawVert *vtxOffset = vtxDst;
    ImDrawIdx *idxOffset = idxDst;


    for (int i = 0; i < imDrawData->CmdListsCount; i++) {
        const ImDrawList *cmdList = imDrawData->CmdLists[i];
        memcpy(vtxOffset, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxOffset, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtxOffset += cmdList->VtxBuffer.Size;
        idxOffset += cmdList->IdxBuffer.Size;
    }

    m_vertexBuffer.lock()->upload(vtxDst, imDrawData->TotalVtxCount * sizeof(ImDrawVert));
    m_indexBuffer.lock()->upload(idxDst, imDrawData->TotalIdxCount * sizeof(ImDrawIdx));

    delete[] vtxDst;
    delete[] idxDst;
}

void EditorMaster::update_mouse_cursor() {
    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();

    Cursor cursorType;
    switch (imgui_cursor) {
        case ImGuiMouseCursor_TextInput:
            cursorType = Cursor::TEXT;
            break;
        case ImGuiMouseCursor_ResizeNS:
            cursorType = Cursor::VERT_RESIZE;
            break;
        case ImGuiMouseCursor_ResizeEW:
            cursorType = Cursor::HORI_RESIZE;
            break;
        case ImGuiMouseCursor_Hand:
            cursorType = Cursor::HAND;
            break;
        default:
            cursorType = Cursor::ARROW;
            break;
    }
    DesktopApplication::instance().window().set_cursor_shape(cursorType);
}

bool EditorMaster::receive(const Engine::OnRenderContextInit &ev) {
    EG_TRACE("BEGIN");

    auto& context = *ev.context;

    ImGuiIO &io = ImGui::GetIO();
    unsigned char *pixels;
    TextureCreateInfo fontCreateInfo = {};
    io.Fonts->GetTexDataAsRGBA32(&pixels, &fontCreateInfo.imageCreateInfo.width, &fontCreateInfo.imageCreateInfo.height);
    fontCreateInfo.filter = Filter::LINEAR;

    fontCreateInfo.imageCreateInfo.bufferData = std::vector<unsigned char>(pixels,
                                                                           pixels + fontCreateInfo.imageCreateInfo.width * fontCreateInfo.imageCreateInfo.height * 4);
    fontCreateInfo.imageCreateInfo.aspects = {ImageAspect::COLOR};
    fontCreateInfo.imageCreateInfo.memoryProperties = {MemoryProperty::DEVICE_LOCAL};
    fontCreateInfo.imageCreateInfo.usages = {ImageUsage::SAMPLED, ImageUsage::TRANSFER_DST};
    fontCreateInfo.imageCreateInfo.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
    fontCreateInfo.imageCreateInfo.tiling = ImageTiling::OPTIMAL;
    fontCreateInfo.imageCreateInfo.format = Format::R8G8B8A8_UNORM;
    fontCreateInfo.imageCreateInfo.arrayLayers = 1;
    fontCreateInfo.imageCreateInfo.mipLevels = 1;


    m_font = context.create_texture(fontCreateInfo);


    VertexLayout vertexLayout = VertexLayout(5, {
            Format::R32G32_SFLOAT,
            Format::R32G32_SFLOAT,
            Format::R8G8B8A8_UNORM
    });


    DescriptorBindingDescription binding = {};
    binding.shaderStage = ShaderStage::FRAGMENT;
    binding.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
    binding.binding = 0;

    m_descriptorLayout = context.create_descriptor_set_layout({binding});


    ShaderCreateInfo pipelineInfo = {context.main_render_pass(), {
            {ShaderStage::VERTEX, "data/shaders/text.vert"},
            {ShaderStage::FRAGMENT, "data/shaders/text.frag"},
    }};
    pipelineInfo.vertexLayout = vertexLayout;
    pipelineInfo.dynamicStates = true;
    m_blendDisabledShader = context.create_shader(pipelineInfo);

    pipelineInfo.blendEnable = true;
    m_blendEnabledShader = context.create_shader(pipelineInfo);

    m_descriptor = context.create_descriptor_set(m_descriptorLayout.lock(), {m_font.lock()});

    io.Fonts->TexID = (ImTextureID)&m_descriptor;

    m_vertexBuffer = context.create_vertex_buffer(nullptr, 0, vertexLayout,
                                                  UpdateType::HOST);
    m_indexBuffer = context.create_index_buffer(nullptr, 0, IndexBufferType::UINT_16,
                                                UpdateType::HOST);

    EG_TRACE("END");
    return false;
}

bool EditorMaster::receive(const Engine::OnRenderCommandBufferMainRenderPass &ev) {

    auto commandBuffer = ev.commandBuffer;

    ImGuiIO &io = ImGui::GetIO();
    ImDrawData *imDrawData = ImGui::GetDrawData();
    // Render commands
    if (imDrawData->CmdListsCount > 0) {

        init_render_state(m_blendEnabledShader.lock(), commandBuffer, imDrawData);

        ImVec2 clip_off = imDrawData->DisplayPos;         // (0,0) unless using multi-viewports
        ImVec2 clip_scale = imDrawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

        int32_t vertexOffset = 0;
        int32_t indexOffset = 0;

        commandBuffer->bind_vertex_buffer(m_vertexBuffer.lock());
        commandBuffer->bind_index_buffer(m_indexBuffer.lock());

        for (int32_t i = 0; i < imDrawData->CmdListsCount; i++) {

            ImDrawList *cmdList = imDrawData->CmdLists[i];
            for (int32_t j = 0; j < cmdList->CmdBuffer.Size; j++) {

                ImDrawCmd *cmd = &cmdList->CmdBuffer[j];
                if (cmd->UserCallback != NULL) {
                    if (cmd->UserCallback == EnableBlending){
                        init_render_state(m_blendEnabledShader.lock(), commandBuffer, nullptr);
                    }
                    else if (cmd->UserCallback == DisableBlending){
                        init_render_state(m_blendDisabledShader.lock(), commandBuffer, nullptr);
                    }
                    else{
                        cmd->UserCallback(cmdList, cmd);
                    }
                }
                else {
                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec4 clip_rect;
                    clip_rect.x = (cmd->ClipRect.x - clip_off.x) * clip_scale.x;
                    clip_rect.y = (cmd->ClipRect.y - clip_off.y) * clip_scale.y;
                    clip_rect.z = (cmd->ClipRect.z - clip_off.x) * clip_scale.x;
                    clip_rect.w = (cmd->ClipRect.w - clip_off.y) * clip_scale.y;

                    uint32_t x = std::max((int32_t) (clip_rect.x), 0);
                    uint32_t y = std::max((int32_t) (clip_rect.y), 0);
                    uint32_t w = (uint32_t) (clip_rect.z - clip_rect.x);
                    uint32_t h = (uint32_t) (clip_rect.w - clip_rect.y);
                    commandBuffer->set_scissor(w, h, x, y);

                    commandBuffer->bind_descriptor_sets(static_cast<Handle<DescriptorSet> *>(cmd->TextureId)->lock(), 0);

                    commandBuffer->draw_indexed(cmd->ElemCount, indexOffset, vertexOffset);

                    indexOffset += cmd->ElemCount;
                }
            }
            vertexOffset += cmdList->VtxBuffer.Size;
        }
    }
    return false;
}


EG_EDITOR_END
