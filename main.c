#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define SAMPLE_RATE 44100
#define MAX_SAMPLES (SAMPLE_RATE * 2) // 2 seconds of audio

// Maps RGB values (0-255) to sound and plays it
void RGBToSound(unsigned char red, unsigned char green, unsigned char blue) {
    // Create audio buffer
    float* samples = (float*)malloc(sizeof(float) * MAX_SAMPLES);
    if (!samples) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for audio samples");
        return;
    }

    // Map RGB components to frequency ranges
    // Red: Low frequencies (50-500 Hz)
    float redFreq = 50.0f + (float)red / 255.0f * 450.0f;
    
    // Green: Mid frequencies (500-2000 Hz)
    float greenFreq = 500.0f + (float)green / 255.0f * 1500.0f;
    
    // Blue: High frequencies (2000-8000 Hz)
    float blueFreq = 2000.0f + (float)blue / 255.0f * 6000.0f;
    
    // Generate audio samples by combining all three frequencies
    for (int i = 0; i < MAX_SAMPLES; i++) {
        float t = (float)i / SAMPLE_RATE;
        
        // Determine amplitude for each component (0.0-0.33)
        float redAmp = (float)red / 255.0f * 0.33f;
        float greenAmp = (float)green / 255.0f * 0.33f;
        float blueAmp = (float)blue / 255.0f * 0.33f;
        
        // Combine frequencies with different amplitudes
        samples[i] = redAmp * sinf(2.0f * PI * redFreq * t) +
                     greenAmp * sinf(2.0f * PI * greenFreq * t) +
                     blueAmp * sinf(2.0f * PI * blueFreq * t);
        
        // clamp to range [-1,1]
        if (samples[i] > 1.0f) samples[i] = 1.0f;
        if (samples[i] < -1.0f) samples[i] = -1.0f;
    }
    
    // Create sound from samples
    Wave wave = {
        .frameCount = MAX_SAMPLES,
        .sampleRate = SAMPLE_RATE,
        .sampleSize = 32,
        .channels = 1,
        .data = samples
    };
    
    // Convert wave to sound
    Sound sound = LoadSoundFromWave(wave);
    PlaySound(sound);
    printf("Generated sound from RGB(%d, %d, %d)\n", red, green, blue);
    printf("Red frequency: %.2f Hz\n", redFreq);
    printf("Green frequency: %.2f Hz\n", greenFreq);
    printf("Blue frequency: %.2f Hz\n", blueFreq);
    
    // wait until done playing
    while (IsSoundPlaying(sound)) {}
    
    UnloadSound(sound);
    free(samples);
}

// Parse a string to an unsigned char value (0-255)
unsigned char parseColorValue(const char* str) {
    int value = atoi(str);
    
    // Clamp value to 0-255 range
    if (value < 0) return 0;
    if (value > 255) return 255;
    
    return (unsigned char)value;
}

// Print usage information
void printUsage(const char* programName) {
    printf("Usage: %s <red> <green> <blue>\n", programName);
    printf("  where red, green, blue are integers between 0-255\n");
    printf("Example: %s 255 128 64\n", programName);
}

int main(int argc, char* argv[]) {
    // Check if correct number of arguments
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Parse RGB values from command line arguments
    unsigned char red = parseColorValue(argv[1]);
    unsigned char green = parseColorValue(argv[2]);
    unsigned char blue = parseColorValue(argv[3]);
    
    // Initialize audio device
    InitWindow(400, 300, "RGB to Sound");
    InitAudioDevice();
    
    // Set target FPS
    SetTargetFPS(60);
    
    
    // Main game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Press SPACE to play sound", 50, 120, 20, WHITE);
            DrawText(TextFormat("RGB: %d, %d, %d", red, green, blue), 100, 160, 20, WHITE);
            if (IsKeyPressed(KEY_SPACE)) {
                RGBToSound(red, green, blue);
            }
        EndDrawing();
    }
    
    // Clean up
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}