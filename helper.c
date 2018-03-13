#ifndef __HELPER_C
#define __HELPER_C
// Helping code. :)
#define arraylength(T) sizeof(T)/sizeof(T[0])
//TODO: fix these
#define to1DArray(...) (int[]){__VA_ARGS__}
#define to2DArray(arg1,...) {to1DArray(arg1), to2DArray(__VA_ARGS__)}

#endif // __HELPER_C