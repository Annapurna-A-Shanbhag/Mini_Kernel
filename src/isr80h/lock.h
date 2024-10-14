#ifndef LOCKS_H
#define LOCKS_H

#include "../task/process.h"
#include "../locks/lock.h"

void* isr80h_acquire_lock();
void* isr80h_release_lock();



#endif