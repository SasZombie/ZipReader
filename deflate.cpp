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
        : file(filename, std::ios::binary), currentByte(0), bitsRemaining(0)
    {
        if (!file)
        {
            throw std::runtime_error("Unable to open file");
        }
    }

    unsigned int readBits(int n)
    {
        if (n < 1 || n > 32)
        {
            std::cout << "Warning! Invalid readBits input. Truncated to 1 from " << n << '\n';
            n = 1;
        }

        unsigned int result = 0;
        int bitsRead = 0;

        for (int i = 0; i < n; ++i)
        {
            bool bit = readBit();
            if (bitsRemaining > 0 || (bitsRead > 0))
            {
                if (bit)
                {
                    result |= (1 << (n - 1 - i));
                }
                bitsRead++;
            }
            else
            {
                break;
            }
        }

        if (bitsRead == 0)
        {
            throw std::runtime_error("End of file reached before reading the specified number of bits");
        }

        return result;
    }

    bool readBit()
    {
        if (bitsRemaining == 0)
        {
            if (!readNextByte())
            {
                return false;
            }
        }

        bool bit = (currentByte >> (bitsRemaining - 1)) & 1;
        bitsRemaining--;
        return bit;
    }

    bool readNextByte()
    {
        if (file.read(reinterpret_cast<char *>(&currentByte), 1))
        {
            bitsRemaining = 8;
            return true;
        }
        return false;
    }

    ~BitReader()
    {
        if (file.is_open())
        {
            file.close();
        }
    }

private:
    std::ifstream file;
    unsigned char currentByte;
    int bitsRemaining;
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
}

//TODO: This bit reader somehow is trash. Make new one
int main()
{
    BitReader reader("viszualize");

    std::string encoded = "01100110000110100011010101111000100";

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

    for (size_t i = 0; i < HCLEN; ++i)
    {
        std::cout << CodeLengthTable[i] << " --> " << codeLengths[i] << '\n';
    }

    std::vector<size_t> lengthCount(16, 0);

    for (const size_t len : codeLengths)
    {
        if (len > 0)
            ++lengthCount[len];
    }

    for (const size_t len : lengthCount)
    {
        std::cout << len << '\n';
    }

    std::shared_ptr<Node> root = buildHuffmanTree(lengthCount);

    std::vector<std::string> codes;

    codes.resize(16);

    std::shared_ptr<Node> head = root;

    generateCode(head, "", codes);

    for (const std::string_view str : codes)
    {
        if (str.size() != 0)
            std::cout << str << '\n';
    }

    unsigned int buff[6];
    std::bitset<6> set;
    int index = 0;

    reader.readBits(5);
    reader.readBits(6);

    // for (size_t i = 0; i < 10; ++i)
    // {
    //     std::string str;
    //     head = root;
    //     while (head.get()->length == -1)
    //     {
    //         unsigned int currentBit = reader.readBits(1);
    //         // buff[index] = currentBit;
    //         str = str + std::to_string(currentBit);

    //         if (currentBit == 0)
    //             head = head->left;
    //         else
    //             head = head->right;
    //     }

    //     std::cout << str << '\n';
    // }
}