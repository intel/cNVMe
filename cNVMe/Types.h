/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2017
Types.h - A header file for all needed type includes
*/

// Project Includes
#include "Payload.h" // Here to prevent import loop

#pragma once

#include "Logger.h"

// STL Includes
#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <exception>
#include <functional>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

// Unsigned values
typedef uint8_t  UINT_8;
typedef uint16_t UINT_16;
typedef uint32_t UINT_32;
typedef uint64_t UINT_64;

// Signed vales
typedef int8_t   INT_8;
typedef int16_t  INT_16;
typedef int32_t  INT_32;
typedef int64_t  INT_64;

// Cause I'm lazy
typedef UINT_8 BYTE;

// Type Macros
#define POINTER_TO_MEMORY_ADDRESS(p) (UINT_64)&(*p)
#define MEMORY_ADDRESS_TO_8POINTER(m) (UINT_8*)m
#define MEMORY_ADDRESS_TO_64POINTER(m) (UINT_64*)m

// Init macro
#define ALLOC_BYTE_ARRAY(name, size) BYTE* name = new BYTE[size]; memset(name, 0, size);
