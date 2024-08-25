#include "misc_gen.h"

#ifndef NDEBUG

bool g_debug = false;
map<string, vector<int> > g_dbInt_v_map;
map<string, vector<float> > g_dbFloat_v_map;

ofstream g_log("log.txt", ios::trunc);

#endif
