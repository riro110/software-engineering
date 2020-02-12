// RegularExpressions.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
//正規表現のパターンマッチを行うクラス

#include <iostream>
#include <set>
using namespace std;

//<式>の抽象クラス
class RegularExpression{
public:
	set<string> abc;
	virtual void printMember(){};
	virtual set<string> match(set<string> inputState)=0;
};

//<選択式>に対応するクラス
class AlternationExpression : public RegularExpression{
public:
	RegularExpression *alt1, *alt2;
	AlternationExpression(RegularExpression *a1, RegularExpression *a2){
		alt1 = a1, alt2 = a2;
	}

	void printMember(){
		cout << "(";
		alt1->printMember();
		cout << "^";
		alt2->printMember();
		cout << ")";
	}

	set<string> match(set<string> inputState);

};

set<string> AlternationExpression::match(set<string> inputState){

	set<string> state1 = alt1->match(inputState);
	set<string> state2 = alt2->match(inputState);

	for(set<string>::iterator itr = state2.begin(); itr != state2.end(); itr++){
		state1.insert(*itr);
	}

	return state1;
}

//<連接式>に対応するクラス
class SequenceExpression : public RegularExpression{
public:
	RegularExpression *seq1, *seq2;
	SequenceExpression(RegularExpression *s1, RegularExpression *s2){
		seq1 = s1, seq2 = s2;
	}

	void printMember(){
		cout << "(";
		seq1->printMember();
		cout << "&";
		seq2->printMember();
		cout << ")";
	}

	set<string> match(set<string> inputState);

};

set<string> SequenceExpression::match(set<string> inputState){
	return seq2->match(seq1->match(inputState));
}

//<反復式>に対応するクラス
class RepetitionExpression : public RegularExpression{
public:
	RegularExpression *rep;
	RepetitionExpression(RegularExpression *r){
		rep = r;
	}
	void printMember(){
		cout << "(";
		rep->printMember();
		cout << "*";
		cout << ")";
	}
	set<string> match(set<string> inputState);
};

set<string> RepetitionExpression::match(set<string> inputState){

	set<string> aState, finalState;
	aState = inputState;
	finalState = inputState;

	while(aState.size() != 0){
		aState = rep->match(aState);
		for(set<string>::iterator itr = aState.begin(); itr != aState.end(); itr++){
			finalState.insert(*itr);
		}
	}

	return finalState;
}

//<リテラル式>に対応するクラス
class LiteralExpression : public RegularExpression{
public:
	string lit;
	LiteralExpression(string l){
		lit = l;
	}

	void printMember(){
		cout << lit;
	}

	set<string> match(set<string> inputState);
};

set<string> LiteralExpression::match(set<string> inputState){

	int len = lit.size();
	set<string> finalState;

	for(set<string>::iterator itr = inputState.begin(); itr != inputState.end(); itr++){
		if(len <= (*itr).size() && (*itr).substr(0, len) == lit)
			finalState.insert((*itr).substr(len, ((*itr).size() - len)));
	}

	return finalState;
}

int main(){
	SequenceExpression *expr = new SequenceExpression(
		new RepetitionExpression(
			new AlternationExpression(
				new LiteralExpression("dog"),
				new LiteralExpression("cat")
			)
		),
		new LiteralExpression("weather")
	);
	/*
	set<string> s1 = {"dogdogcatweather"},
		s2 = {"weather"},
		s3 = {"pigweather"},
		s4 = {"catweathercat"};

	set<string> r1 = expr->match(s1),
		r2 = expr->match(s2),
		r3 = expr->match(s3),
		r4 = expr->match(s4);

	printf("%d\n", r1.size());
	printf("%d\n", r2.size());
	printf("%d\n", r3.size());
	printf("%d\n", r4.size());
	*/
	expr->printMember();
	cout << endl;
	string buf;
	set<string> searchString;
	cout << "捜査対象の文字列を入力: ";
	cin >> buf;

	searchString.insert(buf);
	set<string> result = expr->match(searchString);
	if(result.size() != 0 && *(result.begin()) == "")
		cout << "〇" << endl;
	else
		cout << "×" << endl;
}
