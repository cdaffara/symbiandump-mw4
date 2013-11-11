/* Automatically generated from kjs_navigator.cpp using create_hash_table. DO NOT EDIT ! */

#include "lookup.h"

namespace KJS {

static const struct HashEntry NavigatorTableEntries[] = {
   { "appName", Navigator::AppName, DontDelete|ReadOnly, 0, 0 }/* 1928575421 */ ,
   { 0, 0, 0, 0, 0 },
   { "plugins", Navigator::_Plugins, DontDelete|ReadOnly, 0, 0 }/* 3805841733 */ ,
   { "appVersion", Navigator::AppVersion, DontDelete|ReadOnly, 0, 0 }/* 737224111 */ ,
   { "NetworkNotAllowed", Navigator::NetworkNotAllowed, DontDelete|ReadOnly, 0, 0 }/* 1135189233 */ ,
   { "mimeTypes", Navigator::_MimeTypes, DontDelete|ReadOnly, 0, &NavigatorTableEntries[19] }/* 541397713 */ ,
   { "vendorSub", Navigator::VendorSub, DontDelete|ReadOnly, 0, &NavigatorTableEntries[20] }/* 2200565686 */ ,
   { 0, 0, 0, 0, 0 },
   { "onLine", Navigator::Online, DontDelete|ReadOnly, 0, 0 }/* 3891351994 */ ,
   { "platform", Navigator::Platform, DontDelete|ReadOnly, 0, 0 }/* 1656147642 */ ,
   { 0, 0, 0, 0, 0 },
   { "NetworkAccessAllowed", Navigator::NetworkAccessAllowed, DontDelete|ReadOnly, 0, 0 }/* 2231046963 */ ,
   { "appCodeName", Navigator::AppCodeName, DontDelete|ReadOnly, 0, &NavigatorTableEntries[17] }/* 2036402863 */ ,
   { "NetworkAccessible", Navigator::NetworkAccessible, DontDelete|ReadOnly, 0, 0 }/* 3337096018 */ ,
   { "language", Navigator::Language, DontDelete|ReadOnly, 0, &NavigatorTableEntries[18] }/* 389985029 */ ,
   { "productSub", Navigator::ProductSub, DontDelete|ReadOnly, 0, 0 }/* 2097335881 */ ,
   { "vendor", Navigator::Vendor, DontDelete|ReadOnly, 0, 0 }/* 4183282360 */ ,
   { "userAgent", Navigator::UserAgent, DontDelete|ReadOnly, 0, 0 }/* 1491767165 */ ,
   { "product", Navigator::Product, DontDelete|ReadOnly, 0, 0 }/* 3666888811 */ ,
   { "cookieEnabled", Navigator::CookieEnabled, DontDelete|ReadOnly, 0, 0 }/* 2175375748 */ ,
   { "javaEnabled", Navigator::JavaEnabled, DontDelete|Function, 0, 0 }/* 3101780464 */ 
};

const struct HashTable NavigatorTable = { 2, 21, NavigatorTableEntries, 17 };

} // namespace

#include "lookup.h"

namespace KJS {

static const struct HashEntry PluginsTableEntries[] = {
   { "refresh", Plugins::Refresh, DontDelete|Function, 0, 0 }/* 561650708 */ ,
   { "length", Plugins::Length, DontDelete|ReadOnly, 0, 0 }/* 2484182065 */ 
};

const struct HashTable PluginsTable = { 2, 2, PluginsTableEntries, 2 };

} // namespace

#include "lookup.h"

namespace KJS {

static const struct HashEntry MimeTypesTableEntries[] = {
   { "length", MimeTypes::Length, DontDelete|ReadOnly, 0, 0 }/* 2484182065 */ 
};

const struct HashTable MimeTypesTable = { 2, 1, MimeTypesTableEntries, 1 };

} // namespace

#include "lookup.h"

namespace KJS {

static const struct HashEntry PluginTableEntries[] = {
   { 0, 0, 0, 0, 0 },
   { "name", Plugin::Name, DontDelete|ReadOnly, 0, &PluginTableEntries[4] }/* 4135969945 */ ,
   { "filename", Plugin::Filename, DontDelete|ReadOnly, 0, 0 }/* 2442346670 */ ,
   { "description", Plugin::Description, DontDelete|ReadOnly, 0, 0 }/* 432724751 */ ,
   { "length", Plugin::Length, DontDelete|ReadOnly, 0, 0 }/* 2484182065 */ 
};

const struct HashTable PluginTable = { 2, 5, PluginTableEntries, 4 };

} // namespace

#include "lookup.h"

namespace KJS {

static const struct HashEntry MimeTypeTableEntries[] = {
   { "suffixes", MimeType::Suffixes, DontDelete|ReadOnly, 0, 0 }/* 272507524 */ ,
   { 0, 0, 0, 0, 0 },
   { "enabledPlugin", MimeType::EnabledPlugin, DontDelete|ReadOnly, 0, 0 }/* 4109007998 */ ,
   { "type", MimeType::Type, DontDelete|ReadOnly, 0, &MimeTypeTableEntries[4] }/* 924663163 */ ,
   { "description", MimeType::Description, DontDelete|ReadOnly, 0, 0 }/* 432724751 */ 
};

const struct HashTable MimeTypeTable = { 2, 5, MimeTypeTableEntries, 4 };

} // namespace
