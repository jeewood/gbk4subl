import sublime, sublime_plugin
import os

def log(msg):
    print(msg)
    pass

def isView(id):
    if not id: return false
    window = sublime.active_window()
    view = window.active_view() if window != None else None
    return (view is not None and view.id() == id)

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
            view.set_status('_ISGBKFILE','GBK')
            view.set_encoding('UTF-8')
            return 'GBK'
        except UnicodeDecodeError as e:
            if str.find(str(e),'4095')!=-1:
                view.set_status('_ISGBKFILE','GBK')
                view.set_encoding('UTF-8')
                return 'GBK'
    return 'UNKNOWN'

def SetVar(view, var, value):
    if(isView(view.id())):
        view.erase_status(var)
        view.set_status(var, value)

def GetVar(view, var):
    if(isView(view.id())):
        value = view.get_status(var)
        #if (int(value)!=0):
        #    SetVar(view,var,'0')
        return value
    return 0

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
            #pos = view.sel()[0]
            buf = fp.read()
            fp.close()
            text = buf.decode('gbk')
            text = text.replace('\r\n','\n').replace('\r','\n')
            reg_all = sublime.Region(0, view.size())
            view.erase_status(str(view.file_name()) + '_GBK_STATUS')
            view.set_status(str(view.file_name()) + '_GBK_STATUS','<L>')
            view.replace(edit,reg_all,text)
            view.set_scratch(True)
            #print(view.visible_region())

class SaveUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        view = self.view
        if isView(view.id()):
            view.erase_status('_ISGBKFILE')
            view.erase_status(str(view.file_name()) + '_GBK_STATUS')
            view.run_command('save',{"encoding": "utf-8"})

class PluginEventListener(sublime_plugin.EventListener):
    def on_load(self, view):
        if isView(view.id()) and  view.get_status('_ISGBKFILE')!='GBK' and file_encoding(view)=='GBK':
            view.run_command('to_utf8')

    def on_post_save(self, view):
        if isView(view.id()) and view.get_status('_ISGBKFILE')=='GBK':
            view.run_command('from_utf8')

    def on_pre_save(self, view):
        if isView(view.id()):
            file_encoding(view)
            pos = view.sel()[0]
            row,col = view.rowcol(pos.b)
            x = view.viewport_position()
            y = view.layout_to_text(x)
            SetVar(view,'_pos_layout',str(y))
            SetVar(view,'_pos_a',str(row))
            SetVar(view,'_pos_b',str(col))

    def on_close(self, view):
        if isView(view.id()) and view.get_status('_ISGBKFILE')=='GBK':
            view.run_command('from_utf8')

    def on_activated(self, view):
        if isView(view.id()) and (view.get_status('_ISGBKFILE')=='' or view.encoding() != 'UTF-8') \
            and file_encoding(view)=='GBK':
            view.run_command('to_utf8')

    def on_text_command(self, view, command_name, args):
        if isView(view.id()) and (view.get_status('_ISGBKFILE')=='' or view.encoding() != 'UTF-8') \
            and file_encoding(view)=='GBK':
            view.run_command('to_utf8')
    
    def on_modified(self, view):
        if isView(view.id()):
            if view.file_name() and os.path.exists(view.file_name()):
                if view.encoding()!='UTF-8' and file_encoding(view)=='GBK':
                    view.run_command('to_utf8')
                elif view.get_status(str(view.file_name()) + '_GBK_STATUS')=='<L>':
                    view.set_status(str(view.file_name()) + '_GBK_STATUS','<A>')
                elif view.get_status(str(view.file_name()) + '_GBK_STATUS')=='<A>':
                    view.erase_status(str(view.file_name()) + '_GBK_STATUS')
                else:
                    view.set_scratch(False)
            if view.is_dirty():
                view.erase_status(str(view.file_name()) + '_GBK_STATUS')

            a = GetVar(view,'_pos_a')
            b = GetVar(view,'_pos_b')
            if (a!='' and b!='' and a!='0' and b!='0'):
                pt = view.text_point(int(a),int(b))
                view.sel().clear()
                view.sel().add(sublime.Region(pt))
                view.show(pt)
                y = GetVar(view,'_pos_layout')
                view.set_viewport_position(view.text_to_layout(int(y)))
