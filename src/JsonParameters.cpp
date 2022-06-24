#include "JsonParameters.h"

#include <SPIFFS.h>

#include "ArduinoJson-v6.19.4.h"

#define JSON_DEFAULT_SIZE 4096

Parameters::Parameters() {}

String Parameters::getSerialized() {
    File file = SPIFFS.open("/parameters.json");

    /* if (!file || file.isDirectory()) {
        Serial.println("Failed to open file for reading");
        return "";
    } */

    char fileContent[file.size()];
    for (uint16_t i = 0; file.available(); i++)
        fileContent[i] = file.read();

    file.close();
    return String(fileContent);
}

void Parameters::loadParametersJson() {
    String serializedParameters = getSerialized();

    DynamicJsonDocument parameters(JSON_DEFAULT_SIZE);
    deserializeJson(parameters, serializedParameters);  //, DeserializationOption::Filter(filter));
    _numberOfParameters = parameters.size();

    uint8_t selectedSetup = parameters[0]["value"].as<uint8_t>();

    for (uint8_t parametersIndex = 0; parametersIndex < _numberOfParameters; parametersIndex++) {
        _parametersIds[parametersIndex] = parameters[parametersIndex]["id"].as<String>();
        _parametersTitles[parametersIndex] = parameters[parametersIndex]["title"].as<String>();
        _parametersSignificantFigures[parametersIndex] = parameters[parametersIndex]["significantFigures"].as<String>();
        _parametersDecimalPlaces[parametersIndex] = parameters[parametersIndex].containsKey("decimalPlaces") ? parameters[parametersIndex]["decimalPlaces"].as<String>() : "0";
        _parametersTypes[parametersIndex] = parameters[parametersIndex].containsKey("type") ? parameters[parametersIndex]["type"].as<String>() : "uint";

        if (parameters[parametersIndex].containsKey("value")) {
            _parametersValues[parametersIndex] = parameters[parametersIndex]["value"].as<String>();
        } else if (parameters[parametersIndex].containsKey("values")) {
            _parametersValues[parametersIndex] = parameters[parametersIndex]["values"][selectedSetup].as<String>();
        }
    }

    parameters.clear();
}

void Parameters::saveParametersJson() {
    String oldSerializedParameters = getSerialized();

    DynamicJsonDocument parameters(JSON_DEFAULT_SIZE);
    deserializeJson(parameters, oldSerializedParameters);
    _numberOfParameters = parameters.size();

    uint8_t selectedSetup = parameters[0]["value"].as<uint8_t>();

    for (uint8_t parametersIndex = 0; parametersIndex < _numberOfParameters; parametersIndex++) {
        if (parameters[parametersIndex].containsKey("value")) {
            parameters[parametersIndex]["value"] = _parametersValues[parametersIndex];
        } else if (parameters[parametersIndex].containsKey("values")) {
            parameters[parametersIndex]["values"][selectedSetup] = _parametersValues[parametersIndex];
        }

        parameters[parametersIndex]["significantFigures"] = _parametersSignificantFigures[parametersIndex];
    }

    String newSerializedParamters;
    serializeJson(parameters, newSerializedParamters);

    File file = SPIFFS.open("/parameters.json", "w+");
    file.print(newSerializedParamters);

    parameters.clear();
    file.close();
}

uint8_t Parameters::getParameterIndex(String id) {
    for (uint8_t parameterIndex = 0; parameterIndex < sizeof(_parametersIds) / sizeof(_parametersIds[0]); parameterIndex++) {
        if (_parametersIds[parameterIndex] == id) return parameterIndex;
    }

    return 0;
}

String Parameters::getParameterValue(String id) {
    return _parametersValues[getParameterIndex(id)];
};

void Parameters::increaseParameterValue(uint8_t parameterIndex, uint8_t position) {
    uint8_t realPosition = position >= _parametersValues[parameterIndex].length() - !!_parametersDecimalPlaces[parameterIndex].toInt() - _parametersDecimalPlaces[parameterIndex].toInt() ? position + 1 : position;
    String strValue = _parametersValues[parameterIndex].substring(realPosition, realPosition + 1);
    char increasedValue[1];

    if (strValue == "+" || strValue == "-") {
        increasedValue[0] = strValue == "+" ? '-' : '+';
    } else {
        uint8_t valueToIncrease = strValue.toInt();
        itoa((valueToIncrease + 1) % 10, increasedValue, 10);
    }

    _parametersValues[parameterIndex].setCharAt(realPosition, increasedValue[0]);
}

void Parameters::decreaseParameterValue(uint8_t parameterIndex, uint8_t position) {
    uint8_t realPosition = position >= _parametersValues[parameterIndex].length() - !!_parametersDecimalPlaces[parameterIndex].toInt() - _parametersDecimalPlaces[parameterIndex].toInt() ? position + 1 : position;
    String strValue = _parametersValues[parameterIndex].substring(realPosition, realPosition + 1);
    char decreasedValue[1];

    if (strValue == "+" || strValue == "-") {
        decreasedValue[0] = strValue == "+" ? '-' : '+';
    } else {
        uint8_t valueToDecrease = strValue.toInt();
        itoa((valueToDecrease - 1) < 0 ? 9 : (valueToDecrease - 1), decreasedValue, 10);
    }

    _parametersValues[parameterIndex].setCharAt(realPosition, decreasedValue[0]);
}

void Parameters::setParameterSignificantFigures(uint8_t parameterIndex, uint8_t significantFigures) {
    uint8_t previousSignificantFigures = _parametersSignificantFigures[parameterIndex].toInt();
    String parameterValue = _parametersValues[parameterIndex];
    String unsignedValue;
    unsignedValue = parameterValue.substring(parameterValue[0] != '+' && parameterValue[0] != '-' ? 0 : 1);

    if (significantFigures < previousSignificantFigures) {
        for (uint8_t i = 0; i < previousSignificantFigures - significantFigures; i++)
            unsignedValue = unsignedValue.substring(1);
    } else if (significantFigures > previousSignificantFigures) {
        for (uint8_t i = 0; i < significantFigures - previousSignificantFigures; i++)
            unsignedValue = "0" + unsignedValue;
    } else
        return;

    parameterValue = (parameterValue[0] != '+' && parameterValue[0] != '-') ? unsignedValue : parameterValue[0] + unsignedValue;
    _parametersValues[parameterIndex] = parameterValue;
    _parametersSignificantFigures[parameterIndex] = String(significantFigures);
};

void Parameters::resetToBoot() {
    File bootFile = SPIFFS.open("/parameters.boot.json");
    char bootFileContent[bootFile.size()];

    for (uint16_t i = 0; bootFile.available(); i++)
        bootFileContent[i] = bootFile.read();

    bootFile.close();

    File defaultFile = SPIFFS.open("/parameters.json", "w+");
    defaultFile.print(bootFileContent);
    defaultFile.close();
}
