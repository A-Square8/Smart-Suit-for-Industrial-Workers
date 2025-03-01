#ifndef RADIATION_SENSOR_H
#define RADIATION_SENSOR_H

#include <stdlib.h>
#include <math.h>

// Semiconductor detector constants
#define RAD_SENSITIVITY 150.0  // counts per μSv
#define RAD_BACKGROUND 0.15  // Background radiation in μSv/h
#define RAD_MIN_DETECTION 0.05  // Minimum detectable radiation in μSv/h
#define RAD_ENERGY_RESOLUTION 7.0  // Energy resolution in %

// Isotope identification parameters
#define ISO_THRESHOLD_COUNTS 10  // Minimum counts needed for identification
#define ISO_CONFIDENCE_THRESHOLD 0.7  // Confidence threshold for identification

// Function to simulate radiation detector counts
int simulate_radiation_counts(double radiation_level_usvh, double integration_time_s) {
    // Calculate expected counts
    double expected_counts = radiation_level_usvh * RAD_SENSITIVITY * (integration_time_s / 3600.0);
    
    // Add statistical variation (Poisson distribution approximated by normal)
    double std_dev = sqrt(expected_counts);
    double random_variation = ((rand() % 201) - 100) / 100.0 * std_dev;
    
    // Calculate actual counts (ensure non-negative)
    int counts = (int)(expected_counts + random_variation + 0.5);
    return (counts > 0) ? counts : 0;
}

// Function to detect radiation presence above background
int detect_radiation(double radiation_level_usvh) {
    // Calculate signal-to-background ratio
    double signal_to_background = radiation_level_usvh / RAD_BACKGROUND;
    
    // Detection requires signal significantly above background and above minimum detection
    return (signal_to_background > 1.5 && radiation_level_usvh > RAD_MIN_DETECTION) ? 1 : 0;
}

// Function to identify isotope from energy spectrum (simplified)
int identify_isotope(int spectrum[], int spectrum_size) {
    // This is a simplified model - real identification uses complex algorithms
    
    // Check if we have enough counts for identification
    int total_counts = 0;
    for (int i = 0; i < spectrum_size; i++) {
        total_counts += spectrum[i];
    }
    
    if (total_counts < ISO_THRESHOLD_COUNTS) {
        return 0;  // Not enough counts for identification
    }
    
    // Find peak energy
    int peak_index = 0;
    for (int i = 1; i < spectrum_size; i++) {
        if (spectrum[i] > spectrum[peak_index]) {
            peak_index = i;
        }
    }
    
    // Simplified isotope identification based on peak energy
    // In reality, this would use reference libraries and multiple peaks
    if (peak_index < spectrum_size / 4) return 1;  // Low energy (e.g., Am-241)
    if (peak_index < spectrum_size / 2) return 2;  // Medium energy (e.g., Cs-137)
    return 3;  // High energy (e.g., Co-60)
}

#endif
