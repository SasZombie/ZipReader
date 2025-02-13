#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <memory>
#include <queue>
#include <algorithm>
#include <unordered_map>

class BitReader
{
public:
    BitReader(const std::string &filename)
        : file(filename, std::ios::binary), currentByte(0), bitsRemaining(0), mask(128)
    {
        if (!file)
        {
            throw std::runtime_error("Unable to open file");
        }
    }

    void readByte()
    {
        file.read(reinterpret_cast<char *>(&currentByte), 1);

        if (file.gcount() != 1)
        {
            // std::cerr << "File read error\n";

            throw std::runtime_error("Error reading byte\n");
        }
        this->bitsRemaining = 8;
        this->mask = 128;
    }

    bool readBit()
    {
        if (!this->bitsRemaining)
        {
            this->readByte();
        }

        bool val = (currentByte & mask) >> (bitsRemaining - 1);
        --bitsRemaining;
        mask = mask / 2;

        return val;
    }

    unsigned char readBits(unsigned int bits)
    {
        if (bits < 1 || bits > 8)
        {
            std::cout << "Warning: incorrect value of bits, truncated to 1\n";
            bits = 1;
        };

        // std::bitset<8> number;

        unsigned char number = 0;

        for (size_t i = 0; i < bits; ++i)
        {
            bool rezult = readBit();
            number |= (rezult << (bits - i - 1));
        }

        return number;
    }

private:
    std::ifstream file;
    unsigned char currentByte;
    int bitsRemaining;
    size_t mask;
};

void printSpecialString(const std::string_view string);

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

struct HuffemanCode
{
    size_t symbol;
    size_t length;
    size_t canonicCode;

    friend std::ostream &operator<<(std::ostream &os, const HuffemanCode &codes)
    {
        os << codes.symbol << " -> " << codes.length << " -> " << std::bitset<8>(codes.canonicCode).to_string().substr(8 - codes.length) << " -> " << codes.canonicCode << '\n';

        return os;
    }
};

struct Node
{
    size_t depth = 0;
    HuffemanCode code;
    std::unique_ptr<Node> left = nullptr;
    std::unique_ptr<Node> right = nullptr;

    std::strong_ordering operator<=>(const Node &other) const = default;
};

void insertInTree(Node *head, const HuffemanCode &code, int i)
{
    if (i <= 0)
    {
        head->code = code;
        return;
    }


    if ((code.canonicCode & (1UL << (i - 1))) == 0)
    {
        if (head->left == nullptr)
        {
            head->left = std::make_unique<Node>();
            head->left->depth = head->depth + 1;  
        }
        insertInTree(head->left.get(), code, i - 1);
    }
    else
    { 
        if (head->right == nullptr)
        {
            head->right = std::make_unique<Node>(); 
            head->right->depth = head->depth + 1;   
        }
        insertInTree(head->right.get(), code, i - 1);
    }
}
void printTree(Node *node)
{
    if (node == nullptr)
        return;

    if (node->left == nullptr && node->right == nullptr)
    { 
        std::cout << "Leaf node: " << node->code << " Depth: " << node->depth << std::endl;
    }

    printTree(node->left.get());
    printTree(node->right.get());
}

size_t decodeNextSymbol(BitReader &reader, Node *head, int iter)
{
    if (head->left == nullptr && head->right == nullptr)
    {
        return head->code.symbol;
    }

    if (reader.readBit())
    {
        if (head->right.get() == nullptr)
        {
            std::cerr << "Error IN Right, not existant\n";
            std::exit(EXIT_FAILURE);
        }
        return decodeNextSymbol(reader, head->right.get(), iter + 1);
    }
    else
    {
        if (head->left.get() == nullptr)
        {
            std::cerr << "Error IN Left, not existant\n"
                      << iter;
            std::exit(EXIT_FAILURE);
        }
        return decodeNextSymbol(reader, head->left.get(), iter + 1);
    }

    std::cerr << "Error, should not get here\n";
    return 69;
}

int main()
{

    BitReader reader("testDeflate.zip");

    std::cout << "First 3 bits: " << std::bitset<3>(reader.readBits(3)) << '\n';

    const size_t HLIT = std::bitset<5>(reader.readBits(5)).to_ulong() + 257;
    const size_t HDIST = 1 + std::bitset<5>(reader.readBits(5)).to_ulong();
    const size_t HCLEN = 4 + (reader.readBits(4));

    std::cout << "HLIT: " << HLIT << "\nHDIST: " << HDIST << "\nHCLEN: " << HCLEN << '\n';

    constexpr size_t CodeLengthTable[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    constexpr size_t CodeLenTableSize = sizeof(CodeLengthTable) / sizeof(CodeLengthTable[0]);

    std::vector<size_t> codeLengths(19, 0);

    for (size_t i = 0; i < HCLEN; ++i)
    {
        codeLengths[CodeLengthTable[i]] = reader.readBits(3);
    }

    for (size_t i = 0; i < CodeLenTableSize; ++i)
    {
        std::cout << CodeLengthTable[i] << " --> " << codeLengths[i] << '\n';
    }

    std::cout << "============================================\n";

    std::vector<std::pair<size_t, size_t>> pairs;

    for (size_t i = 0; i < CodeLenTableSize; ++i)
    {
        if (codeLengths[i] != 0)
            pairs.push_back({CodeLengthTable[i], codeLengths[i]});
    }
    std::sort(pairs.begin(), pairs.end(), [&](std::pair<size_t, size_t> &left, std::pair<size_t, size_t> &right)
              {
                if(left.second == right.second)
                {
                    auto it = std::find(std::begin(CodeLengthTable), std::end(CodeLengthTable), left.first);
                    int index = std::distance(std::begin(CodeLengthTable), it);

                    it = std::find(std::begin(CodeLengthTable), std::end(CodeLengthTable), right.first);
                    int index2 = std::distance(std::begin(CodeLengthTable), it);

                    return index < index2;
                }
                
                return left.second < right.second; });

    std::cout << "============================================\n";

    for (const auto &elem : pairs)
    {
        std::cout << elem.first << " --> " << elem.second << '\n';
    }

    // Symbol -> Code
    std::vector<std::pair<size_t, size_t>> canonicalCodes;
    size_t code = 0;
    size_t prevBitLen = 0;

    for (const auto &[symbol, bitLen] : pairs)
    {
        if (bitLen > prevBitLen)
        {
            code <<= (bitLen - prevBitLen);
        }

        canonicalCodes.push_back({symbol, code});
        ++code;
        prevBitLen = bitLen;
    }

    std::cout << "============================================\n";

    std::vector<HuffemanCode> codes(pairs.size());

    for (size_t i = 0; i < canonicalCodes.size(); ++i)
    {
        const auto pair = canonicalCodes[i];

        std::cout << pair.first << " --> " << std::bitset<8>(pair.second).to_string().substr(8 - pairs[i].second) << '\n';

        codes[i] = {pair.first, pairs[i].second, pair.second};
    }
    std::cout << "============================================\n";

    for (const auto &cod : codes)
    {
        std::cout << cod;
    }
    std::cout << "============================================\n";

    Node head;

    for (const auto &cod : codes)
    {
        insertInTree(&head, cod, cod.length);
    }

    printTree(&head);

    std::cout << "============================================\n";


    for(size_t i = 0; i < HDIST; ++i)
    {
        std::cout << (char) decodeNextSymbol(reader, &head, 0) + 13 << ' ';
    }    

}