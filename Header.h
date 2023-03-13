#pragma once
#include<iostream>
#include<cstdlib>
#include<stdio.h>
#include<vector>
#include<windows.h>
#include<iomanip>
#include<sstream>
#include<map>
#include<string>
using namespace std;

int ReadSector(LPCWSTR drive, int readPoint, BYTE*& sector);
void ReadSector2(LPCWSTR disk, BYTE*& DATA, unsigned int _nsect);
int64_t Get_Bytes(BYTE* sector, int offset, int number);
void Read_BPB(BYTE* sector, LPCWSTR disk);
string toString(BYTE* DATA, int offset, int number);
string toBinary(int n);
int Read_Entry_INFORMATION(BYTE* Entry, int start);
int Read_Entry_FILE_NAME(BYTE* Entry, int start, int ID);
void Read_Entry_DATA(BYTE* Entry, int start);
void print_Tab(int tab);
string get_nameFile(int id);
void Print_Folder_Tree(int a, int tab, int vt);
void Read_BPB(BYTE* sector, LPCWSTR disk);
void read_MFT(unsigned int MFTStart, unsigned int sectors_per_cluster, LPCWSTR disk);
void folder_Tree(unsigned int len_MFT, unsigned int MFTStart, LPCWSTR disk);
void Print_Sector(BYTE* sector);