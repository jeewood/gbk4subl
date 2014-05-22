@echo off

xcopy /Y /S "User\*.*" "..\User"

xcopy /Y /S "lang\*.*" "..\..\..\lang\"

xcopy /Y /S "Tools\*.*" "..\..\..\"


