#Requires -Version 5.1
$ErrorActionPreference = 'Stop'

# Stage only ASCII date directories like 20260904 and 20260905-2.

Set-Location -LiteralPath $PSScriptRoot

$targets = Get-ChildItem -LiteralPath $PSScriptRoot -Directory |
#    Where-Object { $_.Name -match '^\d{8}(-\d+)?$' }
    Where-Object { $_.Name -match '^\d{8}([._-]\d+)?$' }

if (-not $targets) {
    Write-Host 'No date directories found.'
    exit 0
}

$targets | Sort-Object Name | ForEach-Object { 
    # $hasChanges = git status --porcelain -- $_.FullName | Measure-Object | ForEach-Object { $_.Count -gt 0 }
    $hasChanges = [bool](git status --porcelain -- $_.FullName)
    if ($hasChanges) { git add -- $_.FullName }
}

# $targets | Sort-Object Name | ForEach-Object {
#     git add -- $_.FullName
# }

git status --short
