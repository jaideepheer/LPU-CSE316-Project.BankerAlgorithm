#ifndef __HELPER_H
#define __HELPER_H
// Helping code. :)
// This gives the length of a non-dynamically defined array.
#define arraylength(T) sizeof(T)/sizeof(T[0])

// This clips value T to keep it in range.
#define clip(T,Lower,Upper) (T>Upper?Upper:(T<Lower)?Lower:T)

//TODO: fix these
#define to1DArray(...) (int[]){__VA_ARGS__}
#define to2DArray(arg1,...) {to1DArray(arg1), to2DArray(__VA_ARGS__)}

#endif // __HELPER_H