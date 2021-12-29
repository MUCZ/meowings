#ifndef DUMMYUI
#define DUMMYUI
#include <unordered_map>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "chatclient.hh"

using namespace std;

extern unordered_map<string,string> commandMap;
extern ChatClient* g_client;

void online_mainMenu();
void help_online_mainMenu();
void mainMenu();

#endif /* DUMMYUI */
