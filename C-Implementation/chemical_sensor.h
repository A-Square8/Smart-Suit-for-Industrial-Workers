#ifndef CHEMICAL_SENSOR_H
#define CHEMICAL_SENSOR_H

#include <stdlib.h>
#include <math.h>

// Electrochemical sensor constants
#define EC_SENSITIVITY 20.0  // nA/ppm
#define EC_ZERO_CURRENT 5.0  // nA baseline current
#define EC_RESPONSE_TIME 30  // T90 response time in seconds
#define EC_RESOLUTION 0.1  // ppm

// Gas types
#define GAS_CO 1
#define GAS_H2S 2
#define GAS_SO2 3
#define GAS_NO2 4
#define GAS_CL2 5
#define GAS_NH3 6
#define GAS_O2 7

// Cross-sensitivity matrix (simplified)
// Each row represents a gas, each column represents interference from other gases
const double CROSS_SENSITIVITY[7][7] = {
    {1.00, 0.05, 0.00, -0.10, 0.00, 0.00, 0.00},  // CO
    {0.00, 1.00, 0.10, 0.00, -0.20, 0.00, 0.00},  // H2S
    {0.00, 0.00, 1.00, 0.15, 0.00, 0.00, 0.00},   // SO2
    {-0.05, 0.00, 0.00, 1.00, 0.00, 0.00, 0.00},  // NO2
    {0.00, -0.10, 0.00, 0.00, 1.00, 0.00, 0.00},  // CL2
    {0.00, 0.00, 0.00, 0.00, 0.00, 1.00, 0.00},   // NH3
    {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 1.00}    // O2
};

// Function to simulate electrochemical sensor current
double ec_sensor_current(int gas_type, double concentration, double interfering_conc[]) {
    // Calculate baseline current from target gas
    double current = EC_ZERO_CURRENT + (concentration * EC_SENSITIVITY);
    
    // Add cross-sensitivity effects
    for (int i = 0; i < 7; i++) {
        if (i != gas_type - 1) {  // Skip the target gas
            current += interfering_conc[i] * EC_SENSITIVITY * CROSS_SENSITIVITY[gas_type-1][i];
        }
    }
    
    // Add random noise
    double noise = ((rand() % 201) - 100) / 100.0 * EC_SENSITIVITY * EC_RESOLUTION;
    
    return current + noise;
}

// Function to convert sensor current to gas concentration
double current_to_concentration(double current, int gas_type) {
    // Simple linear conversion
    double concentration = (current - EC_ZERO_CURRENT) / EC_SENSITIVITY;
    
    // Apply resolution quantization
    concentration = round(concentration / EC_RESOLUTION) * EC_RESOLUTION;
    
    // Ensure non-negative result
    return (concentration > 0) ? concentration : 0;
}

// Function to simulate temperature effect on sensor
double apply_temperature_effect(double concentration, double temperature) {
    // Simplified temperature compensation
    // Assume calibration at 20°C, with 0.2% change per °C
    double temp_factor = 1.0 + (temperature - 20.0) * 0.002;
    return concentration * temp_factor;
}

#endif
