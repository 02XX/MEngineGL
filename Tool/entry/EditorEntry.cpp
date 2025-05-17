#include "Editor/Editor.hpp"

int main()
{
    MEngine::Editor editor;
    editor.Init();
    editor.Update(0.016f); // 60 FPS
    editor.Shutdown();
    return 0;
}