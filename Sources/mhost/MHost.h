/*
scripting management host
*/

#include "../cgiout/cgiout.h"
#include "../platform/platform.h"
#include "../mozilla/mozilla_v2.h"
#include "../cgi_equiv/cgi_equiv.h"
#include "../cgi_admin/cgi_admin.h"
#include "../cgi_access/cgi_access.h"

#define M_HOST_EQUIV "manage"

class MHost {
 private:
	CString Cmd;	
	cgi_access_manager Access;
	cgi_admin_manager Admin;
	void ProcessPost(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);
	void ProcessPostAgnes(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);
	void ProcessGet(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);
	void AgnesAddForm(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);  
       	void PasswordInput(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);
	static void StrOk(cgiOutStream& CGIStream);
	static void StrOk(cgiOutStream& CGIStream, const CString&);
	static void StrOp(cgiOutStream& CGIStream, const CString&);
	static void StrOp(cgiOutStream& CGIStream, const CString& StrC, const CString& StrV);
	static void StrError(cgiOutStream& CGIStream, const CString&);
	static void StrError(cgiOutStream& CGIStream);
	int CheckAdminPassword(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);
	void PassChangeForm(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void ProcessPostPasswd(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void ProcessPostPasswdReminder(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void GAMakePasswordReminderForm(cgiOutStream& CGIStream, const CString& Victim, cgi_equiv_manager& Equivs);
	void AliasSelect(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void GAEntry(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void GAPostEntry(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void GAPostCache(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);
	void GAPasswd(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void GADelete(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void GADeleteAll(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void GACache(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);
	void GAEmail(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&, const CString&);
	int GAPostCommon(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs, CString&);
	void GAPostDelete(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);
	void GAPostEmail(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);
	CString GAMakeRoot(cgiOutStream& CGIStream, const CString& Victim, cgi_equiv_manager& Equivs);
	void GATemplate(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&, const CString&);
	void GAPostTemplate(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&);
	void GAGeneral(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void GAPostGeneral(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void ProcessPostPurge(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
	void GAPostDelete(cgiOutStream& CGIStream, const CString& Victim, pParam_v2& Params, cgi_equiv_manager& Equivs);
	static void MakeDirectory(cgiOutStream& CGIStream, const CString& Tmp);
	void ProcessPostTestMailExchanger(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs);
public:
	static void MakePaths(cgiOutStream& CGIStream);
	MHost(cgiOutStream& CGIStream);
	virtual ~MHost(void);
	CString HostId(void);
	void Operate(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& equivs); 
	int Accept(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs, CString& Victim);
};
