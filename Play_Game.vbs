Set shell = CreateObject("WScript.Shell")
Set fso = CreateObject("Scripting.FileSystemObject")

repoPath = fso.GetParentFolderName(WScript.ScriptFullName)
shell.CurrentDirectory = repoPath
shell.Run Chr(34) & repoPath & "\run_windows.bat" & Chr(34), 0, False
