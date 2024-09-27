#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <bitset>
#include <string_view>
#include <memory>

// Local File Header
struct localFileHeader
{
    std::string sig, ver, gpf, compMethod, flt, fld, crc32, fName, eField;
    uint32_t compSize, uncompSize;
    uint16_t flen, eFlen;

    std::unique_ptr<char> data;
};

char *readBytes(std::ifstream &file, size_t numberBytes);
void printLocalDescriptionTable(const localFileHeader &header);
void createLocalDescriptionTable(std::ifstream &file);
void printSpecialString(const std::string_view string);

[[nodiscard]] char *readBytes(std::ifstream &file, size_t numberBytes)
{
    // I know I can use uniqe_ptr but it seems too powerfull for this
    char *buff = new char[numberBytes];
    file.read(buff, numberBytes);

    if (file.gcount() != static_cast<long>(numberBytes))
    {
        std::cerr << "Error reading byes, I wanted to read " << numberBytes << " but I only read " << file.gcount() << " bytes\n";
        delete[] buff;

        return nullptr;
    }

    return buff;
}

void createLocalDescriptionTable(std::ifstream &file)
{
    localFileHeader currentHeader;

    size_t lengths[] = {4, 2, 2, 2, 2, 2, 4};
    std::string *fields[] = {&currentHeader.sig, &currentHeader.ver, &currentHeader.gpf, &currentHeader.compMethod,
                             &currentHeader.flt, &currentHeader.fld, &currentHeader.crc32};
    char * buff = nullptr;

    for (size_t i = 0; i < sizeof(lengths) / sizeof(lengths[0]); ++i)
    {
        buff = readBytes(file, lengths[i]);

        if (fields[i] != nullptr)
        {
            *fields[i] = std::string(buff, lengths[i]);
        }

        delete[] buff;
    }

    unsigned char buffer[4];
    file.read(reinterpret_cast<char*>(buffer), 4);

    uint32_t compressedSize = 
        static_cast<uint32_t>(buffer[0]) |             
        (static_cast<uint32_t>(buffer[1]) << 8)  |     
        (static_cast<uint32_t>(buffer[2]) << 16) |     
        (static_cast<uint32_t>(buffer[3]) << 24); 

    currentHeader.compSize = compressedSize;
    std::cout << static_cast<unsigned int>(static_cast<unsigned char>(compressedSize)) << ' ' << currentHeader.compSize << '\n';


    file.read(reinterpret_cast<char*>(buffer), 4);

    uint32_t uncompressedSize = 
        static_cast<uint32_t>(buffer[0]) |             
        (static_cast<uint32_t>(buffer[1]) << 8)  |     
        (static_cast<uint32_t>(buffer[2]) << 16) |     
        (static_cast<uint32_t>(buffer[3]) << 24); 

    currentHeader.uncompSize = uncompressedSize;
    std::cout << currentHeader.uncompSize << '\n';

    buff = readBytes(file, 2);
    currentHeader.flen = static_cast<unsigned char>(buff[0]);
    delete[] buff;

    buff = readBytes(file, 2);
    currentHeader.eFlen = static_cast<unsigned char>(buff[0]);
    delete[] buff;

    buff = readBytes(file, currentHeader.flen);
    currentHeader.fName = std::string(buff, currentHeader.flen);
    delete[] buff;

    buff = readBytes(file, currentHeader.eFlen);
    currentHeader.eField = std::string(buff, currentHeader.eFlen);
    delete[] buff;

    currentHeader.data = std::make_unique<char>(currentHeader.compSize);

    // for(size_t i = 0; i < currentHeader.uncompSize; ++i)
    // {
    //     std::cout << i << ' ';
    //     //We know we only read 1 byte so we can hack this
    //     // currentHeader.data.get()[i] = readBytes(file, 1)[0];
    // }

    printLocalDescriptionTable(currentHeader);
}

void printSpecialString(const std::string_view string)
{
    for (size_t i = 0; i < string.size(); ++i)
    {
        if (std::isprint(static_cast<unsigned char>(string[i])))
        {
            std::cout << string[i];
        }
        else
        {
            // If non-printable, print its hex value in escape format
            std::cout << "\\" << std::oct << static_cast<unsigned int>(static_cast<unsigned char>(string[i]));
        }
    }
}

void printLocalDescriptionTable(const localFileHeader &header)
{
    std::cout << "Description for File Table\nLocal file header signature: ";
    printSpecialString(header.sig);
    std::cout << "\nVersion needed to extract:   ";
    printSpecialString(header.ver);
    std::cout << "\nGeneral Purpose Flag:        ";
    printSpecialString(header.gpf);
    std::cout << "\nCompresion Method:           ";
    printSpecialString(header.compMethod);
    std::cout << "\nFile Last Modification time: ";
    printSpecialString(header.flt);
    std::cout << "\nFile Last Modification date: ";
    printSpecialString(header.fld);
    std::cout << "\nCRC-32 of uncompressed data: ";
    printSpecialString(header.crc32);
    std::cout << "\nCompressed size:             " << header.compSize;
    std::cout << "\nUncompressed size:           " << header.uncompSize;
    std::cout << "\nFile Name Length:            " << header.flen;
    std::cout << "\nExtra Field Length:          " << header.eFlen;
    std::cout << "\nFile Name:                   ";
    printSpecialString(header.fName);
    std::cout << "\nExtra Field Name:            ";
    printSpecialString(header.eField);
}

int main()
{
    std::ifstream file("test.zip", std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Could not open file\n";
        return -1;
    }

    createLocalDescriptionTable(file);

    // file.seekg(26);

    // char* buff = readBytes(file, 2);
    // // printSpecialString(std::string(buff, 2));
    // std::cout << buff[0];
    // delete[] buff;

    file.close();
}