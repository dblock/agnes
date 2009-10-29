#include <agnespch.hpp>

faces_manager::faces_manager(const CString& container){
	if (container.StrLength()) {
		nextgen_v2 faces_file(container);
		CString s;
		while (!faces_file.feofc()) {
			if (!(s = faces_file.read_line()).StrLength()) continue;
			CVector<CString> FaceTokens; s.Tokenizer("--", FaceTokens);
			if (FaceTokens.Count() == 3) faces_list+=new agnes_face(FaceTokens[0].Val(), FaceTokens[1], FaceTokens[2]);
		}
	}
}

CString faces_manager::link(int id){
	for (int i=0;i<faces_list.count();i++)
		if (faces_list[i]->id() == id) return(faces_list[i]->link());
	return("");
}

CString faces_manager::comment(int id){
	for (int i=0;i<faces_list.count();i++)
		if (faces_list[i]->id() == id) return(faces_list[i]->comment());
	return("");
}

void faces_manager::show_face(cgiOutStream& CGIStream, int id){
	for (int i=0;i<faces_list.count();i++)
		if (faces_list[i]->id() == id) {
			if (faces_list[i]->link().StrLength()) CGIStream << img_.o("SRC",faces_list[i]->link().asString(), 0) << " ";
			return;
		}
}

void faces_manager::show_table(cgiOutStream& CGIStream, agnes_article * article, int Cols){ 
	if (Cols < 1) Cols = 2;
	CGIStream << tr_ << td_.o("ALIGN","right",0) << "&nbsp;" << !td_
		  << td_ << table_;
	for (int i=0;i<faces_list.count();i++){
		if (!(i%Cols)) CGIStream << tr_;
		CString iVal; iVal += faces_list[i]->id();
		CGIStream << "\n" << td_ << input_.o("TYPE","radio","CHECKED",
						     bool_to_space(article?(article->article_face() == faces_list[i]->id()):(i == (faces_list.count()-1))).asString(),
						     "NAME","face","VALUE", iVal.asString(), 0);	
		if (faces_list[i]->link().StrLength()) CGIStream << img_.o("SRC",faces_list[i]->link().asString(),0);
		CGIStream << " " << faces_list[i]->comment() << !td_;
		if (i%Cols == (Cols-1)) CGIStream << !tr_ << "\n";
	}
	for (int j=Cols - faces_list.count()%Cols - 1;j>=0;j--) {
		CGIStream << td_ << "&nbsp;" << !td_;
		if (!j) CGIStream << !tr_;
	}
	// if (faces_list.count()%Cols != Cols) CGIStream << !tr_;
	CGIStream << !td_ << !table_ << !td_ << !tr_;
}
