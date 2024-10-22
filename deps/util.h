#pragma once

template<typename T>
inline T* offsetPtr(void* ptr, int offset) { return (T*)(((char*)ptr) + offset); }