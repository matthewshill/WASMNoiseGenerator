#include <emscripten/webaudio.h>

u_int8_t audioThreadStack[4096];

int main() {
    EMSCRIPTEN_WEBAUDIO_T context = emscripten_create_audio_context(0);
    
    emscripten_start_wasm_audio_worklet_thread_async(context, audioThreadStack, sizeof(audioThreadStack), &AudioThreadInitialized, 0);
    
    return 0;
}

void AudioThreadInitialized(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success,void *userData) {
    if(!success) return;
}