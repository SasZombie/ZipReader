#include <memory>
#include <iostream>
#include <vector>
#include <string>

struct Node
{
    std::string name;

    std::shared_ptr<Node> prev;

    std::vector<std::shared_ptr<Node>> nodes;

    Node() = default;

    Node(std::string name, std::shared_ptr<Node> prev)
        : name(name), prev(std::move(prev)) {}
};


int main()
{


    // std::vector<int> arr = {1, 1, 2, 1};

    // std::shared_ptr<Node> root = std::make_shared<Node>();
    // int count = 0;

    // for (int i : arr)
    // {

    //     switch (i)
    //     {
    //     case 1:
    //     {
    //         std::shared_ptr<Node> child1 = std::make_shared<Node>("Child" + std::to_string(count), root);
    //         root->nodes.push_back(child1);
    //         ++count;

    //         root = child1;

    //         break;
    //     }

    //     case 2:
    //     {
    //         --count;

    //         root = root->prev;
    //         break;
    //     }

    //     default:
    //         break;
    //     }

    // }

    // std::cout << root->name;

    // int a = 0;
    // std::shared_ptr<Node> root = std::make_shared<Node>();
    // root->name = "Sas";
    // std::shared_ptr<Node> child1 = std::make_shared<Node>("Child", root);
    // root->nodes.push_back(child1);
    // root = child1;

    // if (a == 0)
    // {

    // }

    // switch (a)
    // {
    // case 0:
    // {
    //     std::shared_ptr<Node> child1 = std::make_shared<Node>("Child", root);
    //     root->nodes.push_back(child1);
    //     root = child1;
    //     break;
    // }

    // default:
    //     break;
    // }

    // std::cout << root->name << '\n';

    // root = root->prev;

    // std::cout << root->name << '\n';
    // }
}