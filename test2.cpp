#include <iostream>
#include <memory>
#include <vector>
#include <string>

class Node : public std::enable_shared_from_this<Node> {
public:
    std::string name;
    std::vector<std::shared_ptr<Node>> children;
    std::weak_ptr<Node> parent;  // Weak pointer to prevent circular references

    explicit Node(std::string nodeName) : name(std::move(nodeName)) {}

    void addChild(std::shared_ptr<Node> child) {
        child->parent = shared_from_this();
        children.push_back(std::move(child));
    }

    void printTree(int level = 0) const {
        std::cout << std::string(level * 2, ' ') << name << "\n";
        for (const auto& child : children) {
            child->printTree(level + 1);
        }
    }
};

int main() {
    std::shared_ptr<Node> root = std::make_shared<Node>("Root");
    std::shared_ptr<Node> current = root; // Start at the root

    std::vector<int> arr = {1, 1, 2, 1}; // Commands sequence
    int count = 0;

    for (int i : arr) {
        switch (i) {
        case 1: { // Create a child and move to it
            std::shared_ptr<Node> child = std::make_shared<Node>("Node " + std::to_string(++count));
            current->addChild(child);
            current = child; // Move to the new child
            break;
        }

        case 2: { // Move back to the parent
            if (auto parent = current->parent.lock()) {
                current = parent;
            }
            break;
        }

        default:
            break;
        }
    }

    std::cout << "Tree Structure:\n";
    root->printTree(); // Print tree structure

    return 0;
}
