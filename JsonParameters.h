#ifndef JSONPARAMETERS_H
#define JSONPARAMETERS_H

#define MAX_PARAMETERS 10

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>

class Parameters {
   private:
    String _parametersJson;
    String _parametersIds[MAX_PARAMETERS];
    String _parametersTitles[MAX_PARAMETERS];
    String _parametersValues[MAX_PARAMETERS];
    String _parametersSignificantFigures[MAX_PARAMETERS];

    uint8_t _numberOfParameters;

   public:
    Parameters();

    void increaseParameterValue(uint8_t parameterIndex, uint8_t position);
    void decreaseParameterValue(uint8_t parameterIndex, uint8_t position);
    void setParametersJson();
    void saveParametersJson();

    uint8_t getParameterIndex(String id);
    uint8_t getParameterSignificantFigures(uint8_t parameterIndex) { return (uint8_t)_parametersSignificantFigures[parameterIndex].toInt(); };
    String getParameterValue(uint8_t parameterIndex) { return _parametersValues[parameterIndex]; };
    String getParameterTitle(uint8_t parameterIndex) { return _parametersTitles[parameterIndex]; };
};

#endif  // PARAMETERS_H
