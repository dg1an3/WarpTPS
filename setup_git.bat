@echo off
REM Setup Git configuration for WarpTPS repository

echo Setting up Git commit message template...
git config --local commit.template .gitmessage

echo Setting up Git user configuration (if not already set)...
git config --local --get user.name >nul 2>&1
if errorlevel 1 (
    echo Please enter your name:
    set /p USERNAME=
    git config --local user.name "%USERNAME%"
)

git config --local --get user.email >nul 2>&1
if errorlevel 1 (
 echo Please enter your email:
    set /p USEREMAIL=
    git config --local user.email "%USEREMAIL%"
)

echo.
echo Git configuration complete!
echo.
echo Configured settings:
git config --local --list | findstr /C:"user." /C:"commit.template"
echo.
echo You can now use 'git commit' and the template will appear in your editor.
echo.

pause
