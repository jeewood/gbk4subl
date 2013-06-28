//#include "stdafx.h"
#pragma once

#ifndef UNICODE
#define UNICODE
#define _UNICODE_DEFINED_JWZ
#endif

#include "Windows.h"
#include "stdio.h"

#define TOWORD(a, b)            ((WORD)(((BYTE)(b)) | ((WORD)((BYTE)(a))) << 8))
#define TODWORD(a, b, c)    ((DWORD)(((WORD)(a)<<16) | ((DWORD)((WORD)(TOWORD(b,c))))))
#define B(s) ((BYTE)(s))

template<class T>
class String
{
private:
        #define TL sizeof(T)
        struct _td
        {
                T * d;
                int l;
                int al;
        } data;

        void Init()
        {
                data.d = NULL;
                data.l = 0;
                data.al = 0;
        }

        T* alloc(int size)                                                      { return alloc(size,false); }
        T* alloc(int size, bool f)
        {
                T* ns = data.d;
                if (size<=0) return ns;
                if (f || data.al<size)
                {
                        data.al = size;
                        int i=0;
                        while(i++<10)
                        {
                                if (data.d)
                                {
                                        ns = (T*)malloc((data.al+1)*TL);
                                        if (ns) 
                                        {
                                                if (data.al<data.l) data.l = data.al;
                                                MemCpy(ns,data.d,data.l);
                                                MemCpy(ns+data.l,0,data.al-data.l+1);
                                                free(data.d);
                                                data.d = ns;
                                        }
                                }
                                else
                                {
                                        ns = (T*)malloc((data.al+1)*TL);
                                        if (ns)
                                        {
                                                MemCpy(ns,0,data.al+1);
                                                data.d = ns;
                                        }
                                }
                                if (ns) break;
                        }
                }
                return ns;
        }

        static bool _chisupr(T c)
        {
                return (c>='A' && c<='Z');
        }

        static bool _chislwr(T c)
        {
                return (c>='a' && c<='z');
        }

        static T _upr(T c)
        {
                return (c>='a' && c<='z')? c-'a'+'A' : c;
        }

        static T _lwr(T c)
        {
                return (c>='A' && c<='Z')? c-'A'+'a' : c;
        }

        static int _cmpch(T c, T d, bool ig)
        {
                if (ig) return _upr(c)-_upr(d);
                return c-d;
        }

        static int _cmp(const T* d, const T* s, int l, bool ig)
        {
                if ((!d && !s) || l==0) return 0;
                if (!d || !s) return d?d[0]:-s[0];

                int ld = StrLen(d);
                int ls = StrLen(s);
                if (l==-1) l=(ld<=ls)?ls:ld;
                int r=0,i;
                for(i=0;i<l && d[i] && s[i];i++)
                {
                        if (r=_cmpch(d[i],s[i],ig)) return r;
                }
                if (i==l) return 0;
                return d[i]-s[i];
        }

        int _find(const T* s, int o,bool ig) const
        {
                int ls = StrLen(s);

                if (!data.d || !s || o>data.l || ls>data.l) return -1;
                
                T* d = data.d+o;
                while(*d || d<data.d+data.l)    //J2ME¼ÓÈë|| d<data.d+data.l
                {
                        if (_cmpch(*d,s[0],ig)==0)
                        {
                                if (_cmp(d,s,ls,ig)==0) 
                                        return d-data.d;
                        }
                        d++;
                }
                return -1;
        }

        int _find(const T* s, const T* ne, int &o,bool ig) const
        {
                int ls = StrLen(s);
                int le = StrLen(ne);
                if (!data.d || !s || o>data.l || ls>data.l) return -1;
                
                T* d = data.d+o;
                while(*d || d<data.d+data.l)    //J2ME¼ÓÈë|| d<data.d+data.l
                {
                        if (_cmpch(*d,s[0],ig)==0)
                        {
                                if (_cmp(d,s,ls,ig)==0)
                                {
                                        o = d-data.d;
                                        return 1;
                                }
                        }
                        if (ne && _cmpch(*d,ne[0],ig)==0)
                        {
                                if (_cmp(d,ne,le,ig)==0)
                                {
                                        o = d - data.d;
                                        return -1;
                                }
                        }
                        if (d<data.d+data.l)d++;
                }
                return 0;
        }

        int _findr(const T* s,int o,bool ig) const
        {
                int ls = StrLen(s)-1;
                if (!s || !data.d || o>data.l || ls>data.l) return -1;
                int ld = data.l-1-o;
                
                while(ld>=ls)
                {
                        if (_cmpch(data.d[ld],s[ls],ig)==0)
                        {
                                if (_cmp(&data.d[ld-ls],s,ls+1,ig)==0) return ld;//data.l-(ld+1);
                        }
                        ld --;
                }
                return -1;
        }

        String& _replace(const T* s, const T* r, bool ig) 
        { 
                int ls = StrLen(s);
                int lr = StrLen(r);
                if (!s || !data.d) return *this;
                int i=0,li=0,cnt=0;
                i=_find(s,li,ig);
                if (i!=-1)
                {
                        _set(i,ls,r,lr);
                        li = i+lr;

                        while((i=_find(s,li,ig))!=-1)
                        {
                                _set(i,ls,r,lr);
                                li = i+lr;
                        }
                }

                return *this; 
        }

        //can use 0-9 instead of 0123456789, a-z instead of a...z
        //-9 || a- is not permission
        bool _cIs(T c, const String& s) const
        {
                if (s.ise()) return false;
                int i=0,n=1;

                while(i<s.len())
                {
                        if (s[n]=='-' && s[n+1]!='-')
                        {
                                if (s[i]=='-')
                                {
                                        if (c=='-') return true;
                                        i++;n++;
                                }
                                i++;n++;
                                continue;
                        }
                        if (s[i]=='-')
                        {
                                if (i>0 && s[i-1]<=c && c<=s[n]) return true;
                                i+=2;n+=2;
                                continue;
                        }
                        if (s[i]==c) return true;
                        i++; n++;
                }
                return false;
        }

        bool _set(int i, int l, const T* s, int n) 
        {
                if (!data.d || i<0 || l<0 || n<0) return false;
                if (n>StrLen(s)) n=StrLen(s);

                if (0 < n-l)
                        alloc(data.l+(n-l));
                MemMove(data.d+i+n,data.d+i+l,data.l-(l+i));
                if (s)
                        MemCpy(data.d+i,s,n);
                data.l += n-l;
                data.d[data.l] = 0;
                return true;
        }

        String _mid(int i, int l) const
        {
                String t;
                if (l==-1) l = data.l-i;
                if (data.d && (i+l<=data.l))
                        t.assign(data.d+i,l);
                return t;
        }

        bool _match(T* b, T* e, int &ib, int &ie, int ob, int oe)
        {
                int lb = StrLen(b);
                int le = StrLen(e);
                int cb=0,ibt=0,iet=0;
                //ib = 0;
                //ie = data.l;
                if (!b && !e) return true;
                if (!b || !e)
                {
                        if (b)
                        {
                                ib = _find(b,ob,false);
                                if (ib==-1) return false;                               //begin string is not empty,but not found! ib & ie is invaild.
                        }

                        if (e)
                        {
                                ie = _find(e,ib,false);
                                if (ie==-1 || ie>oe) return false;              //end string is not empty,but not found! ib & ie is invaild.
                        }
                                
                        return true;
                }
                // begin & end string are not empty.
                ib = _find(b,ob,false);
                if (ib==-1) return false;
                cb++;
                ibt = ib+lb;
                while(1)
                {
                        iet = _find(e,b,ibt,false);                                     //find s return 1, find ne return -1, otherwise return 0;
                        if (iet==-1) 
                        {
                                cb++;
                                ibt += lb;
                        }
                        if (iet== 0) return false;
                        if (iet== 1)
                        {
                                if (cb>0) cb--;
                                if (!cb) break;
                                ibt+=le;
                        }
                }
                ie = ibt;
                if (ie>oe) return false;
                return true;
        }

        String _get(T* b, T* e, T* ps, T* pe, int i,bool ig,int* o=0)
        {
                String t;
                int original = 0;
                if (o) original = *o;
                if (data.d+original && (b || e))
                {
                        int f = (i/4);

                        int ips=original,lps=StrLen(ps);                //Position of Presearched start string, length of Presearched start string
                        int ipe=data.l-1,lpe=StrLen(pe);                //Position of Presearched end string, length of Presearched end string
                        int ie=data.l-1,le=StrLen(e);                   
                        int ib=original,lb=StrLen(b);
                        if (!_match(ps,pe,ips,ipe,ib,ie)) 
                        {
                                if (o) *o = data.l;
                                return t;
                        }
                        if (!_match(b,e,ib,ie,ips+lps,ipe))
                        {
                                if (b)
                                {
                                        ib = _find(b,ips+lps,ig);
                                        if (ib==-1) 
                                        {
                                                if (o) *o = data.l;
                                                return t;
                                        }
                                }
                                if (e)
                                {
                                        ie = (f<=1)?_find(e,ib+lb,ig):oneof(e,ib+lb);
                                        if (ie==-1 || ie>ipe)
                                        {
                                                if (o) *o = data.l;
                                                return t;
                                        }
                                }
                        }
                        switch(i%4)
                        {
                        case 0: t.assign(data.d+ib+lb,ie-(ib+lb));              if (f%2) _set(ib+lb,ie-(ib+lb),0,0); break;             //0 ²»º¬b,e
                        case 1: t.assign(data.d+ib,ie+le-ib);                   if (f%2) _set(ib,ie+le-ib,0,0); break;                  //1 º¬b,e       
                        case 2: t.assign(data.d+ib,ie-ib);                              if (f%2) _set(ib,ie-ib,0,0); break;                             //2 º¬b,²»º¬e
                        case 3: t.assign(data.d+ib+lb,ie+le-(ib+lb));   if (f%2) _set(ib+lb,ie+le-(ib+lb),0,0); break;  //3 ²»º¬b,º¬e
                        }
                        if (o) *o = ie+le;
                }
                return t;
        }

        String _getoneof(const String& oneofch, int o)
        {
                int i;
                T* b = NULL;
                T* bl = NULL;
                for(i=o;i<data.l;i++)
                {
                        if (_cIs(data.d[i],oneofch))
                        {
                                if (!b)
                                {
                                        b = &data.d[i];
                                }
                        }
                        else if (b)
                        {
                                bl = &data.d[i];
                                break;
                        }
                }
                String r;
                if (b)
                {
                        if (bl)
                        {
                                r.assign(b,bl-b);
                        }
                        else
                        {
                                r.assign(b,&data.d[data.l]-b);
                        }
                }
                return r;
        }

        String _left(int l, bool rm) 
        { 
                String t;
                if (l==0) return t;
                if (l<0 && data.l+l>0) l = data.l+l; 
                if (data.d)
                {
                        t.assign(data.d,l); 
                        if (rm) _set(0,l,0,0);
                }
                return t;
        }

        String _right(int l, bool rm) 
        { 
                String t; 
                if (l==0) return t;
                if (l<0 && data.l+l>0) l = data.l+l; 
                if (data.d)
                {
                        t.assign(data.d+(data.l-l),l); 
                        if (rm) _set(data.l-l,l,0,0);
                }
                return t;
        }

        String& _trim(const T* s, int d)
        {
                int i = 0, j= data.l-1;
                if (s && data.d)
                {
                        if (d==0 || d==1)
                        {
                                for(;i<data.l;i++)
                                {
                                        if (!_cIs(data.d[i],s)) break;
                                }
                        }
                        if (d==0 || d==2)
                        {
                                for(;j>=0;j--)
                                {
                                        if (!_cIs(data.d[j],s)) break;
                                }
                        }
                        if (i || j!=data.l-1)
                                assign(&data.d[i],j-i+1);
                }
                return *this;
        }

        String& _uplwr(bool u) 
        {
                if (data.d)
                {
                        for(int i=0;i<data.l;i++)
                        {
                                if (u)
                                {
                                        if (_chislwr(data.d[i])) data.d[i]=data.d[i]-'a'+'A';
                                }
                                else
                                {
                                        if (_chisupr(data.d[i])) data.d[i]=data.d[i]-'A'+'a';
                                }
                        }
                }
                return *this;
        }

        __int64 toi64() const 
        {
                __int64 r = 0;
                if (data.d && data.l)
                {
                        String p;
                        p.addch('0');p.addch('-');p.addch('9');
                        p.addch('-');p.addch('-');p.addch('+');
                        if (In(p))
                        {
                                int i = 0;
                                bool neg = false;
                                if (data.d[0]=='-')
                                {
                                        neg = true;
                                        i++;
                                }
                                if (data.d[0]=='+') i++;

                                for(;i<data.l;i++)
                                        r = r*10+(data.d[i]-'0');
                                if (neg) r = -r;
                        }
                }
                return r;
        }

        __int64 hex2i64() const
        {
                __int64 r = 0;
                String p;
                p.addch('0');p.addch('-');p.addch('9');
                p.addch('a');p.addch('-');p.addch('f');
                p.addch('A');p.addch('-');p.addch('F');
                if (data.d && data.l && In(p))
                {
                        for(int i=0;i<data.l;i++)
                        {
                                int j = 0;
                                if (data.d[i]>='0' && data.d[i]<='9')
                                        j = data.d[i]-'0';
                                else if (data.d[i]>='A' && data.d[i]<='F')
                                        j = data.d[i]-'A'+10;
                                else
                                        j = data.d[i]-'a'+10;

                                r = r*16+j;
                        }
                }
                return r;
        }

        bool _assign(const T* s, int l, bool a)
        {
                int ls = StrLen(s);
                if (ls<0) return false; //jwz debug
                if (l<0) l = ls;
                if (a)
                {
                        if (data.l+l > data.al) alloc(data.l+l); //J2ME¼ÓÈë+1
                        if (data.d)
                        {
                                if (s && data.l+l<=data.al)
                                        MemCpy(data.d+data.l,s,l);//((ls>l && l>0)?l:ls));
                                data.l += l;
                                data.d[data.l] = '\0';
                        }
                }
                else
                {
                        if (l > data.al) alloc(l); //J2ME¼ÓÈë
                        if (data.d)
                        {
                                if (s && l<=data.al)
                                        MemCpy(data.d,s,l);//((ls>l && l>0)?l:ls));
                                data.l = l;
                                data.d[data.l] = '\0';
                        }
                }

                return true;
        }
        
        static T* MemCpy(T* dst, const T* src, int n)
        {
                if (!dst || n<=0) return dst;
                T* r = dst;
                if (!src)
                {
                        while (n--)
                                *r++ = '\0';
                        return r;
                }
                while(n-->0)
                        *r++ = *src++;
                return dst;
        }

        static T* MemMove(T* dst, const T* src, int n)
        {
                register T* ret = dst;
                if (dst>src)
                {
                        for(src+=n,ret+=n;n--;)
                                *(--ret) = *(--src);
                }
                else
                {
                        for(;n--;)
                                *(ret++) = *(src++);
                }
                return ret;
        }

        static int StrLen(const T* s)
        {
                int i = 0;
                if ((int)s>0 && (int)s<100)
                {
                        i = -1;
                        return i;
                }
                if (s)
                        while(*s++) i++;
                return i;
        }

public:

        bool assign(T c,bool a=false)                                                                   { return assign((String)c,a); }
        bool assign(const String& s, bool a=false)                                              { return _assign(s.data.d, s.data.l,a); }
        bool assign(const T* s,bool a=false)                                                    { return _assign(s,StrLen(s),a); }
        bool assign(const T* s, int l, bool a=false)                                    { return _assign(s,l,a); }

        bool addch(T c)                                                                                                 { return _assign((String)c,1,true); }
        bool nchar(T c, int l, bool a=false)                                                    { if (!a) clear(); while(l>0 && l--) addch(c); return true; }

        String()                                                                                                                { Init(); }
        String(T c)                                                                                                             { Init(); alloc(1); data.d[0] = c; data.l = 1; }
        String(int l)                                                                                                   { Init(); alloc(l); data.l = l; }
        String(const T* s)                                                                                              { Init(); assign(s,StrLen(s),false);}
        String(const String& s)                                                                                 { Init(); assign(s.data.d,s.data.l,false); }
        String(const T* s, int l)                                                                               { Init(); assign(s,l,false); }

        ~String()                                                                                                               { if (data.d) { free(data.d); } Init(); }

        int oneof(const T* s) const                                                                             { return oneof(s,0); }
        int oneof(const T* s, int o) const                                                              
        { 
                if (s && data.d && o<data.l) 
                { 
                        for(int i=o; i<data.l;i++) 
                        { 
                                if (_cIs(data.d[i],s)) return i; 
                        } 
                } 
                return -1; 
        }

        int find(const T* s) const                                                                              { return _find(s,0,false); }
        int find(const T* s,int o) const                                                                { return _find(s,o,false); }
        int find_i(const T* s) const                                                                    { return _find(s,0,true); }
        int find_i(const T* s,int o) const                                                              { return _find(s,o,true); }

        int findr(const T* s) const                                                                             { return _findr(s,0,false); }
        int findr(const T* s,int o) const                                                               { return _findr(s,o,false); }
        int findr_i(const T* s) const                                                                   { return _findr(s,0,true); }
        int findr_i(const T* s,int o) const                                                             { return _findr(s,o,true); }

        bool set(int i, int l, const T* s, int n)                                               { return _set(i,l,s,n); }
        bool insert(int i, const T* s, int n=0)                                                 { if (!n || n>StrLen(s))n=StrLen(s); return _set(i,0,s,n); }
        bool insert(const T *s, const T *d, int n=0)
        {
                if(!n || n>StrLen(d))n=StrLen(d);

                int i=find(s,0);
                if (i==-1) return false;
                return _set(i+StrLen(s),0,d,n);
        }
        String& replace(T *b, T *e,const T *r)                                                  { return replace(b,e,r,1); }
        String& replace(T *b, T *e,const T *r, int i)                                   { String s=_get(b,e,NULL,NULL,i,false,0); return _replace(*s,r,false); }
        String& replace(const T* s, const T* r)                                                 { return _replace(s,r,false); }
        String& replace_i(const T* s, const T* r)                                               { return _replace(s,r,true); }

        String& replaceoneof_all(const String& s, const T* r)                   
        { 
                int i = data.l; 
                while(1)
                { 
                        _replace(getoneof(s,0),r,true); 
                        if(i==data.l) break;
                        i = data.l;
                } 
                return *this; 
        }

        String get(T* b, T* e)                                                                                  { return _get(b,e,NULL,NULL,0,false,0); }
        String get(T* b, T* e, int i)                                                                   { return _get(b,e,NULL,NULL,i%4,false,0); }
        String get(T* b, T* e, T* p)                                                                    { return _get(b,e,p,NULL,0,false,0); }
        String get(T* b, T* e, T* p, int i)                                                             { return _get(b,e,p,NULL,i%4,false,0); }

        String get_i(T* b, T* e)                                                                                { return _get(b,e,NULL,NULL,0,true,0); }
        String get_i(T* b, T* e, int i)                                                                 { return _get(b,e,NULL,NULL,i%4,true,0); }
        String get_i(T* b, T* e, T* p)                                                                  { return _get(b,e,p,NULL,0,true,0); }
        String get_i(T* b, T* e, T* p, int i)                                                   { return _get(b,e,p,NULL,i%4,true,0); }
        
        String get1of(T* b, T* e)                                                                               { return _get(b,e,NULL,NULL,8,false,0);}
        String get1of(T* b, T* e, int i)                                                                { return _get(b,e,NULL,NULL,i%4+8,false,0);}
        String get1of(T* b, T* e, T* p)                                                                 { return _get(b,e,p,NULL,8,false,0);}
        String get1of(T* b, T* e, T* p, int i)                                                  { return _get(b,e,p,NULL,i%4+8,false,0);}
        
        String get(int p1, int p2)                                                                              { return _mid(p1,p2-p1); }

        String get(const String& b, int p)
        {
                String r;
                int p1 = find(b);
                if (p1==-1) return r;
                
                return _mid(p1+b.len(),p-p1);
        }


        String get(T* b, T* e, T* ps, T* pe)                                                    { return _get(b,e,ps,pe,0,false,0); }
        String get(T* b, T* e, T* ps, T* pe, int i)                                             { return _get(b,e,ps,pe,i,false,0); }
        String get(T* b, T* e, T* ps, T* pe, bool ig)                                   { return _get(b,e,ps,pe,0,ig,0); }
        String get(T* b, T* e, T* ps, T* pe, int i,bool ig)                             { return _get(b,e,ps,pe,i,ig,0); }
        String get(T* b, T* e, T* ps, T* pe, int i,bool ig, int *o)             { return _get(b,e,ps,pe,i,ig,o); }
        String getoneof(const String & p, int i)                                                { return _getoneof(p,i); }
        String Tokenize(const String& t, int &o)                                                
        {
                String r;
                int i=_find(t,o,false);
                if (i!=-1)
                {
                        r.assign(data.d+o,i-o);
                        o = i+t.len();
                }
                else
                {
                        r.assign(data.d+o,data.l-o);
                        o = data.l;
                }
                return r;
        }
        
        String mid(int i, int l) const                                                                  { return _mid(i,l); }
        String mid(int i) const                                                                                 { return _mid(i,data.l-i); }
        String mid(const T* s) const                                                                    { String t; int i=find(s); int l=StrLen(s); if (i!=-1) return _mid(i+l,data.l-(i+l)); return t; }

        String fetch(int i,int l)                                                                               { String t = mid(i,l); _set(i,l,0,0);  return t; }
        String fetch(T* b, T* e)                                                                                { return fetch(b,e,0,0); }
        String fetch(T* b, T* e, int i)                                                                 { return fetch(b,e,0,i); }
        String fetch(T* b, T* e, T* p)                                                                  { String t = _get(b,e,p,NULL,4,false); return t; }
        String fetch(T* b, T* e, T* p, int i)                                                   { String t = _get(b,e,p,NULL,i%4+4,false); return t; }
        String fetch_i(T* b, T* e)                                                                              { return fetch_i(b,e,0,0); }
        String fetch_i(T* b, T* e, int i)                                                               { return fetch_i(b,e,0,i); }
        String fetch_i(T* b, T* e, T* p)                                                                { String t = _get(b,e,p,NULL,4,true); return t; }
        String fetch_i(T* b, T* e, T* p, int i)                                                 { String t = _get(b,e,p,NULL,i%4+4,true); return t; }

        String lfetch(int l)                                                                                    { return _left(l,true); }
        String lfetch(const T* s)                                                                               { int i = find(s);return _left(i==-1?0:i,true); }
        String lfetch_i(const T* s)                                                                             { int i = find_i(s);return _left(i==-1?0:i,true); }
        String rfetch(int l)                                                                                    { return _right(l,true); }
        String rfetch(const T* s)                                                                               { int i = findr(s);return _right(i==-1?data.l:data.l-1-i,true); }
        String rfetch_i(const T* s)                                                                             { int i = findr_i(s);return _right(i==-1?data.l:data.l-1-i,true); }

        String left(int l)                                                                                              { return _left(l,false); }
        String left(const T* s)                                                                                 { int i = find(s);return _left(i==-1?0:i,false); }
        String left(const T* s) const                                                                   { int i = find(s);return _mid(0,i==-1?0:i); }
        String left_i(const T* s)                                                                               { int i = find_i(s,false);return _left(i==-1?0:i); }

        String right(int l)                                                                                             { return _right(l, false); }
        String right(const T* s)                                                                                { int i = findr(s);return _right(i==-1?data.l:data.l-1-i, false); }
        String right_i(const T* s)                                                                              { int i = findr_i(s);return _right(i==-1?data.l:data.l-1-i,false); }

        #define U(s) (s).toupper()
        String& toupper()                                                                                               { return _uplwr(true); } 
        
        #define L(s) (s).tolower()
        String& tolower()                                                                                               { return _uplwr(false); }

        String& ltrim(const T* s)                                                                               { return _trim(s,1); }
        String& rtrim(const T* s)                                                                               { return _trim(s,2); }
        String& trim(const T* s)                                                                                { return _trim(s,0); }

        bool In(const T* ps) const                                                                              { for(int i=0; i<data.l; i++) { if (!_cIs(data.d[i],ps)) return false; } return true; }

        bool isAlpha() const                                                                                    { String p;p.addch('a');p.addch('-');p.addch('z');p.addch('A');p.addch('-');p.addch('Z');return In(p); }
        bool isDigit() const                                                                                    { String p;p.addch('0');p.addch('-');p.addch('9');p.addch('.');return In(p); }
        bool isHexDigit() const                                                                                 { String p;p.addch('0');p.addch('-');p.addch('9');p.addch('a');p.addch('-');p.addch('f');p.addch('A');p.addch('-');p.addch('F');return In(p); }
        bool isPrint() const                                                                                    { String p;p.addch(' ');p.addch('-');p.addch('~');return In(p); }
        bool ise() const                                                                                                { return (data.l==0 || !data.d); }
        bool isne() const                                                                                               { return (data.l!=0); }
        bool include(const T* s) const                                                                  { return (find(s)!=-1); }
        bool include_i(const T* s) const                                                                { return (find_i(s)!=-1); }
        bool startwith(const T* s) const                                                                { return _cmp(data.d,s,StrLen(s),false)==0; }
        bool startwith_i(const T* s) const                                                              { return _cmp(data.d,s,StrLen(s),true)==0; }
        bool endwith(const T* s) const                                                                  { int ls = StrLen(s); return _cmp(data.d+(data.l-ls),s,ls,false)==0; }
        bool endwith_i(const T* s) const                                                                { int ls = StrLen(s); return _cmp(data.d+(data.l-ls),s,ls,true)==0; }

        int compare(const T* s) const                                                                   { return _cmp(data.d,s,-1,false); }
        static int compare(const T* s, const T* d)                                              { return _cmp(s,d,-1,false); }

        T* buffer(int l)                                                                                                { alloc(l,true); return (T*)data.d; }
        void fixed()                                                                                                    { alloc(data.l,true); }

        int& length()                                                                                                   { return data.l; }
        int len() const                                                                                                 { return data.l; }
        int alength() const                                                                                             { return data.al; }


        int toi() const                                                                                                 { return (int)toi64(); }
        long tol() const                                                                                                { return (long)toi64(); }
        int htoi() const                                                                                                { return (int)hex2i64(); }
        long htol() const                                                                                               { return (long)hex2i64(); }

        T* operator* () const                                                                                   { return (T*)data.d; }
        operator T*() const                                                                                             { return (T*)data.d; }
        T* c_str() const                                                                                                { return (T*)data.d; }

        void clear()                                                                                                    { if (data.al>128) empty(); else if (data.d) { memset(data.d,0,sizeof(T)*data.al+1); data.l = 0; } }
        void empty()                                                                                                    { if (data.d) free(data.d); Init(); }

        T& operator[](int idx)                                                                                  { if (idx>data.al) alloc(idx); if (idx>data.l) data.l = idx; return data.d[idx]; }

        const String& operator=(const T c)                                                              { assign(c,false); return *this; }
        //const String& operator=(const T* s)                                                           { if (data.d!=s)assign(s,StrLen(s)); return *this; }
        const String& operator=(const String& s)                                                { if (this!=&s) assign(s,s.len(),false); return *this; }

        const String& operator+=(const T c)                                                             { assign(c,true); return *this; }
        //const String& operator+=(const T* s)                                                  { if (data.d==s) { String bs(s); assign(bs,true); } else assign(s,StrLen(s),true); return *this; }
        const String& operator+=(const String& s)                                               { if (this==&s) { String bs(s); assign(bs,true); } else assign(s,true); return *this; }
        
    friend String operator+(const String& s1, const String& s2)         { String r = s1; r.assign(s2,true); return r; }
        friend String operator+(const String& s1, const T* s2)                  { String r = s1; String r1=s2; r.assign(r1,true); return r; }
        friend String operator+(const T* s1, const String& s2)                  { String r = s1; r.assign(s2,true); return r; }
    friend String operator+(const String& s, T c)                                       { return s+(String)c; }
    friend String operator+(T c, const String& s)                                       { return (String)c+s; }

        friend bool operator==(const String& s1, const String& s2)              { return (_cmp(*s1,*s2,-1,false)==0); }
        friend bool operator==(const String& s1, const T* &s2)                  { return (_cmp(s1.data.d,(T*)s2,-1,false)==0); }
        friend bool operator==(const String& s1, T* s2)                                 { return (_cmp(s1.data.d,(T*)s2,-1,false)==0); }
        friend bool operator==(const T* &s1, const String& s2)                  { return (_cmp(s1,s2.data.d,-1,false)==0); }
        friend bool operator==(T* s1, const String& s2)                                 { return (_cmp((T*)s1,s2.data.d,-1,false)==0); }

        friend bool operator!=(const String& s1,const String& s2)               { return (_cmp(s1.data.d,s2.data.d,-1,false)!=0); }
        friend bool operator!=(const String& s1,const T* &s2)                   { return (_cmp(s1.data.d,(T*)s2,-1,false)!=0); }
        friend bool operator!=(const String& s1,T* s2)                                  { return (_cmp(s1.data.d,(T*)s2,-1,false)!=0); }
        friend bool operator!=(const T* &s1, const String& s2)                  { return (_cmp(s1,s2.data.d,-1,false)!=0); }
        friend bool operator!=(T* s1, const String& s2)                                 { return (_cmp((T*)s1,s2.data.d,-1,false)!=0); }

        friend bool operator>(const String& s1, const String& s2)               { return (_cmp(s1.data.d,s2.data.d,-1,false)>0); }
        friend bool operator>(const String& s1, const T* &s2)                   { return (_cmp(s1.data.d,(T*)s2,-1,false)>0); }
        friend bool operator>(const String& s1, T* s2)                                  { return (_cmp(s1.data.d,(T*)s2,-1,false)>0); }
        friend bool operator>(const T* &s1, const String& s2)                   { return (_cmp(s1,s2.data.d,-1,false)>0); }
        friend bool operator>(T* s1, const String& s2)                                  { return (_cmp((T*)s1,s2.data.d,-1,false)>0); }

        friend bool operator>=(const String& s1, const String& s2)              { return (_cmp(s1.data.d,s2.data.d,-1,false)>=0); }
        friend bool operator>=(const String& s1, const T* s2)                   { return (_cmp(s1.data.d,(T*)s2,-1,false)>=0); }
        friend bool operator>=(const String& s1, T* s2)                                 { return (_cmp(s1.data.d,(T*)s2,-1,false)>=0); }
        friend bool operator>=(const T* &s1, const String& s2)                  { return (_cmp(s1,s2.data.d,-1,false)>=0); }
        friend bool operator>=(T* s1, const String& s2)                                 { return (_cmp((T*)s1,s2.data.d,-1,false)>=0); }

        friend bool operator<(const String& s1, const String& s2)               { return (_cmp(s1.data.d,s2.data.d,-1,false)<0); }
        friend bool operator<(const String& s1, const T* &s2)                   { return (_cmp(s1.data.d,(T*)s2,-1,false)<0); }
        friend bool operator<(const String& s1, T* s2)                                  { return (_cmp(s1.data.d,(T*)s2,-1,false)<0); }
        friend bool operator<(const T* &s1, const String& s2)                   { return (_cmp(s1,s2.data.d,-1,false)<0); }
        friend bool operator<(T* s1, const String& s2)                                  { return (_cmp((T*)s1,s2.data.d,-1,false)<0); }

        friend bool operator<=(const String& s1, const String& s2)              { return (_cmp(s1.data.d,s2.data.d,-1,false)<=0); }
        friend bool operator<=(const String& s1, const T* &s2)                  { return (_cmp(s1.data.d,(T*)s2,-1,false)<=0); }
        friend bool operator<=(const String& s1, T* s2)                                 { return (_cmp(s1.data.d,(T*)s2,-1,false)<=0); }
        friend bool operator<=(const T* &s1, const String& s2)                  { return (_cmp(s1,s2.data.d,-1,false)<=0); }
        friend bool operator<=(T* s1, const String& s2)                                 { return (_cmp((T*)s1,s2.data.d,-1,false)<=0); }
};


typedef String<char> AStr;
typedef String<TCHAR> TStr;
typedef String<TCHAR> CStr;


TStr CA2T(AStr s, int cp=CP_ACP);
AStr CT2A(TStr s, int cp=CP_ACP);
AStr CA2U(AStr a);
AStr CU2A(AStr u);
AStr Hex2Bin(AStr H);
AStr Bin2Hex(AStr B, bool uc = true);
AStr jPrintf(const char* f, ...);
TStr jPrintf(const TCHAR* f, ...);
AStr jPrintfv(const char* f, va_list ap);
AStr i2s(int i);
AStr l2s(long i);
AStr ReadFile(const char* f);

TStr ReadIni(const AStr& file);
TStr ReadIni(const TStr& file);
bool WriteFile(const char* f, AStr d);
bool WriteFile(const char* f, TStr d);

void Debug(char* s);
//void Debug(char* fmt,...);
void Log(char* fmt,...);
void Log(wchar_t* s);

void INFO(char* fmt,...);

int FileExists(const AStr& f);

#ifdef _UNICODE_DEFINED_JWZ
#undef UNICODE
#undef _UNICODE_DEFINED_JWZ
#endif