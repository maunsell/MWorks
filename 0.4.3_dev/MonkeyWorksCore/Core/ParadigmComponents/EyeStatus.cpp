//#ifndef CLASSIC_LABLIB_H//#define CLASSIC_LABLIB_H//#include "Classic Lablib.h"//#endif#include "States.h"#include "EyeStatus.h"#include "Experiment.h"//#include "UserData.h"// Eye routines	bool EyeTrigger::isActive(){	return GlobalCurrentExperiment->getBool(active);}		bool EyeTrigger::isInWindow(float locx, float locy){		float distancefromcenter = sqrt((GlobalCurrentExperiment->getFloat(x) - locx)*(GlobalCurrentExperiment->getFloat(x) - locx) + (GlobalCurrentExperiment->getFloat(y) - locy)*(GlobalCurrentExperiment->getFloat(y) - locy));	if(distancefromcenter <= GlobalCurrentExperiment->getFloat(radius)){		return true;	} else {		return false;	}}void EyeTrigger::update(){//	if(isActive()){//		if(isInWindow(E->getFloat(EYE_X_POS_DEG), E->getFloat(EYE_Y_POS_DEG))){		//			if(!(E->getBool(togglevar))){//				E->setBool(togglevar, true);						//mprintf("Toggled true...");				//				if(setadditionalvar){//					E->set(var, value);//				}				//mprintf("Updated: %d to true", togglevar);//			}//		} else if(E->getBool(togglevar)){//			E->setBool(togglevar, false);//			mprintf("Toggled false");//		}//	} else if(E->getBool(togglevar)){//		E->setBool(togglevar, false);//		mprintf("Not active: set to false");//	} else {//		mprintf("Not active");//	}} // TODO: Fixfloat EyeTrigger::getXDeg(){	return GlobalCurrentExperiment->getFloat(x);}float EyeTrigger::getYDeg(){	return GlobalCurrentExperiment->getFloat(y);}float EyeTrigger::getRadiusDeg(){	return GlobalCurrentExperiment->getFloat(radius);}bool EyeTrigger::isAdditionalVarSet() {    return setadditionalvar;}Variable * EyeTrigger::getAdditionalParam() {    return var;}Data * EyeTrigger::getAdditionalData() {    return &value;}void updateEyeTriggers(){	//E->updateEyeTriggers();}		/*// a version of the boxcar filter function modified to work on a ring bufferstatic DOUBLE_POINT boxcar_filter_rb(short width, DOUBLE_POINT raw, DOUBLE_POINT Store[]){	DOUBLE_POINT Filtered;	register short i,count;		// buffer values for boxcar	for (i=width-1;i>=1;i--) {		Store[i] = Store[i-1];	}	Store[0] = raw;		// compute boxcar average	Filtered.h = Filtered.v = 0;	count = 0;	for (i=width-1;i>=0;i--) {		if (Store[i].h > -99999) {		// true value exists				Filtered.h = Filtered.h + Store[i].h;			Filtered.v = Filtered.v + Store[i].v;			count++;		}	}	Filtered.h = Filtered.h/(float)count;	Filtered.v = Filtered.v/(float)count;		return(Filtered);}static DOUBLE_POINT digital_filter(double B[], double A[], short order, DOUBLE_POINT input, DOUBLE_POINT StoreInput[], DOUBLE_POINT StoreOutput[]){	DOUBLE_POINT output;	register short i;		// update buffered input values	for (i=order-1;i>=1;i--) {		StoreInput[i] = StoreInput[i-1];	}	StoreInput[0] = input;		// update buffered output values	for (i=order-1;i>=1;i--) {		StoreOutput[i] = StoreOutput[i-1];	}			// compute filter result	output.h = output.v = 0;	for (i=order-1;i>=0;i--) {		if (StoreInput[i].h == -99999) {		// if an input value does not exist	-- use the current input value			//StoreOutput[0] = input;			//return(input);		// no filtering until all input and output values exist			//output.h = output.h + (StoreInput[0].h*B[i]);			//output.v = output.v + (StoreInput[0].v*B[i]);					}		else {			output.h = output.h + (StoreInput[i].h*B[i]);			output.v = output.v + (StoreInput[i].v*B[i]);		}	}		for (i=order-1;i>=1;i--) {		if (StoreOutput[i].h == -99999) {		// value does not exist	-- ignore the recursive terms (i.e. previous output = 0)			//mprintf("ERROR-- no output");		}		else {			output.h = output.h - (StoreOutput[i].h*A[i]);			output.v = output.v - (StoreOutput[i].v*A[i]);		}	}		// apply the last recursive term	output.h = output.h/A[0];	output.v = output.v/A[0];			StoreOutput[0] = output;	// update buffer of output values for next time	return(output);}*/