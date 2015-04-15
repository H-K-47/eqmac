hackIndex := 0

Loop, %A_WorkingDir%\*.*, 0, 1
{
    If A_LoopFileName Contains template
    {
        Continue
    }

    If A_LoopFileExt In ini
    {
        SplitPath, A_LoopFileLongPath, OutFileName, OutDir, OutExtension, OutNameNoExt, OutDrive

        FileAppend, bEnabled%hackIndex% = 0`n, !hackslist.txt
        FileAppend, sName%hackIndex%    = "%OutNameNoExt%"`n, !hackslist.txt
        FileAppend, `n, !hackslist.txt

        hackIndex := hackIndex + 1
    }
}