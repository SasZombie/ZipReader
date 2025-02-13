#include "xmlParser/xmlparser.hpp"


int main()
{
    const auto root = xmlParser::readXML("sample.xml");
    root->printTree();
}