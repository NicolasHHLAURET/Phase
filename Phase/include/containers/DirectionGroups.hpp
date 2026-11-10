/*
 * DirectionGroups2.hpp
 *
 *  Created on: Feb 17, 2012
 *      Author: yint
 */

#ifndef DIRECTIONGROUPS2_HPP_
#define DIRECTIONGROUPS2_HPP_

//#include <list>
#include "DirectionCartesienne.hpp"

class DirectionGroups {
private:
	vector<DirectionCartesienne> dirList;
	vector<vector<DirectionCartesienne> > output;

public:
	DirectionGroups(vector<DirectionCartesienne> const& dirL) {
		setDirList(dirL);
		generateGroups();
//		showSize();
	}
	void setDirList(vector<DirectionCartesienne> const& dirL);
	void generateGroups();
	vector<vector<DirectionCartesienne> > getOutput() const;
	void showSize();
	void sort();

#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    static constexpr double precision= 1e-15;
#else
    static const double precision= 1e-15;
#endif

};

inline void DirectionGroups::setDirList (vector<DirectionCartesienne> const& dirL){
	if (dirL.size()<2) {
		cerr<<"Error: Less than 2 directions can not be grouped"<<endl;
		abort();
	}
	dirList.assign(dirL.begin(), dirL.end());
	sort();
}

inline void DirectionGroups::generateGroups() {
	vector<DirectionCartesienne> group;
	vector<DirectionCartesienne>::const_iterator it = dirList.begin(), itMinus1;
	group.push_back(*it);
	++it;
	for (itMinus1 = dirList.begin(); it<dirList.end(); ++it, ++itMinus1) {
		const DirectionCartesienne & dir = *it;
		const DirectionCartesienne & dir2 = *itMinus1;
		if (!(fabs(dir.getBeginTheta()-dir2.getBeginTheta())<precision &&
				fabs(dir.getEndTheta()-dir2.getEndTheta())<precision &&
				fabs(dir.getCenterTheta()-dir2.getCenterTheta())<precision &&
				fabs(dir.getSolidAngle()-dir2.getSolidAngle())<precision &&
				dir.getProperty()==dir2.getProperty())) {
			output.push_back(group);
			group.clear();
		}
		group.push_back((*it));
	}

	output.push_back(group); //For the last group;
}

inline vector<vector<DirectionCartesienne> > DirectionGroups::getOutput() const {
	return output;
}

inline void DirectionGroups::showSize() {
	static unsigned int cnt=0;
	cout<<endl;
	for(vector<DirectionCartesienne> it : output){

		cout<<"Group "<<cnt<<" has "<< it.size() <<" directions."<<endl;
//
//		vector<DirectionCartesienne>::iterator it1;
//		for (it1=it.begin();it1<it.end();it1++) {
//				cout<<" Phi:" <<it1->getBeginPhi()<<" "<<it1->getEndPhi()<<". DeltaPhi: "<<setprecision(15)<< it1->getDeltaPhi() << ". Omega: "<< it1->getSolidAngle() <<endl;
//		}
		++cnt;
//		count+=it.size();
	}
//	cout<<count<<endl;
}

inline void DirectionGroups::sort() {

	//Sort the TDDirection set vectors relative transmittance the value of center_theta & center_phi;
	//Using the Bubble Sort relative the center_theta

	unsigned int size = dirList.size();
//	unsigned int max_num_equal_theta = 1; //maximum number of equal center theta over all the direction dirList.
//	unsigned int temp_number = 1; //temp parameter transmittance find the max_num_equal_theta.

	for (unsigned int j = 0; j < size; j++) {
		for (unsigned int i = 0; i < size - 1; i++) {

			if (dirList[i].getCenterTheta() > dirList[i + 1].getCenterTheta()) {
				DirectionCartesienne temp = dirList[i];
				dirList[i] = dirList[i + 1];
				dirList[i + 1] = temp;
			}
			else if (dirList[i].getCenterTheta() == dirList[i + 1].getCenterTheta()) {
				if (dirList[i].getCenterPhi() > dirList[i + 1].getCenterPhi()) {
					DirectionCartesienne temp = dirList[i];
					dirList[i] = dirList[i + 1];
					dirList[i + 1] = temp;
				}
			}
		}
	}
}

#endif /* DIRECTIONGROUPS2_HPP_ */
