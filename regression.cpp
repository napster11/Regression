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
ofstream logging;
long double sse(vector<long double> y, vector<long double> xw, vector<long double> w, long double lambda) {
    int n = y.size();
    long double result = 0;
    long double wValue = 0;
    for(int i = 0;i < n;i++) {
        result += pow((y[i]-xw[i]),2);
    }
    int sz = w.size();
    for(int i = 0;i < sz;i++) {
        wValue += w[i]*w[i];
    }
    return (result + lambda*wValue); 
}
vector<long double> newY(vector<pair<long double,vector<long double> > > x, vector<long double> w) {
    vector<long double> result;

    int n = x.size();
    for(int i = 0;i < n;i++) {
        vector<long double> sec = x[i].second;
        int m = sec.size();
        long double temp = 0.0;
        for(int j = 0;j < m;j++) {
            temp += (sec[j]*w[j]);
        }
        result.push_back(temp);
    }
    return result;
}

vector<long double> PartialMultiplication(vector<pair<long double,vector<long double> > > x, vector<long double> w, int ind) {
    vector<long double> result;
    int n = x.size();
    for(int i = 0;i < n;i++) {
        vector<long double> sec = x[i].second;
        int m = sec.size();
        long double temp = 0.0;
        for(int j = 0;j < m;j++) {
            if(j != ind) {
                temp += (sec[j]*w[j]);
            }
        }
        result.push_back(temp);
    }
    return result;
}
long double Multiply(vector<long double> t, vector<long double> m) {
    int n = t.size();
    long double result = 0.0;

    for(int i = 0;i < n;i++) {
        result += t[i]*m[i];
    }
    return result;
}

int ValidateRidge(vector<pair<long double,vector<long double> > > train,vector<pair<long double,vector<long double> > > valid,long double lambdaValues[]) {

    map<long double,vector<long double> > mp;
    int n = train.size();
    int m = train[0].second.size();
    for(int i = 0;i < m;i++) {
        vector<long double> temp;
        for(int j = 0;j < n;j++) {
            temp.push_back(train[j].second[i]);
        }
        mp.insert(pair<long double,vector<long double> >(i,temp));
    }

    vector<pair<long double,vector<pair<long double, vector<long double> > > > > lw;
    
    for(int l = 0;l < 7;l++) {
        logging << "lambda " << l << endl;
        vector<pair<long double, vector<long double> > > ws;
        for(long double i = 0; i < 10;i++) {
            vector<long double> y;
            vector<long double> w(m,0.0);
            long double lambda = lambdaValues[l];
            int sz = train.size();
            for(int j = 0;j < sz;j++) {
                if(train[j].first == i) {
                    y.push_back(1);
                }else {
                    y.push_back(-1);
                }
            }
            vector<long double> XW = newY(train, w);
            long double objectiveVal = sse(y,XW,w,lambda);
            int xx = 0;
            
            while(1) {
                int m = w.size();
                for(int i = 0;i < m;i++) {
                    vector<long double> partialV = PartialMultiplication(train,w,i);
                    vector<long double> yDiff;
                    int n = partialV.size();
                    for(int j = 0;j < n;j++) {
                        yDiff.push_back(y[j]-partialV[j]);
                    }
                    long double numerator = Multiply(yDiff,mp[i]);
                    long double denominator = Multiply(mp[i],mp[i]) + lambda;
                    long double nW = numerator/denominator;
                    w[i] = nW;
                }    
                vector<long double> XW1 = newY(train, w);
                long double newObjectiveVal = sse(y,XW1, w,lambda);
                if(((objectiveVal - newObjectiveVal)/objectiveVal) <= 0.01 ) {
                    break;
                }else {
                    objectiveVal = newObjectiveVal;
                }
            }
            ws.push_back(make_pair(i,w));
        }
        lw.push_back(make_pair(lambdaValues[l] ,ws));
    }
    int lz = lw.size(); 
    for(int i = 0;i < lz;i++) {
        vector<pair<long double, vector<long double> > > wValues = lw[i].second;
        int wl = wValues.size();
        for(int j = 0;j < wl;j++) {
            int kl = wValues[j].second.size();  
        }
    }
    int validSize = valid.size();
    int fl = 0;
    int index = 0;
    long double mxAccuracy;
    for(int i = 0;i < lz;i++) {
        long double lambda = lw[i].first;
        vector<pair<long double, vector<long double> > > wValues = lw[i].second;
        int wl = wValues.size(); 
        long double pos = 0;
        for(int j = 0;j < validSize;j++) {
            long double acc;
            int flag = 0;
            double ind = 0;
            for(int k = 0;k < wl;k++) {
                long double tem =  Multiply(valid[j].second, wValues[k].second);
                if(flag == 0) {
                    acc = tem;
                    flag = 1;
                }else {
                    if(tem > acc) {
                        acc = tem;
                        ind = k;
                    }
                }
            }
            if(valid[j].first == wValues[ind].first) {
                pos++;
            }
        }
        long double accuracy = pos/validSize;
        if(fl == 0) {
            mxAccuracy = accuracy;
            fl = 1;
        }else {
            if(accuracy > mxAccuracy) {
                mxAccuracy = accuracy;
                index = i;
            }else if(accuracy == mxAccuracy) {
                if(i > index) {
                    index = i;
                }
            } 
        }
    }
    return index;
}

void TestRidge(vector<pair<long double,vector<long double> > > train,vector<pair<long double,vector<long double> > > test,long double lambdaValues[], string optionsFileName,string outPutFileName) {
    
    map<long double,vector<long double> > mp;
    int n = train.size();
    int m = train[0].second.size();
    for(int i = 0;i < m;i++) {
        vector<long double> temp;
        for(int j = 0;j < n;j++) {
            temp.push_back(train[j].second[i]);
        }
        mp.insert(pair<long double,vector<long double> >(i,temp));
    }

    vector<pair<long double,vector<pair<long double, vector<long double> > > > > lw;
    
    for(int l = 0;l < 2;l++) {
        logging << "lambda " << l << endl;
        vector<pair<long double, vector<long double> > > ws;
        for(long double i = 0; i < 10;i++) {
            vector<long double> y;
            vector<long double> w(m,0.0);
            long double lambda = lambdaValues[l];
            int sz = train.size();
            for(int j = 0;j < sz;j++) {
                if(train[j].first == i) {
                    y.push_back(1);
                }else {
                    y.push_back(-1);
                }
            }
            vector<long double> XW = newY(train, w);
            long double objectiveVal = sse(y,XW,w,lambda);
            int xx = 0;
            
            while(1) {
                int m = w.size();
                for(int i = 0;i < m;i++) {
                    vector<long double> partialV = PartialMultiplication(train,w,i);
                    vector<long double> yDiff;
                    int n = partialV.size();
                    for(int j = 0;j < n;j++) {
                        yDiff.push_back(y[j]-partialV[j]);
                    }
                    long double numerator = Multiply(yDiff,mp[i]);
                    long double denominator = Multiply(mp[i],mp[i]) + lambda;
                    long double nW = numerator/denominator;
                    w[i] = nW;
                }    
                vector<long double> XW1 = newY(train, w);
                long double newObjectiveVal = sse(y,XW1, w,lambda);
                if(((objectiveVal - newObjectiveVal)/objectiveVal) <= 0.01 ) {
                    break;
                }else {
                    objectiveVal = newObjectiveVal;
                }
            }
            ws.push_back(make_pair(i,w));
        }
        lw.push_back(make_pair(lambdaValues[l] ,ws));
    }
    int lz = lw.size(); 
    for(int i = 0;i < lz;i++) {
        vector<pair<long double, vector<long double> > > wValues = lw[i].second;
        int wl = wValues.size();
        for(int j = 0;j < wl;j++) {
            int kl = wValues[j].second.size();
        }
    }
    int testSize = test.size();
    int fl = 0;
    int index = 0;
    long double mxAccuracy;
    vector<vector<long double> > predictedClass;
    for(int i = 0;i < lz;i++) {
        long double lambda = lw[i].first;
        vector<long double> values;
        vector<pair<long double, vector<long double> > > wValues = lw[i].second;
        int wl = wValues.size(); 
        long double pos = 0;
        for(int j = 0;j < testSize;j++) {
            long double acc;
            int flag = 0;
            double ind = 0;
            for(int k = 0;k < wl;k++) {
                long double tem =  Multiply(test[j].second, wValues[k].second);
                if(flag == 0) {
                    acc = tem;
                    flag = 1;
                }else {
                    if(tem > acc) {
                        acc = tem;
                        ind = k;
                    }
                }
            }
            values.push_back(wValues[ind].first);
            if(test[j].first == wValues[ind].first) {
                pos++;
            }
        }
        predictedClass.push_back(values);
        long double accuracy = pos/testSize;
        if(fl == 0) {
            mxAccuracy = accuracy;
            fl = 1;
        }else {
            if(accuracy > mxAccuracy) {
                mxAccuracy = accuracy;
                index = i;
            }else if(accuracy == mxAccuracy) {
                if(i > index) {
                    index = i;
                }
            } 
        }
    }
    cout  << "ACCURACY: " << mxAccuracy << endl;
    vector<pair<long double, vector<long double> > > FinalWValues = lw[index].second;
    int wsz = FinalWValues.size();

    ofstream out(outPutFileName);
    ofstream wout(optionsFileName);

    for(int i = 0;i < wsz;i++) {
        vector<long double> ws = FinalWValues[i].second;
        int wsize = ws.size();
        for(int j = 0;j < wsize;j++) {
            if(j < wsize-1) {
                wout << ws[j] << ",";
            }else {
                wout << ws[j] << endl;
            }
        }
    }
    int classSize = predictedClass[index].size();
    for(int i = 0;i < classSize;i++) {
        out << predictedClass[index][i] << endl;
    }
}

int main(int argc, char* argv[]) {
    if(argc < 5) {
        cout << "Insufficient Arguments \n";
        return 0;
    }
    string trainingFileName = argv[1];
    string validationFileName = argv[2];
    string testFileName = argv[3];
    string outputFileName = argv[4];
    string optionsFileName = "";
    if(argc > 5) {
        optionsFileName = argv[5];
    }

    ifstream training(trainingFileName);
    ifstream validation(validationFileName);
    ifstream testing(testFileName);
    string loggingFileName = "log_" + outputFileName;
    logging.open(loggingFileName);
    out.open(outputFileName);
    
    vector<pair<long double,vector<long double> > > train;
    vector<pair<long double,vector<long double> > > valid;
    vector<pair<long double,vector<long double> > > test;

    string token;

    while(getline(training,token)) {
        istringstream ss(token);
        string temp;
        int flag = 1;
        long double classLabel;
        vector<long double> v;
        size_t sz;
        long double euc = 0.0;
        while(getline(ss,temp,',')) {
            if(flag == 1) {
                classLabel = stod(temp, &sz);
                flag = 0;
            }else {
                v.push_back(stod(temp,&sz));
                euc += (stod(temp)*stod(temp));
            }
        }
        int n = v.size();
        for(int i = 0;i < n;i++) {
            v[i] /= sqrt(euc);
        }

        train.push_back(make_pair(classLabel,v));
    }

    while(getline(validation,token)) {
        istringstream ss(token);
        string temp;
        int flag = 1;
        long double classLabel;
        vector<long double> v;
        size_t sz;
        long double euc = 0.0;
        while(getline(ss,temp,',')) {
            if(flag == 1) {
                classLabel = stod(temp, &sz);
                flag = 0;
            }else {
                v.push_back(stod(temp,&sz));
                euc += (stod(temp)*stod(temp));
            }
        }
        int n = v.size();
        for(int i = 0;i < n;i++) {
            v[i] /= sqrt(euc);
        }
        valid.push_back(make_pair(classLabel,v));
    }

    while(getline(testing,token)) {
        istringstream ss(token);
        string temp;
        int flag = 1;
        long double classLabel;
        vector<long double> v;
        size_t sz;
        long double euc = 0.0;
        while(getline(ss,temp,',')) {
            if(flag == 1) {
                classLabel = stod(temp, &sz);
                flag = 0;
            }else {
                v.push_back(stod(temp,&sz));
                euc += (stod(temp)*stod(temp));
            }
        }
        int n = v.size();
        for(int i = 0;i < n;i++) {
            v[i] /= sqrt(euc);
        }
        test.push_back(make_pair(classLabel,v));
    }

    int m = valid.size();
    int n = train.size();
    int t = test.size();

    time_t st,en;
    st = clock();
    long double lambdaValues[7] = {0.01,0.05,0.1,0.5,1.0,2.0,5.0};
    int l = ValidateRidge(train, valid,lambdaValues);
    logging << " l " << l << endl;
    long double FinalLambda = lambdaValues[l];
    long double testLambdaValues[2] = {FinalLambda, 2*FinalLambda};
    train.insert(train.end(), valid.begin(),valid.end());
    TestRidge(train,test,testLambdaValues,optionsFileName, outputFileName );
    return 0;

}
