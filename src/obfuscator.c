#include <windows.h>
#include <assert.h>

CheckExt(const char* buffer, char* ext) {
    size_t buf_len = strlen(buffer);
    size_t i = 0;
    for (; i < buf_len && buffer[i] != '.'; i++){}
    return strcmp(buffer+i, ext) == 0;
}

void Obfuscator(char* start, size_t sz) {
    for (size_t i = 0; i < sz-1; i++)
    {
       start[i] ^= sz;
    }
}

main(int argc, char const *argv[])
{
    if(argc < 2) return GetLastError();
    if(!CheckExt(argv[1],".obj")) return GetLastError();

    HANDLE hFile = 
        CreateFileA(argv[1], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    assert(hFile != INVALID_HANDLE_VALUE);

    size_t szFile =  GetFileSize(hFile, NULL);

    //Anonymous mapping of our file in memory, easier access
    HANDLE hMap;
    if(!(hMap=CreateFileMapping(hFile, NULL, PAGE_READWRITE,0, szFile ,NULL))) return GetLastError();

    // Map file (view) inside our process address space
    void* pFile = MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0,0);
    assert(pFile != NULL);
    void* pFile2 = pFile;

    //COFF File Header
    IMAGE_FILE_HEADER* FileHeader = ((IMAGE_FILE_HEADER*)pFile)++; 

    //COFF section headers
    IMAGE_SECTION_HEADER*  data_header = NULL;
    unsigned long data_location = 0; 
    unsigned long data_size  = 0;

    //iterate through sections
    for (size_t i = 0; i < FileHeader->NumberOfSections; i++) {
        IMAGE_SECTION_HEADER* coff_section = ((IMAGE_SECTION_HEADER*)pFile)++;

        if(strncmp(coff_section->Name,".data",8) == 0) {
            data_header = coff_section;
            data_location = coff_section->PointerToRawData;
            data_size = coff_section->SizeOfRawData;
            break;
        }
    }
    pFile = pFile2;//Backup, restore original pointer to be able to use the PointerToRawData of the data section

    char* StartOfChars = (char*)pFile+data_location;
    size_t szString = 0;
    unsigned long nb_of_zeros = 0;

    //Obfuscate each string present in the .data section
    for (
        size_t i = 0;
     i < data_header->SizeOfRawData;
     i++)
    {
            if(StartOfChars[i] == '\0') {
                if(++nb_of_zeros > 1) continue; //avoid executing this stub more than once after reaching the end of string

                Obfuscator(StartOfChars+(i-szString), szString); //start obfuscating from the beginning of the string
                szString = 0;
                continue;
            }
            else if(StartOfChars[i] < 32 || StartOfChars[i] > 127) break; //ASCII Printable characters range

            nb_of_zeros = 0;
            szString++;
    }

    //Commit changes to the file
    FlushViewOfFile(pFile, 0);
    UnmapViewOfFile(pFile);
    CloseHandle(hMap);
    CloseHandle(hFile);
    return 0;
}
