#include <iostream>
#include <set>

//正規表現のパターンマッチを行うクラス

using namespace std;

class AlternationExpression;
class SequenceExpression;
class RepetitionExpression;
class LiteralExpression;
class RegExpVisitor;

//<式>の抽象クラス
class RegularExpression{
public:
	virtual void printMember(){};
	virtual set<string> accept(RegExpVisitor* aVisiter){};
};

//Visitorクラスの抽象クラス
class RegExpVisitor{
public:
	virtual set<string> visitAlternation(AlternationExpression* alternationExp){};
	virtual set<string> visitSequence(SequenceExpression* sequenceExp){};
	virtual set<string> visitRepetition(RepetitionExpression* repetitionExp){};
	virtual set<string> visitLiteral(LiteralExpression* literalExp){};
};


//<選択式>に対応するクラス
class AlternationExpression : public RegularExpression{
public:
	RegularExpression* alt1, * alt2;
	AlternationExpression(RegularExpression* a1, RegularExpression* a2){
		alt1 = a1, alt2 = a2;
	}

	void printMember(){
		cout << "(";
		alt1->printMember();
		cout << "^";
		alt2->printMember();
		cout << ")";
	}

	set<string> accept(RegExpVisitor* aVisitor);

};

set<string> AlternationExpression::accept(RegExpVisitor* aVisitor){
	return aVisitor->visitAlternation(this);
}

//<連接式>に対応するクラス
class SequenceExpression : public RegularExpression{
public:
	RegularExpression* seq1, * seq2;
	SequenceExpression(RegularExpression* s1, RegularExpression* s2){
		seq1 = s1, seq2 = s2;
	}

	void printMember(){
		cout << "(";
		seq1->printMember();
		cout << "&";
		seq2->printMember();
		cout << ")";
	}

	set<string> accept(RegExpVisitor* aVisitor);
};

set<string> SequenceExpression::accept(RegExpVisitor* aVisitor){
	return aVisitor->visitSequence(this);
}

//<反復式>に対応するクラス
class RepetitionExpression : public RegularExpression{
public:
	RegularExpression* rep;

	RepetitionExpression(RegularExpression* r){
		rep = r;
	}

	void printMember(){
		cout << "(";
		rep->printMember();
		cout << "*";
		cout << ")";
	}

	set<string> accept(RegExpVisitor* aVisitor);
};

set<string> RepetitionExpression::accept(RegExpVisitor* aVisitor){
	return aVisitor->visitRepetition(this);
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

	set<string> accept(RegExpVisitor* aVisitor);

};

set<string> LiteralExpression::accept(RegExpVisitor* aVisitor){
	return aVisitor->visitLiteral(this);
}


//正規表現のマッチを行うクラス
class RegExpMatcher: public RegExpVisitor{
private:
	set<string> inputState;
public:

	static RegExpMatcher *createMatcher(string str){
		RegExpMatcher* temp = new RegExpMatcher();
		set<string> s = set<string>();
		s.insert(str);
		temp->setInputState(s);
		return temp;
	}

	void setInputState(set<string> s);
	set<string> getInputState();
	set<string> visitAlternation(AlternationExpression* alternationExp);
	set<string> visitSequence(SequenceExpression* sequenceExp);
	set<string> visitRepetition(RepetitionExpression* repetitionExp);
	set<string> visitLiteral(LiteralExpression* literalExp);
};

void RegExpMatcher::setInputState(set<string> state){
	this->inputState = state;
}

set<string> RegExpMatcher::getInputState(){
	return this->inputState;
}

set<string> RegExpMatcher::visitAlternation(AlternationExpression* alternationExp){
	set<string> originalState = inputState, state1, state2;
	state1 = alternationExp->alt1->accept(this);
	state2 = alternationExp->alt2->accept(this);
	for(set<string>::iterator itr = state2.begin(); itr != state2.end(); itr++){
		state1.insert(*itr);
	}
	return state1;
}

set<string> RegExpMatcher::visitSequence(SequenceExpression* sequenceExp){
	inputState = sequenceExp->seq1->accept(this);
	return sequenceExp->seq2->accept(this);
}

set<string> RegExpMatcher::visitRepetition(RepetitionExpression* repetitionExp){
	set<string> aState, finalState;
	aState = inputState;
	while(aState.size() != 0){
		inputState = repetitionExp->rep->accept(this);
		for(set<string>::iterator itr = aState.begin(); itr != aState.end(); itr++){
			finalState.insert(*itr);
		}
	}

	return finalState;
}

set<string> RegExpMatcher::visitLiteral(LiteralExpression* literalExp){
	string lit = literalExp->lit;
	unsigned int len = lit.size();
	set<string> finalState;

	for(set<string>::iterator itr = inputState.begin(); itr != inputState.end(); itr++){
		if(len <= (*itr).size() && (*itr).substr(0, len) == lit)
			finalState.insert((*itr).substr(len, ((*itr).size() - len)));
	}

	return finalState;
}

int main()
{
	LiteralExpression* zdog = new LiteralExpression("dog");
	LiteralExpression* zcat = new LiteralExpression("cat");
	AlternationExpression* zalt = new AlternationExpression(zdog, zcat);
	RepetitionExpression* zrep = new RepetitionExpression(zalt);
	LiteralExpression* zwea = new LiteralExpression("weather");
	SequenceExpression* zseq = new SequenceExpression(zrep, zwea);
	RegExpMatcher* vv = RegExpMatcher::createMatcher("dogdogcatweather");
	set<string> result = zseq->accept(vv);
	for(set<string>::iterator itr = result.begin(); itr != result.end(); itr++){
		cout << *itr;
	}
	cout << endl;
}
