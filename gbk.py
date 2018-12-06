import sublime, sublime_plugin
import os

cnt = 3
gbuf = ''

def log(msg):
    print(msg)
    pass

def GetView():
    window = sublime.active_window()
    return window.active_view() if window != None else None

def isView(id):
    if not id: return false
    view = GetView()
    return (view is not None and view.id() == id)

def SetVar(var, value):
    view = GetView()
    if(isView(view.id())):
        view.set_status(var, value)

def GetVar(var):
    view = GetView()
    if(isView(view.id())):
        value = view.get_status(var)
        return value
    return ''

def DelVar(var):
    view = GetView()
    if(isView(view.id())):
        view.erase_status(var)

def done():
    view = GetView()
    sets=view.settings()
    view.run_command('to_utf8')

def ToUTF8():
    view = GetView()
    print('ToUTF8 is runing...')
    view.run_command('to_utf8')

def file_encoding(view):
    file_name = view.file_name()
    if not file_name: return
    if not os.path.exists(file_name):
        return
    sets = view.settings()
    if sets.has(file_name+'_ISGBKFILE'):
        SetVar('_ISGBKFILE','GBK')
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
        try:
            buf.decode('gbk')
            SetVar('_ISGBKFILE','GBK')
            sets.set(file_name+'_ISGBKFILE','GBK')
            view.set_encoding('UTF-8')
            return 'GBK'
        except UnicodeDecodeError as e:
            return 'UNKNOWN'

class FromUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        if isView(view.id()):
            global cnt
            cnt = 0
            view.set_scratch(True)
            reg_all = sublime.Region(0, view.size())
            text = view.substr(reg_all)
            text = text.replace('\n','\r\n')
            text = text.encode('gbk')
            if view.file_name() and os.path.exists(view.file_name()):
                fp = open(view.file_name(),'wb')
                fp.write(text)
                fp.close()

class ToUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        sets = view.settings()
        if isView(view.id()) and view.file_name() and os.path.exists(view.file_name()):
            view.set_scratch(True)
            fp = open(view.file_name(),'rb')
            buf = fp.read()
            fp.close()
            text = buf.decode('gbk')
            text = text.replace('\r\n','\n').replace('\r','\n')
            reg_all = sublime.Region(0, view.size())
            vp = view.viewport_position()
            view.set_viewport_position((0,0),False)
            view.replace(edit,reg_all,text)
            view.set_encoding('UTF-8')
            view.set_viewport_position(vp,False)
            if sets.has('rs'):
                rs = sets.get('rs')
                print('rs: ',rs)
                sel = view.sel()
                sel.clear()
                for x in rs:
                    sel.add(sublime.Region(x[0],x[1]))
                sets.erase('rs')

class SaveUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        if isView(view.id()):
            view.run_command('save',{"encoding": "utf-8"})

class PluginEventListener(sublime_plugin.EventListener):
    def on_load(self, view):
        if file_encoding(view)=='GBK':
            view.run_command('to_utf8')
    
    def on_post_save(self, view):
        sets = view.settings()
        if  file_encoding(view)=='GBK':
            view.run_command('from_utf8')
    
    def on_close(self, view):
        if file_encoding(view)=='GBK':
            view.run_command('from_utf8')

    def on_activated(self, view):
        if file_encoding(view)=='GBK':
            view.run_command('to_utf8')
            
    def on_pre_save(self,view): #on_selection_modified
        if file_encoding(view)=='GBK':
            sets= view.settings()
            sel = view.sel()
            rs = [(x.a, x.b) for x in sel]
            sets.set('rs',rs)

    def on_modified(self,view):
        global cnt
        if cnt==1 or cnt==5:
            view.set_scratch(False)
        if cnt<5:
            cnt=cnt+1
        sets = view.settings()
        if cnt==2 and file_encoding(view)=='GBK':
            #sublime.set_timeout(done,0)
            view.run_command('to_utf8')
            pass
