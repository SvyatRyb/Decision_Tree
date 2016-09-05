#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

struct Object {
    std::vector<double> attributes;
    unsigned int classification;
};

class Node {
private:
    Node* left = NULL;
    Node* right = NULL;
    double condition;
    unsigned int attr_cond;
    unsigned int pred_class;
public:
    Node() {}
    ~Node() {
        delete left;
        delete right;
    }
    std::vector<Object> objects;
    void learn() {
        double gini = 0;
//counting gini for this node
        std::map<unsigned int, unsigned int> num_class;
        for (size_t i = 0; i < objects.size(); ++i) {
            num_class[objects[i].classification] += 1;
        }
        pred_class = num_class.begin()->first;
        for (auto it = num_class.begin(); it != num_class.end(); ++it) {
            double prob = static_cast<double>(it->second) / objects.size();
            gini += prob * (1 - prob);
            if (it->second > num_class[pred_class]) {
                pred_class = it->first;
            }
        }

        if (gini < 0.2){
            return;
        }
        double best_cond;
        unsigned int best_cond_attr;
        double best_dgini = 0;
//looking for the best attribute
        for (size_t i = 0; i < objects[0].attributes.size(); ++i) {
            std::map<unsigned int, unsigned int> right_classes = num_class;
            std::map<unsigned int, unsigned int> left_classes;
            sort(objects.begin(), objects.end(),
                 [i](Object first, Object second) {
                return (first.attributes[i] <= second.attributes[i]);
            });
            for (size_t j = 0; j < objects.size() - 1; ++j) {
                left_classes[objects[j].classification] += 1;
                right_classes[objects[j].classification] -= 1;
                if (objects[j].classification == objects[j + 1].classification) {
                    continue;
                }
                double gini_left, gini_right;
                for (auto it = left_classes.begin(); it != left_classes.end(); ++it) {
                    double prob = static_cast<double>(it->second) / (j + 1);
                    gini_left += prob * (1 - prob);
                }
                for (auto it = right_classes.begin(); it != right_classes.end(); ++it) {
                    double prob = static_cast<double>(it->second) / (objects.size() - j - 1);
                    gini_right += prob * (1 - prob);
                }
                double dgini = gini - (static_cast<double>(j + 1) / objects.size()) * gini_left
                                    - (static_cast<double>(objects.size() - j - 1) / objects.size()) * gini_right;
                if (dgini > best_dgini) {
                    best_dgini = dgini;
                    best_cond_attr = i;
                    best_cond = objects[j].attributes[i];
                }
            }
        }

        if (best_dgini > 0) {
            condition = best_cond;
            attr_cond = best_cond_attr;
        }
        left = new Node;
        right = new Node;
        for (size_t i = 0; i < objects.size(); ++i) {
            if (objects[i].attributes[attr_cond] <= condition) {
                left->objects.push_back(objects[i]);
            } else {
                right->objects.push_back(objects[i]);
            }
        }
        left->learn();
        right->learn();
    }
    unsigned int give_answer (Object object_n) {
        if (left == NULL) {
            return pred_class;
        }
        if (object_n.attributes[attr_cond] <= condition) {
            return left->give_answer(object_n);
        }
        return right->give_answer(object_n);
    }
};

int main()
{
    std::ifstream fin ("input.txt");
    std::ofstream fout ("output.txt");
    size_t num, num_obj;
    Node root;
    fin >> num >> num_obj;
    for(size_t i = 0; i < num; ++i) {
        Object input;
        for (size_t j = 0; j < num_obj; ++j) {
            double inp;
            fin >> inp;
            input.attributes.push_back(inp);
        }
        fin >> input.classification;
        root.objects.push_back(input);
    }
    root.learn();
    fin >> num;
    for(size_t i = 0; i < num; ++i) {
        Object input;
        for (size_t j = 0; j < num_obj; ++j) {
            double inp;
            fin >> inp;
            input.attributes.push_back(inp);
        }
        fout << (root.give_answer(input)) << "\n";
    }
    fin.close();
    fout.close();
    return 0;
}
