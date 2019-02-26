#include <string>
#include <iostream>
#include <fstream>
#include <set>
#include <memory>

//#define _DEBUG
//#define _DEFDATA


struct RadixTree
{
private:

    struct Node;
    struct NodeComparator {
        bool operator()(std::unique_ptr<Node> const &a, std::unique_ptr<Node> const &b) {
            return a->label < b->label;
        }
    };

    struct Node {
        std::string label;
        bool isEnd;
        std::set<std::unique_ptr<Node>, NodeComparator> childs;

        Node(const std::string &label, int isEnd, std::set<std::unique_ptr<Node>, NodeComparator> childs = std::set<std::unique_ptr<Node>, NodeComparator>())
            : label(label)
            , isEnd(isEnd)
            , childs(std::move(childs))
        {
        }

        ~Node()
        {
#ifdef _DEBUG
            std::cout << "delete " << label << " " << isEnd << std::endl;
#endif
            childs.clear();
        }

        bool insert(std::string &word, int depth = 0)
        {
#ifdef _DEBUG
            std::cout << "Start insert '" << word << "' into node '" << label << "'" << std::endl;
#endif
            std::string equalPart;
            for (int i = 0; i < static_cast<int>(std::min(word.length(), label.length())); ++i)
            {
#ifdef _DEBUG
                std::cout << "  " << word.at(i) << " == " << label.at(i) << " -> " << (word.at(i) == label.at(i)) << std::endl;
#endif
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
                {
#ifdef _DEBUG
                    std::cout << "equalPart is empty: '" << equalPart << "'" << std::endl;
#endif
                    return false;
                }

                auto childNode = std::make_unique<Node>(label.substr(equalPart.length()), isEnd, std::move(childs));
                label = equalPart;
                isEnd = equalPart.length() == word.length();

                //childs are empty now, because we move their memory to created 'childNode'. Right? =)
                childs.clear();
                childs.insert(std::move(childNode));

                if (equalPart.length() == 0)
                    childs.insert(std::make_unique<Node>(word, true));

                if (equalPart.length() > 0 && equalPart.length() != word.length())
                    childs.insert(std::make_unique<Node>(word.substr(equalPart.length()), true));

                return true;
            }

            depth++;
            for ( auto &child : childs )
            {
                std::string subWord = word.substr(equalPart.length());
                bool res = child->insert(subWord, depth);
                if (res)
                    return true;
            }

            childs.insert(std::make_unique<Node>(word.substr(equalPart.length()), true));
            return true;
        }
    };

    std::unique_ptr<Node> m_root;
public:
    RadixTree() = default;

    ~RadixTree()
    { }

    void insert(std::string &word)
    {
        if (m_root == nullptr)
        {
            m_root = std::make_unique<Node>(word, true);
#ifdef _DEBUG
            std::cout << "Create first node " << word << std::endl;
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
            node = m_root.get();

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
        for ( auto &child : node->childs )
            toTree(child.get(), depth);
    }

    void toList(Node *node = nullptr, std::string parentLabel = std::string())
    {
        if (node == nullptr)
            node = m_root.get();

        //if user try to print empty tree
        if (node == nullptr)
            return;

        if (node->isEnd)
            std::cout << (parentLabel + node->label) << " " << (parentLabel + node->label == parentLabel ? parentLabel + node->label : parentLabel + node->label[0] ) << std::endl;

        for ( auto &child : node->childs )
            toList(child.get(), parentLabel + node->label);
    }
};

//$ nickname < testdata.txt
//cyrillic test: echo -e "Данил\nДенис\nДаша" | iconv -t cp1251 | ./nickname | iconv -f cp1251
int main(int, char *[])
{
    RadixTree tree;

#ifdef _DEFDATA
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
