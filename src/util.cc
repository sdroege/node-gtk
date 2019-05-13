/*
 * util.cc
 * Copyright (C) 2016 romgrk <romgrk@Romgrk-ARCH>
 *
 * Distributed under terms of the MIT license.
 */

#include <node.h>
#include <nan.h>
#include <girepository.h>
#include <glib-object.h>

#include "util.h"
#include "macros.h"

using v8::Local;
using v8::Value;
using v8::Object;
using v8::String;

namespace Util {

const char* ArrayTypeToString (GIArrayType array_type) {
    switch (array_type) {
    case GI_ARRAY_TYPE_C:
        return "C-array";
    case GI_ARRAY_TYPE_ARRAY:
        return "GArray";
    case GI_ARRAY_TYPE_PTR_ARRAY:
        return "GPtrArray";
    case GI_ARRAY_TYPE_BYTE_ARRAY:
        return "GByteArray";
    }
    g_assert_not_reached();
}

char* GetSignalName(const char* signal_detail) {
    char* signal_name;
    char* detail_start;
    if ((detail_start = g_strrstr(signal_detail, "::")) != NULL) {
        signal_name = g_strndup(signal_detail, reinterpret_cast<gsize>(detail_start) - reinterpret_cast<gsize>(signal_detail));
    } else {
        signal_name = g_strdup(signal_detail);
    }
    return signal_name;
}


/**
 * This function is used to call "process._tickCallback()" inside NodeJS.
 * We want to do this after we run the LibUV eventloop because there might
 * be pending Micro-tasks from Promises or calls to 'process.nextTick()'.
 */
void CallNextTickCallback() {
    auto global = Nan::GetCurrentContext()->Global();
    Local<Object> processObject = TO_OBJECT (global->Get(UTF8("process")));
    Local<Value> tickCallbackValue = processObject->Get(UTF8("_tickCallback"));
    if (tickCallbackValue->IsFunction()) {
        Nan::CallAsFunction(TO_OBJECT (tickCallbackValue), processObject, 0, nullptr);
    }
}

}
