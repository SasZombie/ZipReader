#include "xmlParser/xmlparser.hpp"


int main()
{
    const auto root = xmlParser::readXML("TestDocx/word/document.xml");
    root->printTree();
}