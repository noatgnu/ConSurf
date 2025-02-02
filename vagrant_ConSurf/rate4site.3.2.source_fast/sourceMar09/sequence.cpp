// $Id: sequence.cpp 874 2006-08-30 15:43:13Z osnatzomer $

#include "sequence.h"

#include <algorithm>
using namespace std;


sequence::sequence(const string& str,
				   const string& name,
				   const string& remark,
				   const int id,
				   const alphabet* inAlph)
: _alphabet(inAlph->clone()), _remark(remark), _name(name),_id(id) 
{
	for (int k=0; k < str.size() ;k += _alphabet->stringSize()) {
		int charId = inAlph->fromChar(str, k);
		if (charId == -99) {
			string textToPrint = "unable to read sequence: " + name;
			errorMsg::reportError(textToPrint);
		}

		_vec.push_back(charId);
	}
}


sequence::sequence(const sequence& other) 
: _vec(other._vec), _alphabet(other._alphabet->clone()), 
  _remark(other._remark), _name(other._name),_id(other._id) 
{
	
}
// convert the other sequence to the alphabet inAlph.
sequence::sequence(const sequence& other,const alphabet* inAlph)
: _alphabet(inAlph->clone()), _remark(other._remark), _name(other._name), _id(other._id)
{
	// if the other.alphabet is amino or nucleotide and the inAlph is indel
	
	if ( (other._alphabet->size() == 20 && inAlph->size() == 2)
		|| (other._alphabet->size() == 4 && inAlph->size() == 2) )
	{
		for (int k=0; k < other.seqLen() ;k += other._alphabet->stringSize()) 
		{
			int charId = other._vec[k];
			
			if (charId == other._alphabet->gap())
					_vec.push_back(inAlph->fromChar("-",0));
			else
				_vec.push_back(inAlph->fromChar("X",0)); //also converts "." (charId==-3) to "X"
				//	unknown amino/nucleotide is converted to "X" and not to "?"
		}
	}
		
	// if the other.alphabet is amino or nucleotide and the inAlph is mulAlphabet
	else if ( (other._alphabet->size() == 20 && inAlph->size()%20 == 0)
		|| (other._alphabet->size() == 4 && inAlph->size()%4 == 0) )
	{
		for (int k=0; k < other.seqLen() ;++k) 
		{
			int charId = other._vec[k];
			string ch = other._alphabet->fromInt(charId);
			int mulCharId = _alphabet->fromChar(ch,0);
			_vec.push_back(mulCharId);
		}
	//	 debug OZ
			//cout << "other sequence: " << other << endl;
			//cout << "mul sequence " << (*this) << endl;
	//	 end of debug
	}
	// (currently, there is no implimentions for other converts)
	else
	{
		string error = "unable to convert this kind of alphabet";
		errorMsg::reportError(error);
	}
}

sequence::~sequence()
{
	if (_alphabet) 
		delete _alphabet;
}

void sequence::resize(const int k, const int* val) {
	if (val == NULL) {
		_vec.resize(k,_alphabet->unknown());
	}
	else {
		_vec.resize(k,*val);
	}
}

string sequence::toString() const{
	string tmp;
	for (int k=0; k < _vec.size() ; ++k ){
		tmp+= _alphabet->fromInt(_vec[k]);
	}
	return tmp;
}

string sequence::toString(const int pos) const{
	return _alphabet->fromInt(_vec[pos]);
}	

void sequence::addFromString(const string& str) {
	for (int k=0; k < str.size() ; k+=_alphabet->stringSize()) {
		_vec.push_back(_alphabet->fromChar(str,k));
	}
}

class particip {
public:
	explicit particip()  {}
	bool operator()(int i) {
		return (i==-1000);
	}
};

//removePositions: the poitions to be removed are marked as '1' in posToRemoveVec
//all othehr positions are '0' 
void sequence::removePositions(const vector<int> & posToRemoveVec) 
{
	if(posToRemoveVec.size() != seqLen())
		errorMsg::reportError("the input vector must be same size as sequence length. in sequence::removePositions");
	for (int k=0; k < posToRemoveVec.size(); ++k) {
		if (posToRemoveVec[k] == 1) 
			_vec[k] = -1000;
	}
	vector<int>::iterator vec_iter;
	vec_iter =  remove_if(_vec.begin(),_vec.end(),particip());
	_vec.erase(vec_iter,_vec.end()); // pg 1170, primer.
}
