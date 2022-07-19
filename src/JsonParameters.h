#ifndef JSONPARAMETERS_H
#define JSONPARAMETERS_H

#define MAX_PARAMETERS 20

#include <Arduino.h>
#include <Wire.h>

class Parameters {
   private:
    String _parametersIds[MAX_PARAMETERS];
    String _parametersTitles[MAX_PARAMETERS];
    String _parametersValues[MAX_PARAMETERS];
    String _parametersTypes[MAX_PARAMETERS];
    uint8_t _parametersDecimalPlaces[MAX_PARAMETERS];
    uint8_t _parametersSignificantFigures[MAX_PARAMETERS];
    uint _parametersMax[MAX_PARAMETERS];
    uint _parametersMin[MAX_PARAMETERS];

    uint8_t _numberOfParameters;
    uint8_t _updateFlag = false;

   public:
    Parameters();

    void increaseParameterValue(uint8_t parameterIndex, uint8_t position);
    void decreaseParameterValue(uint8_t parameterIndex, uint8_t position);
    void loadParametersJson();
    void saveParametersJson();
    void setParameterValue(uint8_t parameterIndex, String value) { _parametersValues[parameterIndex] = value; };
    void setParameterDecimalPlaces(uint8_t parameterIndex, uint8_t decimalPlaces);
    void setParameterSignificantFigures(uint8_t parameterIndex, uint8_t significantFigures);
    void resetToBoot();
    void applyLimits(uint8_t parameterIndex);

    uint8_t getUpdateFlag() { _updateFlag = false; return true; };
    uint8_t getParameterIndex(String id);
    uint8_t getParameterDecimalPlaces(uint8_t parameterIndex) { return _parametersDecimalPlaces[parameterIndex]; };
    uint8_t getParameterSignificantFigures(uint8_t parameterIndex) { return _parametersSignificantFigures[parameterIndex]; };

    String getParameterType(uint8_t parameterIndex) { return _parametersTypes[parameterIndex]; };
    String getParameterTitle(uint8_t parameterIndex) { return _parametersTitles[parameterIndex]; };
    String getParameterValue(uint8_t parameterIndex) { return _parametersValues[parameterIndex]; };
    String getParameterValue(String id);
    String getSerialized();
};

#endif  // PARAMETERS_H
