/* Automatically generated from kjs_console.cpp using create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry ConsoleTableEntries[] = {
   { "error", ConsoleObject::error, DontDelete|Function, 1, &ConsoleTableEntries[7] }/* 1650821305 */ ,
   { "debug", ConsoleObject::debug, DontDelete|Function, 1, &ConsoleTableEntries[9] }/* 3512055898 */ ,
   { 0, 0, 0, 0, 0 },
   { "log", ConsoleObject::log, DontDelete|Function, 1, &ConsoleTableEntries[8] }/* 3271412756 */ ,
   { 0, 0, 0, 0, 0 },
   { 0, 0, 0, 0, 0 },
   { "info", ConsoleObject::information, DontDelete|Function, 1, 0 }/* 1669601513 */ ,
   { "warn", ConsoleObject::warn, DontDelete|Function, 1, &ConsoleTableEntries[10] }/* 3495910600 */ ,
   { "assert", ConsoleObject::assert, DontDelete|Function, 1, 0 }/* 1334751078 */ ,
   { "lineno", ConsoleObject::lineno, DontDelete|ReadOnly, 0, 0 }/* 2337212102 */ ,
   { "sourceURL", ConsoleObject::sourceURL, DontDelete|ReadOnly, 0, 0 }/* 1079628921 */ 
};

const struct HashTable ConsoleTable = { 2, 11, ConsoleTableEntries, 7 };

} // namespace
