/*
This file is part of cNVMe and is released under the MIT License
(C) - Charles Machalow - 2016
Types.h - A header file for all needed type includes
*/

// Project Includes
#include "Payload.h" // Here to prevent import loop

#pragma once

// STL Includes
#include <cassert>
#include <cstdint>
#include <exception>
#include <string>
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
