import sublime, sublime_plugin
import os

def SetStatusBar(view, var, value):
    view.set_status(var, value)

def GetStatusBar(view, var):
    return view.get_status(var)

def UTF8_nBOM(buf):
    bAllAscii=True
    nBytes = 0
    for ch in buf:
        if ch & 0x80 != 0: bAllAscii=False
        if nBytes==0:
            if ch>=0x80:
                if ch >=0xFC and ch <= 0xFD:
                    nBytes=6
                elif ch >= 0xF8:
                    nBytes=5
                elif ch >= 0xF0:
                    nBytes=4
                elif ch >= 0xE0:
                    nBytes=3
                elif ch >= 0xC0:
                    nBytes=2
                else:
                    return False
                nBytes-=1
        else:
            if (ch & 0xC0) != 0x80: return False
            nBytes-=1
    if nBytes > 0 or bAllAscii: return False
    return True

def Detect(view):
    file_name = view.file_name()
    if not file_name or not os.path.exists(file_name): return
    ad = GetStatusBar(view,'_ISGBKFILE') 
    if (ad!=''): return ad
    size=os.path.getsize(file_name)
    fp = open(file_name,'rb')
    buf=line=b''
    isCh=False
    while not isCh:
        line = fp.readline()
        if not line:break
        for ch in line:
            if ch>=0x80:
                isCh=True
                break
        if isCh: buf=line
    fp.close()
    ln = len(buf)
    if (ln>2 and buf[0]==0xef and buf[1]==0xbb and buf[2]==0xbf) or \
        (ln>1 and ((buf[0]==0xff and buf[1]==0xfe) or (buf[0]==0xfe and buf[1]==0xff))) or \
        ln==0 or UTF8_nBOM(buf):
        SetStatusBar(view,'_ISGBKFILE','UTF-8')
        return 'UTF-8'
    else:
        SetStatusBar(view,'_ISGBKFILE','GBK')
        return 'GBK'
    '''
    try:
        buf.decode('utf8')
        return 'UTF-8'
    except UnicodeDecodeError as e:
        buf.decode('gbk')
        SetStatusBar(view, '_ISGBKFILE','GBK')
        view.set_encoding('UTF-8')
        return 'GBK'
    '''

class FromUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        view.set_read_only(True)
        reg_all = sublime.Region(0, view.size())
        text = view.substr(reg_all)
        text = text.replace('\n','\r\n')
        try:
            text = text.encode('gbk')
            if view.file_name() and os.path.exists(view.file_name()):
                fp = open(view.file_name(),'wb')
                fp.write(text)
                fp.close()
        except UnicodeDecodeError as e:
            pass
        view.set_read_only(False)

class ToUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit, args):
        view = self.view
        if view.file_name() and os.path.exists(view.file_name()):
            view.set_scratch(True)
            try:
                fp = open(view.file_name(),'rb')
                buf = fp.read()
                fp.close()
                text = buf.decode('gbk')
                text = text.replace('\r\n','\n').replace('\r','\n')
                reg_all = sublime.Region(0, view.size())
                sel = view.sel()
                rs = [(x.a, x.b) for x in sel]
                vp = view.viewport_position()
                view.set_viewport_position((0,0),False)
                view.replace(edit,reg_all,text)
                view.set_encoding('UTF-8')
                view.set_viewport_position(vp,False)
                sel.clear()
                for x in rs:
                    sel.add(sublime.Region(x[0],x[1]))
            except UnicodeDecodeError as e:
                pass
            print('ToUtf8Command done view.id: ',view.id(), ' args:',args)

class PluginEventListener(sublime_plugin.EventListener):
    def on_load_async(self, view):
        if Detect(view)=='GBK':
            view.run_command('to_utf8',{'args':'on load async'})

    def on_post_save_async(self, view):
        if  Detect(view)=='GBK':
            view.run_command('from_utf8')
    
    def on_close(self, view):
        if Detect(view)=='GBK': 
            view.run_command('from_utf8')

    def on_activated_async(self, view):
        if Detect(view)=='GBK':
            cmd1 = view.command_history(-1,True)
            cmd0 = view.command_history(0,True)
            if cmd0[0]!='to_utf8' and not view.is_dirty():
                view.run_command('to_utf8',{'args':'on activated async'})
            
    def on_modified_async(self,view):
        if Detect(view)=='GBK':
            cmd1 = view.command_history(-1,True)
            cmd0 = view.command_history(0,True)
            if cmd0[0]!='to_utf8' and cmd0[0]!='revert':
                view.set_scratch(False)
            if cmd0[0]=='revert':
                view.run_command('to_utf8',{'args':'on modified async'})

