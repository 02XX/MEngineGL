#include "Editor/MEngineEditor.hpp"

int main()
{
    MEngine::MEngineEditor editor;
    editor.Init();
    editor.Update(0.016f); // 60 FPS
    editor.Shutdown();
    return 0;
}