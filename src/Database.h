#ifndef DATABASE_H
#define DATABASE_H

#include <ext_type.h>

void Database_Init();
const char* Database_Name(u16 index);
u16 Database_ObjectIndex(u16 index);

#endif