#pragma once

#include <Arduino.h>

String htmlEscape(String s);
String inputText(const String& id, const String& label, const String& value, const String& placeholder = "");
String inputDate(const String& id, const String& label, const String& value);
String textArea(const String& id, const String& label, const String& value, const String& placeholder = "");
String optionSelected(const String& current, const String& value);