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
    sets=sublime.load_settings(view.file_name())
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

def file_encoding(view):
    try:
        os.path.exists(view.file_name())
    except:
        return 'NODISKFILE'
    fp = open(view.file_name(),'rb')
    buf = fp.read(40960)
    fp.close()
    try:
        buf.decode('utf8')
        return 'UTF-8'
    except UnicodeDecodeError as e:
        try:
            buf.decode('gbk')
            SetVar('_ISGBKFILE','GBK')
            view.set_encoding('UTF-8')
            return 'GBK'
        except UnicodeDecodeError as e:
            if str.find(str(e),'40959')!=-1:
                SetVar('_ISGBKFILE','GBK')
                view.set_encoding('UTF-8')
                return 'GBK'
    return 'UNKNOWN'

class FromUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        if isView(view.id()):
            view.set_scratch(True)
            reg_all = sublime.Region(0, view.size())
            text = view.substr(reg_all)
            text = text.replace('\n','\r\n')
            text = text.encode('gbk')
            if view.file_name() and os.path.exists(view.file_name()):
                fp = open(view.file_name(),'wb')
                fp.write(text)
                fp.close()
                #view.set_scratch(True)

class ToUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        if isView(view.id()) and view.file_name() and os.path.exists(view.file_name()):
            global cnt
            cnt = 0
            view.set_scratch(True)
            fp = open(view.file_name(),'rb')
            buf = fp.read()
            fp.close()
            text = buf.decode('gbk')
            text = text.replace('\r\n','\n').replace('\r','\n')
            reg_all = sublime.Region(0, view.size())
            view.replace(edit,reg_all,text)

            sets=sublime.load_settings(view.file_name())
            
            if sets.has('_pos_layout_x') and sets.has('_pos_layout_y'):
                sublime.set_timeout(done,1)

class SaveUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        if isView(view.id()):
            view.run_command('save',{"encoding": "utf-8"})

class PluginEventListener(sublime_plugin.EventListener):
    def on_load(self, view):
        if isView(view.id()) and  GetVar('_ISGBKFILE')!='GBK' and file_encoding(view)=='GBK':
            view.set_scratch(True)
            view.run_command('to_utf8')
    
    def on_post_save(self, view):
        if isView(view.id()) and GetVar('_ISGBKFILE')=='GBK':
            view.run_command('from_utf8')
    
    def on_close(self, view):
        if isView(view.id()) and GetVar('_ISGBKFILE')=='GBK':
            view.run_command('from_utf8')

    def on_activated(self, view):
        if isView(view.id()) and (GetVar('_ISGBKFILE')=='' or view.encoding() != 'UTF-8') \
            and file_encoding(view)=='GBK':
            view.run_command('to_utf8')
    
    def on_text_command(self, view, command_name, args):
        if isView(view.id()) and (GetVar('_ISGBKFILE')=='' or view.encoding() != 'UTF-8') \
            and file_encoding(view)=='GBK':
            view.run_command('to_utf8')
    
    def on_pre_save(self,view): #on_selection_modified
        sets=sublime.load_settings(view.file_name())
        pos = view.sel()[0].begin()
        lpt = view.viewport_position()
        sets.set('_pos_layout_x',str(lpt[0]))
        sets.set('_pos_layout_y',str(lpt[1]))
        sets.set('_pos_x',str(pos))

    def on_modified(self,view):
        global cnt
        if cnt==1 :
            view.set_scratch(False)
        if cnt<5:
            cnt=cnt+1
        if isView(view.id()) and (GetVar('_ISGBKFILE')=='' or view.encoding() != 'UTF-8') \
            and file_encoding(view)=='GBK':
            view.run_command('to_utf8')
