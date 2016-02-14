import sublime, sublime_plugin
import os

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
            
def restore_position_cb():
    view = GetView()
    lptx = float(GetVar('_pos_layout_x'))
    lpty = float(GetVar('_pos_layout_y'))
    #print('point:',lptx,lpty)
    posa =  int(GetVar('_pos_x'))
    posb =  int(GetVar('_pos_y'))
    pos = sublime.Region(posa,posb)
    view.sel().add(pos)
    view.sel().subtract(view.sel()[0])
    view.set_viewport_position((lptx+1,lpty))
    view.set_viewport_position((lptx,lpty))
    DelVar('_pos_layout_x')
    DelVar('_pos_layout_y')

def file_encoding(view):
    try:
        os.path.exists(view.file_name())
    except:
        return 'NODISKFILE'
    fp = open(view.file_name(),'rb')
    buf = fp.read(4096)
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
            if str.find(str(e),'4095')!=-1:
                SetVar('_ISGBKFILE','GBK')
                view.set_encoding('UTF-8')
                return 'GBK'
    return 'UNKNOWN'

class FromUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        if isView(view.id()):
            reg_all = sublime.Region(0, view.size())
            text = view.substr(reg_all)
            text = text.replace('\n','\r\n')
            text = text.encode('gbk')
            if view.file_name() and os.path.exists(view.file_name()):
                fp = open(view.file_name(),'wb')
                fp.write(text)
                fp.close()
                view.set_scratch(True)

class ToUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        if isView(view.id()) and view.file_name() and os.path.exists(view.file_name()):
            fp = open(view.file_name(),'rb')
            buf = fp.read()
            fp.close()
            text = buf.decode('gbk')
            text = text.replace('\r\n','\n').replace('\r','\n')
            reg_all = sublime.Region(0, view.size())
            SetVar('__GBK_S','<L>')
            view.replace(edit,reg_all,text)
            view.set_scratch(True)

class SaveUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        if isView(view.id()):
            #DelVar('_ISGBKFILE')
            DelVar('__GBK_S')
            view.run_command('save',{"encoding": "utf-8"})

class PluginEventListener(sublime_plugin.EventListener):
    def on_load(self, view):
        if isView(view.id()) and  GetVar('_ISGBKFILE')!='GBK' and file_encoding(view)=='GBK':
            view.run_command('to_utf8')

    def on_post_save(self, view):
        if isView(view.id()) and GetVar('_ISGBKFILE')=='GBK':
            view.run_command('from_utf8')

    def on_pre_save(self, view):
        if isView(view.id()):
            file_encoding(view)
            pos = view.sel()[0]
            lpt = view.viewport_position()
            print('vp:',lpt,'Sel:',pos)
            SetVar('_pos_x',str(pos.a))
            SetVar('_pos_y',str(pos.b))
            SetVar('_pos_layout_x',str(lpt[0]))
            SetVar('_pos_layout_y',str(lpt[1]))

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
    

    def on_modified(self, view):
        if isView(view.id()):
            if view.file_name() and os.path.exists(view.file_name()):
                if view.encoding()!='UTF-8' and file_encoding(view)=='GBK':
                    view.run_command('to_utf8')
                elif GetVar('__GBK_S')=='<L>':
                    SetVar('__GBK_S','<A>')
                elif GetVar('__GBK_S')=='<A>':
                    DelVar('__GBK_S')
                    view.set_scratch(True)
                    sublime.set_timeout(restore_position_cb,50)
                else:
                    view.set_scratch(False)
            if view.is_dirty():
                DelVar('__GBK_S')


