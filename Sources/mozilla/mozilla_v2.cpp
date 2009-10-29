#include <agnespch.hpp>

int pParam_v2::entries_count(void){
	return(entry_manager.entries_count());
}

void pParam_v2::entries_clear(void){
	entry_manager.clear();
}

CString pParam_v2::get_name(int index){
	if (method_post)
		return(entry_manager.get_name(index));
	else {
		return "";
	}
}


CString pParam_v2::get_entry(int index){
	if (method_post)
		return(entry_manager.get_value(index));
	else {
		return "";
	}
}

CString pParam_v2::get_entry_list(const CString& name){
	CString Result;
	if (method_post){
		for (int i=0;i<entry_manager.entries_count();i++)
			if (entry_manager.get_name(i) == name) {
				if (Result.StrLength()) Result+="\t";
				Result+=entry_manager.get_value(i);
			}	
	}
	return Result;
}

CString pParam_v2::get_entry(const CString& name){
	if (method_post)
		return(entry_manager.get_value(name));
	else {
		return "";
	}
}

void pParam_v2::create_cmdl(void){
	if (argv.Count() >= 3) {
		cmdl = getenv("SCRIPT_NAME");
		cmdl += "?";
		cmdl += argv[1];
		cmdl += "+";
		cmdl += argv[2];    
	}
}

pParam_v2::pParam_v2(const CVector<CString>& args, const CVector<CString>& posts){
	argv = args;
	create_cmdl();
	if (posts.Count()){
		method_post = 1;
		for (int i=0; i<posts.Count();i++){
			CString Val = posts[i];
			Val.Replace('+',' ');
			Val.unescape_url();
			int iPos = Val.Pos('=');
			if (iPos >=0) {
				CString Left(Val.Copy(0, iPos));
				CString Right(Val.Copy(iPos+1, Val.StrLength()));
				entry_manager.add_value(Left, Right);
			}
		}
	} else method_post = 0;
}

void pParam_v2::add_entry(const CString& name, const CString& value){
	entry_manager.add_value(name, value);
}

void pParam_v2::set_entry(const CString& name, const CString& value){
	entry_manager.set_value(name, value);
}

pParam_v2::pParam_v2(int i_argc, char * i_argv[]){
	CString iArgv;
	for (int i=0;i<i_argc;i++) {
		iArgv = i_argv[i]; iArgv.unescape_url();
		argv+=iArgv;
	}
	create_cmdl();
	if ((!getenv("CONTENT_TYPE"))||
		(!getenv("CONTENT_LENGTH"))||
		(!getenv("REQUEST_METHOD"))) method_post = -1;
	else if (strcmp(getenv("REQUEST_METHOD"), "POST")) method_post = 0; 
	else {
		method_post = 1;
		/*
		if(strcmp(getenv("CONTENT_TYPE"),"application/x-www-form-urlencoded"))
		cgi_error("this script can only be used to decode form results");
		*/
		get_entries();
	}
}

int pParam_v2::isPost(void){
	return(method_post);
}

pParam_v2::~pParam_v2(void){
	
}



CString pParam_v2::ReadStdin(int Size) {

#ifndef ANSI

	setmode(fileno(stdin), O_BINARY);

#endif

	FILE * StandardInput = fdopen(0, "rb");

	char * Segment = ::new char[Size+1];

	Size = fread(Segment, sizeof(char), Size, StandardInput);

	Segment[Size] = 0;

	CString Result; Result.StrAppend(Segment, Size);

	delete[] Segment;

	fclose(StandardInput);

	return Result;	

}

void pParam_v2::get_entries(void){ 
	int cl = atoi(getenv("CONTENT_LENGTH"));  
	CString cType = getenv("CONTENT_TYPE");    
	entry_manager.set_value("CONTENT_TYPE", cType); 
	entry_manager.set_value("CONTENT_TYPE_MULTIPART", cType);
	CVector<CString> GValues; cType.Tokenizer(';', GValues);
	for (int j=0;j<GValues.Count();j++){
		CString GValue = GValues[j]; GValue.StrTrim32();
		CVector<CString> GVector = GValue.Tokenizer('=');
		if (GVector.Count() >= 2) {
			CString GName = GVector[0];
			int i=0;
			while (entry_manager.get_value(GName).StrLength()) {
				GName = GVector[0]; GName+=(i++);
			} 

			entry_manager.set_value(GName, GVector[1]);      
		} else {
			CString Tmp("CONTENT_TYPE=");Tmp+=GValue; putenv(Tmp.asString());
			entry_manager.set_value("CONTENT_TYPE",GValue);
		}
	}    

	RawData = ReadStdin(cl);
	if (entry_manager.get_value("CONTENT_TYPE") == "multipart/form-data") {    
		CString Boundary = entry_manager.get_value("BOUNDARY");
		if (Boundary.StrLength()) {
			CVector<CString> Boundaries = RawData.Tokenizer(Boundary);
			for (int l=0;l<Boundaries.Count();l++){
			/*
			process each boundary
				*/
				CVector<CString> BGValues; Boundaries[l].Tokenizer(';', BGValues);
				for (int j=0;j<BGValues.Count();j++){
					CString BGValue = BGValues[j]; BGValue.StrTrim32();
					CVector<CString> BGVector = BGValue.Tokenizer('=');
					if ((BGVector.Count() >= 2)&&(BGVector[0]=="NAME")) {
						if (BGVector[1][0] == '\"') BGVector[1].StrDelete(0, 1);
						int sPos = BGVector[1].Pos('\"'); if (sPos == -1) sPos = BGVector[1].Pos(' '); if (sPos == -1) sPos = BGVector[1].Pos('\r'); if (sPos == -1) sPos = BGVector[1].Pos('\n');	    
						if (sPos != -1) BGVector[1].StrDelete(sPos, BGVector[1].StrLength()); BGVector[1].unescape_url();
						CString BP = Boundaries[l]; BP.ExtractLine(); BP.ExtractLine();
						if (BP.EndsWith("--")) BP.StrDelete(BP.StrLength()-strlen("--"), BP.StrLength()); BP.StrTrim32(); BP.unescape_url();
						CString BGName = BGVector[1];
						int f=0;
						while (entry_manager.get_value(BGName).StrLength()) {
							BGName = BGVector[0]; BGName+=(f++);
						}
						entry_manager.set_value(BGName, BP);
					}
				}
			}
		}     
	} else {

		CVector<CString> ValParams;
		RawData.Tokenizer('&', ValParams);
		for (int i = 0;i<ValParams.Count();i++){
			ValParams[i].Replace('+',' ');
			int iPos = ValParams[i].Pos('=');
			if (iPos >=0) {
				CString Source = ValParams[i].Copy(0, iPos); Source.unescape_url();
				CString Target = ValParams[i].Copy(iPos+1, ValParams[i].StrLength()); Target.unescape_url();

				entry_manager.add_value(Source, Target);
			} 
		}
	}
}

CString pParam_v2::c_params(void){
	CString result;
	for (int i=0;i<extra_params.Count();i++){
		result+="+";
		result+=extra_params[i];
	}
	return(result);
}

CString pParam_v2::getRawData(void) const {
	return RawData;
}

