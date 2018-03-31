#ifndef __HELPER_H
#define __HELPER_H

// Helping code. :)

// Debug print...
#define DBG printf("[DebugMarker]\n");
// Clear Screen
#define clrscr() printf("\033[H\033[J")
// This gives the length of a non-dynamically defined array.
#define arraylength(T) sizeof(T)/sizeof(T[0])

// This clips value T to keep it in range.
#define clip(T,Lower,Upper) (T>Upper?Upper:(T<Lower)?Lower:T)

int digitCount(int num);

#endif // __HELPER_H