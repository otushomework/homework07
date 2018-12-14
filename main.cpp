#include <string>
#include <iostream>
#include <fstream>
#include <set>

//#define _DEBUG


struct RadixTree
{
private:

    struct Node;
    struct NodeComparator {
        bool operator()(Node const *a, Node const *b) {
            return a->label < b->label;
        }
    };

    struct Node {
        std::string label;
        int isEnd;
        std::set<Node *, NodeComparator> childs;

        Node() = default;
        ~Node()
        {
            for ( Node *child : childs )
                delete child;

            childs.clear();
        }

        bool insert(std::string &word, int depth = 0)
        {
            std::string equalPart;
            for (int i = 0; i < static_cast<int>(std::min(word.length(), label.length())); ++i)
            {
                if (word.at(i) == label.at(i))
                    equalPart += word.at(i);
                else
                    break;
            }

#ifdef _DEBUG
            std::cout << "equalPart '" << equalPart << "' " << word << " " << label << std::endl;
#endif

            if (equalPart.length() < label.length())
            {
                if (equalPart.length() == 0 && depth > 0)
                    return false;

                Node *childNode = new Node{label.substr(equalPart.length()), isEnd, childs};
                label = equalPart;
                isEnd = equalPart.length() == word.length();
                childs.clear();
                childs.insert(childNode);

                if (equalPart.length() == 0)
                    childs.insert(new Node{word, true});

                if (equalPart.length() > 0 && depth > 0 && equalPart.length() != word.length())
                    childs.insert(new Node{word.substr(equalPart.length()), true});

                return true;
            }

            depth++;
            for ( Node *child : childs )
            {
                std::string subWord = word.substr(equalPart.length());
                bool res = child->insert(subWord, depth);
                if (res)
                    return true;
            }

            childs.insert(new Node{word.substr(equalPart.length()), true});
            return true;
        }
    };

    Node *m_root = nullptr;
public:
    RadixTree() = default;

    ~RadixTree()
    {
        delete m_root;
    }

    void insert(std::string &word)
    {
        if (m_root == nullptr)
        {
            m_root = new Node{word, true};
#ifdef _DEBUG
            std::cout << "Create first node" << std::endl;
#endif
        }
        else
        {
            m_root->insert(word);
        }
    }

    void toTree(Node *node = nullptr, int depth = 0, bool quotes = false)
    {
        if (node == nullptr)
            node = m_root;

        //if user try to print empty tree
        if (node == nullptr)
            return;

        std::string space;
        for (int i = 0; i < depth; ++i)
        {
            space += " ";
        }

        std::cout << space << (quotes ? "\"" : "") << node->label << (quotes ? "\"" : "") << (node->isEnd ? "$" : "") << std::endl;

        depth++;
        for ( Node *child : node->childs )
            toTree(child, depth);
    }

    void toList(Node *node = nullptr, std::string parentLabel = std::string())
    {
        if (node == nullptr)
            node = m_root;

        //if user try to print empty tree
        if (node == nullptr)
            return;

        if (node->isEnd)
            std::cout << (parentLabel + node->label) << " " << (parentLabel + node->label == parentLabel ? parentLabel + node->label : parentLabel + node->label[0] ) << std::endl;

        for ( Node *child : node->childs )
            toList(child, parentLabel + node->label);
    }
};

//$ nickname < testdata.txt
int main(int, char *[])
{
    RadixTree tree;

#ifdef _DEBUG
    std::ifstream file;
    file.open("testdata.txt");
    for(std::string line; std::getline(file, line);)
#else
    for(std::string line; std::getline(std::cin, line);)
#endif
    {
        tree.insert(line);
    }

    tree.toList();
    tree.toTree();

    return 0;
}
