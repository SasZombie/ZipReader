#include <iostream>
#include <memory>
#include <bitset>

struct HuffemanCode {
    size_t symbol;
    size_t length;
    size_t canonicCode;

    friend std::ostream &operator<<(std::ostream &os, const HuffemanCode &codes)
    {
        os << codes.symbol << " -> " << codes.length << " -> " 
           << std::bitset<8>(codes.canonicCode).to_string().substr(8 - codes.length) 
           << " -> " << codes.canonicCode << '\n';
        return os;
    }
};

struct Node {
    size_t depth = 0;
    HuffemanCode code;
    std::unique_ptr<Node> left = nullptr;
    std::unique_ptr<Node> right = nullptr;
};

void insertInTree(Node *head, const HuffemanCode &code, int i)
{
    if (i <= 0) {
        head->code = code;  // Base case: Set the code at the leaf node
        return;
    }

    // Check the (i-1)th bit in canonicCode and decide whether to go left or right
    if ((code.canonicCode & (1UL << (i - 1))) == 0) {  // If the (i-1)th bit is 0
        if (head->left == nullptr) {
            head->left = std::make_unique<Node>();  // Create a new left node if it doesn't exist
            head->left->depth = head->depth + 1;    // Increment depth
        }
        insertInTree(head->left.get(), code, i - 1);  // Recurse on the left child
    }
    else {  // If the (i-1)th bit is 1
        if (head->right == nullptr) {
            head->right = std::make_unique<Node>();  // Create a new right node if it doesn't exist
            head->right->depth = head->depth + 1;    // Increment depth
        }
        insertInTree(head->right.get(), code, i - 1);  // Recurse on the right child
    }
}

void insertInTree(Node *head, const HuffemanCode &code, int i)
{
    if (i <= 0)
    {
        head->code = code;
        return;
    }

    if (code.canonicCode & (1UL << i))
    {
        if (head->left == nullptr)
        {
            head->depth = head->depth + 1;
            head->left = std::make_unique<Node>();
        }

        insertInTree(head->left.get(), code, i - 1);
    }
    else
    {
        if (head->right == nullptr)
        {
            head->depth = head->depth + 1;
            head->right = std::make_unique<Node>();
        }

        insertInTree(head->right.get(), code, i - 1);
    }
}
void printTree(Node* node) {
    if (node == nullptr) return;
    
    // Print the node data
    if (node->left == nullptr && node->right == nullptr) {  // Leaf node
        std::cout << "Leaf node: " << node->code << " Depth: " << node->depth << std::endl;
    }

    // Recurse on left and right children
    printTree(node->left.get());
    printTree(node->right.get());
}

int main() {
    Node root;  // Root of the tree

    // Example Huffman code to insert
    HuffemanCode code1 = {65, 7, 0b1000001};  // Symbol 'A', length 7, canonic code 1000001
    HuffemanCode code2 = {66, 7, 0b1000010};  // Symbol 'B', length 7, canonic code 1000010

    insertInTree(&root, code1, 7);  // Insert the Huffman code for 'A'
    insertInTree(&root, code2, 7);  // Insert the Huffman code for 'B'

    // Print the tree nodes with their depths and Huffman code
    std::cout << "Tree contents:" << std::endl;
    printTree(&root);  // Print the tree starting from the root

    return 0;
}
