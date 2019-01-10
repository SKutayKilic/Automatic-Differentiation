#include "variable.h"
using namespace std;
Node::~Node() {}

Variable::Variable() {

}

Variable::Variable(int _id, string _name, double _value) {
    value=_value;
    name=_name;
    id=_id;
    derivative=0.0;
    from= nullptr;

}

Variable::~Variable() {
    //delete to;
}

void Variable::setFrom(Function *_from) {
    from=_from;
}

void Variable::addTo(Function *_to) {
    to.push_back(_to);
}

vector<Node *> Variable::getIncomings() {
    vector<Node *> temp;
    temp.push_back((Node*)from);
    return temp;
}

vector<Node *> Variable::getOutgoings() {
    vector<Node *> temp;
    for(int i=0; i<to.size(); i++)
        temp.push_back((Node*)to[i]);
    return temp;

}
