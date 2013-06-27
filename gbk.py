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
            view.erase_status(str(view.file_name()) + '_GBK_STATUS')
            view.set_status(str(view.file_name()) + '_GBK_STATUS','<L>')
            view.replace(edit,reg_all,text)
            view.set_scratch(True)

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