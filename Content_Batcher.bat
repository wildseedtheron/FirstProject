@echo off
SETLOCAL EnableExtensions EnableDelayedExpansion

REM Get the top-level directory of the current repository
FOR /F "tokens=*" %%i IN ('git rev-parse --show-toplevel') DO SET "repoRoot=%%i"
SET "contentPath=%repoRoot%\Content"
SET "contentPath=!contentPath:\=/!"

CD /D "%repoRoot%"

REM Unstage any currently staged files
git reset

REM Get the list of all changed files under the Content directory
git status "%contentPath%" --porcelain > "%repoRoot%\status.txt"

SET "lastDir="
FOR /F "tokens=2 delims= " %%f in ('type "%repoRoot%\status.txt"') do (
    SET "dirPath=%%~dpf"
    SET "dirPath=!dirPath:\=/!"

    REM Get the relative directory path without the file name
    SET "relativeDirPath=!dirPath:%contentPath%=!"
    SET "relativeDirPath=!relativeDirPath:~0,-1!"
    SET "commitDir=!relativeDirPath:/= !"

    REM Check if we've moved to a new directory
    IF NOT "!lastDir!" == "!commitDir!" (
        IF NOT "!lastDir!" == "" (
            REM Commit the changes in the last directory
            git commit -m "Content: !lastDir!"
            IF !ERRORLEVEL! NEQ 0 (
                echo Commit failed for "!lastDir!". Exiting.
                exit /b 1
            )
            REM Push the commit
            git push origin main
            IF !ERRORLEVEL! NEQ 0 (
                echo Push failed for "!lastDir!". Exiting.
                exit /b 1
            )
        )
        REM Update lastDir to the new directory
        SET "lastDir=!commitDir!"
    )

    REM Add the file to staging
    git add "%%f"
)

REM Commit and push for the last directory
IF NOT "!lastDir!" == "" (
    git commit -m "Content: !lastDir!"
    IF !ERRORLEVEL! NEQ 0 (
        echo Commit failed for "!lastDir!". Exiting.
        exit /b 1
    )
    git push origin main
    IF !ERRORLEVEL! NEQ 0 (
        echo Push failed for "!lastDir!". Exiting.
        exit /b 1
    )
)

REM Cleanup
REM del "%repoRoot%\status.txt"

echo Script completed.
:end
ENDLOCAL
