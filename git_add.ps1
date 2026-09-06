#Requires -Version 5.1
$ErrorActionPreference = 'Stop'

# Stage only ASCII date directories like 20260904 and 20260905-2.
# The Chinese-named template was already staged manually.

Set-Location -LiteralPath $PSScriptRoot

$targets = Get-ChildItem -LiteralPath $PSScriptRoot -Directory |
    Where-Object { $_.Name -match '^\d{8}(-\d+)?$' }

if (-not $targets) {
    Write-Host 'No date directories found.'
    exit 0
}

Write-Host 'Directories to stage:'
$targets | Sort-Object Name | ForEach-Object { Write-Host ('  ' + $_.Name) }

$targets | Sort-Object Name | ForEach-Object {
    git add -- $_.FullName
}

git status --short
