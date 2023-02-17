#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <vector>

void update_buffer(double* buffer, int size, int& offset, double n);
void rank_values(double* buffer, int* result, int size);
int factorial(int n);
int dataToIndex(int* data, int size);
void indexToData(int* data, int size, int index);
void crunchNumbers(const std::string& dataFolder, const std::string& outputFileName, int sample_size, int low_times, float low_ratio, uint64_t RAM_GB);
