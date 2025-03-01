#ifndef MAGNETIC_SENSOR_H
#define MAGNETIC_SENSOR_H

#include <stdlib.h>
#include <math.h>

// Magnetoresistive sensor constants
#define MR_SENSITIVITY 12.0  // mV/V/mT
#define MR_RANGE 6.0  // ±mT
#define MR_HYSTERESIS 0.4  // % of full scale
#define MR_LINEARITY 0.2  // % of full scale

// 3D magnetometer constants
#define MAG3D_RANGE 8.0  // ±Gauss
#define MAG3D_RESOLUTION 0.5  // mGauss
#define MAG3D_ACCURACY 2.0  // % of full scale

// Function to simulate magnetoresistive sensor output
double magnetoresistive_output(double magnetic_field_mT, double supply_voltage) {
    // Limit to sensor range
    if (magnetic_field_mT > MR_RANGE) magnetic_field_mT = MR_RANGE;
    if (magnetic_field_mT < -MR_RANGE) magnetic_field_mT = -MR_RANGE;
    
    // Calculate ideal output
    double output = supply_voltage * MR_SENSITIVITY * magnetic_field_mT / 1000.0;
    
    // Add non-linearity (simplified)
    double nonlinearity = output * ((rand() % 201) - 100) / 1000.0 * MR_LINEARITY / 100.0;
    
    // Add noise
    double noise = supply_voltage * ((rand() % 201) - 100) / 10000.0;  // ±0.01V noise
    
    return output + nonlinearity + noise;
}

// Function to simulate 3D magnetometer reading
void read_3d_magnetometer(double actual_field[3], double measured_field[3]) {
    for (int i = 0; i < 3; i++) {
        // Limit to sensor range
        double field = actual_field[i];
        if (field > MAG3D_RANGE) field = MAG3D_RANGE;
        if (field < -MAG3D_RANGE) field = -MAG3D_RANGE;
        
        // Add measurement error
        double error = field * ((rand() % 201) - 100) / 1000.0 * MAG3D_ACCURACY / 100.0;
        
        // Add resolution quantization (round to nearest resolution step)
        double resolution_gauss = MAG3D_RESOLUTION / 1000.0;
        field = round((field + error) / resolution_gauss) * resolution_gauss;
        
        measured_field[i] = field;
    }
}

#endif
