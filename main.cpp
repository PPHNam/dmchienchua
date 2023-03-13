#include "Header.h"

int main(int argc, char** argv)
{
    wstring disk_name = L"\\\\.\\F:";
    LPCWSTR drive = disk_name.c_str();
    BYTE* sector = new BYTE[512];
    if (ReadSector(drive, 0, sector))
    {
        Print_Sector(sector);
        Read_BPB(sector, drive);
        delete[] sector;
    }
    else
    {
        cout << "Đọc không thành công !!!" << endl;
        return 0;
    }
}