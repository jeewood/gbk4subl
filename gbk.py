import sublime, sublime_plugin
import os

cnt = 3

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
    if sets.has('_pos_layout_x') and sets.has('_pos_layout_x'):
        lptx = float(sets.get('_pos_layout_x'))
        lpty = float(sets.get('_pos_layout_y'))
        view.set_viewport_position((lptx,lpty),False)
        sets.erase('_pos_layout_x')
        sets.erase('_pos_layout_y')
    if sets.has('_pos_x'):
        pos = int(sets.get('_pos_x'))
        view.sel().clear()
        view.sel().add(sublime.Region(pos,pos))
        sets.erase('_pos_x')

def ToUTF8():
    view = GetView()
    print('ToUTF8 is runing...')
    view.run_command('to_utf8')


def CheckUnicodeWithoutBOM(buf):
    bAllAscii=True
    nBytes = 0
    for ch in buf:
        if ch & 0x80 != 0:
            bAllAscii=False
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
            if (ch & 0xC0) != 0x80:
                return False
            nBytes-=1
    if nBytes > 0:
        return False
    if bAllAscii:
        return False
    return True

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
            #print('from_utf8:',cnt)
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
        if isView(view.id()) and view.file_name() and os.path.exists(view.file_name()):
            global cnt
            cnt = 0
            #print('to_utf8:',cnt)
            view.set_scratch(True)
            fp = open(view.file_name(),'rb')
            buf = fp.read()
            fp.close()
            text = buf.decode('gbk')
            text = text.replace('\r\n','\n').replace('\r','\n')
            reg_all = sublime.Region(0, view.size())
            view.replace(edit,reg_all,text)
            view.set_encoding('UTF-8')

class SaveUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        if isView(view.id()):
            view.run_command('save',{"encoding": "utf-8"})

class PluginEventListener(sublime_plugin.EventListener):
    def on_load(self, view):
        if file_encoding(view)=='GBK':
            sublime.set_timeout(done,1)
    
    def on_post_save(self, view):
        if  file_encoding(view)=='GBK':
            view.run_command('from_utf8')
    
    def on_close(self, view):
        if file_encoding(view)=='GBK':
            view.run_command('from_utf8')

    def on_activated(self, view):
        if file_encoding(view)=='GBK':
            sublime.set_timeout(done,1)
            
    def on_pre_save(self,view): #on_selection_modified
        if file_encoding(view)=='GBK':
            sets= view.settings()
            pos = view.sel()[0].begin()
            lpt = view.viewport_position()
            sets.set('_pos_layout_x',str(lpt[0]))
            sets.set('_pos_layout_y',str(lpt[1]))
            sets.set('_pos_x',str(pos))
            sets.set('_need_reload','True')

    def on_modified(self,view):
        global cnt
        if cnt==1 or cnt==5:
            view.set_scratch(False)
        if cnt<5:
            cnt=cnt+1
        if view.settings().has('_need_reload') and file_encoding(view)=='GBK':
            view.settings().erase('_need_reload')
            sublime.set_timeout(done,1)
            pass
