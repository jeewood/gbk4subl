import sublime, sublime_plugin
import os

def SetStatusBar(view, var, value):
    view.set_status(var, value)

def GetStatusBar(view, var):
    return view.get_status(var)

def Detect(view):
    file_name = view.file_name()
    if not file_name or not os.path.exists(file_name): return
    if (GetStatusBar(view,'_ISGBKFILE')=='GBK'):
        return 'GBK'
    size=os.path.getsize(file_name)
    fp = open(file_name,'rb')
    buf=line=b''
    isCh=False
    while not isCh:
        line = fp.readline()
        if not line:break
        for ch in line:
            if ch>=128:
                isCh=True
                break
        buf=line
    fp.close()

    try:
        buf.decode('utf8')
        return 'UTF-8'
    except UnicodeDecodeError as e:
        buf.decode('gbk')
        SetStatusBar(view, '_ISGBKFILE','GBK')
        view.set_encoding('UTF-8')
        return 'GBK'

class FromUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        view.set_read_only(True)
        reg_all = sublime.Region(0, view.size())
        text = view.substr(reg_all)
        text = text.replace('\n','\r\n')
        text = text.encode('gbk')
        if view.file_name() and os.path.exists(view.file_name()):
            fp = open(view.file_name(),'wb')
            fp.write(text)
            fp.close()
        view.set_read_only(False)

class ToUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit, args):
        view = self.view
        if view.file_name() and os.path.exists(view.file_name()):
            view.set_scratch(True)
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
            #print('ToUtf8Command done view.id: ',view.id(), ' args:',args)

class PluginEventListener(sublime_plugin.EventListener):
    def on_load_async(self, view):
        if Detect(view)=='GBK':
            pass

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
                view.run_command('to_utf8',{'args':'on_activated_async'})
            
    def on_modified_async(self,view):
        if Detect(view)=='GBK':
            cmd1 = view.command_history(-1,True)
            cmd0 = view.command_history(0,True)
            if cmd0[0]!='to_utf8' and cmd0[0]!='revert':
                view.set_scratch(False)
            if cmd0[0]=='revert':
                view.run_command('to_utf8',{'args':'on_modified_async'})

