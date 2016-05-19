// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

inline bool operator< (FColor lhs, FColor rhs)
{ return lhs.DWColor() < rhs.DWColor(); }

#ifdef _MSC_VER
#pragma warning(disable: 4125)
#pragma warning(disable: 4800)
#pragma warning(disable: 4913) // user defined binary operator ',' exists but no overload could convert all operands, default built-in binary operator ',' used
#endif
