# Setup Git configuration for WarpTPS repository

Write-Host "Setting up Git commit message template..." -ForegroundColor Cyan
git config --local commit.template .gitmessage

Write-Host "`nSetting up Git user configuration (if not already set)..." -ForegroundColor Cyan

$userName = git config --local --get user.name 2>$null
if (-not $userName) {
    $userName = Read-Host "Please enter your name"
    git config --local user.name $userName
}

$userEmail = git config --local --get user.email 2>$null
if (-not $userEmail) {
    $userEmail = Read-Host "Please enter your email"
    git config --local user.email $userEmail
}

Write-Host "`nGit configuration complete!" -ForegroundColor Green
Write-Host "`nConfigured settings:" -ForegroundColor Yellow
git config --local --list | Select-String -Pattern "user\.|commit\.template"

Write-Host "`nYou can now use 'git commit' and the template will appear in your editor." -ForegroundColor Green
Write-Host ""
