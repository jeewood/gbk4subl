import sublime, sublime_plugin
import os

def log(msg):
    sublime.message_dialog(str(msg))
    
def status(msg):
    sublime.status_message(str(msg))

def file_encoding(view):
    try:
        if os.path.exists(view.file_name()):
            try:
                fp = open(view.file_name(),'rb')
                buf = fp.read(4096)
                fp.close()
                buf = buf.decode('gbk')
                view.set_status('_ISGBKFILE','GBK')
                view.set_encoding('UTF-8')
                return 'GBK'
            except UnicodeDecodeError as e:
                if str.find(str(e),'4095')!=-1:
                    view.set_status('_ISGBKFILE','GBK')
                    view.set_encoding('UTF-8')
                    return 'GBK'
    except:
        pass 
    return 'UNKNOWN'

class FromUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        if self.view.get_status('_ISGBKFILE') == 'GBK':
            reg_all = sublime.Region(0, self.view.size())
            text = self.view.substr(reg_all)
            text = text.replace('\n','\r\n')
            text = text.encode('gbk')
            fp = open(self.view.file_name(),'wb')
            fp.write(text)
            fp.close()
            self.view.set_status('_GBK_STATUS','<A>')
            self.view.set_scratch(True)

class ToUtf8Command(sublime_plugin.TextCommand):
    def run(self, edit):
        if file_encoding(self.view)=='GBK':
            fp = open(self.view.file_name(),'rb')
            buf = fp.read()
            fp.close()
            text = buf.decode('gbk')
            text = text.replace('\r\n','\n').replace('\r','\n')
            reg_all = sublime.Region(0, self.view.size())
            self.view.set_status('_GBK_STATUS','<A>')
            self.view.replace(edit,reg_all,text)
            self.view.set_scratch(True)

class PluginEventListener(sublime_plugin.EventListener):
    def on_load(self, view):
        if file_encoding(view)=='GBK':
            view.run_command('to_utf8')

    def on_post_save(self, view):
        if view.get_status('_ISGBKFILE')=='GBK':
            view.run_command('from_utf8')

    def on_pre_save(self, view):
        file_encoding(view)

    def on_close(self, view):
        if view.get_status('_ISGBKFILE')=='GBK':
            view.run_command('from_utf8')

    def on_activated(self, view):
        if view.get_status('_ISGBKFILE')=='':
            view.set_status('_GBK_STATUS','<A>')
            file_encoding(view)

    def on_modified(self, view):
        if view.get_status('_GBK_STATUS')=='<A>':
            view.erase_status('_GBK_STATUS')
        else:
            view.set_scratch(False)

