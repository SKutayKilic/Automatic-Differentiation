#include <vector>
#include <string>
#include <unordered_map>
#include "graph.h"
#include "variable.h"
#include <cmath>
#include "function.h"
#include <queue>

using namespace std;

// helper function that checks whether the given string is number or not.
bool isNumber(const string& s)
{
    char* end = 0;
    double val = strtod(s.c_str(), &end);
    return end != s.c_str() && val != HUGE_VAL;
}

int Graph::getVariable(string inp){
    int res;
    if( isNumber(inp) ){

        double val = stod(inp.c_str());
        idCount++;
        id[inp] = idCount;
        name[idCount] = inp;
        vars[idCount] = new Variable(idCount, inp, val);
        res = idCount;
        type[idCount] = VARIABLE;

    }
    else{
        if(id.find(inp)==id.end()){

            idCount++;
            id[inp] = idCount;
            name[idCount] = inp;
            vars[idCount] = new Variable(idCount, inp, 0);
            res = idCount;
            type[idCount] = VARIABLE;
        }
        else{
            res = id[inp];
        }
    }
    // printf("%s -> %d\n",inp.c_str(), res);
    return res;
}

int Graph::getFunction(string fnc){
    idCount++;

    name[idCount] = fnc;
    type[idCount] = FUNCTION;
    Function *f;
    if(fnc.compare("mult")==0)
        f = new Multiplication(idCount, fnc);
    else if(fnc.compare("add")==0)
        f = new Addition(idCount, fnc);
    else if(fnc.compare("subs")==0)
        f = new Subtraction(idCount, fnc);
    else if(fnc.compare("divide")==0)
        f = new Division(idCount, fnc);

    else if(fnc.compare("sin")==0)
        f = new Sine(idCount, fnc);
    else if(fnc.compare("cos")==0)
        f = new Cosine(idCount, fnc);
    else if(fnc.compare("identity")==0)
        f = new Identity(idCount, fnc);
    else if(fnc.compare("tan")==0)
        f = new Tangent(idCount, fnc);
    else if(fnc.compare("acos")==0)
        f = new ArcCosine(idCount, fnc);
    else if(fnc.compare("asin")==0)
        f = new ArcSine(idCount, fnc);
    else if(fnc.compare("atan")==0)
        f = new ArcTangent(idCount, fnc);
    else if(fnc.compare("exp")==0)
        f = new Exponential(idCount, fnc);
    else if(fnc.compare("log")==0)
        f = new Log(idCount, fnc);
    else if(fnc.compare("log10")==0)
        f = new Log10(idCount, fnc);
    else if(fnc.compare("pow")==0)
        f = new Power(idCount, fnc);
    else if(fnc.compare("sqrt")==0)
        f = new Sqrt(idCount, fnc);
    fncs[idCount] = f;
    return idCount;
}

Graph::~Graph() {}

void Graph::addUnaryFunction(string fnc, string inp, string out){

    int fId = getFunction(fnc);
    int inpId = getVariable(inp);
    int outId = getVariable(out);
    fncs[fId]->addInput(vars[inpId]);
    fncs[fId]->setOutput(vars[outId]);

    vars[inpId]->addTo(fncs[fId]);
    vars[outId]->setFrom(fncs[fId]);
}


void Graph::addBinaryFunction(string fnc, string inp1, string inp2, string out){
    int fId=getFunction(fnc);
    int inpId1=getVariable(inp1);
    int inpId2=getVariable(inp2);
    int outId=getVariable(out);

        fncs[fId]->addInput(vars[inpId1]);
        fncs[fId]->addInput(vars[inpId2]);
        fncs[fId]->setOutput(vars[outId]);

        vars[inpId1]->addTo(fncs[fId]);
        vars[inpId2]->addTo(fncs[fId]);
        vars[outId]->setFrom(fncs[fId]);


}



void Graph::addAssignment(string lvalue, string rvalue){
    addUnaryFunction("identity", rvalue, lvalue);
}

template <class Container>
void split(const string& str, Container& cont) //this splits the lines from text file
{
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(cont));
}

void Graph::readGraph(string fileName){
    ifstream infile(fileName);
    string line;
    vector<string> datas;
    getline(infile, line);
    split(line,datas);
    while(datas[0].compare("input")==0){
        inputNodes.push_back(getVariable(datas[1]));
        datas.clear();
        getline(infile, line);
        split(line,datas);
    }
    outputNode=getVariable(datas[1]);
    datas.clear();
    while(getline(infile, line)){
        split(line,datas);
        if(datas[2].compare("mult")==0 || datas[2].compare("add")==0 || datas[2].compare("subs")==0 ||
                datas[2].compare("divide")==0 || datas[2].compare("pow")==0 )
            addBinaryFunction(datas[2], datas[3], datas[4], datas[0]);
        else if(datas[2].compare("cos")==0 || datas[2].compare("sin")==0 || datas[2].compare("tan")==0 ||
                datas[2].compare("asin")==0 || datas[2].compare("acos")==0 ||
                datas[2].compare("atan")==0 || datas[2].compare("exp")==0 || datas[2].compare("log")==0 ||
                datas[2].compare("sqrt")==0 || datas[2].compare("identity")==0)
            addUnaryFunction(datas[2], datas[3], datas[0]);
        else
            addUnaryFunction("identity", datas[2], datas[0]);
        datas.clear();

    }
}


bool Graph::isCyclic(){
    if(vars[outputNode]->to.size()!=0) return true;
    vector<int> indegrees(idCount+1, 0);
    for(int i=1; i<=idCount; i++){
        if(type[i]==FUNCTION){
            indegrees[i]=fncs[i]->inputs.size();
        }
        else{
            if(vars[i]->from==nullptr)indegrees[i]=0;
            else indegrees[i]=1;
        }
    }

    queue<int> sources;
    for(int i=1; i<=idCount; i++){
        if(type[i]==VARIABLE){
            if(vars[i]->from== nullptr)sources.push(i);
        }
    }
    int counter=0;
    while(!sources.empty()){
        int sourceId=sources.front();
        sources.pop();
        counter++;
        if(type[sourceId]==FUNCTION){
            fncs[sourceId]->doForward();
            indegrees[fncs[sourceId]->output->id]=0;
            sources.push(fncs[sourceId]->output->id);
        }
        else{
            for(int i=0; i<vars[sourceId]->to.size(); i++){
                indegrees[vars[sourceId]->to[i]->id]--;
            }
            int firstId=-1;
            for(int i=0; i<vars[sourceId]->to.size(); i++){
                if(indegrees[vars[sourceId]->to[i]->id]==0 && vars[sourceId]->to[i]->id!=firstId){
                    sources.push(vars[sourceId]->to[i]->id);
                    firstId=vars[sourceId]->to[i]->id;
                }
            }

        }
        if(counter>idCount) return true;
    }
    return counter!=idCount;
    //ERROR: COMPUTATION GRAPH HAS CYCLE!
}

double Graph::forwardPass(vector<double> inputValues , ofstream& myfile) {
    cout.precision(20);
    vector<int> indegrees(idCount+1, 0);
    for(int i=1; i<=idCount; i++){
        if(type[i]==FUNCTION){
            indegrees[i]=fncs[i]->inputs.size();
        }
        else{
            if(vars[i]->from==nullptr)indegrees[i]=0;
            else indegrees[i]=1;
        }
    }

    for(int i=0; i<inputValues.size(); i++){
        int inputId=inputNodes[i];
        vars[inputId]->value=inputValues[i];
    }
    queue<int> sources;
    for(int i=1; i<=idCount; i++){
        if(type[i]==VARIABLE){
            if(vars[i]->from== nullptr)sources.push(i);
        }
    }
    while(!sources.empty()){
        int sourceId=sources.front();
        sources.pop();
        if(type[sourceId]==FUNCTION){
            fncs[sourceId]->doForward();
            indegrees[fncs[sourceId]->output->id]=0;
            sources.push(fncs[sourceId]->output->id);
        }
        else{
            for(int i=0; i<vars[sourceId]->to.size(); i++){
                indegrees[vars[sourceId]->to[i]->id]--;
            }
            int firstId=-1;
            for(int i=0; i<vars[sourceId]->to.size(); i++){
                if(indegrees[vars[sourceId]->to[i]->id]==0 && vars[sourceId]->to[i]->id!=firstId){
                    sources.push(vars[sourceId]->to[i]->id);
                    firstId=vars[sourceId]->to[i]->id;
                }
            }

        }
    }
    myfile<<vars[outputNode]->value<<endl;

}

void Graph::backwardPass(ofstream& myfile){

    //cout.precision(20);
    vector<int> outdegrees(idCount+1, 0);
    for(int i=1; i<=idCount; i++){
        if(type[i]==FUNCTION){
            outdegrees[i]=1;
        }
        else{
            if(i!=outputNode)outdegrees[i]=vars[i]->to.size();
            vars[i]->derivative=0; //to reset derivative values
        }
    }
    vars[outputNode]->derivative=1;
    queue<int> sources;
    sources.push(outputNode);
    while(!sources.empty()){
        int sourceId=sources.front();
        sources.pop();
        if(type[sourceId]==FUNCTION){
            fncs[sourceId]->doBackward();
            for(int i=0; i<fncs[sourceId]->inputs.size(); i++){
                outdegrees[fncs[sourceId]->inputs[i]->id]--;
                //cout<<"outdegree of "<<fncs[sourceId]->inputs[i]->name<<" is"<<outdegrees[fncs[sourceId]->inputs[i]->id]<<endl;
            }
            int firstId=-1;
            for(int i=0; i<fncs[sourceId]->inputs.size(); i++){
                if(outdegrees[fncs[sourceId]->inputs[i]->id]==0 && fncs[sourceId]->inputs[i]->id!=firstId){
                    sources.push(fncs[sourceId]->inputs[i]->id);
                    firstId=fncs[sourceId]->inputs[i]->id;
                }
            }
        }
        else if(vars[sourceId]->from!= nullptr){
            outdegrees[vars[sourceId]->from->id]--;
            sources.push(vars[sourceId]->from->id);
        }
    }
    for(int i=0; i<inputNodes.size(); i++){
        myfile<<vars[inputNodes[i]]->derivative<<" ";
    }
    myfile<<endl;
}


int Graph::getNumOfInputs() {
    return inputNodes.size();
}