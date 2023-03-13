#include "Header.h"

vector<int> fileID;
vector<int> parentID;
vector<string> nameFile;
bool check = false;

int ReadSector(LPCWSTR drive, int readPoint, BYTE*& sector)
{
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,    
        GENERIC_READ,           
        FILE_SHARE_READ | FILE_SHARE_WRITE,        
        NULL,                 
        OPEN_EXISTING,         
        0,                    
        NULL);                 

    if (device == INVALID_HANDLE_VALUE) 
    {
        cout << "Khong the mo File!! " << endl;
        cout << endl;
        return 0;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        cout << "Doc File bi loi " << endl;
        return 0;
    }
    else
    {
        cout << "Doc thanh cong !!!" << endl;
        cout << endl;
        return 1;
    }
}

void ReadSector2(LPCWSTR disk, BYTE*& DATA, unsigned int _nsect)
{
    DWORD dwBytesRead(0);

    HANDLE diaMem = NULL;
    diaMem = CreateFile(disk,    
        GENERIC_READ,             
        FILE_SHARE_READ | FILE_SHARE_WRITE,           
        NULL,                     
        OPEN_EXISTING,            
        0,                        
        NULL);                    

    if (diaMem != NULL)
    {
        LARGE_INTEGER li;
        li.QuadPart = _nsect * 512;
        SetFilePointerEx(diaMem, li, 0, FILE_BEGIN);
        // Read the boot sector
        if (!ReadFile(diaMem, DATA, 512, &dwBytesRead, NULL))
        {
            printf("Error in reading floppy disk\n");
        }

        CloseHandle(diaMem);
        // Close the handle
    }
}


int64_t Get_Bytes(BYTE* sector, int offset, int number)
{
    int64_t k = 0;
    memcpy(&k, sector + offset, number);
    return k;
}


string toString(BYTE* DATA, int offset, int number)
{
    char* tmp = new char[number + 1];
    memcpy(tmp, DATA + offset, number);
    string s = "";
    for (int i = 0; i < number; i++)
        if (tmp[i] != 0x00)
            s += tmp[i];

    return s;
}


string toBinary(int n)
{
    string r;
    while (n != 0)
    {
        r = (n % 2 == 0 ? "0" : "1") + r; n /= 2;
    }
    return r;
}


int Read_Entry_INFORMATION(BYTE* Entry, int start)
{
    int status = Get_Bytes(Entry, start + 56, 4);
    string bin = toBinary(status);
    for (int i = bin.length() - 1; i >= 0; i--)
    {
        int n = bin.length();
        if (bin[i] == '1')
        {
            if (i == n - 2)
            {
                return -1;
            }
            if (i == n - 3)
            {
                return -1;
            }
        }
    }
    cout << "Attribute $STANDARD_INFORMATION" << endl;
    int size = Get_Bytes(Entry, start + 4, 4);
    cout << "\t- Length of attribute (include header): " << size << endl;
    cout << "\t- Status Attribute of File: " << bin << endl;
    for (int i = bin.length() - 1; i >= 0; i--)
    {
        int n = bin.length();
        if (bin[i] == '1')
        {
            if (i == n - 1)
                cout << "\t\t => Read Only" << endl;
            if (i == n - 4)
                cout << "\t\t => Vollabel" << endl;
            if (i == n - 5)
                cout << "\t\t => Directory" << endl;
            if (i == n - 6)
                cout << "\t\t => Archive" << endl;
        }
    }
    cout << endl;
    return size;
}

int Read_Entry_FILE_NAME(BYTE* Entry, int start, int ID)
{
    cout << "Attribute $FILE_NAME" << endl;
    int size = Get_Bytes(Entry, start + 4, 4);
    cout << "\t- Length of attribute (include header): " << size << endl;
    int parent_file = Get_Bytes(Entry, start + 24, 6);
    cout << "\t- Parent file: " << parent_file << endl;
    parentID.push_back(parent_file);
    int lengthName = Get_Bytes(Entry, start + 88, 1);
    cout << "\t- Length of name file: " << lengthName << endl;
    string name = toString(Entry, start + 90, lengthName * 2);
    cout << "\t- Name of file: " << name << endl;
    string exts = "";
    for (int i = name.length() - 1; i >= name.length() - 5; i--)
    {
        if (name[i] == '.')
            break;
        exts += name[i];
    }
    reverse(exts.begin(), exts.end());

    if (exts == "doc" || exts == "docx")
        cout << "\t\t\t => Use Microsoft Office Word to open!\n";
    if (exts == "ppt" || exts == "pptx")
        cout << "\t\t\t => Use Microsoft Office PowerPoint to open!\n";
    if (exts == "xls" || exts == "xlsx")
        cout << "\t\t\t => Use Microsoft Office Excel to open!\n";
    if (exts == "sln" || exts == "cpp" || exts == "java" || exts == "html" || exts == "css")
        cout << "\t\t\t => Use Microsoft Visual Studio to open!\n";
    if (exts == "pdf")
        cout << "\t\t\t => Use Foxit PDF Reader or Web Browers (Edge, Chrome, ...) to open!\n";
    if (exts == "txt") check = true;
    nameFile.push_back(name);
    cout << endl;

    return size;
}

void Read_Entry_DATA(BYTE* Entry, int start)
{
    cout << "Attribute $DATA" << endl;
    int size = Get_Bytes(Entry, start + 4, 4);
    cout << "\t- Length of attribute (include header): " << size << endl;
    int sizeFile = Get_Bytes(Entry, start + 16, 4);
    cout << "\t- Size of file: " << sizeFile << endl;
    int type = Get_Bytes(Entry, start + 8, 1);
    if (type == 0 && check == true)
    {
        cout << "\t\t=> Resident" << endl;
        int cont_Size = Get_Bytes(Entry, start + 16, 4);
        int cont_Start = Get_Bytes(Entry, start + 20, 2);
        string content = toString(Entry, start + cont_Start, cont_Size);
        cout << endl;
        cout << "Content: " << content << endl;
    }
    else
        cout << "\t\t=> Non-resident" << endl;
    cout << endl;
}

void print_Tab(int tab)
{
    for (int i = 0; i < tab; i++)
        cout << "\t";
}

string get_nameFile(int id)
{
    string kq = "";
    int t = -1;
    for (int i = 0; i < fileID.size(); i++)
        if (fileID[i] == id)
        {
            t = i;
            break;
        }

    if (t != -1)
        kq = nameFile[t];
    return kq;
}

void Print_Folder_Tree(int a, int tab, int t)
{
    tab++;
    print_Tab(tab);
    cout << get_nameFile(a) << endl;
    fileID[t] = -1;
    parentID[t] = -1;

    vector<int> child;
    vector<int> VT;
    for (int j = 0; j < fileID.size(); j++)
        if (parentID[j] == a)
        {
            child.push_back(fileID[j]);
            VT.push_back(j);
        }

    if (child.size() == 0)
        return;
    for (int i = 0; i < child.size(); i++)
        Print_Folder_Tree(child[i], tab, VT[i]);
}

void Read_BPB(BYTE* sector, LPCWSTR disk)
{
    unsigned int bytes_per_sector = Get_Bytes(sector, 0x0B, 2); // Bytes Per Sector
    unsigned int sectors_per_cluster = Get_Bytes(sector, 0x0D, 1); // Sectors Per Cluster
    unsigned int sectors_per_track = Get_Bytes(sector, 0x18, 2); // Sectors Per Track
    unsigned int total_sectors = Get_Bytes(sector, 0x28, 8); // Total Sectors
    unsigned int MFTStart = Get_Bytes(sector, 0x30, 8); // Cluster start of MFT
    unsigned int MFTMirrorStart = Get_Bytes(sector, 0x38, 8); // Cluster start of MFTMirror
    cout << endl;
    cout << "Bytes Per Sector : " << bytes_per_sector << endl;
    cout << "Sectors Per Cluster : " << sectors_per_cluster << endl;
    cout << "Sectors Per Track : " << sectors_per_track << endl;
    cout << "Total Sectors : " << total_sectors << endl;
    cout << "Cluster start of MFT : " << MFTStart << endl;
    cout << "Cluster start of MFTMirror : " << MFTMirrorStart << endl;
    cout << endl;
    read_MFT(MFTStart, sectors_per_cluster, disk);
}

void read_MFT(unsigned int MFTStart, unsigned int sectors_per_cluster, LPCWSTR disk)
{
    BYTE* MFT = new BYTE[512];
    MFTStart *= sectors_per_cluster;
    ReadSector2(disk, MFT, MFTStart);
    int Entry_in4 = Get_Bytes(MFT, 0x014, 2);
    cout << "Thuoc tinh cua $INFORMATION Entry bat dau tai: " << Entry_in4 << endl;
    int len_in4 = Get_Bytes(MFT, 0x048, 4);
    cout << "Chieu dai cua $INFORMATION Entry: " << len_in4 << endl;
    int Entry_Name = Entry_in4 + len_in4;
    cout << "Thuoc tinh cua $FILE NAME Entry bat dau tai: " << Entry_Name << endl;
    int len_Name = Get_Bytes(MFT, 0x09C, 4);
    cout << "Length of $FILE NAME Entry: " << len_Name << endl;
    int tmp = Get_Bytes(MFT, 0x108, 4);
    int Entry_Data = 0;
    if (tmp == 64) {
        Entry_Data = Entry_Name + len_Name + Get_Bytes(MFT, 0x10C, 4);
        cout << "Attribute $DATA Entry bat dau tai: " << Entry_Data << endl;
        int len_data = Get_Bytes(MFT, 0x134, 4);
        cout << "Length of DATA Entry: " << len_data << endl;
    }
    else {
        Entry_Data = Entry_Name + len_Name;
        cout << "Attribute $DATA Entry bat dau tai: " << Entry_Data << endl;
        int len_data = Get_Bytes(MFT, 0x10C, 4);
        cout << "Chieu dai cua DATA Entry: " << len_data << endl;
    }
    unsigned int len_MFT = MFTStart + (Get_Bytes(MFT, Entry_Data + 24, 8) + 1) * 8;
    cout << "Number sector in MFT is: " << len_MFT - MFTStart << endl;
    cout << endl;
    folder_Tree(len_MFT, MFTStart, disk);
}

void folder_Tree(unsigned int len_MFT, unsigned int MFTStart, LPCWSTR disk)
{
    for (int i = 2; i < len_MFT - MFTStart; i += 2)
    {
        int currentSector = MFTStart + i;
        BYTE* currentEntry = new BYTE[512];
        ReadSector2(disk, currentEntry, currentSector);
        if (toString(currentEntry, 0x00, 4) == "FILE")
        {
            check = false;
            int ID = Get_Bytes(currentEntry, 0x02C, 4);
            if (ID > 38)
            {
                cout << endl;
                cout << endl;
                cout << "ID File: " << ID << endl;
                int startInfor = Get_Bytes(currentEntry, 0x014, 2);
                int sizeInfor = Read_Entry_INFORMATION(currentEntry, startInfor);
                if (sizeInfor == -1)
                    continue;
                int startName = sizeInfor + 56;
                int sizeName = Read_Entry_FILE_NAME(currentEntry, startName, ID);
                int startData = startName + sizeName;
                if (Get_Bytes(currentEntry, startData, 4) == 64)
                {

                    int len_obj = Get_Bytes(currentEntry, startData + 4, 4);
                    startData += len_obj;
                    Read_Entry_DATA(currentEntry, startData);
                }
                else
                {
                    while (Get_Bytes(currentEntry, startData, 4) != 128)
                    {
                        startData += 4;
                    }

                    Read_Entry_DATA(currentEntry, startData);
                }

                fileID.push_back(ID);
            }
        }
        delete currentEntry;
    }
    cout << "---------------------------------------------------------" << endl;
    cout << "\t \t \t CAY THU MUC" << endl;
    for (int i = 0; i < fileID.size(); i++)
        if (fileID[i] != -1 && parentID[i] != -1)
            Print_Folder_Tree(fileID[i], -1, i);
}


void Print_Sector(BYTE* sector)
{
    int count = 0;
    int num = 0;

    cout << "         0  1  2  3  4  5  6  7    8  9  A  B  C  D  E  F" << endl;

    cout << "0x0" << num << "0  ";
    bool flag = 0;
    for (int i = 0; i < 512; i++)
    {
        count++;
        if (i % 8 == 0)
            cout << "  ";
        printf("%02X ", sector[i]);
        if (i == 255)
        {
            flag = 1;
            num = 0;
        }

        if (i == 511) break;
        if (count == 16)
        {
            int index = i;

            cout << endl;

            if (flag == 0)
            {
                num++;
                if (num < 10)
                    cout << "0x0" << num << "0  ";
                else
                {
                    char hex = char(num - 10 + 'A');
                    cout << "0x0" << hex << "0  ";
                }

            }
            else
            {
                if (num < 10)
                    cout << "0x1" << num << "0  ";
                else
                {
                    char hex = char(num - 10 + 'A');
                    cout << "0x1" << hex << "0  ";
                }
                num++;
            }

            count = 0;
        }
    }
    cout << endl;
}