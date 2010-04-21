/*
 *  monkeyWorksStreamUtilities.c
 *  MonkeyWorksMatlab
 *
 *  Created by David Cox on 12/20/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "monkeyWorksStreamUtilities.h"


double scarab_extract_float(ScarabDatum *datum){
	//mexPrintf("%d\n", datum->data.opaque.size);
	//char *hack = (char *)mxCalloc(datum->data.opaque.size, sizeof(char));
    //hack = strncpy(hack, (const char*)datum->data.opaque.data,
    //                                            datum->data.opaque.size);
	
#if	__LITTLE_ENDIAN__
	return *((double *)(datum->data.opaque.data));
#else
	int i;
	unsigned char swap_buffer[sizeof(double)];
	unsigned char *datum_bytes = (unsigned char *)datum->data.opaque.data;
	for(i = 0; i < sizeof(double); i++){
		swap_buffer[i] = datum_bytes[sizeof(double) - i - 1];
	}
	
	return *((double *)swap_buffer);
#endif
}

int getScarabEventCode(ScarabDatum *datum){
	
	ScarabDatum *code_datum = scarab_list_get(datum, SCARAB_EVENT_CODEC_CODE_INDEX);
	return code_datum->data.integer;
	
}

long long getScarabEventTime(ScarabDatum *datum){
	ScarabDatum *time_datum = scarab_list_get(datum, SCARAB_EVENT_TIME_INDEX);
	return time_datum->data.integer;
}

ScarabDatum *getScarabEventPayload(ScarabDatum *datum){
	ScarabDatum *payload_datum = scarab_list_get(datum, SCARAB_EVENT_PAYLOAD_INDEX);
	return payload_datum;
}


mxArray *recursiveGetScarabDict(ScarabDatum *datum);
mxArray *recursiveGetScarabList(ScarabDatum *datum){
	
	int i;
	
	//mexPrintf("recursiveGetScarabList");
	
	if(datum->type != SCARAB_LIST){
		return NULL;
	}
	
	int n = datum->data.list->size;
	ScarabDatum **values = datum->data.list->values;
	
	
	mxArray *cell_matrix = mxCreateCellMatrix(1, n);
	
	for(i = 0; i < n; i++){
		
		mxArray *mx_datum;
		
		if(values[i] != NULL){
			
			switch(values[i]->type){
				
				case SCARAB_INTEGER:
					mx_datum = mxCreateDoubleScalar((double)values[i]->data.integer);
					break;
				case SCARAB_FLOAT:
					mx_datum = mxCreateDoubleScalar((double)values[i]->data.floatp);
					break;
				case SCARAB_FLOAT_OPAQUE:
					mx_datum = mxCreateDoubleScalar(scarab_extract_float(values[i]));
					break;
				case SCARAB_OPAQUE:
					mx_datum = mxCreateString((const char *)scarab_extract_string(values[i]));
					break;
				case SCARAB_DICT:
					mx_datum = recursiveGetScarabDict(values[i]);
					break;
				case SCARAB_LIST:
					mx_datum = recursiveGetScarabList(values[i]);
					break;
				default:
					mx_datum = NULL;
					break;
			}
			
			if(mx_datum != NULL){
				mxSetCell(cell_matrix, i, mx_datum);
			}
		}
	}
	
	return cell_matrix;
}


mxArray *recursiveGetScarabDict(ScarabDatum *datum){
	
	int i;
	
	//mexPrintf("recursiveGetScarabDict");
	
	if(datum->type != SCARAB_DICT){
		return NULL;
	}
	
	int n = scarab_dict_number_of_elements(datum);
	ScarabDatum **keys = scarab_dict_keys(datum);
	ScarabDatum **values = scarab_dict_values(datum);
	
	char **fields = (char **) calloc(n, sizeof(char *));

	int dictionaries = 0;
	int lists = 0;
	int floats = 0;
	int unknowns = 0;
	
	for(i = 0; i < n; i++){
		if(keys[i] != 0) {
			switch(keys[i]->type) {
				case SCARAB_OPAQUE:
					fields[i] = (char *)scarab_extract_string(keys[i]);
					break;
				case SCARAB_DICT:
				{
					const char *dictPrefix = "dict";
					++dictionaries;
					int len = snprintf(0, 0, "%s%d", dictPrefix, dictionaries);
					char *str = (char *)malloc((len + 1) * sizeof(char));
					
					len = snprintf(str, len + 1, "%s%d", dictPrefix, dictionaries);
					
					fields[i]=str;
				}
					break;
				case SCARAB_LIST:
				{
					const char *listPrefix = "list";
					++lists;
					int len = snprintf(0, 0, "%s%d", listPrefix, lists);
					char *str = (char *)malloc((len + 1) * sizeof(char));
					
					len = snprintf(str, len + 1, "%s%d", listPrefix, lists);
					
					fields[i]=str;
				}
					break;
				case SCARAB_FLOAT:
				case SCARAB_FLOAT_INF:
				case SCARAB_FLOAT_NAN:
				case SCARAB_FLOAT_OPAQUE:
				{
					const char *floatPrefix = "float";
					++floats;
					int len = snprintf(0, 0, "%s%d", floatPrefix, floats);
					char *str = (char *)malloc((len + 1) * sizeof(char));
					
					len = snprintf(str, len + 1, "%s%d", floatPrefix, floats);
					
					fields[i]=str;
				}
					break;
				case SCARAB_INTEGER:
				{
					const char *intPrefix = "int";
					int intValue = values[i]->data.integer;
					int len = snprintf(0, 0, "%s%d", intPrefix, intValue);
					char *str = (char *)malloc((len + 1) * sizeof(char));
					
					len = snprintf(str, len + 1, "%s%d", intPrefix, intValue);
					
					fields[i]=str;
				}
					break;
				default:
				{
					const char *unknownPrefix = "unknown";
					++unknowns;
					int len = snprintf(0, 0, "%s%d", unknownPrefix, unknowns);
					char *str = (char *)malloc((len + 1) * sizeof(char));
					
					len = snprintf(str, len + 1, "%s%d", unknownPrefix, unknowns);
					
					fields[i]=str;
				}
					break;
					
	
			}
		}
	}
	
	mwSize dims = 1;
	mwSize ndims = 1;
	mxArray *struct_array = mxCreateStructArray(ndims, &dims, n, (const char **)fields);
	
	for(i = 0; i < n; i++){
		
		mxArray *mx_datum;
		
		if(values[i] != NULL){
			
			switch(values[i]->type){
				
				case SCARAB_INTEGER:
					mx_datum = mxCreateDoubleScalar((double)values[i]->data.integer);
					break;
				case SCARAB_FLOAT:
					mx_datum = mxCreateDoubleScalar((double)values[i]->data.floatp);
					break;
				case SCARAB_FLOAT_OPAQUE:
					mx_datum = mxCreateDoubleScalar(scarab_extract_float(values[i]));
					break;
				case SCARAB_OPAQUE:
					mx_datum = mxCreateString((const char *)scarab_extract_string(values[i]));
					break;
				case SCARAB_DICT:
					mx_datum = recursiveGetScarabDict(values[i]);
					break;
				case SCARAB_LIST:
					mx_datum = recursiveGetScarabList(values[i]);
					break;
				default:
					mx_datum = NULL;
					break;
			}
			
			if(mx_datum != NULL){
				mxArray *old_thing = mxGetField(struct_array, 0, fields[i]);
				if(old_thing){
					mxDestroyArray(old_thing);
				}
				
				mxSetField(struct_array, 0, fields[i], mx_datum);
			}
		}
	}
	
	return struct_array;
}

mxArray *getScarabEventData(ScarabDatum *datum){
	if(datum == NULL){
		//mexPrintf("Bad data event -- setting value to 0.0");
		return mxCreateDoubleScalar(0.0);
	}
	
	ScarabDatum *payload = getScarabEventPayload(datum);
	
	if(!payload){
		//mexPrintf("Bad data event -- setting value to 0.0");
		return mxCreateDoubleScalar(0.0);
	}
	
	if(payload->type == SCARAB_INTEGER){
#if VERBOSE
		//mexPrintf("int");
#endif
		return mxCreateDoubleScalar(payload->data.integer);
	} else if(payload->type == SCARAB_FLOAT){
#if VERBOSE
		//mexPrintf("true_float");
#endif
		return  mxCreateDoubleScalar((double)payload->data.floatp);
	} else if(payload->type == SCARAB_FLOAT_NAN ||
			  payload->type == SCARAB_FLOAT_OPAQUE || 
			  payload->type == SCARAB_OPAQUE){
#if VERBOSE
		//mexPrintf("float_opaque");
#endif
		//return mxCreateDoubleScalar(0.0);
		return mxCreateDoubleScalar(scarab_extract_float(payload)); // HACK
	} else if(payload->type == SCARAB_DICT){
		
#if VERBOSE
		//mexPrintf("dict");
#endif
		return recursiveGetScarabDict(payload);
	} else if(payload->type == SCARAB_LIST){
		
#if VERBOSE
		//mexPrintf("list");
#endif
		return recursiveGetScarabList(payload);
	}
	
	return mxCreateDoubleScalar(0.0);
}


mxArray *getScarabEventDataArray(ScarabDatum *datum){
	
	//mexPrintf("getScarabEventDataArray");
	
	if(datum == NULL){
		//mexPrintf("Bad data event -- setting value to 0.0");
		return NULL;
	}
	
	ScarabDatum *payload = getScarabEventPayload(datum);
	
	if(!payload){
		//mexPrintf("Bad data event -- setting value to 0.0");
		return NULL;
	}
	
	if(payload->type == SCARAB_DICT){
#if VERBOSE
		//mexPrintf("dict");
#endif
		
		// make a struct out of the dict
		return recursiveGetScarabDict(payload);
		
	} else if(payload->type == SCARAB_LIST){
#if VERBOSE
		//mexPrintf("list");
#endif
		// make an array out of the list
		return recursiveGetScarabList(payload);
	}
	
	return NULL;
}


int getScarabSystemEventType(ScarabDatum *payload){
	
	ScarabDatum *type_datum = scarab_list_get(payload, 
											  0);
	return type_datum->data.integer;
}

int getScarabSystemEventControlType(ScarabDatum *payload){
	
	ScarabDatum *type_datum = scarab_list_get(payload, 
											  1);
	return type_datum->data.integer;
}

ScarabDatum *getScarabSystemEventPayload(ScarabDatum *payload){
	return scarab_list_get(payload, 2);
}


mxArray *extractCodec(ScarabDatum *system_payload){
	
	return NULL;
	
}

int scarabEventToEventStruct(mxArray *eventlist, int index, ScarabDatum *datum){
	long code = getScarabEventCode(datum);
	long long time = getScarabEventTime(datum);
	
	mxArray *old_code = mxGetField(eventlist, index, (const char *)"event_code");
	if(old_code != NULL){
		mxDestroyArray(old_code);
	}
	mxSetField(eventlist, index, (const char *)"event_code", mxCreateDoubleScalar((double)code));		
	
	mxArray *old_time = mxGetField(eventlist, index, (const char *)"time_us");
	if(old_time != NULL){
		mxDestroyArray(old_time);
	}
	
	
	mxSetField(eventlist, index, (const char *)"time_us", mxCreateDoubleScalar((double)time));
	
	if(VERBOSE){
		//mexPrintf("Got a data event\n");
	}
	
	mxArray *data = getScarabEventData(datum);
	mxArray *old_data = mxGetField(eventlist, index, (const char *)"data");
	if(old_data){
		mxDestroyArray(old_data);
	}
	mxSetField(eventlist, index, (const char *)"data", data);
		
	return code;
	
}


int scarabEventToDataStruct(mxArray *data_struct, int index, ScarabDatum *datum){
	
	//mexPrintf("Parsing event...");
	
	mxArray *eventlist = mxGetField(data_struct, 0, "events");
	
	long code = getScarabEventCode(datum);
	long long time = getScarabEventTime(datum);
	
	mxArray *old_code = mxGetField(eventlist, index, (const char *)"event_code");
	if(old_code != NULL){
		mxDestroyArray(old_code);
	}
	mxSetField(eventlist, index, (const char *)"event_code", mxCreateDoubleScalar((double)code));		
	
	mxArray *old_time = mxGetField(eventlist, index, (const char *)"time_us");
	if(old_time != NULL){
		mxDestroyArray(old_time);
	}
	
	mxSetField(eventlist, index, (const char *)"time_us", mxCreateDoubleScalar((double)time));
	
	
	if(VERBOSE){
		mexPrintf("Got a data event\n");
	}
	
	mxArray *data = getScarabEventData(datum);
	mxArray *old_data = mxGetField(eventlist, index, (const char *)"data");
	if(old_data){
		mxDestroyArray(old_data);
	}
	mxSetField(eventlist, index, (const char *)"data", data);
		
	return code;
	
}


int getScarabError(ScarabSession * session) {
    return scarab_session_geterr(session);
}

int getScarabOSError(ScarabSession * session) {
    if(getScarabError(session)) {
        return scarab_session_getoserr(session);
    } else {
        return 0;
    }
}

const char * getScarabErrorName(int error) {
    return scarab_moderror(error);
}

const char * getScarabErrorDescription(int error) {
    return scarab_strerror(error);
}

const char * getOSErrorDescription(int oserror) {
    return scarab_os_strerror(oserror);
}
