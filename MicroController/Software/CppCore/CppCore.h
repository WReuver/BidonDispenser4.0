#ifndef CppCore_H_
#define CppCore_H_

#include <stdlib.h>

void * operator new(size_t size);                                           // Create an instance of a class
void operator delete(void * ptr);                                           // Delete an instance of a class

void * operator new[](size_t size);                                         // Create an array of instances of a class
void operator delete[](void * ptr);                                         // Delete an array of instances of a class

// Some other (probably?) important functions (tbh, I don't know what they are for)
__extension__ typedef int __guard __attribute__((mode (__DI__)));
extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);
extern "C" void __cxa_pure_virtual(void);

#endif