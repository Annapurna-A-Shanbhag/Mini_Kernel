#ifndef FAT16_H
#define FAT16_H

#include "../file.h"
#include "../../string/string.h"
#include "../../memory/heap/heap.h"
#include "../../disk/streamer.h"
#include "../../memory/memory.h"
#include "../../status.h"


struct filesystem* fat16_initialization();

#endif