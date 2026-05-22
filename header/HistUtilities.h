#ifndef HISTUTILITIES_H
#define HISTUTILITIES_H
 
#include <vector>
 
template<typename T>
void WriteAll(T* h){if(h)h->Write();}
 
template<typename T>
void WriteAll(std::vector<T>& v) {for(auto& entry : v) WriteAll(entry);}
 
#endif