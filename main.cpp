#include <emscripten/webaudio.h>
#include <emscripten/em_math.h>

uint8_t audioThreadStack[4096];

void AudioThreadInitialized(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success, void *userData);
void AudioWorkletProcessorCreated(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success, void *userData);
EM_BOOL GenerateNoise(int numInputs, const AudioSampleFrame *inputs, int numOutputs, AudioSampleFrame *outputs, int numParams, const AudioParamFrame *params, void *userData);
EM_BOOL OnCanvasClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);

int main() {
    EMSCRIPTEN_WEBAUDIO_T context = emscripten_create_audio_context(0);
    
    emscripten_start_wasm_audio_worklet_thread_async(context, audioThreadStack, sizeof(audioThreadStack), &AudioThreadInitialized, 0);
    
    return 0;
}

void AudioThreadInitialized(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success,void *userData) {
    if(!success) return;

    WebAudioWorkletProcessorCreateOptions opts = {
        .name = "noise-generator",
    };
    emscripten_create_wasm_audio_worklet_processor_async(audioContext, &opts, &AudioWorkletProcessorCreated, 0);
}

void AudioWorkletProcessorCreated(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success, void *userData) {
    if(!success) return;

    int outputChannelCounts[] = {1};
    EmscriptenAudioWorkletNodeCreateOptions options = {
        .numberOfInputs = 0,
        .numberOfOutputs = 1,
        .outputChannelCounts = outputChannelCounts
    };

    EMSCRIPTEN_AUDIO_WORKLET_NODE_T wasmAudioWorklet = emscripten_create_wasm_audio_worklet_node(audioContext, "noise-generator", &options, &GenerateNoise ,0);
    emscripten_audio_node_connect(wasmAudioWorklet, audioContext,0 ,0);
    emscripten_set_click_callback("canvas", (void*)audioContext, 0 ,OnCanvasClick);
}

EM_BOOL GenerateNoise(int numInputs, const AudioSampleFrame *inputs, int numOutputs, AudioSampleFrame *output, int numParams, const AudioParamFrame *params, void *userData){
    for(int i =  0; i < 128; ++i) {
        output[0].data[i] = (float)emscripten_random() * 2.0f - 1.0f;
    }

    return EM_TRUE;
}

EM_BOOL OnCanvasClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) {
    EMSCRIPTEN_WEBAUDIO_T audioContext = (EMSCRIPTEN_WEBAUDIO_T) userData;
    if(emscripten_audio_context_state(audioContext) != AUDIO_CONTEXT_STATE_RUNNING) {
        emscripten_resume_audio_context_sync(audioContext);
    }

    return EM_FALSE;
}