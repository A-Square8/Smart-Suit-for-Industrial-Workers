#ifndef ACOUSTIC_SENSOR_H
#define ACOUSTIC_SENSOR_H

#include <stdlib.h>
#include <math.h>

// MEMS microphone constants
#define MIC_SENSITIVITY -38.0  // dBV/Pa
#define MIC_SNR 65.0  // Signal-to-noise ratio in dB
#define MIC_RANGE 120.0  // Acoustic overload point in dB SPL
#define MIC_RESPONSE_TIME 0.1  // Response time in milliseconds

// Frequency response parameters
#define MIC_LOW_FREQ 100.0  // Hz
#define MIC_HIGH_FREQ 10000.0  // Hz
#define MIC_RESONANT_FREQ 15000.0  // Hz

// Function to convert sound pressure to microphone output voltage
double mic_output_voltage(double sound_pressure_pa) {
    // Convert dBV/Pa to V/Pa
    double sensitivity_v_pa = pow(10, MIC_SENSITIVITY / 20.0);
    
    // Calculate output voltage
    double output = sound_pressure_pa * sensitivity_v_pa;
    
    // Add noise based on SNR
    double noise_voltage = output / pow(10, MIC_SNR / 20.0);
    double noise = ((rand() % 201) - 100) / 100.0 * noise_voltage;
    
    return output + noise;
}

// Function to simulate frequency response effect
double apply_frequency_response(double signal_amplitude, double frequency) {
    double normalized_output = 1.0;
    
    // Simple model of frequency response
    if (frequency < MIC_LOW_FREQ) {
        // Roll-off below low frequency cutoff
        normalized_output = frequency / MIC_LOW_FREQ;
    } else if (frequency > MIC_HIGH_FREQ) {
        // Roll-off above high frequency cutoff
        if (frequency < MIC_RESONANT_FREQ) {
            // Approaching resonance peak
            normalized_output = 1.0 + 0.5 * (frequency - MIC_HIGH_FREQ) / (MIC_RESONANT_FREQ - MIC_HIGH_FREQ);
        } else {
            // Sharp roll-off after resonance
            normalized_output = 1.5 * exp(-(frequency - MIC_RESONANT_FREQ) / 1000.0);
        }
    }
    
    return signal_amplitude * normalized_output;
}

// Function to convert dB SPL to Pascal
double dbspl_to_pascal(double db_spl) {
    // Reference: 0 dB SPL = 20 μPa
    return 20e-6 * pow(10, db_spl / 20.0);
}

#endif
