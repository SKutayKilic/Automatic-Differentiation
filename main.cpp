#include <iostream>
#include "graph.h"

template <class Container>
void split(const string& str, Container& cont) //this splits the lines from text file
{
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(cont));
}

int main(int argc, char* argv[]) {
    Graph mygraph;
    mygraph.readGraph(argv[1]);

    ofstream outputs;
    outputs.open (argv[3]);

    ofstream derivatives;
    derivatives.open (argv[4]);

    if(mygraph.isCyclic()){
        outputs<<"ERROR: COMPUTATION GRAPH HAS CYCLE!"<<endl;
        derivatives<<"ERROR: COMPUTATION GRAPH HAS CYCLE!"<<endl;
        return 1;
    } ;

    ifstream valuefile(argv[2]);
    string line;
    getline(valuefile, line);
    vector<string> datas;
    int numOfInput=mygraph.getNumOfInputs();
    vector<double> values(numOfInput);

    string nameOfOutput=mygraph.name[mygraph.outputNode];
    outputs<<nameOfOutput<<endl;
    outputs.precision(numeric_limits<long double>::digits10);
    derivatives.precision(numeric_limits<long double>::digits10);
    for(int i=0; i<mygraph.getNumOfInputs(); i++){
        derivatives<<"d"<<nameOfOutput<<"/d"<<mygraph.name[mygraph.inputNodes[i]]<<" ";
    }
    derivatives<<endl;
    while(getline(valuefile, line)){
        vector<string> datas2;
        split(line,datas2);
        for(int i=0; i<numOfInput; i++){
            values[i]=stold(datas2[i]);
        }

        mygraph.forwardPass(values, outputs);
        mygraph.backwardPass(derivatives);
    }


    return 0;
}