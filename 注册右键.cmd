@echo off
set p=%~dp0sublime_text.exe
set paths=%p:\=\\%
echo Windows Registry Editor Version 5.00 > sublime.reg
echo.  >> sublime.reg
echo [HKEY_CLASSES_ROOT\*\shell\Sublime] >> sublime.reg
echo @="Su&blime Text" >>sublime.reg
echo "Icon"="\"%paths%\",0" >>sublime.reg
echo.  >>sublime.reg
echo [HKEY_CLASSES_ROOT\*\shell\Sublime\command] >> sublime.reg
echo @="\"%paths%\" \"%%1\"" >>sublime.reg
sublime.reg
del sublime.reg
