#include "web_helpers.h"

String htmlEscape(String s) {
    s.replace("&", "&amp;");
    s.replace("<", "&lt;");
    s.replace(">", "&gt;");
    s.replace("\"", "&quot;");
    s.replace("'", "&#39;");
    return s;
}

String inputText(const String& id, const String& label, const String& value, const String& placeholder) {
    return "<label for='" + id + "'>" + label + "</label>"
           "<input type='text' id='" + id + "' value='" + htmlEscape(value) + "' placeholder='" + htmlEscape(placeholder) + "'>";
}

String inputDate(const String& id, const String& label, const String& value) {
    return "<label for='" + id + "'>" + label + "</label>"
           "<input type='date' id='" + id + "' value='" + htmlEscape(value) + "'>";
}

String textArea(const String& id, const String& label, const String& value, const String& placeholder) {
    return "<label for='" + id + "'>" + label + "</label>"
           "<textarea id='" + id + "' placeholder='" + htmlEscape(placeholder) + "'>" + htmlEscape(value) + "</textarea>";
}

String optionSelected(const String& current, const String& value) {
    return current == value ? " selected" : "";
}