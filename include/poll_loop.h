#pragma once
#include "main.h"

void poll_loop();
void respond_to_client(int i, const char* buffer, int number_of_bytes_received);