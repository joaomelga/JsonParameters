#include "Parameters.h"

#include <SPIFFS.h>

Parameters::Parameters() {}

void Parameters::setParametersJson() {
    File file = SPIFFS.open("/parameters.json");

    /* if (!file || file.isDirectory()) {
        Serial.println("Failed to open file for reading");
        return "";
    } */

    char fileContent[file.size()];
    for (uint16_t i = 0; file.available(); i++)
        fileContent[i] = file.read();

    _parametersJson = String(fileContent);

    DynamicJsonDocument parameters(1024);
    deserializeJson(parameters, _parametersJson);  //, DeserializationOption::Filter(filter));
    _numberOfParameters = parameters.size();

    for (uint8_t parametersIndex = 0; parametersIndex < _numberOfParameters; parametersIndex++) {
        _parametersIds[parametersIndex] = parameters[parametersIndex]["id"].as<String>();
        _parametersValues[parametersIndex] = parameters[parametersIndex]["value"].as<String>();
        _parametersTitles[parametersIndex] = parameters[parametersIndex]["title"].as<String>();
        _parametersSignificantFigures[parametersIndex] = parameters[parametersIndex]["significantFigures"].as<String>();
    }

    parameters.clear();
    file.close();
}

void Parameters::saveParametersJson() {
    DynamicJsonDocument parameters(1024);

    for (uint8_t parametersIndex = 0; parametersIndex < _numberOfParameters; parametersIndex++) {
        parameters[parametersIndex]["id"] = _parametersIds[parametersIndex];
        parameters[parametersIndex]["value"] = _parametersValues[parametersIndex];
        parameters[parametersIndex]["title"] = _parametersTitles[parametersIndex];
        parameters[parametersIndex]["significantFigures"] = _parametersSignificantFigures[parametersIndex];

        Serial.println(_parametersIds[parametersIndex]);
        Serial.println(parameters[parametersIndex]["id"].as<String>());
    }

    String parametersJson;
    serializeJson(parameters, parametersJson);

    File file = SPIFFS.open("/parameters.json", "w+");
    file.print(parametersJson);

    parameters.clear();
    file.close();
}

uint8_t Parameters::getParameterIndex(String id) {
    for (uint8_t parameterIndex = 0; parameterIndex < sizeof(_parametersIds) / sizeof(_parametersIds[0]); parameterIndex++) {
        if (_parametersIds[parameterIndex] == id) return parameterIndex;
    }

    return 0;
}

void Parameters::increaseParameterValue(uint8_t parameterIndex, uint8_t position) {
    uint8_t valueToIncrease = _parametersValues[parameterIndex].substring(position, position + 1).toInt();
    char increasedValue[1];

    itoa((valueToIncrease + 1) % 10, increasedValue, 10);
    _parametersValues[parameterIndex].setCharAt(position, increasedValue[0]);
}

void Parameters::decreaseParameterValue(uint8_t parameterIndex, uint8_t position) {
    uint8_t valueToDecrease = _parametersValues[parameterIndex].substring(position, position + 1).toInt();
    char decreasedValue[1];

    itoa((valueToDecrease - 1) < 0 ? 9 : (valueToDecrease - 1), decreasedValue, 10);
    _parametersValues[parameterIndex].setCharAt(position, decreasedValue[0]);
}
