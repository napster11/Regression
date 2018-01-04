#include<iostream>
#include<map>
#include<vector>
#include<sstream>
#include<fstream>
#include<cstring>
#include<cmath>
#include<algorithm>
#include<cstdlib>
using namespace std;

ofstream out;

bool comp(pair<double,double> a, pair<double,double> b) {
    return (a.second < b.second);
}
double dist(vector<double> x, vector<double> cent) {

    double dis = 0.0;

    int n = min(x.size(),cent.size());
    for(int i = 0;i < n;i++) {
        dis += pow((x[i]-cent[i]),2);
    }
    return sqrt(dis);
}

int testValidationSet(vector<pair<double,vector<double> > > valid, map<double, vector<pair<double,double> > > dis) {

    int index = 1;
    double perf;
    map<double,vector<pair<double,double> > >::iterator vit;
    for(int i = 1;i <= 20;i++) {
        double truePositive = 0;
        double trueNegative = 0;
        double accuracy;
        for(vit = dis.begin();vit != dis.end();vit++) {
            vector<pair<double,double> > v = (*vit).second;
            map<double,int> mm;
            double mx = 0;
            double ind;
            for(int j = 0;j < i;j++) {
                mm[v[j].first]++;
                if(mm[v[j].first] > mx) {
                    mx = mm[v[j].first];
                    ind = v[j].first;
                }
            }
            if(ind == valid[(*vit).first].first) {
                truePositive++;
            }else {
                trueNegative++;
            }
        }    
        accuracy = (truePositive)/(truePositive+trueNegative);
        if(i == 1) {
            perf = accuracy;
        }else if(accuracy > perf) {
            perf = accuracy;
            index = i;
        }    
    }
    return index;
}

double testSet(vector<pair<double,vector<double> > > test, int k, map<double, vector<pair<double,double> > > trainDis, string outputFileName) {
    
    ofstream out(outputFileName);
    double perf;
    map<double,vector<pair<double,double> > >::iterator tt;
    double truePositive = 0;
    double trueNegative = 0;
    double accuracy;
    int i = 1;
    for(tt = trainDis.begin();tt != trainDis.end();tt++) {
        vector<pair<double,double> > v = (*tt).second;
        map<double,int> mm;
        double mx = 0;
        double ind;
        for(int j = 0;j < k;j++) {
            mm[v[j].first]++;
            if(mm[v[j].first] > mx) {
                mx = mm[v[j].first];
                ind = v[j].first;
            }
        }
        out << ind << endl;
        if(ind == test[(*tt).first].first) {
            truePositive++;
        }else {
            trueNegative++;
        }
        i++;
    }    
    accuracy = (truePositive)/(truePositive+trueNegative);
    return accuracy;
}

int main(int argc, char* argv[]) {
    string trainingFileName = argv[1];
    string validationFileName = argv[2];
    string testFileName = argv[3];
    string outputFileName = argv[4];

    ifstream training(trainingFileName);
    ifstream validation(validationFileName);
    ifstream testing(testFileName);
    out.open(outputFileName);
    
    vector<pair<double,vector<double> > > train;
    vector<pair<double,vector<double> > > valid;
    vector<pair<double,vector<double> > > test;

    string token;

    while(getline(training,token)) {
        istringstream ss(token);
        string temp;
        int flag = 1;
        double classLabel;
        vector<double> v;
        size_t sz;
        while(getline(ss,temp,',')) {
            if(flag == 1) {
                classLabel = stod(temp, &sz);
                flag = 0;
            }else {
                v.push_back(stod(temp,&sz));
            }
        }
        train.push_back(make_pair(classLabel,v));
    }

    while(getline(validation,token)) {
        istringstream ss(token);
        string temp;
        int flag = 1;
        double classLabel;
        vector<double> v;
        size_t sz;
        while(getline(ss,temp,',')) {
            if(flag == 1) {
                classLabel = stod(temp, &sz);
                flag = 0;
            }else {
                v.push_back(stod(temp,&sz));
            }
        }
        valid.push_back(make_pair(classLabel,v));
    }

    while(getline(testing,token)) {
        istringstream ss(token);
        string temp;
        int flag = 1;
        double classLabel;
        vector<double> v;
        size_t sz;
        while(getline(ss,temp,',')) {
            if(flag == 1) {
                classLabel = stod(temp, &sz);
                flag = 0;
            }else {
                v.push_back(stod(temp,&sz));
            }
        }
        test.push_back(make_pair(classLabel,v));
    }

    int m = valid.size();
    int n = train.size();
    int t = test.size();
    map<double, vector<pair<double,double> > > dis;
    for(int i = 0;i < m;i++) {
        for(int j = 0;j < n;j++) {    
            double distance = dist(valid[i].second, train[j].second);
            dis[i].push_back(make_pair(train[j].first,distance));    
        }
    }

    //Sort the distance matrix in increasing order of distance
    map<double, vector<pair<double,double> > >::iterator it;
    for(it = dis.begin();it != dis.end();it++) {
        vector<pair<double,double> > pairWiseDist = (*it).second;
        sort(pairWiseDist.begin(), pairWiseDist.end(), comp);
        (*it).second = pairWiseDist;
    }
    int k = testValidationSet(valid,dis);

    //Merge training and validation set
    train.insert(train.end(),valid.begin(),valid.end());
    int trainLen = train.size();
    int testLen = test.size();

    //Compute Distance matrix of test set w.r.t merged training set
    map<double, vector<pair<double,double> > > trainDis;
    for(int i = 0;i < testLen;i++) {
        for(int j = 0;j < trainLen;j++) {
            double distance = dist(test[i].second, train[j].second);
            trainDis[i].push_back(make_pair(train[j].first,distance));
        }
    }

    //Sort the distance matrix
    map<double, vector<pair<double,double> > >::iterator trainIt;
    for(trainIt = trainDis.begin();trainIt != trainDis.end();trainIt++) {
        vector<pair<double,double> > pairWiseDist = (*trainIt).second;
        sort(pairWiseDist.begin(), pairWiseDist.end(), comp);
        (*trainIt).second = pairWiseDist;
    }
    double acc = testSet(test,k,trainDis, outputFileName);
    cout << "ACCURACY: " << acc << endl;
    return 0;

}