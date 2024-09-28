#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <bitset>
#include <string_view>
#include <memory>
#include <vector>

void printSpecialString(const std::string_view string);
// Local File Header
struct centralDirectoryFileHeader
{
    std::string sig, ver, gpf, compMethod, flt, fld, crc32, fName, eField, ifAttr, exFAttr, fileComment;
    uint16_t flen, eFlen, fComlen, diskNumber;
    uint32_t compSize, uncompSize, relativeOffsetOfHeader;
};

struct EOCD
{
    std::string sig, nrDisk, nrDiskWhereCDStarts, comment;
    uint16_t nrCentralDirRecOnDisk, nrCentralDirTotal, comlen;
    uint32_t sizeOfCD, offsetRelStart;

    friend std::ostream &operator<<(std::ostream &os, const EOCD &eocd)
    {
        os << "sig: " << eocd.sig << "\n";
        os << "nrDisk: " << eocd.nrDisk << "\n";
        os << "nrDiskWhereCDStarts: " << eocd.nrDiskWhereCDStarts << "\n";
        os << "nrCentralDirRecOnDisk: " << eocd.nrCentralDirRecOnDisk << "\n";
        os << "nrCentralDirTotal: " << eocd.nrCentralDirTotal << "\n";
        os << "comlen: " << eocd.comlen << "\n";
        os << "sizeOfCD: " << eocd.sizeOfCD << "\n";
        os << "offsetRelStart: " << eocd.offsetRelStart << "\n";
        os << "comment: " << eocd.comment << "\n";
        return os;
    }
};

struct localFileHeader
{
    std::string sig, ver, gpf, compMethod, flt, fld, crc32, fName, eField;
    uint16_t flen, eFlen;
    uint32_t compSize, uncompSize;

    std::vector<char> data;

    // Overload operator<< to print all fields
    friend std::ostream &operator<<(std::ostream &os, const localFileHeader &ms)
    {
        os << "Description for File Table\nLocal file header signature: ";
        printSpecialString(ms.sig);
        os << "\nVersion needed to extract:   ";
        printSpecialString(ms.ver);
        os << "\nGeneral Purpose Flag:        ";
        printSpecialString(ms.gpf);
        os << "\nCompresion Method:           ";
        printSpecialString(ms.compMethod);
        os << "\nFile Last Modification time: ";
        printSpecialString(ms.flt);
        os << "\nFile Last Modification date: ";
        printSpecialString(ms.fld);
        os << "\nCRC-32 of uncompressed data: ";
        printSpecialString(ms.crc32);
        os << "\nCompressed size:             " << ms.compSize;
        os << "\nUncompressed size:           " << ms.uncompSize;
        os << "\nFile Name Length:            " << ms.flen;
        os << "\nExtra Field Length:          " << ms.eFlen;
        os << "\nFile Name:                   ";
        printSpecialString(ms.fName);
        os << "\nExtra Field Name:            ";
        printSpecialString(ms.eField);
        os << "\nData Extracted:\n";

        for (const char c : ms.data)
        {
            os << c;
        }

        return os;
    }
};

char *readBytes(std::ifstream &file, size_t numberBytes);
void createLocalDescriptionTable(std::ifstream &file);
EOCD scanForEOCD(std::ifstream &file);

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
    unsigned char buff[4];

    for (size_t i = 0; i < sizeof(lengths) / sizeof(lengths[0]); ++i)
    {
        file.read(reinterpret_cast<char *>(buff), lengths[i]);

        if (fields[i] != nullptr)
        {
            *fields[i] = std::string(reinterpret_cast<char*>(buff), lengths[i]);
        }
    }

    file.read(reinterpret_cast<char *>(buff), 4);

    uint32_t compressedSize =
        static_cast<uint32_t>(buff[0]) |
        (static_cast<uint32_t>(buff[1]) << 8) |
        (static_cast<uint32_t>(buff[2]) << 16) |
        (static_cast<uint32_t>(buff[3]) << 24);

    currentHeader.compSize = compressedSize;

    file.read(reinterpret_cast<char *>(buff), 4);

    uint32_t uncompressedSize =
        static_cast<uint32_t>(buff[0]) |
        (static_cast<uint32_t>(buff[1]) << 8) |
        (static_cast<uint32_t>(buff[2]) << 16) |
        (static_cast<uint32_t>(buff[3]) << 24);

    currentHeader.uncompSize = uncompressedSize;

    file.read(reinterpret_cast<char *>(buff), 2);
    uint16_t flen =
        static_cast<uint16_t>(buff[0]) |
        (static_cast<uint16_t>(buff[1]) << 8);

    currentHeader.flen = flen;

    file.read(reinterpret_cast<char *>(buff), 2);
    uint16_t eFlen =
        static_cast<uint16_t>(buff[0]) |
        (static_cast<uint16_t>(buff[1]) << 8);
    currentHeader.eFlen = eFlen;

    char* dynBuff = readBytes(file, currentHeader.flen);
    currentHeader.fName = std::string(dynBuff, currentHeader.flen);
    delete[] dynBuff;

    dynBuff = readBytes(file, currentHeader.eFlen);
    currentHeader.eField = std::string(dynBuff, currentHeader.eFlen);
    delete[] dynBuff;

    for (size_t i = 0; i < currentHeader.uncompSize; ++i)
    {
        // We know we only read 1 byte so we can hack this
        file.read(reinterpret_cast<char *>(buff), 1);

        currentHeader.data.push_back(buff[0]);
    }

    std::cout << currentHeader;
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
            std::cout << "\\" << std::oct << static_cast<unsigned int>(static_cast<unsigned char>(string[i])) << std::dec;
        }
    }
}

EOCD scanForEOCD(std::ifstream &file)
{
    EOCD eocd;
    constexpr unsigned char values[] = {0x50, 0x4b, 0x05, 0x06};
    file.seekg(0);

    unsigned char current_byte;
    size_t sequence_index = 0;

    while (file.read(reinterpret_cast<char *>(&current_byte), 1))
    {
        if (current_byte == values[sequence_index])
        {
            sequence_index++;

            if (sequence_index == 4)
            {
                eocd.sig = "06054b50";

                unsigned char buff[4];

                file.read(reinterpret_cast<char *>(buff), 2);
                eocd.nrDisk = std::string(reinterpret_cast<char *>(buff), 2);

                file.read(reinterpret_cast<char *>(buff), 2);
                eocd.nrDiskWhereCDStarts = std::string(reinterpret_cast<char *>(buff), 2);

                file.read(reinterpret_cast<char *>(buff), 2);
                uint16_t nrCentralDisk =
                    static_cast<uint16_t>(buff[0]) |
                    (static_cast<uint16_t>(buff[1]) << 8);

                eocd.nrCentralDirRecOnDisk = nrCentralDisk;

                file.read(reinterpret_cast<char *>(buff), 2);

                uint16_t nrCentralDiskTot =
                    static_cast<uint16_t>(buff[0]) |
                    (static_cast<uint16_t>(buff[1]) << 8);

                eocd.nrCentralDirTotal = nrCentralDiskTot;

                file.read(reinterpret_cast<char *>(buff), 4);
                uint32_t sizeCD =
                    static_cast<uint32_t>(buff[0]) |
                    (static_cast<uint32_t>(buff[1]) << 8) |
                    (static_cast<uint32_t>(buff[2]) << 16) |
                    (static_cast<uint32_t>(buff[3]) << 24);

                eocd.sizeOfCD = sizeCD;

                file.read(reinterpret_cast<char *>(buff), 4);
                uint32_t offSetOfStart =
                    static_cast<uint32_t>(buff[0]) |
                    (static_cast<uint32_t>(buff[1]) << 8) |
                    (static_cast<uint32_t>(buff[2]) << 16) |
                    (static_cast<uint32_t>(buff[3]) << 24);

                eocd.offsetRelStart = offSetOfStart;

                file.read(reinterpret_cast<char *>(buff), 2);
                uint16_t commentLen =
                    static_cast<uint16_t>(buff[0]) |
                    (static_cast<uint16_t>(buff[1]) << 8);

                eocd.comlen = commentLen;

                char* dymbuff = readBytes(file, commentLen);

                eocd.comment = std::string(dymbuff, commentLen);

                delete[] dymbuff;
                return eocd;
            }
        }
        else
        {
            sequence_index = 0;
        }
    }

    std::cerr << "Sequence not found, not a valid zip file\n";
    exit(EXIT_FAILURE);
}

int main()
{

    std::ifstream file("test.zip", std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Could not open file\n";
        return -1;
    }

    // createLocalDescriptionTable(file);
    std::cout << scanForEOCD(file);

    file.close();
}