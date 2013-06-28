//#include "stdafx.h"
#include "String.h"
#include "windows.h"

TStr CA2T(AStr s, int cp)
{
        if (s.ise()) return L"";
        int len = MultiByteToWideChar(cp, 0, *s, -1, NULL, 0)-1; 
        TStr r(len+1);
        MultiByteToWideChar(cp, 0, *s, -1, *r, len);
        r[len]='\0';
        r.length() = len;
        return r;
}

AStr CT2A(TStr s, int cp)
{
        if (s.ise()) return "";
        int len = WideCharToMultiByte(cp, 0, *s, -1, NULL, 0,NULL,NULL)-1; 
        AStr r(len+1);
        WideCharToMultiByte(cp, 0, *s, -1, *r, len,NULL,NULL);
        r[len]='\0';
        r.length() = len;
        return r;
}

AStr CA2U(AStr a)
{
        if (a.ise()) return "";
        AStr res;
        int l = MultiByteToWideChar(CP_ACP,0,*a,-1,NULL,0)-1;
        if (l!=-1)
        {
                TStr r(l+1);
                MultiByteToWideChar(CP_ACP,0,*a,-1,*r,l);
                r[l]=L'\0';
                r.length() = l;

                l = WideCharToMultiByte(CP_UTF8,0,*r,-1,NULL,0,NULL,NULL)-1;
                if (l!=-1)
                {
                        res.buffer(l+1);
                        WideCharToMultiByte(CP_UTF8, 0, *r, -1, *res, l,NULL,NULL);
                        res[l]='\0';
                        res.length() = l;
                }
        }

        return res;
}

AStr CU2A(AStr u)
{
        if (u.ise()) return "";
        AStr res;
        int l = MultiByteToWideChar(CP_UTF8,0,*u,-1,NULL,0)-1;
        if (l!=-1)
        {
                TStr r(l+1);
                MultiByteToWideChar(CP_UTF8,0,*u,-1,*r,l);
                r[l]=L'\0';
                r.length() = l;

                l = WideCharToMultiByte(CP_ACP,0,*r,-1,NULL,0,NULL,NULL)-1;
                if (l!=-1)
                {
                        res.buffer(l+1);
                        res[l]='\0';
                        WideCharToMultiByte(CP_ACP, 0, *r, -1, *res, l,NULL,NULL);
                        res.length() = l;
                }
        }

        return res;
}


AStr Hex2Bin(AStr H)
{
        AStr res;
        if (H.In("0-9a-fA-F"))
        {
                int i=0;
                long c;
                if (H.length() % 2==1)
                {
                        c = (long)H.mid(0,1).htol();
                        res=(char)c;
                        i=1;
                }
                for(;i<H.length();i+=2)
                {
                        c = (long)H.mid(i,2).htol();
                        res += (char)c;
                }
        }
        return res;
}

AStr Bin2Hex(AStr B, bool uc)
{
        AStr res,hd = "0123456789abcdef";
        int i=0;
        if (uc) hd.toupper();
        char c;
        for(;i<B.length();i++)
        {
                c = B[i];
                res += hd[(c>>4) & 0xF];
                res += hd[c & 0xF];
        }
        return res;
}

AStr jPrintf(const char* f, ...)
{
        int l = strlen(f)+512;
        AStr res;
        
        va_list ap;
        va_start(ap,f);
        
        while(1){
                res.buffer(l);
                res.length() = _vsnprintf( *res, res.alength(), f, ap );
                if (res.length()!=-1) break;
                l+=l;
        }
        va_end(ap);
        return res;
        
}


TStr jPrintf(const TCHAR* f, ...)
{
        int l = wcslen(f)+512;
        TStr res;
        
        va_list ap;
        va_start(ap,f);
        
        while(1){
                res.buffer(l);
                res.length() = _vsnwprintf( *res, res.alength(), f, ap );
                if (res.length()!=-1) break;
                l+=l;
        }
        va_end(ap);
        return res;
        
}

AStr jPrintfv(const char* f, va_list ap)
{
        int l = strlen(f)+512;
        AStr res;

        while(1){
                res.buffer(l);
                res.length() = _vsnprintf( *res, res.alength(), f, ap );
                if (res.length()!=-1) break;
                l+=l;
        }
        return res;

}


AStr i2s(int i)
{
        return jPrintf("%d",i);
}

AStr l2s(long i)
{
        return jPrintf("%d",i);
}

TStr _FRead(FILE* f)
{
        TStr r;
        fseek(f,0,SEEK_END);
        int e = ftell(f);
        fseek(f,0,SEEK_SET);
        char* c = (char*)malloc(e+2);
        if (0==c) return r;
        wchar_t* p = 0;
        memset(c,0,e+2);
        fread(c,sizeof(char),e,f);

        if ((BYTE)c[0]==0xFF && (BYTE)c[1]==0xFE)
        {
                r = (wchar_t*)&c[2];
        }
        else
        {
                char* d = 0;
                if ((BYTE)c[0]==0xEF && (BYTE)c[1]==0xBB && (BYTE)c[2]==0xBF)
                        d = &c[3];
                r = CA2T(d?d:c,d?CP_UTF8:CP_ACP);
        }
        
        free(c);
        fclose(f);
        return r;
}

TStr ReadIni(const AStr& file)
{
        FILE* f = fopen(*file,"rb");
        if (NULL==f) return 0;
        return _FRead(f);
}

TStr ReadIni(const TStr& file)
{
        FILE* f = _wfopen(*file,L"rb");
        if (NULL==f) return 0;
        return _FRead(f);
}

AStr ReadFile(const char* f)
{
        AStr r;
        if (f)
        {
                FILE * fp = fopen(f,"rb");
                if (fp)
                {
                        int l;
                        char buf[1028];
                        while(!feof(fp))
                        {
                                memset(buf,0,1028);
                                l = fread(buf, sizeof(char), 1024, fp);
                                buf[l]=0;
                                buf[l+1]=0;
                                r.assign(buf,l,true);
                        }
                        fclose(fp);
                }
        }
        return r;
}
        
bool WriteFile(const char* f, AStr d)
{
        if (f)
        {
                FILE * fp = fopen(f,"a+");
                if (fp)
                {
                        fwrite(*d,sizeof(char),d.length(),fp);
                        fclose(fp);
                        return true;
                }
        }
        return false;
}

bool WriteFile(const char* f, TStr d)
{
        if (f)
        {
                FILE * fp = fopen(f,"wb");
                if (fp)
                {
                        fwrite(*d,sizeof(TCHAR),d.length(),fp);
                        fclose(fp);
                        return true;
                }
        }
        return false;
}
        
void Debug(char* p)
{

#ifdef _WIN32_WCE
        SYSTEMTIME st;
        GetLocalTime(&st);
        FILE* log;
        log = fopen(".\\Log.txt", "a+");
        if (log == NULL){
                return;
        }
        fprintf(log,"[===%04d-%02d-%02d %02d:%02d:%02d===]\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
        fprintf(log, p);
        fclose(log);
#else
        va_list fmt;
        va_start(fmt,p);
        char buf[40960];
        vsprintf(buf,fmt,p);
        if (strlen(buf)<=0) return ;
        printf(buf);
#endif
        va_end(p);
}
/*
void Debug(char* fmt,...)
{
        va_list p;
        va_start(p,fmt);

#ifdef _WIN32_WCE
        SYSTEMTIME st;
        GetLocalTime(&st);
        FILE* log;
        log = fopen(".\\Log.txt", "a+");
        if (log == NULL){
                return;
        }
        fprintf(log,"[===%04d-%02d-%02d %02d:%02d:%02d===]\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
        vfprintf(log, fmt,p);
        fclose(log);
#else
        char buf[40960];
        vsprintf(buf,fmt,p);
        if (strlen(buf)<=0) return ;
        printf(buf);
#endif
        va_end(p);
}
*/
void Log(char* fmt,...)
{
        va_list p;
        va_start(p,fmt);
        
        FILE* log;
        log = fopen("log.txt", "a+");
        if (log == NULL){
                return;
        }
        vfprintf(log, fmt,p);
        fclose(log);
        va_end(p);
}

void Log(wchar_t* s)
{
        FILE* log;
        log = fopen("weather.txt", "a+");
        if (log == NULL){
                return;
        }
        fwprintf(log, s);
        fclose(log);
}

void INFO(char* f,...)
{
        int l = strlen(f)+512;
        AStr res;

        va_list ap;
        va_start(ap,f);

        while(1){
                res.buffer(l);
                res.length() = _vsnprintf( *res, res.alength(), f, ap );
                if (res.length()!=-1) break;
                l+=l;
        }
        va_end(ap);
        MessageBox(GetActiveWindow(),*CA2T(res),L"Info",MB_SETFOREGROUND);
}

int FileExists( const AStr& f)
{
        if (f.ise()) return 0;
        DWORD fa = GetFileAttributes(CA2T(f));
        if (fa==-1) return 0;
        if ((fa & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY) return 2;
        return 1;
}