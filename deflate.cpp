#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <memory>
#include <queue>

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

struct Node
{
    int length;
    int count;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    Node(int l, int c)
        : length(l), count(c), left(nullptr), right(nullptr) {}

    std::strong_ordering operator<=>(const Node &other) const = default;
};
std::shared_ptr<Node> buildHuffmanTree(const std::vector<size_t> &lenCounts);
void generateCode(std::shared_ptr<Node> head, const std::string &code, std::vector<std::string> &codes);
std::string traverse();


std::shared_ptr<Node> buildHuffmanTree(const std::vector<size_t> &lenCounts)
{
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>> minHeap;

    for (size_t i = 0; i < lenCounts.size(); ++i)
    {
        if (lenCounts[i] > 0)
        {
            minHeap.push(std::make_shared<Node>(i, lenCounts[i]));
        }
    }

    while (minHeap.size() > 1)
    {
        std::shared_ptr<Node> left = minHeap.top();
        minHeap.pop();

        std::shared_ptr<Node> right = minHeap.top();
        minHeap.pop();

        std::shared_ptr<Node> combined = std::make_shared<Node>(-1, left->count + right->count);

        combined->left = left;
        combined->right = right;

        minHeap.push(combined);
    }

    return minHeap.top();
}

void generateCode(std::shared_ptr<Node> head, const std::string &code, std::vector<std::string> &codes)
{
    if (!head.get())
        return;

    if (head->length != -1)
    {
        codes[head->length] = code;
    }

    generateCode(head->left, code + "0", codes);
    generateCode(head->right, code + "1", codes);
}

std::string traverse()
{
    return std::string();
}

//TODO: This bit reader somehow is trash. Make new one
int main()
{
    BitReader reader("viszualize");

    std::cout << "First 3 bits: " << std::bitset<3>(reader.readBits(3)) << '\n';

    const size_t HLIT = std::bitset<5>(reader.readBits(5)).to_ulong() + 257;
    const size_t HDIST = 1 + std::bitset<5>(reader.readBits(5)).to_ulong();
    const size_t HCLEN = 4 + (reader.readBits(4));

    std::cout << "HLIT: " << HLIT << "\nHDIST: " << HDIST << "\nHCLEN: " << HCLEN << '\n';

    constexpr size_t CodeLengthTable[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    std::vector<size_t> codeLengths;

    for (size_t i = 0; i < HCLEN; ++i)
    {
        codeLengths.push_back(reader.readBits(4));
    }

    // for (size_t i = 0; i < HCLEN; ++i)
    // {
    //     std::cout << CodeLengthTable[i] << " --> " << codeLengths[i] << '\n';
    // }

    std::vector<size_t> lengthCount(16, 0);

    for (const size_t len : codeLengths)
    {
        if (len > 0)
            ++lengthCount[len];
    }

    // for (const size_t len : lengthCount)
    // {
    //     std::cout << len << '\n';
    // }

    std::shared_ptr<Node> root = buildHuffmanTree(lengthCount);

    std::vector<std::string> codes;

    codes.resize(16);

    std::shared_ptr<Node> head = root;

    generateCode(head, "", codes);

    // for (const std::string_view str : codes)
    // {
    //     if (str.size() != 0)
    //         std::cout << str << '\n';
    // }

    for (size_t i = 0; i < 10; ++i)
    {
        std::string str;
        head = root;
        while (head.get()->length == -1)
        {
            unsigned int currentBit = reader.readBit();
            str = str + std::to_string(currentBit);

            if (currentBit == 0)
                head = head->left;
            else
                head = head->right;
        }

        std::cout << str << '\n';
    }
}