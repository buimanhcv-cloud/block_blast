Set WshShell = CreateObject("WScript.Shell")
WshShell.Run Chr(34) & Replace(WScript.ScriptFullName, "Play.vbs", "BlockPuzzleGame.exe") & Chr(34), 0
Set WshShell = Nothing
